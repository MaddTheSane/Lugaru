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
	var overallDetail = OverallDetail.Medium
	var floatingJump = false
	var mouseJump = false
	var ambientSound = true
	var blood = Blood.LowDetail
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
	
	
	enum OverallDetail: Int, CustomStringConvertible, CustomDebugStringConvertible {
		case Low = 0
		case Medium = 1
		case High = 2
		
		var description: String {
			switch self {
			case .Low:
				return "Low"
			case .Medium:
				return "Medium"
			case .High:
				return "High"
			}
		}
		
		var debugDescription: String {
			switch self {
			case .Low:
				return "Overall Detail: Low"
			case .Medium:
				return "Overall Detail: Medium"
			case .High:
				return "Overall Detail: High"
			}
		}
	}
	
	enum Difficulty: Int, CustomStringConvertible, CustomDebugStringConvertible {
		case Easy = 0
		case Medium = 1
		case Hard = 2
		
		var description: String {
			switch self {
			case .Easy:
				return "Easy"
			case .Medium:
				return "Medium"
			case .Hard:
				return "Hard"
			}
		}
		
		var debugDescription: String {
			switch self {
			case .Easy:
				return "Difficulty: Easy"
			case .Medium:
				return "Difficulty: Medium"
			case .Hard:
				return "Difficulty: Hard"
			}
		}
	}
	
	enum Blood: Int, CustomStringConvertible, CustomDebugStringConvertible {
		case Off = 0
		case LowDetail = 1
		case HighDetail = 2
		
		var description: String {
			switch self {
			case .Off:
				return "Off"
			case .LowDetail:
				return "On, low detail"
			case .HighDetail:
				return "On, high detail (slower)"
			}
		}
		
		var debugDescription: String {
			switch self {
			case .Off:
				return "Blood: Off"
			case .LowDetail:
				return "Blood: On, low detail"
			case .HighDetail:
				return "Blood: On, high detail (slower)"
			}
		}
	}
}

var preferences = Preferences()

