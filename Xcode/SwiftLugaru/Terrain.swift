//
//  Terrain.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/23/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation

let maxTerrainSize = 256
//#define curr_terrain_size			size
let subdivision = 64
let max_patch_elements = (maxTerrainSize/subdivision)*(maxTerrainSize/subdivision)*54

enum Environment {
	case snow //0
	case grass //1
	case desert //2
}
