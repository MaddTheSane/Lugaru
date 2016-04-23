//
//  Model.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/21/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation
import simd
import OpenGL.GL

//MARK: - preferences
var decalsEnabled = true

//MARK: -
let max_model_decals = 300

class Model {
	private(set) var modelType = Type.Nothing
	private var oldType = Type.Nothing
	
	var vertexNum: Int16 = 0
	var TriangleNum: Int16 = 0
	var hastexture = false
	
	var possible = [Int32](count: maxModelVertex, repeatedValue: 0)
	var owner = [Int32](count: maxTexturedTriangle, repeatedValue: 0)
	var vertex = [float3](count: maxModelVertex, repeatedValue: float3(0))
	var normals = [float3](count: maxModelVertex, repeatedValue: float3(0))
	var facenormals = [float3](count: maxTexturedTriangle, repeatedValue: float3(0))
	var Triangles = [TexturedTriangle](count: maxTexturedTriangle, repeatedValue: TexturedTriangle())
	var vArray = [GLfloat](count: maxTexturedTriangle * 24, repeatedValue: 0)
	
	/*int possible[max_model_vertex];
	int owner[max_textured_triangle];
	XYZ vertex[max_model_vertex];
	XYZ normals[max_model_vertex];
	XYZ facenormals[max_textured_triangle];
	TexturedTriangle Triangles[max_textured_triangle];
	GLfloat vArray[max_textured_triangle*24];*/
	
	private(set) var textureptr: GLuint = 0
	var texture = Texture()
	var numPossible: Int = 0
	var color = false
	
	var boundingspherecenter = float3()
	var boundingsphereradius = Float(0)
	
	enum DecalType: Int {
		case Shadow = 0
		case Footprint
		case Blood
		case BloodFast
		case PermanentShadow
		case Break
		case BloodSlow
		case Bodyprint
	}
	
	struct Decal {
		var textureCoordinates = [[Float]](count: 3, repeatedValue: [Float](count: 2, repeatedValue: 0))
		var decals = [float3](count: 3, repeatedValue: float3(0))
		var type: Int32 = 0
		var opacity: Float = 0
		var rotation: Float = 0
		var aliveTime: Float = 0
		var decalPosition = float3(0)
	}
	var decals = [Decal]()
	
	func removeDecal(which: Int) {
		if decalsEnabled {
			if modelType != .Decals {
				return
			}
			decals.removeAtIndex(which)
		}
	}
	
	func makeDecal() {
		
	}
	
	var numDecals = 0

	var flat = false

	
	enum Type {
		case Nothing
		case NoTexture
		case Raw
		case Decals
		case Normal
	}

	/// Textures List
	struct Texture {
		var xsz: Int = 0
		var ysz: Int = 0
		var txt: UnsafeMutablePointer<GLubyte> = nil
	}
	
	struct TexturedTriangle {
		var vertex: (Int16, Int16, Int16) = (0,0,0)
		var gx: (Float, Float, Float) = (0,0,0)
		var gy: (Float, Float, Float) = (0,0,0)
	}

	deinit {
		if textureptr != 0 {
			glDeleteTextures(1, &textureptr)
		}
	}
}
