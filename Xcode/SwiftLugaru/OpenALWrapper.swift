//
//  OpenALWrapper.swift
//  Lugaru
//
//  Created by C.W. Betts on 5/12/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation
import simd
import OpenAL


/// For non looping samples
let OPENAL_LOOP_OFF: Int32 = 0x00000001
/// For forward looping samples.
let OPENAL_LOOP_NORMAL: Int32 = 0x00000002
/// Attempts to make samples use 3d hardware acceleration. (if the card supports it)
let OPENAL_HW3D: UInt32 = 0x00001000
/// Tells software (not hardware) based sample not to be included in 3d processing.
let OPENAL_2D: UInt32 = 0x00002000
/// value to play on any free channel, or to allocate a sample in a free sample slot.
let OPENAL_FREE: Int32 = -1
/// for a channel index , this flag will affect ALL channels available!  Not supported by every function.
let OPENAL_ALL: Int32 = -3


///Used as a basic namespace
final class OpenALWrapper {
	typealias DSPUnit = UnsafeRawPointer

	fileprivate static var implChannels = [Channel]()
	static func channelAtIndex(_ idx: Int) -> Channel {
		return implChannels[idx]
	}
	
	final class Channel {
		var sid: ALuint = 0
		var sample: Sample?
		var startPaused = false
		var position: float3 = float3() {
			didSet {
				regenPosition()
			}
		}
		
		func regenPosition() {
			guard let sptr = sample else {
				return
			}
			//let sid = sptr.si
			if sptr.is2D {
				alSourcei(sid, AL_SOURCE_RELATIVE, AL_TRUE);
				alSource3f(sid, AL_POSITION, 0.0, 0.0, 0.0);
			} else {
				alSourcei(sid, AL_SOURCE_RELATIVE, AL_FALSE);
				alSource3f(sid, AL_POSITION, position.x, position.y, position.z);
			}
		}
		
		static func pauseAll(_ pause: Bool = true) {
			guard initialized else {
				return
			}
			implChannels.forEach() { $0.paused = pause }
		}
		
		var paused: Bool {
			get {
				if !initialized {
					return false;
				} else if startPaused {
					return true
				}
				
				var state: ALint = 0;
				alGetSourceiv(sid, AL_SOURCE_STATE, &state);
				return state == AL_PAUSED ? true : false
			}
			set {
				guard initialized else {
					return;
				}
				var state: ALint = 0;
				if startPaused {
					state = AL_PAUSED;
				} else {
					alGetSourceiv(sid, AL_SOURCE_STATE, &state);
				}
				
				if ((newValue) && (state == AL_PLAYING)) {
					alSourcePause(sid);
				} else if ((!newValue) && (state == AL_PAUSED)) {
					alSourcePlay(sid);
					startPaused = false;
				}
			}
		}

		func stop() {
			guard initialized else {
				return
			}

			alSourceStop(sid);
			startPaused = false;
		}
		
		static func stopAll() {
			guard initialized else {
				return
			}
			implChannels.forEach() { $0.stop() }
		}
		
		fileprivate var loopMode: Int32 {
			guard initialized else {
				return 0
			}
			var loop: ALint = 0;
			alGetSourceiv(sid, AL_LOOPING, &loop);
			if loop != 0 {
				return OPENAL_LOOP_NORMAL
			}
			return OPENAL_LOOP_OFF;
		}
		
		fileprivate var playing: Bool {
			guard initialized else {
				return false
			}
			
			var state: ALint = 0;
			alGetSourceiv(sid, AL_SOURCE_STATE, &state);
			return((state == AL_PLAYING) ? true : false);
		}
		
		@discardableResult
		func setAttributes(position pos: UnsafePointer<float3>?, velocity vel: UnsafePointer<float3>?) -> Bool {
			if !initialized {
				return false;
			}
			
			if pos != nil {
				position = (pos?.pointee)! * float3(1, 1, -1)
			}
			
			// we ignore velocity, since doppler's broken in the Linux AL at the moment...
			
			return true;
		}
		
		static func setVolumeForAll(_ vol: Int32) {
			guard initialized else {
				return
			}
			
			implChannels.forEach() { $0.setVolume(vol) }
		}
		
