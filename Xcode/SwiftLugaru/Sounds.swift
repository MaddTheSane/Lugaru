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
private func snd_mode(snd: SoundTypes) -> UInt32 {
	switch snd {
	case .Alarm, .ConsoleFail, .ConsoleSuccess, .FireStart, .FireEnd:
		return OPENAL_2D
		
	default:
		return OPENAL_HW3D
	}
}

var channels = [Int32](count: 100, repeatedValue: OPENAL_FREE)
var samp: Array<OpenALWrapper.Sample> = {
	var toRet = Array<OpenALWrapper.Sample>()
	for i in 0..<SoundTypes.Count.rawValue {
		let aSamp = OpenALWrapper.Sample(index: OPENAL_FREE, file: ConvertFileName(":Data:Sounds:" + Sounds[i].name), name: Sounds[i].name, mode: snd_mode(SoundTypes(rawValue: i)!), offset: 0, length: 0)!
		toRet.append(aSamp)
	}
	//footstepsound = footstepsn1;
	//footstepsound2 = footstepsn2;
	//footstepsound3 = footstepst1;
	//footstepsound4 = footstepst2;
	// Huh?
	// OPENAL_Sample_SetMode(samp[whooshsound], OPENAL_LOOP_NORMAL);
	for i in SoundTypes.Stream_Fire.rawValue...SoundTypes.Stream_MenuTheme.rawValue {
		toRet[i].mode = OPENAL_LOOP_NORMAL
	}
	return toRet
}()

enum SoundTypes: Int {
	case FootstepSnow1 = 0
	case FootstepSnow2
	case FootstepStone1
	case FootstepStone2
	case FootstepGrass1
	case FootstepGrass2
	case Land
	case Jump
	case Hawk
	case Whoosh
	case Land1
	case Land2
	case Break
	case LowWhoosh
	case MidWhoosh
	case HighWhoosh
	case MoveWhoosh
	case HeavyImpact
	case WhooshHit
	case Thud
	case Alarm
	case Break2
	case KnifeDraw
	case KnifeSheathe
	case FleshStab
	case FleshStabRemove
	case KnifeSwish
	case KnifeSlice
	case SwordSlice
	case Skid
	case SnowSkid
	case BushRustle
	case Clank1
	case Clank2
	case Clank3
	case Clank4
	case ConsoleSuccess
	case ConsoleFail
	case MetalHit
	case ClawSlice
	case Splatter
	case Growl
	case Growl2
	case Bark
	case Bark2
	case Bark3
	case Snarl
	case Snarl2
	case BarkGrowl
	case RabbitAttack
	case RabbitAttack2
	case RabbitAttack3
	case RabbitAttack4
	case RabbitPain
	case RabbitPain1
	case RabbitChitter
	case RabbitChitter2
	case SwordStaff
	case StaffBody
	case StaffHead
	case StaffBreak
	case FireStart
	case FireEnd
	case Stream_Fire
	case Stream_GrassTheme
	case Stream_SnowTheme
	case Stream_DesertTheme
	case Stream_Wind
	case Stream_DesertAmbient
	case Stream_FightTheme
	case Stream_MenuTheme
	
	case Count
}


private struct SoundValues {
	var index: SoundTypes
	var name: String
}

