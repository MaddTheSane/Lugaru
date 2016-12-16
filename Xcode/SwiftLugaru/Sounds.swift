//
//  Sounds.swift
//  Lugaru
//
//  Created by C.W. Betts on 5/21/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation
import simd


/// FIXME: dimensionality is not a property of the sound sample.
/// This should be decided at the time of playback
private func snd_mode(_ snd: SoundTypes) -> UInt32 {
	switch snd {
	case .alarm, .consoleFail, .consoleSuccess, .fireStart, .fireEnd:
		return OPENAL_2D
		
	default:
		return OPENAL_HW3D
	}
}

var channels = [Int32](repeating: OPENAL_FREE, count: 100)
var samp: Array<OpenALWrapper.Sample> = {
	var toRet = Array<OpenALWrapper.Sample>()
	for i in 0..<SoundTypes.count.rawValue {
		let aSamp = OpenALWrapper.Sample(index: OPENAL_FREE, file: ConvertFileName(":Data:Sounds:" + Sounds[i].name), name: Sounds[i].name, mode: snd_mode(SoundTypes(rawValue: i)!), offset: 0, length: 0)!
		toRet.append(aSamp)
	}
	//footstepsound = footstepsn1;
	//footstepsound2 = footstepsn2;
	//footstepsound3 = footstepst1;
	//footstepsound4 = footstepst2;
	// Huh?
	// OPENAL_Sample_SetMode(samp[whooshsound], OPENAL_LOOP_NORMAL);
	for i in SoundTypes.stream_Fire.rawValue...SoundTypes.stream_MenuTheme.rawValue {
		toRet[i].mode = OPENAL_LOOP_NORMAL
	}
	return toRet
}()

enum SoundTypes: Int {
	case footstepSnow1 = 0
	case footstepSnow2
	case footstepStone1
	case footstepStone2
	case footstepGrass1
	case footstepGrass2
	case land
	case jump
	case hawk
	case whoosh
	case land1
	case land2
	case `break`
	case lowWhoosh
	case midWhoosh
	case highWhoosh
	case moveWhoosh
	case heavyImpact
	case whooshHit
	case thud
	case alarm
	case break2
	case knifeDraw
	case knifeSheathe
	case fleshStab
	case fleshStabRemove
	case knifeSwish
	case knifeSlice
	case swordSlice
	case skid
	case snowSkid
	case bushRustle
	case clank1
	case clank2
	case clank3
	case clank4
	case consoleSuccess
	case consoleFail
	case metalHit
	case clawSlice
	case splatter
	case growl
	case growl2
	case bark
	case bark2
	case bark3
	case snarl
	case snarl2
	case barkGrowl
	case rabbitAttack
	case rabbitAttack2
	case rabbitAttack3
	case rabbitAttack4
	case rabbitPain
	case rabbitPain1
	case rabbitChitter
	case rabbitChitter2
	case swordStaff
	case staffBody
	case staffHead
	case staffBreak
	case fireStart
	case fireEnd
	case stream_Fire
	case stream_GrassTheme
	case stream_SnowTheme
	case stream_DesertTheme
	case stream_Wind
	case stream_DesertAmbient
	case stream_FightTheme
	case stream_MenuTheme
	
	case count
}


private struct SoundValues {
	var index: SoundTypes
	var name: String
}