		func setVolume(_ vol1: Int32) {
			guard initialized else {
				return
			}
			
			let vol = max(min(vol1, 255), 0)
			let gain = ALfloat(vol) / 255.0
			alSourcef(sid, AL_GAIN, gain);
		}
		
		static func setFrequencyForAll(_ freq: Int32) {
			guard initialized else {
				return
			}
			
			implChannels.forEach() { $0.setFrequency(freq) }
		}
		
		func setFrequency(_ freq: Int32) {
			guard initialized else {
				return
			}

			if (freq == 8012) {
				// hack
				alSourcef(sid, AL_PITCH, 8012.0 / 44100.0);
			} else {
				alSourcef(sid, AL_PITCH, 1.0);
			}
		}
		
		deinit {
			alSourceStop(sid);
			alSourcei(sid, AL_BUFFER, 0);
			alDeleteSources(1, &sid);
		}
	}
	
	final class Sample {
		let name: String
		///buffer id.
		fileprivate(set) var bid: ALuint = 0
		var mode: Int32 = 0
		let is2D: Bool
		
		init?(index: Int32, file: URL, name name_or_data: String? = nil, mode: UInt32, offset: Int32, length: Int32) {
			guard initialized else {
				return nil
			}
			if index != OPENAL_FREE {
				return nil  // this is all the game does...
			}
			if offset != 0 {
				return nil  // this is all the game does...
			}
			if length != 0 {
				return nil  // this is all the game does...
			}
			if mode != OPENAL_HW3D && mode != OPENAL_2D {
				return nil  // this is all the game does...
			}
			
			var format: ALenum = AL_NONE;
			var size: ALsizei = 0;
			var frequency: ALuint = 0;
			var data = decodeToPCM(file, format: &format, size: &size, frequency: &frequency);
			guard data != nil else {
				return nil;
			}
			defer {
				free(data)
			}
			
			var bid: ALuint = 0;
			alGetError();
			alGenBuffers(1, &bid);
			guard (alGetError() == AL_NO_ERROR) else {
				return nil
			}
			alBufferData(bid, format, data, size, ALsizei(frequency))
			self.bid = bid
			self.mode = OPENAL_LOOP_OFF
			is2D = (mode == OPENAL_2D)
			self.name = name_or_data ?? file.lastPathComponent
		}
		
		deinit {
			alDeleteBuffers(1, &bid);
		}
		
		func deleteSample() {
			implChannels.forEach { (channel) in
				if channel.sample === self {
					alSourceStop(channel.sid)
					alSourcei(channel.sid, AL_BUFFER, 0)
					channel.sample = nil
				}
			}
		}
		
		fileprivate func stop() {
			guard initialized else {
				return;
			}
			
			implChannels.forEach { (channel) in
				if channel.sample === self {
					alSourceStop(channel.sid);
					channel.startPaused = false;
				}
			}
		}
	}
	
	fileprivate static var initialized = false
	fileprivate static var listenerPosition = float3()
	
	fileprivate static func setListenerAttributes(position pos: UnsafePointer<float3>?, velocity vel: inout UnsafePointer<float3>, f: float3, t: float3) {
		if !initialized {
			return;
		}
		if pos != nil {
			alListener3f(AL_POSITION, (pos?.pointee[0])!, (pos?.pointee[1])!, -(pos?.pointee[2])!);
			listenerPosition[0] = (pos?.pointee[0])!;
			listenerPosition[1] = (pos?.pointee[1])!;
			listenerPosition[2] = -(pos?.pointee[2])!;
		}
		
		let vec: [ALfloat] = [ f.x, f.y, -f.z, t.z, t.y, -t.z ];
		alListenerfv(AL_ORIENTATION, vec);
		
		// we ignore velocity, since doppler's broken in the Linux AL at the moment...
		
		// adjust existing positions...
		for channel in implChannels {
			channel.regenPosition()
		}
	}
	