private let Sounds: [SoundValues] = {
	var toRet = [SoundValues]()
	func DECLARE_SOUND(a: SoundTypes, _ b: String) {
		toRet.append(SoundValues(index: a, name: b))
	}
	// TODO: better, more elegant way to do this?
	DECLARE_SOUND(.FootstepSnow1, "footstepsnow1.ogg")
	DECLARE_SOUND(.FootstepSnow2, "footstepsnow2.ogg")
	DECLARE_SOUND(.FootstepStone1, "footstepstone1.ogg")
	DECLARE_SOUND(.FootstepStone2, "footstepstone2.ogg")
	DECLARE_SOUND(.FootstepGrass1, "footstepgrass1.ogg")
	DECLARE_SOUND(.FootstepGrass2, "footstepgrass2.ogg")
	DECLARE_SOUND(.Land, "land.ogg")
	DECLARE_SOUND(.Jump, "jump.ogg")
	DECLARE_SOUND(.Hawk, "hawk.ogg")
	DECLARE_SOUND(.Whoosh, "whoosh.ogg")
	DECLARE_SOUND(.Land1, "land1.ogg")
	DECLARE_SOUND(.Land2, "land2.ogg")
	DECLARE_SOUND(.Break, "broken.ogg")
	DECLARE_SOUND(.LowWhoosh, "lowwhoosh.ogg")
	DECLARE_SOUND(.MidWhoosh, "midwhoosh.ogg")
	DECLARE_SOUND(.HighWhoosh, "highwhoosh.ogg")
	DECLARE_SOUND(.MoveWhoosh, "movewhoosh.ogg")
	DECLARE_SOUND(.HeavyImpact, "heavyimpact.ogg")
	DECLARE_SOUND(.WhooshHit, "whooshhit.ogg")
	DECLARE_SOUND(.Thud, "thud.ogg")
	DECLARE_SOUND(.Alarm, "alarm.ogg")
	DECLARE_SOUND(.Break2, "break.ogg")
	DECLARE_SOUND(.KnifeDraw, "knifedraw.ogg")
	DECLARE_SOUND(.KnifeSheathe, "knifesheathe.ogg")
	DECLARE_SOUND(.FleshStab, "fleshstab.ogg")
	DECLARE_SOUND(.FleshStabRemove, "fleshstabremove.ogg")
	DECLARE_SOUND(.KnifeSwish, "knifeswish.ogg")
	DECLARE_SOUND(.KnifeSlice, "knifeslice.ogg")
	DECLARE_SOUND(.SwordSlice, "swordslice.ogg")
	DECLARE_SOUND(.Skid, "skid.ogg")
	DECLARE_SOUND(.SnowSkid, "snowskid.ogg")
	DECLARE_SOUND(.BushRustle, "bushrustle.ogg")
	DECLARE_SOUND(.Clank1, "clank1.ogg")
	DECLARE_SOUND(.Clank2, "clank2.ogg")
	DECLARE_SOUND(.Clank3, "clank3.ogg")
	DECLARE_SOUND(.Clank4, "clank4.ogg")
	DECLARE_SOUND(.ConsoleSuccess, "consolesuccess.ogg")
	DECLARE_SOUND(.ConsoleFail, "consolefail.ogg")
	DECLARE_SOUND(.MetalHit, "metalhit.ogg")
	DECLARE_SOUND(.ClawSlice, "clawslice.ogg")
	DECLARE_SOUND(.Splatter, "splatter.ogg")
	DECLARE_SOUND(.Growl, "growl.ogg")
	DECLARE_SOUND(.Growl2, "growl2.ogg")
	DECLARE_SOUND(.Bark, "bark.ogg")
	DECLARE_SOUND(.Bark2, "bark2.ogg")
	DECLARE_SOUND(.Bark3, "bark3.ogg")
	DECLARE_SOUND(.Snarl, "snarl.ogg")
	DECLARE_SOUND(.Snarl2, "snarl2.ogg")
	DECLARE_SOUND(.BarkGrowl, "barkgrowl.ogg")
	DECLARE_SOUND(.RabbitAttack, "rabbitattack.ogg")
	DECLARE_SOUND(.RabbitAttack2, "rabbitattack2.ogg")
	DECLARE_SOUND(.RabbitAttack3, "rabbitattack3.ogg")
	DECLARE_SOUND(.RabbitAttack4, "rabbitattack4.ogg")
	DECLARE_SOUND(.RabbitPain, "rabbitpain.ogg")
	DECLARE_SOUND(.RabbitPain1, "rabbitpain2.ogg")
	DECLARE_SOUND(.RabbitChitter, "rabbitchitter.ogg")
	DECLARE_SOUND(.RabbitChitter2, "rabbitchitter2.ogg")
	DECLARE_SOUND(.SwordStaff, "swordstaff.ogg")
	DECLARE_SOUND(.StaffBody, "staffbody.ogg")
	DECLARE_SOUND(.StaffHead, "staffhead.ogg")
	DECLARE_SOUND(.StaffBreak, "staffbreak.ogg")
	DECLARE_SOUND(.FireStart, "firestart.ogg")
	DECLARE_SOUND(.FireEnd, "fireend.ogg")
	DECLARE_SOUND(.Stream_Fire, "fire.ogg")
	DECLARE_SOUND(.Stream_GrassTheme, "music1grass.ogg")
	DECLARE_SOUND(.Stream_SnowTheme, "music1snow.ogg")
	DECLARE_SOUND(.Stream_DesertTheme, "music1desert.ogg")
	DECLARE_SOUND(.Stream_Wind, "wind.ogg")
	DECLARE_SOUND(.Stream_DesertAmbient, "desertambient.ogg")
	DECLARE_SOUND(.Stream_FightTheme, "music2.ogg")
	DECLARE_SOUND(.Stream_MenuTheme, "music3.ogg")
	
	return toRet
}()

var FootstepSound1: SoundTypes = .FootstepSnow1;
var FootstepSound2: SoundTypes = .FootstepSnow2;
var FootstepSound3: SoundTypes = .FootstepStone1;
var FootstepSound4: SoundTypes = .FootstepStone2;


func loadAllSounds() {
	//Force loading the sounds
	_ = samp
}

func emitSoundAt(soundid: Int32, position pos: float3, volume vol: Float)
{
	var pos2 = pos
	PlaySoundEx (channel: soundid, sample: samp[Int(soundid)], dsp: nil, startPaused: true)
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.setAttributes(position: &pos2, velocity: nil)
	aChann.setVolume(Int32(vol))
	aChann.paused = false
}

func emitSound_np(soundid: Int32, volume vol: Float)
{
	PlaySoundEx (channel: soundid, sample: samp[Int(soundid)], dsp: nil, startPaused: true);
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.setVolume(Int32(vol))
	aChann.paused = false
}

func emitStreamAt(soundid: Int32, position pos: float3, volume vol: Float) {
	var pos2 = pos
	PlayStreamEx (channel: soundid, sample: samp[Int(soundid)], dsp: nil, startPaused: true);
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.setAttributes(position: &pos2, velocity: nil)
	aChann.setVolume(Int32(vol))
	aChann.paused = false
}

func emitStream_np(soundid: Int32, volume vol: Float) {
	PlayStreamEx (channel: soundid, sample: samp[Int(soundid)], dsp: nil, startPaused: true);
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.setVolume(Int32(vol))
	aChann.paused = false
}

func resumeStream(soundid: Int32) {
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.paused = false
}

func pauseSound(soundid: Int32) {
	let aChann = OpenALWrapper.channelAtIndex(Int(channels[Int(soundid)]))
	aChann.paused = true
}
