//
//  Preferences.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/25/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation

struct Preferences {
	var screenWidth = 640
	var screenHeight = 640
	var mouseSensitivity: Float = 1
	var blur = false
	var overallDetail = OverallDetail.Medium
	var floatingJump = false
	var mouseJump = false
	var ambientSound = true
	var blood = Blood.Simple
	var autoSloMo = true
	var foliage = true
	var music = true
	var trilinearFiltering = true
	var decalsEnabled = true
	var invertMouse = false
	var gameSpeed: Float = 1
	var difficulty = Difficulty.Hard
	var damageEffects = true
	var text = true
	var debug = false
	var VBLSync = false
	var showPoints = false
	var alwaysBlur = false
	var immediateMode = false
	var velocityBlur = true
	var volume: Float = 0.8
	var forwardKey = "w"
	var backKey = "s"
	var leftKey = "a"
	var rightKey = "d"
	var jumpKey = "space"
	var crouchKey = "shift"
	var drawKey = "e"
	var throwKey = "q"
	var attackKey = "mouse1"
	var chatKey = "unknown"
	
	
	enum OverallDetail: Int {
		case Low = 0
		case Medium = 1
		case High = 2
	}
	
	enum Difficulty: Int {
		case Easy = 0
		case Medium = 1
		case Hard = 2
	}
	
	enum Blood: Int {
		case None = 0
		case Simple = 1
		case Complex = 2
	}
}

var preferences = Preferences()

