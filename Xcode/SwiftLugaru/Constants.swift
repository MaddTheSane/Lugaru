//
//  Constants.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/23/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation

enum Award {
	case Klutz
	case Flawless
	case AllDead
	case NoDead
	case Stealth
	case Swordsman
	case KungFu
	case KnifeFighter
	case Coward
	case Evasion
	case Acrobat
	case LongRange
	case Brutal
	case Hyper
	case Aikido
	case Rambo
	case Fast
	case RealFast
	case DamnFast
	case Strategy
	case Bojutsu
}

enum MapObjective: Int32 {
	case KillEveryone = 0;
	case GoSomewhere = 1;
	case KillSomeone = 2;
	case KillMost = 3;

}

enum PathTypes {
	case KeepWalking
	case Pause
	
	var stringValue: String {
		switch self {
		case .KeepWalking:
			return "keepwalking"
		case .Pause:
			return "pause"
		}
	}
}

enum EditorTypes {
	case Active
	case Sitting
	case SittingWall
	case Sleeping
	case Dead1
	case Dead2
	case Dead3
	case Dead4
	
	var stringValue: String {
		switch self {
		case .Active:
			return "active"
			
		case .Sitting:
			return "sitting"
			
		case .SittingWall:
			return "sitting wall"
			
		case .Sleeping:
			return "sleeping"
			
		case .Dead1:
			return "dead1"
			
		case .Dead2:
			return "dead2"
			
		case .Dead3:
			return "dead3"
			
		case .Dead4:
			return "dead4"
		}
	}
}

enum Bonuses {
	case Tracheotomy
	case Backstab
	case Spinecrusher
	case Ninja
	case Style
	case Cannon
	case Aim
	case DeepImpact
	case TouchOfDeath
	case SwordReverse
	case StaffReverse
	case ReverseKO
	case SolidHit
	case TwoXCombo
	case ThreeXCombo
	case FourXCombo
	case MegaCombo
	case Reversal
	case Stab
	case Slice
	case Bullseye
	case Slash
	case Wolf
	case Finished
	case Tackle
	case Above
}

enum BoneConstraints {
	case BoneConnect //= 0;
	case Constraint //= 1;
	case Muscle //= 2;
}

enum BodySegment: Int32 {
	case Head = 0
	case Neck
	case LeftShoulder
	case LeftElbow
	case LeftWrist
	case LeftHand
	case RightShoulder
	case RightElbow
	case RightWrist
	case RightHand
	case Abdomen
	case LeftHip
	case RightHip
	case Groin
	case LeftKnee
	case LeftAnkle
	case LeftFoot
	case RightKnee
	case RightAnkle
	case RightFoot
}

let maxJoints = 50;
let maxFrames = 50;
let maxMuscles = 100;

enum CharacterAnimation: Int32 {
	case Run = 0;
	case BounceIdle = 1;
	case Stop = 2;
	case JumpUp = 3;
	case JumpDown = 4;
	case Land = 5;
	case Climb = 6;
	case Hang = 7;
	case SpinKick = 8;
	case Temp = 9;
	case GetUpFromFront = 10;
	case GetUpFromBack = 11;
	case Crouch = 12;
	case Sneak = 13;
	case Roll = 14;
	case Flip = 15;
	case spinkickreversed = 16;
	case spinkickreversal = 17;
	case LowKick = 18;
	case Sweep = 19;
	case SweepReversed = 20;
	case SweepReversal = 21;
	case RabbitKick = 22;
	case RabbitKickReversed = 23;
	case RabbitKickReversal = 24;
	case UPunch = 25;
	case StaggerBackHigh = 26;
	case UPunchReversed = 27;
	case UPunchReversal = 28;
	case HurtIdle = 29;
	case BackHandspring = 30;
	case FightIdle = 31;
	case Walk = 32;
	case FightSidestep = 33;
	case Kill = 34;
	case SneakAttack = 35;
	case SneakAttacked = 36;
	case DrawRight = 37;
	case KnifeSlashStart = 38;
	case CrouchStab = 39;
	case CrouchDrawRight = 40;
	case KnifeFollow = 41;
	case KnifeFollowed = 42;
	case KnifeThrow = 43;
	case RemoveKnife = 44;
	case CrouchRemoveKnife = 45;
	case JumpReversed = 46;
	case JumpReversal = 47;
	case LandHard = 48;
	case StaggerBackHard = 49;
	case DropKick = 50;
	case WindUpPunch = 51;
	case WindUpPunchBlocked = 52;
	case BlockHighLeft = 53;
	case BlockHighLeftStrike = 54;
	case WallJumpFront = 55;
	case WallJumpBack = 56;
	case WallJumpLeft = 57;
	case WallJumpRight = 58;
	case Backflip = 59;
	case LeftFlip = 60;
	case RightFlip = 61;
	case WallJumpRightKick = 62;
	case WallJumpLeftKick = 63;
	case KnifeFightIdle = 64;
	case KnifeSneakAttack = 65;
	case KnifeSneakAttacked = 66;
	case SwordStab = 67;
	case SwordSlashLeft = 68;
	case SwordSlashRight = 69;
	case SwordFightIdle = 70;
	case SwordSneakAttack = 71;
	case SwordSneakAttacked = 72;
	case DrawLeft = 73;
	case SwordSlash = 74;
	case SwordGroundStab = 75;
	case DodgeBack = 76;
	case SwordSlashReversed = 77;
	case SwordSlashReversal = 78;
	case KnifeSlashReversed = 79;
	case KnifeSlashReversal = 80;
	case SwordFightIdleBoth = 81;
	case SwordSlashParry = 82;
	case SwordDisarm = 83;
	case SwordSlashParried = 84;
	case WolfIdle = 85;
	case WolfFightIdle = 86;
	case WolfSwordIdle = 87;
	case WolfHurtIdle = 88;
	case WolfCrouch = 89;
	case WolfSneak = 90;
	case WolfRun = 91;
	case WolfStop = 92;
	case WolfClaw = 93;
	case WolfLand = 94;
	case WolfLandHard = 95;
	case WolfRunning = 96;
	case RabbitRunning = 97;
	case FrontFlip = 98;
	case RabbitTackle = 99;
	case RabbitTackling = 100;
	case RabbitTackledFront = 101;
	case RabbitTackledBack = 102;
	case RabbitTackleReversal = 103;
	case RabbitTackleReversed = 104;
	case WolfTackle = 105;
	case WolfTackling = 106;
	case WolfTackledFront = 107;
	case WolfTackledBack = 108;
	case WolfTackleReversal = 109;
	case WolfTackleReversed = 110;
	case WolfSlap = 111;
	case WolfBash = 112;
	case StaffHit = 113;
	case StaffGroundSmash = 114;
	case StaffSpinHit = 115;
	case StaffHitReversed = 116;
	case StaffHitReversal = 117;
	case StaffSpinHitReversed = 118;
	case StaffSpinHitReversal = 119;
	case Sleep = 120;
	case Sit = 121;
	case TalkIdle = 122;
	case SitWall = 123;
	case Dead1 = 124;
	case Dead2 = 125;
	case Dead3 = 126;
	case Dead4 = 127;
	case UserAnimation128 = 128
	case UserAnimation129 = 129
	case UserAnimation130 = 130
	case UserAnimation131 = 131
	case UserAnimation132 = 132
	case UserAnimation133 = 133
	case UserAnimation134 = 134
	case UserAnimation135 = 135
	case UserAnimation136 = 136
	case UserAnimation137 = 137
	case UserAnimation138 = 138
	case UserAnimation139 = 139
}