	static func initialize(_ mixrate: Int32, maxSoftwareChannels maxsoftwarechannels: Int32, flags: UInt32) -> Bool {
		if initialized {
			return false;
		} else if maxsoftwarechannels == 0 {
			return false;
		}
		
		guard flags == 0 else {  // unsupported.
			return false;
		}
		
		let dev = alcOpenDevice(nil);
		guard dev != nil else {
			return false;
		}
		
		let caps: [ALint] = [ALC_FREQUENCY, mixrate, 0];
		let ctx = alcCreateContext(dev, caps);
		guard ctx != nil else {
			alcCloseDevice(dev);
			return false;
		}
		
		alcMakeContextCurrent(ctx);
		alcProcessContext(ctx);
		
		/*
		bool cmdline(const char * cmd);
		if (cmdline("openalinfo")) {
		printf("AL_VENDOR: %s\n", (char *) alGetString(AL_VENDOR));
		printf("AL_RENDERER: %s\n", (char *) alGetString(AL_RENDERER));
		printf("AL_VERSION: %s\n", (char *) alGetString(AL_VERSION));
		printf("AL_EXTENSIONS: %s\n", (char *) alGetString(AL_EXTENSIONS));
		}*/
		
		implChannels = [Channel](repeating: Channel(), count: Int(maxsoftwarechannels))
		for channel in implChannels {
			alGenSources(1, &channel.sid) // !!! FIXME: verify this didn't fail!
		}
		
		initialized = true;
		return true;
	}
	
	static func close() {
		guard initialized else {
			return;
		}
		
		let ctx = alcGetCurrentContext();
		if ctx != nil {
			implChannels = []
			let dev = alcGetContextsDevice(ctx);
			alcMakeContextCurrent(nil);
			alcSuspendContext(ctx);
			alcDestroyContext(ctx);
			alcCloseDevice(dev);
		}
		
		initialized = false;
	}
	
	static func update() {
		guard initialized else {
			return;
		}

		alcProcessContext(alcGetCurrentContext())
	}
	
	static func setSFXMasterVolume(_ volume: Int32) {
		guard initialized else {
			return
		}
		let gain = ALfloat(volume) / 255.0
		alListenerf(AL_GAIN, gain)
	}

	fileprivate static func OPENAL_PlaySoundEx(channel channel1: Int32, sample sptr: Sample, dsp: DSPUnit? = nil, startpaused: Bool) -> Int32 {
		var channel = channel1
		guard initialized else {
			return -1
		}
		guard dsp == nil else {
			return -1
		}
		if channel == OPENAL_FREE {
			for (i, chan) in implChannels.enumerated() {
				var state: ALint = 0;
				alGetSourceiv(chan.sid, AL_SOURCE_STATE, &state);
				if ((state != AL_PLAYING) && (state != AL_PAUSED)) {
					channel = Int32(i)
					break;
				}
			}
		}
		
		if channel < 0 || channel >= Int32(implChannels.count) {
			return -1;
		}
		alSourceStop(implChannels[Int(channel)].sid);
		implChannels[Int(channel)].sample = sptr;
		alSourcei(implChannels[Int(channel)].sid, AL_BUFFER, ALint(sptr.bid))
		alSourcei(implChannels[Int(channel)].sid, AL_LOOPING, (sptr.mode == OPENAL_LOOP_OFF) ? AL_FALSE : AL_TRUE);
		implChannels[Int(channel)].position = float3(0)
		
		implChannels[Int(channel)].startPaused = ((startpaused) ? true : false);
		if (!startpaused) {
			alSourcePlay(implChannels[Int(channel)].sid);
		}
		return channel;
	}
}

