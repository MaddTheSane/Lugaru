//
//  Constants.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/23/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation

enum Award {
	case klutz
	case flawless
	case allDead
	case noDead
	case stealth
	case swordsman
	case kungFu
	case knifeFighter
	case coward
	case evasion
	case acrobat
	case longRange
	case brutal
	case hyper
	case aikido
	case rambo
	case fast
	case realFast
	case damnFast
	case strategy
	case bojutsu
}

enum MapObjective: Int32 {
	case killEveryone = 0;
	case goSomewhere = 1;
	case killSomeone = 2;
	case killMost = 3;

}

enum PathTypes {
	case keepWalking
	case pause
	
	var stringValue: String {
		switch self {
		case .keepWalking:
			return "keepwalking"
		case .pause:
			return "pause"
		}
	}
}

enum EditorTypes {
	case active
	case sitting
	case sittingWall
	case sleeping
	case dead1
	case dead2
	case dead3
	case dead4
	
	var stringValue: String {
		switch self {
		case .active:
			return "active"
			
		case .sitting:
			return "sitting"
			
		case .sittingWall:
			return "sitting wall"
			
		case .sleeping:
			return "sleeping"
			
		case .dead1:
			return "dead1"
			
		case .dead2:
			return "dead2"
			
		case .dead3:
			return "dead3"
			
		case .dead4:
			return "dead4"
		}
	}
}

enum Bonuses {
	case tracheotomy
	case backstab
	case spinecrusher
	case ninja
	case style
	case cannon
	case aim
	case deepImpact
	case touchOfDeath
	case swordReverse
	case staffReverse
	case reverseKO
	case solidHit
	case twoXCombo
	case threeXCombo
	case fourXCombo
	case megaCombo
	case reversal
	case stab
	case slice
	case bullseye
	case slash
	case wolf
	case finished
	case tackle
	case above
}

enum BoneConstraints {
	case boneConnect //= 0;
	case constraint //= 1;
	case muscle //= 2;
}

enum BodySegment: Int32 {
	case head = 0
	case neck
	case leftShoulder
	case leftElbow
	case leftWrist
	case leftHand
	case rightShoulder
	case rightElbow
	case rightWrist
	case rightHand
	case abdomen
	case leftHip
	case rightHip
	case groin
	case leftKnee
	case leftAnkle
	case leftFoot
	case rightKnee
	case rightAnkle
	case rightFoot
}

let maxJoints = 50;
let maxFrames = 50;
let maxMuscles = 100;

enum CharacterAnimation: Int32 {
	case run = 0;
	case bounceIdle = 1;
	case stop = 2;
	case jumpUp = 3;
	case jumpDown = 4;
	case land = 5;
	case climb = 6;
	case hang = 7;
	case spinKick = 8;
	case temp = 9;
	case getUpFromFront = 10;
	case getUpFromBack = 11;
	case crouch = 12;
	case sneak = 13;
	case roll = 14;
	case flip = 15;
	case spinkickreversed = 16;
	case spinkickreversal = 17;
	case lowKick = 18;
	case sweep = 19;
	case sweepReversed = 20;
	case sweepReversal = 21;
	case rabbitKick = 22;
	case rabbitKickReversed = 23;
	case rabbitKickReversal = 24;
	case uPunch = 25;
	case staggerBackHigh = 26;
	case uPunchReversed = 27;
	case uPunchReversal = 28;
	case hurtIdle = 29;
	case backHandspring = 30;
	case fightIdle = 31;
	case walk = 32;
	case fightSidestep = 33;
	case kill = 34;
	case sneakAttack = 35;
	case sneakAttacked = 36;
	case drawRight = 37;
	case knifeSlashStart = 38;
	case crouchStab = 39;
	case crouchDrawRight = 40;
	case knifeFollow = 41;
	case knifeFollowed = 42;
	case knifeThrow = 43;
	case removeKnife = 44;
	case crouchRemoveKnife = 45;
	case jumpReversed = 46;
	case jumpReversal = 47;
	case landHard = 48;
	case staggerBackHard = 49;
	case dropKick = 50;
	case windUpPunch = 51;
	case windUpPunchBlocked = 52;
	case blockHighLeft = 53;
	case blockHighLeftStrike = 54;
	case wallJumpFront = 55;
	case wallJumpBack = 56;
	case wallJumpLeft = 57;
	case wallJumpRight = 58;
	case backflip = 59;
	case leftFlip = 60;
	case rightFlip = 61;
	case wallJumpRightKick = 62;
	case wallJumpLeftKick = 63;
	case knifeFightIdle = 64;
	case knifeSneakAttack = 65;
	case knifeSneakAttacked = 66;
	case swordStab = 67;
	case swordSlashLeft = 68;
	case swordSlashRight = 69;
	case swordFightIdle = 70;
	case swordSneakAttack = 71;
	case swordSneakAttacked = 72;
	case drawLeft = 73;
	case swordSlash = 74;
	case swordGroundStab = 75;
	case dodgeBack = 76;
	case swordSlashReversed = 77;
	case swordSlashReversal = 78;
	case knifeSlashReversed = 79;
	case knifeSlashReversal = 80;
	case swordFightIdleBoth = 81;
	case swordSlashParry = 82;
	case swordDisarm = 83;
	case swordSlashParried = 84;
	case wolfIdle = 85;
	case wolfFightIdle = 86;
	case wolfSwordIdle = 87;
	case wolfHurtIdle = 88;
	case wolfCrouch = 89;
	case wolfSneak = 90;
	case wolfRun = 91;
	case wolfStop = 92;
	case wolfClaw = 93;
	case wolfLand = 94;
	case wolfLandHard = 95;
	case wolfRunning = 96;
	case rabbitRunning = 97;
	case frontFlip = 98;
	case rabbitTackle = 99;
	case rabbitTackling = 100;
	case rabbitTackledFront = 101;
	case rabbitTackledBack = 102;
	case rabbitTackleReversal = 103;
	case rabbitTackleReversed = 104;
	case wolfTackle = 105;
	case wolfTackling = 106;
	case wolfTackledFront = 107;
	case wolfTackledBack = 108;
	case wolfTackleReversal = 109;
	case wolfTackleReversed = 110;
	case wolfSlap = 111;
	case wolfBash = 112;
	case staffHit = 113;
	case staffGroundSmash = 114;
	case staffSpinHit = 115;
	case staffHitReversed = 116;
	case staffHitReversal = 117;
	case staffSpinHitReversed = 118;
	case staffSpinHitReversal = 119;
	case sleep = 120;
	case sit = 121;
	case talkIdle = 122;
	case sitWall = 123;
	case dead1 = 124;
	case dead2 = 125;
	case dead3 = 126;
	case dead4 = 127;
	case userAnimation128 = 128
	case userAnimation129 = 129
	case userAnimation130 = 130
	case userAnimation131 = 131
	case userAnimation132 = 132
	case userAnimation133 = 133
	case userAnimation134 = 134
	case userAnimation135 = 135
	case userAnimation136 = 136
	case userAnimation137 = 137
	case userAnimation138 = 138
	case userAnimation139 = 139
}

