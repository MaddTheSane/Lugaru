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
	var screenHeight = 480
	var mouseSensitivity: Float = 1
	var blur = false
	var overallDetail = OverallDetail.medium
	var floatingJump = false
	var mouseJump = false
	var ambientSound = true
	var blood = Blood.lowDetail
	var autoSloMo = true
	var foliage = true
	var music = true
	var trilinearFiltering = true
	var decalsEnabled = true
	var invertMouse = false
	var gameSpeed: Float = 1
	var difficulty = Difficulty.hard
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
	
	
	enum OverallDetail: Int, CustomStringConvertible, CustomDebugStringConvertible {
		case low = 0
		case medium = 1
		case high = 2
		
		var description: String {
			switch self {
			case .low:
				return "Low"
			case .medium:
				return "Medium"
			case .high:
				return "High"
			}
		}
		
		var debugDescription: String {
			switch self {
			case .low:
				return "Overall Detail: Low"
			case .medium:
				return "Overall Detail: Medium"
			case .high:
				return "Overall Detail: High"
			}
		}
	}
	
	enum Difficulty: Int, CustomStringConvertible, CustomDebugStringConvertible {
		case easy = 0
		case medium = 1
		case hard = 2
		
		var description: String {
			switch self {
			case .easy:
				return "Easier"
			case .medium:
				return "Difficult"
			case .hard:
				return "Insane"
			}
		}
		
		var debugDescription: String {
			switch self {
			case .easy:
				return "Difficulty: Easier"
			case .medium:
				return "Difficulty: Difficult"
			case .hard:
				return "Difficulty: Insane"
			}
		}
	}
	
	enum Blood: Int, CustomStringConvertible, CustomDebugStringConvertible {
		case off = 0
		case lowDetail = 1
		case highDetail = 2
		
		var description: String {
			switch self {
			case .off:
				return "Off"
			case .lowDetail:
				return "On, low detail"
			case .highDetail:
				return "On, high detail (slower)"
			}
		}
		
		var debugDescription: String {
			switch self {
			case .off:
				return "Blood: Off"
			case .lowDetail:
				return "Blood: On, low detail"
			case .highDetail:
				return "Blood: On, high detail (slower)"
			}
		}
	}
}

var preferences = Preferences()