private func decodeToPCM(_ _fName: URL, format: inout ALenum, size: inout ALsizei, frequency freq: inout ALuint) -> UnsafeMutableRawPointer? {
	let fname: URL
	// !!! FIXME: if it's not Ogg, we don't have a decoder. I'm lazy.  :/
	if _fName.pathExtension != nil || _fName.pathExtension == "" {
		let tmpURL = _fName.deletingPathExtension()
		fname = tmpURL.appendingPathExtension("ogg")
	} else {
		fname = _fName.appendingPathExtension("ogg")
	}
	//#ifdef __POWERPC__
	//const int bigendian = 1;
	//#else
	let bigendian: Int32 = 0;
	//#endif
	
	// just in case...
	let io = fopen((fname as NSURL).fileSystemRepresentation, "rb");
	guard io != nil else {
		return nil;
	}
	
	var retval: UnsafeMutablePointer<ALubyte>? = nil;
	
	#if false  // untested, so disable this!
	// Can we just feed it to the AL compressed?
		if alIsExtensionPresent( "AL_EXT_vorbis") != 0 {
			format = alGetEnumValue( "AL_FORMAT_VORBIS_EXT");
			freq = 44100;
			fseek(io, 0, SEEK_END);
			size = ALsizei(ftell(io))
			fseek(io, 0, SEEK_SET);
			retval = UnsafeMutablePointer<ALubyte>(malloc(Int(size)))
			let rc = fread(retval, Int(size), 1, io);
			fclose(io);
			if (rc != 1) {
				free(retval);
				return nil;
			}
			return UnsafeMutablePointer<()>(retval);
		}
	#endif
	
	// Uncompress and feed to the AL.
	var vf = OggVorbis_File();
	if ov_open(io, &vf, nil, 0) == 0 {
		var bitstream: Int32 = 0;
		let info = ov_info(&vf, -1);
		size = 0;
		format = (info?.pointee.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		freq = ALuint((info?.pointee.rate)!)
		
		if ((info?.pointee.channels != 1) && (info?.pointee.channels != 2)) {
			ov_clear(&vf);
			return nil;
		}
		
		var buf = [Int8](repeating: 0, count: 1024 * 16)
		var rc = 0;
		var allocated = 64 * 1024;
		retval =  malloc(allocated).assumingMemoryBound(to: ALubyte.self)
		rc = ov_read(&vf, &buf, Int32(buf.count), bigendian, 2, 1, &bitstream)
		while rc != 0 {
			if rc > 0 {
				size += rc;
				if Int(size) >= allocated {
					allocated *= 2;
					let tmp = realloc(retval!, allocated)?.assumingMemoryBound(to: ALubyte.self)
					guard tmp != nil else {
						free(retval);
						retval = nil;
						break;
					}
					retval = tmp;
				}
				memcpy(retval?.advanced(by: Int(size) - rc), buf, rc);
			}
			rc = ov_read(&vf, &buf, Int32(buf.count), bigendian, 2, 1, &bitstream)
		}
		ov_clear(&vf);
		return UnsafeMutableRawPointer(retval!)
	}
	
	fclose(io);
	return nil;
}

func PlaySoundEx(channel chan: Int32, sample sptr: OpenALWrapper.Sample, dsp: OpenALWrapper.DSPUnit? = nil, startPaused: Bool) {
	func aChan(_ a: Int32) -> OpenALWrapper.Channel {
		return OpenALWrapper.channelAtIndex(Int(channels[Int(a)]))
	}
	let currSample = aChan(chan).sample
	if let currSample = currSample, currSample === samp[Int(chan)] {
		if aChan(chan).paused {
			aChan(chan).stop()
			channels[Int(chan)] = OPENAL_FREE
		} else if aChan(chan).playing {
			let loopMode = aChan(chan).loopMode
			if loopMode & OPENAL_LOOP_OFF != 0 {
				channels[Int(chan)] = OPENAL_FREE;
			}
		}
	} else {
		channels[Int(chan)] = OPENAL_FREE
	}
	
	channels[Int(chan)] = OpenALWrapper.OPENAL_PlaySoundEx(channel: channels[Int(chan)], sample: sptr, dsp: dsp, startpaused: startPaused);
	if (channels[Int(chan)] < 0) {
		channels[Int(chan)] = OpenALWrapper.OPENAL_PlaySoundEx(channel: OPENAL_FREE, sample: sptr, dsp: dsp, startpaused: startPaused);
	}
}

func PlayStreamEx(channel chan: Int32, sample sptr: OpenALWrapper.Sample, dsp: OpenALWrapper.DSPUnit? = nil, startPaused: Bool) {
	func aChan(_ a: Int32) -> OpenALWrapper.Channel {
		return OpenALWrapper.channelAtIndex(Int(channels[Int(a)]))
	}
	
	let currSample = aChan(chan).sample
	if let currSample = currSample, currSample === sptr {
		aChan(chan).stop()
		sptr.stop()
	} else {
		sptr.stop()
		channels[Int(chan)] = OPENAL_FREE
	}
	
	channels[Int(chan)] = OpenALWrapper.OPENAL_PlaySoundEx(channel: channels[Int(chan)], sample: sptr, dsp: dsp, startpaused: startPaused)
	if channels[Int(chan)] < 0 {
		channels[Int(chan)] = OpenALWrapper.OPENAL_PlaySoundEx(channel: OPENAL_FREE, sample: sptr, dsp: dsp, startpaused: startPaused)
	}
}
