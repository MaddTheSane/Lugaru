//
//  Skeleton.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/24/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation
import simd
import OpenGL.GL
import OpenGL.GL.GLU
import OpenGL.GL.Ext

enum AttackType {
	case neutral
	case normal
	case reversed
	case reversal
}

enum AttackHeight {
	case low
	case middle
	case high
}

final class Skeleton {
	struct Joint {
		
	}
	
	struct Muscle {
		var vertices = [Int32]()
		var verticesLow = [Int32]()
		var verticesClothes = [Int32]()
		var length = Float(0)
		var targetLength = Float(0)
		var parent1: UnsafeMutablePointer<Joint>? = nil
		var parent2: UnsafeMutablePointer<Joint>? = nil
		var maxLength = Float()
		var minLength = Float()
		var type: Int32 = 0
		var visible = false
		
		//float rotate1,rotate2,rotate3;
		//float lastrotate1,lastrotate2,lastrotate3;
		//float oldrotate1,oldrotate2,oldrotate3;
		//float newrotate1,newrotate2,newrotate3;
		
		var strength = Float(0)

		
		mutating func doConstraint(_ spinny: Bool) {
			
		}

	}
	
	struct Animation {
		
	}
}