let animationCount = 140;

let maxDialogues = 20;
let maxDialogueLength = 20;

/// maximum number of vertexs
let maxModelVertex = 3000;
/// maximum number of texture-filled triangles in a model
let maxTexturedTriangle = 3000;

enum SoundStream: Int32 {
	case music1Desert = 0;
	case music1Grass = 1;
	case music1Snow = 2;
	case music2 = 3;
	case music3 = 4;
	case music4 = 5;
	case menuMusic = 6;
	case desertAmbient = 7;
	case fireSound = 8;
	case wind = 9;
}

enum GameSounds: Int32 {
	case footstep = 10;
	case footstep2 = 11;
	case footstep3 = 12;
	case footstep4 = 13;
	case jump = 14;
	case land = 15;
	case whoosh = 16;
	case hawk = 17;
	case land1 = 18;
	case land2 = 19;
	case `break` = 20;
	case lowWhoosh = 21;
	case heavyImpact = 22;
	case fireStart = 23;
	case fireEnd = 24;
	case break2 = 25;
	case knifeDraw = 26;
	case knifeSheathe = 27;
	case knifeSwish = 28;
	case knifeSlice = 29;
	case skid = 30;
	case snowSkid = 31;
	case bushRustle = 32;
	case midWhoosh = 33;
	case highWhoosh = 34;
	case moveWhoosh = 35;
	case thud = 36;
	case whooshHit = 37;
	case clank1 = 38;
	case clank2 = 39;
	case clank3 = 40;
	case clank4 = 41;
	case consoleFail = 42;
	case consoleSuccess = 43;
	case swordSlice = 44;
	case metalHit = 45;
	case clawSlice = 46;
	case splatter = 47;
	case growl = 48;
	case growl2 = 49;
	case bark = 50;
	case snarl = 51;
	case snarl2 = 52;
	case barkGrowl = 53;
	case bark2 = 54;
	case bark3 = 55;
	case rabbitAttack = 56;
	case rabbitAttack2 = 57;
	case rabbitAttack3 = 58;
	case rabbitAttack4 = 59;
	case rabbitPain = 60;
	case rabbitPain1 = 61;
	case rabbitPain2 = 62;
	case rabbitChitter = 63;
	case rabbitChitter2 = 64;
	case fleshStab = 65;
	case fleshStabRemove = 66;
	case swordStaff = 67;
	case staffBody = 68;
	case staffHead = 69;
	case alarm = 70;
	case staffBreak = 71;
}

enum CameraMode {
	case normal
	case motionBlur
	case radialZoom
	case realMotionBlur
	case doubleVision
	case glow
}

//let maxplayers = 10;


