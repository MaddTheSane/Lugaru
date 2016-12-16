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
		case neutral
		case normalAttack
		case reversed
		case reversal
	}
	
	enum HeightType {
		case low
		case middle
		case high
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
		case idle = 0
		case sit
		case sleep
		case crouch
		case run
		case stop
		case land
		case landHard
		case flip
		case walljump
		
		case animation_bit_count
	};
	
	struct AnimationBits: OptionSet {
		let rawValue: UInt
	
		init(rawValue: UInt) {
			self.rawValue = rawValue
		}
		static let Idle = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.idle.rawValue))
		static let Sit = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.sit.rawValue))
		static let Sleep = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.sleep.rawValue))
		static let Crouch = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.crouch.rawValue))
		static let Run = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.run.rawValue))
		static let Stop = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.stop.rawValue))
		static let Land = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.land.rawValue))
		static let LandHard = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.landHard.rawValue))
		static let Flip = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.flip.rawValue))
		static let Walljump = AnimationBits(rawValue: 1 << UInt(AnimationBitOffsets.walljump.rawValue))
	};
}
