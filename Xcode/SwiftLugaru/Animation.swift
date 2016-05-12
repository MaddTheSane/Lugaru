//
//  Animation.swift
//  Lugaru
//
//  Created by C.W. Betts on 5/12/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation

class Animation {
	enum AttackType {
		case Neutral
		case NormalAttack
		case Reversed
		case Reversal
	}
	
	enum HeightType {
		case Low
		case Middle
		case High
	}
	
	enum AnimationTypes: Int {
		case runanim = 0
		case bounceidleanim
		case stopanim
		case jumpupanim
		case jumpdownanim
		case landanim
		case landhardanim
		case climbanim
		case hanganim
		case spinkickanim
		case getupfromfrontanim
		case getupfrombackanim
		case crouchanim
		case sneakanim
		case rollanim
		case flipanim
		case frontflipanim
		case spinkickreversedanim
		case spinkickreversalanim
		case lowkickanim
		case sweepanim
		case sweepreversedanim
		case sweepreversalanim
		case rabbitkickanim
		case rabbitkickreversedanim
		case rabbitkickreversalanim
		case upunchanim
		case staggerbackhighanim
		case upunchreversedanim
		case upunchreversalanim
		case hurtidleanim
		case backhandspringanim
		case fightidleanim
		case walkanim
		case fightsidestep
		case killanim
		case sneakattackanim
		case sneakattackedanim
		case drawrightanim
		case knifeslashstartanim
		case crouchdrawrightanim
		case crouchstabanim
		case knifefollowanim
		case knifefollowedanim
		case knifethrowanim
		case removeknifeanim
		case crouchremoveknifeanim
		case jumpreversedanim
		case jumpreversalanim
		case staggerbackhardanim
		case dropkickanim
		case winduppunchanim
		case winduppunchblockedanim
		case blockhighleftanim
		case blockhighleftstrikeanim
		case backflipanim
		case walljumpbackanim
		case walljumpfrontanim
		case rightflipanim
		case walljumprightanim
		case leftflipanim
		case walljumpleftanim
		case walljumprightkickanim
		case walljumpleftkickanim
		case knifefightidleanim
		case knifesneakattackanim
		case knifesneakattackedanim
		case swordfightidleanim
		case drawleftanim
		case swordslashanim
		case swordgroundstabanim
		case dodgebackanim
		case swordsneakattackanim
		case swordsneakattackedanim
		case swordslashreversedanim
		case swordslashreversalanim
		case knifeslashreversedanim
		case knifeslashreversalanim
		case swordfightidlebothanim
		case swordslashparryanim
		case swordslashparriedanim
		case wolfidle
		case wolfcrouchanim
		case wolflandanim
		case wolflandhardanim
		case wolfrunanim
		case wolfrunninganim
		case rabbitrunninganim
		case wolfstopanim
		case rabbittackleanim
		case rabbittacklinganim
		case rabbittackledbackanim
		case rabbittackledfrontanim
		case wolfslapanim
		case staffhitanim
		case staffgroundsmashanim
		case staffspinhitanim
		case staffhitreversedanim
		case staffhitreversalanim
		case staffspinhitreversedanim
		case staffspinhitreversalanim
		case sitanim
		case sleepanim
		case talkidleanim
		case sitwallanim
		case dead1anim
		case dead2anim
		case dead3anim
		case dead4anim
		
		case loadable_anim_end
		
		
		case rabbittacklereversal
		case rabbittacklereversed
		case sworddisarmanim
		case swordslashleftanim
		case swordslashrightanim
		case swordstabanim
		case wolfbashanim
		case wolfclawanim
		case wolffightidle
		case wolfhurtidle
		case wolfsneakanim
		case wolfswordidle
		case wolftackleanim
		case wolftackledbacanim
		case wolftackledfrontanim
		case wolftacklereversal
		case wolftacklereversed
		case wolftacklinganim
		
		case tempanim
		case animation_count
	}
	
	enum AnimationBitOffsets: Int {
		case Idle = 0
		case Sit
		case Sleep
		case Crouch
		case Run
		case Stop
		case Land
		case LandHard
		case Flip
		case Walljump
		
		case animation_bit_count
	};
	
	struct AnimationBits: OptionSetType {
		let rawValue: UInt
	
		init(rawValue: UInt) {
			self.rawValue = rawValue
		}
		static let Idle = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Idle.rawValue))
		static let Sit = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Sit.rawValue))
		static let Sleep = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Sleep.rawValue))
		static let Crouch = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Crouch.rawValue))
		static let Run = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Run.rawValue))
		static let Stop = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Stop.rawValue))
		static let Land = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Land.rawValue))
		static let LandHard = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.LandHard.rawValue))
		static let Flip = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Flip.rawValue))
		static let Walljump = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.Walljump.rawValue))
	};
}