private let Sounds: [SoundValues] = {
	var toRet = [SoundValues]()
	func DECLARE_SOUND(_ a: SoundTypes, _ b: String) {
		toRet.append(SoundValues(index: a, name: b))
	}
	// TODO: better, more elegant way to do this?
	DECLARE_SOUND(.footstepSnow1, "footstepsnow1.ogg")
	DECLARE_SOUND(.footstepSnow2, "footstepsnow2.ogg")
	DECLARE_SOUND(.footstepStone1, "footstepstone1.ogg")
	DECLARE_SOUND(.footstepStone2, "footstepstone2.ogg")
	DECLARE_SOUND(.footstepGrass1, "footstepgrass1.ogg")
	DECLARE_SOUND(.footstepGrass2, "footstepgrass2.ogg")
	DECLARE_SOUND(.land, "land.ogg")
	DECLARE_SOUND(.jump, "jump.ogg")
	DECLARE_SOUND(.hawk, "hawk.ogg")
	DECLARE_SOUND(.whoosh, "whoosh.ogg")
	DECLARE_SOUND(.land1, "land1.ogg")
	DECLARE_SOUND(.land2, "land2.ogg")
	DECLARE_SOUND(.break, "broken.ogg")
	DECLARE_SOUND(.lowWhoosh, "lowwhoosh.ogg")
	DECLARE_SOUND(.midWhoosh, "midwhoosh.ogg")
	DECLARE_SOUND(.highWhoosh, "highwhoosh.ogg")
	DECLARE_SOUND(.moveWhoosh, "movewhoosh.ogg")
	DECLARE_SOUND(.heavyImpact, "heavyimpact.ogg")
	DECLARE_SOUND(.whooshHit, "whooshhit.ogg")
	DECLARE_SOUND(.thud, "thud.ogg")
	DECLARE_SOUND(.alarm, "alarm.ogg")
	DECLARE_SOUND(.break2, "break.ogg")
	DECLARE_SOUND(.knifeDraw, "knifedraw.ogg")
	DECLARE_SOUND(.knifeSheathe, "knifesheathe.ogg")
	DECLARE_SOUND(.fleshStab, "fleshstab.ogg")
	DECLARE_SOUND(.fleshStabRemove, "fleshstabremove.ogg")
	DECLARE_SOUND(.knifeSwish, "knifeswish.ogg")
	DECLARE_SOUND(.knifeSlice, "knifeslice.ogg")
	DECLARE_SOUND(.swordSlice, "swordslice.ogg")
	DECLARE_SOUND(.skid, "skid.ogg")
	DECLARE_SOUND(.snowSkid, "snowskid.ogg")
	DECLARE_SOUND(.bushRustle, "bushrustle.ogg")
	DECLARE_SOUND(.clank1, "clank1.ogg")
	DECLARE_SOUND(.clank2, "clank2.ogg")
	DECLARE_SOUND(.clank3, "clank3.ogg")
	DECLARE_SOUND(.clank4, "clank4.ogg")
	DECLARE_SOUND(.consoleSuccess, "consolesuccess.ogg")
	DECLARE_SOUND(.consoleFail, "consolefail.ogg")
	DECLARE_SOUND(.metalHit, "metalhit.ogg")
	DECLARE_SOUND(.clawSlice, "clawslice.ogg")
	DECLARE_SOUND(.splatter, "splatter.ogg")
	DECLARE_SOUND(.growl, "growl.ogg")
	DECLARE_SOUND(.growl2, "growl2.ogg")
	DECLARE_SOUND(.bark, "bark.ogg")
	DECLARE_SOUND(.bark2, "bark2.ogg")
	DECLARE_SOUND(.bark3, "bark3.ogg")
	DECLARE_SOUND(.snarl, "snarl.ogg")
	DECLARE_SOUND(.snarl2, "snarl2.ogg")
	DECLARE_SOUND(.barkGrowl, "barkgrowl.ogg")
	DECLARE_SOUND(.rabbitAttack, "rabbitattack.ogg")
	DECLARE_SOUND(.rabbitAttack2, "rabbitattack2.ogg")
	DECLARE_SOUND(.rabbitAttack3, "rabbitattack3.ogg")
	DECLARE_SOUND(.rabbitAttack4, "rabbitattack4.ogg")
	DECLARE_SOUND(.rabbitPain, "rabbitpain.ogg")
	DECLARE_SOUND(.rabbitPain1, "rabbitpain2.ogg")
	DECLARE_SOUND(.rabbitChitter, "rabbitchitter.ogg")
	DECLARE_SOUND(.rabbitChitter2, "rabbitchitter2.ogg")
	DECLARE_SOUND(.swordStaff, "swordstaff.ogg")
	DECLARE_SOUND(.staffBody, "staffbody.ogg")
	DECLARE_SOUND(.staffHead, "staffhead.ogg")
	DECLARE_SOUND(.staffBreak, "staffbreak.ogg")
	DECLARE_SOUND(.fireStart, "firestart.ogg")
	DECLARE_SOUND(.fireEnd, "fireend.ogg")
	DECLARE_SOUND(.stream_Fire, "fire.ogg")
	DECLARE_SOUND(.stream_GrassTheme, "music1grass.ogg")
	DECLARE_SOUND(.stream_SnowTheme, "music1snow.ogg")
	DECLARE_SOUND(.stream_DesertTheme, "music1desert.ogg")
	DECLARE_SOUND(.stream_Wind, "wind.ogg")
	DECLARE_SOUND(.stream_DesertAmbient, "desertambient.ogg")
	DECLARE_SOUND(.stream_FightTheme, "music2.ogg")
	DECLARE_SOUND(.stream_MenuTheme, "music3.ogg")
	
	return toRet
}()

var FootstepSound1: SoundTypes = .footstepSnow1;
var FootstepSound2: SoundTypes = .footstepSnow2;
var FootstepSound3: SoundTypes = .footstepStone1;
var FootstepSound4: SoundTypes = .footstepStone2;


func loadAllSounds() {
	//Force loading the sounds
	_ = samp
}

func emitSoundAt(_ soundid: Int32, position pos: float3, volume vol: Float)
{
	var pos2 = pos
	PlaySoundEx (channel: soundid, sample: samp[Int(soundid)], dsp: nil, startPaused: true)
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.setAttributes(position: &pos2, velocity: nil)
	aChann.setVolume(Int32(vol))
	aChann.paused = false
}

func emitSound_np(_ soundid: Int32, volume vol: Float)
{
	PlaySoundEx (channel: soundid, sample: samp[Int(soundid)], dsp: nil, startPaused: true);
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.setVolume(Int32(vol))
	aChann.paused = false
}

func emitStreamAt(_ soundid: Int32, position pos: float3, volume vol: Float) {
	var pos2 = pos
	PlayStreamEx (channel: soundid, sample: samp[Int(soundid)], dsp: nil, startPaused: true);
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.setAttributes(position: &pos2, velocity: nil)
	aChann.setVolume(Int32(vol))
	aChann.paused = false
}

func emitStream_np(_ soundid: Int32, volume vol: Float) {
	PlayStreamEx (channel: soundid, sample: samp[Int(soundid)], dsp: nil, startPaused: true);
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.setVolume(Int32(vol))
	aChann.paused = false
}

func resumeStream(_ soundid: Int32) {
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.paused = false
}

func pauseSound(_ soundid: Int32) {
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.paused = true
}
