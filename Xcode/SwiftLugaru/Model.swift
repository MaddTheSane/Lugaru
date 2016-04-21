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

class Model {
	private(set) var modelType = Type.Nothing
	private var oldType = Type.Nothing
	
	var vertexNum: Int16 = 0
	var TriangleNum: Int16 = 0
	var hastexture = false
	
	var possible = [Int32]()
	var owner = [Int32]()
	var vertex = [float3]()
	var normals = [float3]()
	var facenormals = [float3]()
	var Triangles = [TexturedTriangle]()
	var vArray = [GLfloat]()
	
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
	
	var decalTexCoords = [[[Float]]]()
	var decalVertex = [float3]()
	var decalType = [Int32]()
	var decalOpacity = [Float]()
	var decalRotation = [Float]()
	var decalAliveTime = [Float]()
	var decalPosition = [float3]()
	
	/*float decaltexcoords[max_model_decals][3][2];
	XYZ decalvertex[max_model_decals][3];
	int decaltype[max_model_decals];
	float decalopacity[max_model_decals];
	float decalrotation[max_model_decals];
	float decalalivetime[max_model_decals];
	XYZ decalposition[max_model_decals];*/
	
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
		var vertex: (Int16, Int16, Int16)
		var gx: (Float, Float, Float)
		var gy: (Float, Float, Float)
	}

	deinit {
		if textureptr != 0 {
			glDeleteTextures(1, &textureptr)
		}
	}
}