let animationCount = 140;

let maxDialogues = 20;
let maxDialogueLength = 20;

/// maximum number of vertexs
let maxModelVertex = 3000;
/// maximum number of texture-filled triangles in a model
let maxTexturedTriangle = 3000;

enum SoundStream: Int32 {
	case Music1Desert = 0;
	case Music1Grass = 1;
	case Music1Snow = 2;
	case Music2 = 3;
	case Music3 = 4;
	case Music4 = 5;
	case MenuMusic = 6;
	case DesertAmbient = 7;
	case FireSound = 8;
	case Wind = 9;
}

enum GameSounds: Int32 {
	case Footstep = 10;
	case Footstep2 = 11;
	case Footstep3 = 12;
	case Footstep4 = 13;
	case Jump = 14;
	case Land = 15;
	case Whoosh = 16;
	case Hawk = 17;
	case Land1 = 18;
	case Land2 = 19;
	case Break = 20;
	case LowWhoosh = 21;
	case HeavyImpact = 22;
	case FireStart = 23;
	case FireEnd = 24;
	case Break2 = 25;
	case KnifeDraw = 26;
	case KnifeSheathe = 27;
	case KnifeSwish = 28;
	case KnifeSlice = 29;
	case Skid = 30;
	case SnowSkid = 31;
	case BushRustle = 32;
	case MidWhoosh = 33;
	case HighWhoosh = 34;
	case MoveWhoosh = 35;
	case Thud = 36;
	case WhooshHit = 37;
	case Clank1 = 38;
	case Clank2 = 39;
	case Clank3 = 40;
	case Clank4 = 41;
	case ConsoleFail = 42;
	case ConsoleSuccess = 43;
	case SwordSlice = 44;
	case MetalHit = 45;
	case ClawSlice = 46;
	case Splatter = 47;
	case Growl = 48;
	case Growl2 = 49;
	case Bark = 50;
	case Snarl = 51;
	case Snarl2 = 52;
	case BarkGrowl = 53;
	case Bark2 = 54;
	case Bark3 = 55;
	case RabbitAttack = 56;
	case RabbitAttack2 = 57;
	case RabbitAttack3 = 58;
	case RabbitAttack4 = 59;
	case RabbitPain = 60;
	case RabbitPain1 = 61;
	case RabbitPain2 = 62;
	case RabbitChitter = 63;
	case RabbitChitter2 = 64;
	case FleshStab = 65;
	case FleshStabRemove = 66;
	case SwordStaff = 67;
	case StaffBody = 68;
	case StaffHead = 69;
	case Alarm = 70;
	case StaffBreak = 71;
}

enum CameraMode {
	case Normal
	case MotionBlur
	case RadialZoom
	case RealMotionBlur
	case DoubleVision
	case Glow
}

//let maxplayers = 10;


