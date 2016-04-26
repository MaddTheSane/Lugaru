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
import OpenGL.GL.GLU
import OpenGL.GL.Ext

let max_model_decals = 300

final class Model {
	private(set) var modelType = Type.Nothing
	private var oldType = Type.Nothing
	
	private(set) var vertexNum: Int16 = 0
	var hastexture = false
	
	private var possible = [Int32]() // maxModelVertex
	private var owner = [Int32]() // maxTexturedTriangle
	private var vertex = [float3]() // maxModelVertex
	private var normals = [float3]() // maxModelVertex
	private var faceNormals = [float3]() //maxTexturedTriangle
	private var triangles = [TexturedTriangle]() //maxTexturedTriangle
	private var vArray = [GLfloat]() //maxTexturedTriangle * 24
	
	private(set) var texturePtr: GLuint = 0
	private(set) var texture = Texture()
	private(set) var numPossible: Int = 0
	private(set) var color = false
	
	private(set) var boundingSphereCenter = float3()
	private(set) var boundingSphereRadius = Float(0)
	
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
	
	private struct Decal {
		var textureCoordinates = [[Float]](count: 3, repeatedValue: [Float](count: 2, repeatedValue: 0))
		var vertex = [float3](count: 3, repeatedValue: float3(0))
		var type = DecalType.Shadow
		var opacity: Float = 0
		var rotation: Float = 0
		var aliveTime: Float = 0
		var position = float3(0)
	}
	private var decals = [Decal]()
	
	private(set) var flat = false

	
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
		if texturePtr != 0 {
			glDeleteTextures(1, &texturePtr)
		}
	}
	
	func lineCheck(inout p1: float3, inout _ p2: float3, inout _ p: float3, move: float3, rotate: Float = 0) -> Int {
		var point = float3()
		var oldDistance = Float(0)
		
		p1=p1-move;
		p2=p2-move;
		if(rotate != 0) {
			p1=SwiftLugaru.rotate(p1,byAngles: (0,-rotate,0))
		}
		if(rotate != 0) {
			p2=SwiftLugaru.rotate(p2,byAngles: (0,-rotate,0))
		}
		guard sphereLineIntersection(p1,p2,center: boundingSphereCenter, radius: boundingSphereRadius) else {
			return -1;
		}
		var firstintersecting = -1;
		
		for (j, triangle) in triangles.enumerate() {
			let intersecting = lineFacetd(p1, p2, vertex[Int(triangle.vertex.0)], vertex[Int(triangle.vertex.1)],vertex[Int(triangle.vertex.2)], faceNormals[j],p: &point);
			let distance = distance_squared(point, p1);
			if ((distance < oldDistance || firstintersecting == -1) && (intersecting != 0)) {
				oldDistance=distance;
				firstintersecting=j;
				p=point;
			}
		}
		
		if rotate != 0 {
			p=SwiftLugaru.rotate(p,byAngles: (0,rotate,0));
		}
		p += move;
		return firstintersecting;
	}
	
	func lineCheckSlide(inout p1: float3, inout _ p2: float3, inout _ p: float3, move: float3, rotate: Float = 0) -> Int {
		var point = float3()
		var olddistance = Float(0)
		
		var p1 = p1 - move;
		var p2 = p2 - move;
		if(!sphereLineIntersection(p1,p2,center: boundingSphereCenter, radius: boundingSphereRadius)) {
			return -1;
		}
		var firstintersecting = -1;
		if rotate != 0 {
			p1 = SwiftLugaru.rotate(p1,byAngles: (0,-rotate,0));
			p2 = SwiftLugaru.rotate(p2,byAngles: (0,-rotate,0));
			
		}
		
		for (j, triangle) in triangles.enumerate() {
			let intersecting = lineFacetd(p1, p2, vertex[Int(triangle.vertex.0)], vertex[Int(triangle.vertex.1)], vertex[Int(triangle.vertex.2)], faceNormals[j], p: &point);
			let distance = distance_squared(point, p1);
			if((distance<olddistance||firstintersecting == -1) && (intersecting != 0)){
				olddistance=distance;
				firstintersecting=j;
			}
		}
		
		let distance: Float = {
			let part1 = (faceNormals[firstintersecting].x*p2.x)+(faceNormals[firstintersecting].y*p2.y)+(faceNormals[firstintersecting].z*p2.z)
			let part2 = (faceNormals[firstintersecting].x*vertex[Int(triangles[firstintersecting].vertex.0)].x)+(faceNormals[firstintersecting].y*vertex[Int(triangles[firstintersecting].vertex.0)].y)+(faceNormals[firstintersecting].z*vertex[Int(triangles[firstintersecting].vertex.0)].z)
			return part1 - part2
		}()
		p2 -= faceNormals[firstintersecting] * distance;
		
		if rotate != 0 {
			p2=SwiftLugaru.rotate(p2, byAngles: (0,rotate,0))
		}
		p2 += move;
		return firstintersecting;
	}
	
	func lineCheckPossible(inout p1: float3, inout _ p2: float3, inout _ p: float3, move: float3, rotate: Float = 0) -> Int {
		var olddistance = Float(0)
		var firstintersecting = -1;
		var point = float3()
		
		p1=p1-move;
		p2=p2-move;
		if(!sphereLineIntersection(p1,p2, center: boundingSphereCenter, radius: boundingSphereRadius)) {
			return -1;
		}
		
		if rotate != 0 {
			p1=SwiftLugaru.rotate(p1,byAngles: (0,-rotate,0));
			p2=SwiftLugaru.rotate(p2,byAngles: (0,-rotate,0));
		}
		
		if(numPossible > 0 && numPossible < triangles.count) {
			for poss in possible {
				if(poss >= 0 && Int(poss) < triangles.count){
					let intersecting = lineFacetd(p1, p2, vertex[Int(triangles[Int(poss)].vertex.0)], vertex[Int(triangles[Int(poss)].vertex.1)], vertex[Int(triangles[Int(poss)].vertex.2)], faceNormals[Int(poss)], p: &point);
					let distance = distance_squared(point, p1);
					if((distance<olddistance||firstintersecting == -1) && intersecting != 0) {
						olddistance=distance;
						firstintersecting=Int(poss)
						p=point;
					}
				}
			}
		}
		
		if rotate != 0 {
			p=SwiftLugaru.rotate(p, byAngles: (0,rotate,0));
		}
		p += move;
		return firstintersecting;
	}
	
	func lineCheckSlidePossible(inout p1: float3, inout _ p2: float3, inout _ p: float3, move: float3, rotate: Float = 0) -> Int {
		var olddistance = Float(0)
		var point = float3()
		
		p1=p1-move;
		p2=p2-move;
		guard sphereLineIntersection(p1, p2, center: boundingSphereCenter,radius: boundingSphereRadius) else {
			return -1;
		}
		var firstintersecting = -1;
		if rotate != 0 {
			p1=SwiftLugaru.rotate(p1,byAngles: (0,-rotate,0));
			p2=SwiftLugaru.rotate(p2,byAngles: (0,-rotate,0));
		}
		
		if(numPossible != 0) {
			for poss in possible {
				if poss >= 0 && Int(poss) < triangles.count {
					let intersecting = lineFacetd(p1, p2, vertex[Int(triangles[Int(poss)].vertex.0)], vertex[Int(triangles[Int(poss)].vertex.1)], vertex[Int(triangles[Int(poss)].vertex.2)], faceNormals[Int(poss)], p: &point);
					let distance = distance_squared(point, p1);
					if((distance<olddistance||firstintersecting == -1) && intersecting != 0) {
						olddistance=distance;
						firstintersecting=Int(poss);
					}
				}
			}
		}
		
		if(firstintersecting>0){
			let distance: Float = {
				let part1 = (faceNormals[firstintersecting].x*p2.x)+(faceNormals[firstintersecting].y*p2.y)+(faceNormals[firstintersecting].z*p2.z)
				let part2 = (faceNormals[firstintersecting].x*vertex[Int(triangles[firstintersecting].vertex.0)].x)+(faceNormals[firstintersecting].y*vertex[Int(triangles[firstintersecting].vertex.0)].y)+(faceNormals[firstintersecting].z*vertex[Int(triangles[firstintersecting].vertex.0)].z)
				return abs(part1 - part2)
			}()
			p2 -= faceNormals[firstintersecting] * distance;
		}
		
		if rotate != 0 {
			p2=SwiftLugaru.rotate(p2,byAngles: (0,rotate,0));
		}
		p2 += move;
		return firstintersecting;
	}
	
	func sphereCheck(inout p1: float3, radius: Float, inout _ p: float3, move: float3, rotate: Float = 0) -> Int {
		var olddistance = Float(0)
		var point = float3()
		var firstintersecting = -1;
		
		//XYZ oldp1=p1;
		p1=p1-move;
		if rotate != 0 {
			p1=SwiftLugaru.rotate(p1,byAngles: (0,-rotate,0));
		}
		if(findDistancefast(p1, boundingSphereCenter) > radius*radius+boundingSphereRadius*boundingSphereRadius) {
			return -1;
		}
		
		for _ in 0..<4 {
			for (j, triangle) in triangles.enumerate() {
				var intersecting = false
				let distance: Float = {
					let stage1 = (faceNormals[j].x*p1.x)+(faceNormals[j].y*p1.y)+(faceNormals[j].z*p1.z)
					let stage2 = (faceNormals[j].x*vertex[Int(triangle.vertex.0)].x)+(faceNormals[j].y*vertex[Int(triangle.vertex.0)].y)+(faceNormals[j].z*vertex[Int(triangle.vertex.0)].z)
					return abs(stage1 - stage2)
				}()
				if distance < radius {
					point = p1 - faceNormals[j] * distance;
					if pointInTriangle(point, normal: faceNormals[j], vertex[Int(triangle.vertex.0)], vertex[Int(triangle.vertex.1)], vertex[Int(triangle.vertex.2)]) {
						intersecting = true
					}
					if !intersecting {
						intersecting = sphereLineIntersection(vertex[Int(triangle.vertex.0)],
						                                    vertex[Int(triangle.vertex.1)],
						                                    center: p1, radius: radius);
					}
					if !intersecting {
						intersecting = sphereLineIntersection(vertex[Int(triangle.vertex.1)],
						                                    vertex[Int(triangle.vertex.2)],
						                                    center: p1, radius: radius);
					}
					if !intersecting {
						intersecting = sphereLineIntersection(vertex[Int(triangle.vertex.0)],
						                                    vertex[Int(triangle.vertex.2)],
						                                    center: p1, radius: radius);
					}
					if intersecting {
						p1 += faceNormals[j] * (distance - radius)
						/*start=*p1;
						end=*p1;
						end.y-=radius;
						if(LineFacetd(&start,&end,&vertex[Triangles[j].vertex[0]],&vertex[Triangles[j].vertex[1]],&vertex[Triangles[j].vertex[2]],&faceNormals[j],&point)){
						p1->y=point.y+radius;
						}*/
					}
				}
				if((distance<olddistance||firstintersecting == -1)&&intersecting) {
					olddistance = distance;
					firstintersecting = j;
					p = point;
				}
			}
		}
		if rotate != 0 {
			p=SwiftLugaru.rotate(p, byAngles: (0,rotate,0))
			p1=SwiftLugaru.rotate(p1, byAngles: (0,rotate,0))
		}
		p += move;
		p1 += move;
		return firstintersecting;
	}
	
	func load(fileURL: NSURL, texture textured: Bool) /*throws*/ {
		print("Loading model " + fileURL.path!);
		
		//if(visibleloading){
		//loadscreencolor=2;
		//pgame->LoadingScreen();
		//}
		
		//int oldvertexNum,oldTriangleNum;
		//oldvertexNum=vertexNum;
		//oldTriangleNum=TriangleNum;
		if textured {
			modelType = .Normal;
		} else {
			modelType = .NoTexture
		}
		color = false;
		
		let tfile=fopen( ConvertFileName(fileURL.fileSystemRepresentation), "rb" );
		// read model settings
		var triangleNum: Int16 = 0
		
		fseek(tfile, 0, SEEK_SET);
		do {
			var vNum: Int16 = 0
			var vaListArr = [CVarArgType]()
			vaListArr.append(withUnsafeMutablePointer(&vNum, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&triangleNum, {$0}))
			vfunpackf(tfile, "Bs Bs", getVaList(vaListArr))
			vertexNum = vNum
		}
		//funpackf(tfile, "Bs Bs", &vertexNum, &triangleNum);
		
		// read the model data
		/*if(owner)dealloc(owner);
		if(possible)dealloc(possible);
		if(vertex)dealloc(vertex);
		if(normals)dealloc(normals);
		if(facenormals)dealloc(facenormals);
		if(Triangles)dealloc(Triangles);
		if(vArray)dealloc(vArray);*/
		//deallocate();
		
		numPossible = 0;
		
		owner = [Int32](count: Int(vertexNum), repeatedValue: -1)
		possible = [Int32](count: Int(triangleNum), repeatedValue: 0)
		vertex.removeAll()
		vertex.reserveCapacity(Int(vertexNum))
		normals = [float3](count: Int(vertexNum), repeatedValue: float3(0))
		faceNormals = [float3](count: Int(triangleNum), repeatedValue: float3(0))
		triangles.removeAll()
		triangles.reserveCapacity(Int(triangleNum))
		vArray = [GLfloat](count: Int(triangleNum) * 24, repeatedValue: 0)
		
		for _ in 0..<vertexNum {
			var vaListArr = [CVarArgType]()
			var tmpx = Float()
			var tmpy = Float()
			var tmpz = Float()
			vaListArr.append(withUnsafeMutablePointer(&tmpx, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&tmpy, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&tmpz, {$0}))
			vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr))
			vaListArr.removeAll(keepCapacity: true)
			vertex.append(float3(tmpx, tmpy, tmpz))
		}
		
		for _ in 0..<triangleNum {
			var triangle = TexturedTriangle()
			// funpackf(tfile, "Bi Bi Bi", &Triangles[i].vertex[0], &Triangles[i].vertex[1], &Triangles[i].vertex[2]);
			var vertex1: Int16 = 0
			var vertex2: Int16 = 0
			var vertex3: Int16 = 0
			var vertex4: Int16 = 0
			var vertex5: Int16 = 0
			var vertex6: Int16 = 0
			var vaListArr = [CVarArgType]()
			vaListArr.append(withUnsafeMutablePointer(&vertex1, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex2, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex3, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex4, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex5, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex6, {$0}))
			vfunpackf(tfile, "Bs Bs Bs Bs Bs Bs", getVaList(vaListArr))
			triangle.vertex.0 = vertex1
			triangle.vertex.1 = vertex3
			triangle.vertex.2 = vertex5
			do {
				var float1: Float = 0
				var float2: Float = 0
				var float3: Float = 0
				var vaListArr2 = [CVarArgType]()
				
				vaListArr2.append(withUnsafeMutablePointer(&float1, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float2, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float3, {$0}))
				vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr2))
				triangle.gx.0 = float1
				triangle.gx.1 = float2
				triangle.gx.2 = float3
			}
			do {
				var float1: Float = 0
				var float2: Float = 0
				var float3: Float = 0
				
				var vaListArr2 = [CVarArgType]()
				vaListArr2.append(withUnsafeMutablePointer(&float1, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float2, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float3, {$0}))
				vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr2))
				triangle.gy.0 = float1
				triangle.gy.1 = float2
				triangle.gy.2 = float3
			}
			triangles.append(triangle)
		}
		
		texture.xsz = 0;
		
		fclose(tfile);
		
		updateVertexArray();
		updateBoundingSphere()
	}
	
	func loadDecal(fileURL: NSURL, texture textured: Bool) {
		// Changing the filename so that its more os specific
		let FixedFN = ConvertFileName(fileURL.fileSystemRepresentation);
		
		print("Loading decal... " + String.fromCString(FixedFN)!);
		
		//int oldvertexNum,oldTriangleNum;
		//oldvertexNum=vertexNum;
		//oldTriangleNum=TriangleNum;
		
		modelType = .Decals;
		color=false;
		
		let tfile=fopen( FixedFN, "rb" );
		// read model settings
		
		var triangleNum = Int16()
		fseek(tfile, 0, SEEK_SET);
		do {
			var vNum = Int16()
			var vaListArr = [CVarArgType]()
			vaListArr.append(withUnsafeMutablePointer(&vNum, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&triangleNum, {$0}))
			vfunpackf(tfile, "Bs Bs", getVaList(vaListArr))
			vertexNum = vNum
		}
		
		// read the model data
		
		/*if(owner)dealloc(owner);
		if(possible)dealloc(possible);
		if(vertex)dealloc(vertex);
		if(normals)dealloc(normals);
		if(facenormals)dealloc(facenormals);
		if(Triangles)dealloc(Triangles);
		if(vArray)dealloc(vArray);*/
		//deallocate();
		
		numPossible=0;
		
		owner = [Int32](count: Int(vertexNum), repeatedValue: -1)
		possible = [Int32](count: Int(triangleNum), repeatedValue: 0)
		vertex.removeAll()
		vertex.reserveCapacity(Int(vertexNum))
		normals = [float3](count: Int(vertexNum), repeatedValue: float3(0))
		faceNormals = [float3](count: Int(triangleNum), repeatedValue: float3(0))
		triangles.removeAll()
		triangles.reserveCapacity(Int(triangleNum))
		vArray = [GLfloat](count: Int(triangleNum) * 24, repeatedValue: 0)
		
		
		for _ in 0..<vertexNum {
			var tmpx = Float()
			var tmpy = Float()
			var tmpz = Float()
			var vaListArr = [CVarArgType]()
			vaListArr.append(withUnsafeMutablePointer(&tmpx, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&tmpy, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&tmpz, {$0}))
			vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr))
			vertex.append(float3(tmpx, tmpy, tmpz))
		}
		
		for _ in 0..<triangleNum {
			var triangle = TexturedTriangle()
			// funpackf(tfile, "Bi Bi Bi", &Triangles[i].vertex[0], &Triangles[i].vertex[1], &Triangles[i].vertex[2]);
			var vertex1: Int16 = 0
			var vertex2: Int16 = 0
			var vertex3: Int16 = 0
			var vertex4: Int16 = 0
			var vertex5: Int16 = 0
			var vertex6: Int16 = 0
			var vaListArr = [CVarArgType]()
			vaListArr.append(withUnsafeMutablePointer(&vertex1, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex2, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex3, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex4, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex5, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex6, {$0}))
			vfunpackf(tfile, "Bs Bs Bs Bs Bs Bs", getVaList(vaListArr))
			triangle.vertex.0 = vertex1
			triangle.vertex.1 = vertex3
			triangle.vertex.2 = vertex5
			do {
				var float1: Float = 0
				var float2: Float = 0
				var float3: Float = 0
				var vaListArr2 = [CVarArgType]()
				
				vaListArr2.append(withUnsafeMutablePointer(&float1, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float2, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float3, {$0}))
				vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr2))
				triangle.gx.0 = float1
				triangle.gx.1 = float2
				triangle.gx.2 = float3
			}
			do {
				var float1: Float = 0
				var float2: Float = 0
				var float3: Float = 0
				
				var vaListArr2 = [CVarArgType]()
				vaListArr2.append(withUnsafeMutablePointer(&float1, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float2, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float3, {$0}))
				vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr2))
				triangle.gy.0 = float1
				triangle.gy.1 = float2
				triangle.gy.2 = float3
			}
			triangles.append(triangle)
		}
		
		texture.xsz = 0;
		
		fclose(tfile);
		
		updateVertexArray();
		updateBoundingSphere()
		
		//allow decals
		// We use slightly better struct-base storage of decals
		/*
		if(!decaltexcoords){
		decaltexcoords = (float***)malloc(sizeof(float**)*max_model_decals);
		for(i=0;i<max_model_decals;i++){
		decaltexcoords[i] = (float**)malloc(sizeof(float*)*3);
		for(j=0;j<3;j++){
		decaltexcoords[i][j] = (float*)malloc(sizeof(float)*2);
		}
		}
		//if(decalvertex)free(decalvertex);
		decalvertex = (XYZ**)malloc(sizeof(XYZ*)*max_model_decals);
		for(i=0;i<max_model_decals;i++){
		decalvertex[i] = (XYZ*)malloc(sizeof(XYZ)*3);
		}
		
		decaltype = (int*)malloc(sizeof(int)*max_model_decals);
		decalopacity = (float*)malloc(sizeof(float)*max_model_decals);
		decalrotation = (float*)malloc(sizeof(float)*max_model_decals);
		decalalivetime = (float*)malloc(sizeof(float)*max_model_decals);
		decalposition = (XYZ*)malloc(sizeof(XYZ)*max_model_decals);
		}*/
		
		//return 1;
	}
	
	func loadRaw(fileURL: NSURL) {
		//LOGFUNC;
		
		print("Loading raw... " + fileURL.path!);
		
		//int oldvertexNum,oldTriangleNum;
		//oldvertexNum=vertexNum;
		//oldTriangleNum=TriangleNum;
		
		modelType = .Raw;
		color = false;
		
		let tfile = fopen( ConvertFileName(fileURL.fileSystemRepresentation), "rb" );
		// read model settings
		
		var triangleNum = Int16()
		var vNum = Int16()
		fseek(tfile, 0, SEEK_SET);
		do {
			var vaListArr = [CVarArgType]()
			vaListArr.append(withUnsafeMutablePointer(&vNum, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&triangleNum, {$0}))
			vfunpackf(tfile, "Bs Bs", getVaList(vaListArr))
			vertexNum = vNum
		}
		
		// read the model data
		/*if(owner)dealloc(owner);
		if(possible)dealloc(possible);
		if(vertex)dealloc(vertex);
		if(normals)dealloc(normals);
		if(facenormals)dealloc(facenormals);
		if(Triangles)dealloc(Triangles);
		if(vArray)dealloc(vArray);*/
		//deallocate();
		
		numPossible=0;
		
		owner = [Int32](count: Int(vertexNum), repeatedValue: -1)
		possible = [Int32](count: Int(triangleNum), repeatedValue: 0)
		vertex.removeAll()
		vertex.reserveCapacity(Int(vertexNum))
		triangles.removeAll()
		triangles.reserveCapacity(Int(triangleNum))
		vArray = [GLfloat](count: Int(triangleNum) * 24, repeatedValue: 0)
		
		for _ in 0..<vertexNum {
			var tmpx = Float()
			var tmpy = Float()
			var tmpz = Float()
			var vaListArr = [CVarArgType]()
			vaListArr.append(withUnsafeMutablePointer(&tmpx, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&tmpy, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&tmpz, {$0}))
			vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr))
			vertex.append(float3(tmpx, tmpy, tmpz))
		}
		
		for _ in 0..<triangleNum {
			var triangle = TexturedTriangle()
			// funpackf(tfile, "Bi Bi Bi", &Triangles[i].vertex[0], &Triangles[i].vertex[1], &Triangles[i].vertex[2]);
			var vertex1: Int16 = 0
			var vertex2: Int16 = 0
			var vertex3: Int16 = 0
			var vertex4: Int16 = 0
			var vertex5: Int16 = 0
			var vertex6: Int16 = 0
			var vaListArr = [CVarArgType]()
			vaListArr.append(withUnsafeMutablePointer(&vertex1, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex2, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex3, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex4, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex5, {$0}))
			vaListArr.append(withUnsafeMutablePointer(&vertex6, {$0}))
			vfunpackf(tfile, "Bs Bs Bs Bs Bs Bs", getVaList(vaListArr))
			triangle.vertex.0 = vertex1
			triangle.vertex.1 = vertex3
			triangle.vertex.2 = vertex5
			do {
				var float1: Float = 0
				var float2: Float = 0
				var float3: Float = 0
				var vaListArr2 = [CVarArgType]()
				
				vaListArr2.append(withUnsafeMutablePointer(&float1, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float2, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float3, {$0}))
				vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr2))
				triangle.gx.0 = float1
				triangle.gx.1 = float2
				triangle.gx.2 = float3
			}
			do {
				var float1: Float = 0
				var float2: Float = 0
				var float3: Float = 0
				
				var vaListArr2 = [CVarArgType]()
				vaListArr2.append(withUnsafeMutablePointer(&float1, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float2, {$0}))
				vaListArr2.append(withUnsafeMutablePointer(&float3, {$0}))
				vfunpackf(tfile, "Bf Bf Bf", getVaList(vaListArr2))
				triangle.gy.0 = float1
				triangle.gy.1 = float2
				triangle.gy.2 = float3
			}
			triangles.append(triangle)
		}
		
		fclose(tfile);
		
		//return 1;
	}

	
	func updateVertexArray() {
		guard modelType == .Normal || modelType == .Decals else {
			return;
		}
		
		if !flat {
			for (i,triangle) in triangles.enumerate() {
				let j = i*24;
				vArray[j+0]=triangle.gx.0;
				vArray[j+1]=triangle.gy.0;
				vArray[j+2]=normals[Int(triangle.vertex.0)].x;
				vArray[j+3]=normals[Int(triangle.vertex.0)].y;
				vArray[j+4]=normals[Int(triangle.vertex.0)].z;
				vArray[j+5]=vertex[Int(triangle.vertex.0)].x;
				vArray[j+6]=vertex[Int(triangle.vertex.0)].y;
				vArray[j+7]=vertex[Int(triangle.vertex.0)].z;
				
				vArray[j+8]=triangle.gx.1;
				vArray[j+9]=triangle.gy.1;
				vArray[j+10]=normals[Int(triangle.vertex.1)].x;
				vArray[j+11]=normals[Int(triangle.vertex.1)].y;
				vArray[j+12]=normals[Int(triangle.vertex.1)].z;
				vArray[j+13]=vertex[Int(triangle.vertex.1)].x;
				vArray[j+14]=vertex[Int(triangle.vertex.1)].y;
				vArray[j+15]=vertex[Int(triangle.vertex.1)].z;
				
				vArray[j+16]=triangle.gx.2;
				vArray[j+17]=triangle.gy.2;
				vArray[j+18]=normals[Int(triangle.vertex.2)].x;
				vArray[j+19]=normals[Int(triangle.vertex.2)].y;
				vArray[j+20]=normals[Int(triangle.vertex.2)].z;
				vArray[j+21]=vertex[Int(triangle.vertex.2)].x;
				vArray[j+22]=vertex[Int(triangle.vertex.2)].y;
				vArray[j+23]=vertex[Int(triangle.vertex.2)].z;
			}
		} else {
			for (i,triangle) in triangles.enumerate() {
				let j = i*24;
				vArray[j+0]=triangle.gx.0;
				vArray[j+1]=triangle.gy.0;
				vArray[j+2]=faceNormals[i].x * -1;
				vArray[j+3]=faceNormals[i].y * -1;
				vArray[j+4]=faceNormals[i].z * -1;
				vArray[j+5]=vertex[Int(triangle.vertex.0)].x;
				vArray[j+6]=vertex[Int(triangle.vertex.0)].y;
				vArray[j+7]=vertex[Int(triangle.vertex.0)].z;
				
				vArray[j+8]=triangle.gx.1;
				vArray[j+9]=triangle.gy.1;
				vArray[j+10]=faceNormals[i].x * -1;
				vArray[j+11]=faceNormals[i].y * -1;
				vArray[j+12]=faceNormals[i].z * -1;
				vArray[j+13]=vertex[Int(triangle.vertex.1)].x;
				vArray[j+14]=vertex[Int(triangle.vertex.1)].y;
				vArray[j+15]=vertex[Int(triangle.vertex.1)].z;
				
				vArray[j+16]=triangle.gx.2;
				vArray[j+17]=triangle.gy.2;
				vArray[j+18]=faceNormals[i].x * -1;
				vArray[j+19]=faceNormals[i].y * -1;
				vArray[j+20]=faceNormals[i].z * -1;
				vArray[j+21]=vertex[Int(triangle.vertex.2)].x;
				vArray[j+22]=vertex[Int(triangle.vertex.2)].y;
				vArray[j+23]=vertex[Int(triangle.vertex.2)].z;
				
			}
		}
	}
	
	func updateVertexArrayNoTexture() {
		guard modelType == .Normal || modelType == .Decals else {
			return;
		}
		if !flat {
			for (i,triangle) in triangles.enumerate() {
				let j = i*24;
				vArray[j+2]=normals[Int(triangle.vertex.0)].x;
				vArray[j+3]=normals[Int(triangle.vertex.0)].y;
				vArray[j+4]=normals[Int(triangle.vertex.0)].z;
				vArray[j+5]=vertex[Int(triangle.vertex.0)].x;
				vArray[j+6]=vertex[Int(triangle.vertex.0)].y;
				vArray[j+7]=vertex[Int(triangle.vertex.0)].z;
				
				vArray[j+10]=normals[Int(triangle.vertex.1)].x;
				vArray[j+11]=normals[Int(triangle.vertex.1)].y;
				vArray[j+12]=normals[Int(triangle.vertex.1)].z;
				vArray[j+13]=vertex[Int(triangle.vertex.1)].x;
				vArray[j+14]=vertex[Int(triangle.vertex.1)].y;
				vArray[j+15]=vertex[Int(triangle.vertex.1)].z;
				
				vArray[j+18]=normals[Int(triangle.vertex.2)].x;
				vArray[j+19]=normals[Int(triangle.vertex.2)].y;
				vArray[j+20]=normals[Int(triangle.vertex.2)].z;
				vArray[j+21]=vertex[Int(triangle.vertex.2)].x;
				vArray[j+22]=vertex[Int(triangle.vertex.2)].y;
				vArray[j+23]=vertex[Int(triangle.vertex.2)].z;
			}
		} else {
			for (i,triangle) in triangles.enumerate() {
				let j = i*24;
				vArray[j+2]=faceNormals[i].x * -1;
				vArray[j+3]=faceNormals[i].y * -1;
				vArray[j+4]=faceNormals[i].z * -1;
				vArray[j+5]=vertex[Int(triangle.vertex.0)].x;
				vArray[j+6]=vertex[Int(triangle.vertex.0)].y;
				vArray[j+7]=vertex[Int(triangle.vertex.0)].z;
				
				vArray[j+10]=faceNormals[i].x * -1;
				vArray[j+11]=faceNormals[i].y * -1;
				vArray[j+12]=faceNormals[i].z * -1;
				vArray[j+13]=vertex[Int(triangle.vertex.1)].x;
				vArray[j+14]=vertex[Int(triangle.vertex.1)].y;
				vArray[j+15]=vertex[Int(triangle.vertex.1)].z;
				
				vArray[j+18]=faceNormals[i].x * -1;
				vArray[j+19]=faceNormals[i].y * -1;
				vArray[j+20]=faceNormals[i].z * -1;
				vArray[j+21]=vertex[Int(triangle.vertex.2)].x;
				vArray[j+22]=vertex[Int(triangle.vertex.2)].y;
				vArray[j+23]=vertex[Int(triangle.vertex.2)].z;
			}
		}
	}
	
	func updateVertexArrayNoTexNoNorm() {
		guard modelType == .Normal || modelType == .Decals else {
			return;
		}
		for (i, triangle) in triangles.enumerate() {
			let j = i*24;
			vArray[j+5]=vertex[Int(triangle.vertex.0)].x;
			vArray[j+6]=vertex[Int(triangle.vertex.0)].y;
			vArray[j+7]=vertex[Int(triangle.vertex.0)].z;
			
			vArray[j+13]=vertex[Int(triangle.vertex.1)].x;
			vArray[j+14]=vertex[Int(triangle.vertex.1)].y;
			vArray[j+15]=vertex[Int(triangle.vertex.1)].z;
			
			vArray[j+21]=vertex[Int(triangle.vertex.2)].x;
			vArray[j+22]=vertex[Int(triangle.vertex.2)].y;
			vArray[j+23]=vertex[Int(triangle.vertex.2)].z;
		}
	}
	
	func uniformTextureCoords() {
		for var triangle in triangles {
			triangle.gy.0 = vertex[Int(triangle.vertex.0)].y;
			triangle.gy.1 = vertex[Int(triangle.vertex.1)].y;
			triangle.gy.2 = vertex[Int(triangle.vertex.2)].y;
			triangle.gx.0 = vertex[Int(triangle.vertex.0)].x;
			triangle.gx.1 = vertex[Int(triangle.vertex.1)].x;
			triangle.gx.2 = vertex[Int(triangle.vertex.2)].x;
		}
		updateVertexArray();
	}
	
	// MARK: - scale, rotate, translate
	func flipTextureCoords() {
		for var triangle in triangles {
			triangle.gy.0 = -triangle.gy.0;
			triangle.gy.1 = -triangle.gy.1;
			triangle.gy.2 = -triangle.gy.2;
		}
		
		updateVertexArray();
	}
	
	func scaleTextureCoords(howMuch: Float) {
		for var triangle in triangles {
			triangle.gx.0 *= howMuch;
			triangle.gx.1 *= howMuch;
			triangle.gx.2 *= howMuch;
			triangle.gy.0 *= howMuch;
			triangle.gy.1 *= howMuch;
			triangle.gy.2 *= howMuch;
		}
		
		updateVertexArray();
	}
	
	private func updateBoundingSphere() {
		boundingSphereRadius=0;
		for i in 0..<Int(vertexNum) {
			for j in 0..<Int(vertexNum) {
				if j != i && findDistancefast(vertex[j],vertex[i]) / 2 > boundingSphereRadius {
					boundingSphereRadius = findDistancefast(vertex[j], vertex[i]) / 2;
					boundingSphereCenter = (vertex[i] + vertex[j]) / float3(2);
				}
			}
		}
		boundingSphereRadius = sqrt(boundingSphereRadius);
	}

	func scale(x xscale: Float,y yscale: Float,z zscale: Float) {
		scale(float3(xscale, yscale, zscale))
	}
	
	func scale(amount: float3) {
		for var vert in vertex {
			vert *= amount
		}
		updateVertexArray();
		updateBoundingSphere()
	}
	
	func scaleNormals(x xscale: Float, y yscale: Float, z zscale: Float) {
		scaleNormals(float3(xscale, yscale, zscale))
	}
	
	func scaleNormals(amount: float3) {
		guard modelType == .Normal || modelType == .Decals else {
			return
		}
		for var normal in normals {
			normal *= amount
		}

		for var faceNormal in faceNormals {
			faceNormal *= amount
		}

		updateVertexArray();
	}
	
	func translate(x xscale: Float, y yscale: Float, z zscale: Float) {
		translate(float3(xscale, yscale, zscale))
	}
	
	func translate(amount: float3) {
		for var vert in vertex {
			vert += amount
		}
		updateVertexArray()
		updateBoundingSphere()
	}

	func rotate(x xscale: Float, y yscale: Float, z zscale: Float) {
		rotate(float3(xscale, yscale, zscale))
	}

	
	func rotate(amount: float3) {
		for var vert in vertex {
			vert = SwiftLugaru.rotate(vert, byAngles: (amount.x, amount.y, amount.z))
		}
		updateVertexArray()
		updateBoundingSphere()
	}

	//MARK: -
	
	func calculateNormals(facenormalise: Bool) {
		//if(visibleloading){
		//loadscreencolor=3;
		//pgame->LoadingScreen();
		//}
		guard modelType == .Normal || modelType == .Decals else {
			return;
		}
		
		normals = [float3](count: Int(vertexNum), repeatedValue: float3())
		
		for (i, triangle) in triangles.enumerate() {
			let l_vect_b1 = vertex[Int(triangle.vertex.1)] - vertex[Int(triangle.vertex.0)];
			let l_vect_b2 = vertex[Int(triangle.vertex.2)] - vertex[Int(triangle.vertex.0)];
			faceNormals[i] = cross(l_vect_b1, l_vect_b2);
			
			normals[Int(triangle.vertex.0)] += faceNormals[i];
			normals[Int(triangle.vertex.1)] += faceNormals[i];
			normals[Int(triangle.vertex.2)] += faceNormals[i];
			
			if (facenormalise) {
				faceNormals[i] = normalize(faceNormals[i])
			}
		}
		for var normal in normals {
			normal = normalize(normal)
			normal *= -1;
		}
		updateVertexArrayNoTexture();
	}

	// MARK: - Drawing
	func drawImmediate() {
		drawImmediate(texture: texturePtr)
	}
	
	func drawImmediate(texture texture: GLuint) {
		glBindTexture(GLenum(GL_TEXTURE_2D), texture);
		glBegin(GLenum(GL_TRIANGLES))
		for (i, triangle) in triangles.enumerate() {
			/*if(Triangles[i].vertex[0]<vertexNum&&Triangles[i].vertex[1]<vertexNum&&Triangles[i].vertex[2]<vertexNum&&Triangles[i].vertex[0]>=0&&Triangles[i].vertex[1]>=0&&Triangles[i].vertex[2]>=0){
			if(isnormal(vertex[Int(triangle.vertex.0)].x)&&isnormal(vertex[Int(triangle.vertex.0)].y)&&isnormal(vertex[Int(triangle.vertex.0)].z)
			&&isnormal(vertex[Int(triangle.vertex.1)].x)&&isnormal(vertex[Int(triangle.vertex.1)].y)&&isnormal(vertex[Int(triangle.vertex.1)].z)
			&&isnormal(vertex[Int(triangle.vertex.2)].x)&&isnormal(vertex[Int(triangle.vertex.2)].y)&&isnormal(vertex[Int(triangle.vertex.2)].z)){
			*/
			glTexCoord2f(triangle.gx.0,triangle.gy.0);
			if(color) {
				glColor3f(normals[Int(triangle.vertex.0)].x, normals[Int(triangle.vertex.0)].y, normals[Int(triangle.vertex.0)].z);
			}
			if !color && !flat {
				glNormal3f(normals[Int(triangle.vertex.0)].x, normals[Int(triangle.vertex.0)].y, normals[Int(triangle.vertex.0)].z);
			}
			if(!color&&flat) {
				glNormal3f(faceNormals[i].x,faceNormals[i].y,faceNormals[i].y);
			}
			glVertex3f(vertex[Int(triangle.vertex.0)].x, vertex[Int(triangle.vertex.0)].y, vertex[Int(triangle.vertex.0)].z);
			
			glTexCoord2f(triangle.gx.1, triangle.gy.1);
			if color {
				glColor3f(normals[Int(triangle.vertex.1)].x, normals[Int(triangle.vertex.1)].y, normals[Int(triangle.vertex.1)].z);
			}
			if !color && !flat {
				glNormal3f(normals[Int(triangle.vertex.1)].x, normals[Int(triangle.vertex.1)].y, normals[Int(triangle.vertex.1)].z);
			}
			if !color && flat {
				glNormal3f(faceNormals[i].x,faceNormals[i].y,faceNormals[i].y);
			}
			glVertex3f(vertex[Int(triangle.vertex.1)].x, vertex[Int(triangle.vertex.1)].y, vertex[Int(triangle.vertex.1)].z);
			
			glTexCoord2f(triangle.gx.2,triangle.gy.2);
			if color {
				glColor3f(normals[Int(triangle.vertex.2)].x, normals[Int(triangle.vertex.2)].y, normals[Int(triangle.vertex.2)].z);
			}
			if !color && !flat {
				glNormal3f(normals[Int(triangle.vertex.2)].x, normals[Int(triangle.vertex.2)].y, normals[Int(triangle.vertex.2)].z);
			}
			if(!color&&flat) {
				glNormal3f(faceNormals[i].x, faceNormals[i].y, faceNormals[i].y);
			}
			glVertex3f(vertex[Int(triangle.vertex.2)].x, vertex[Int(triangle.vertex.2)].y, vertex[Int(triangle.vertex.2)].z);
			//}
			//}
		}
		glEnd();
	}
	
	func draw() {
		draw(texture: texturePtr)
	}
	
	func draw(texture texture: GLuint) {
		if modelType != .Normal && modelType != .Decals {
			return
		}
		
		glEnableClientState(GLenum(GL_NORMAL_ARRAY));
		glEnableClientState(GLenum(GL_VERTEX_ARRAY))
		glEnableClientState(GLenum(GL_TEXTURE_COORD_ARRAY))
		
		if !color {
			glInterleavedArrays(GLenum(GL_T2F_N3F_V3F), GLsizei(8*sizeof(GLfloat)), vArray);
		} else {
			glInterleavedArrays(GLenum(GL_T2F_C3F_V3F), GLsizei(8*sizeof(GLfloat)), vArray);
		}
		glBindTexture(GLenum(GL_TEXTURE_2D), texture);
		
		//#if PLATFORM_MACOSX
		//glLockArraysEXT( 0, GLsizei(triangles.count * 3));
		//#endif
		glDrawArrays(GLenum(GL_TRIANGLES), 0, GLsizei(triangles.count * 3));
		//#if PLATFORM_MACOSX
		//glUnlockArraysEXT();
		//#endif
		
		if !color {
			glDisableClientState(GLenum(GL_NORMAL_ARRAY))
		} else {
			glDisableClientState(GLenum(GL_COLOR_ARRAY))
		}
		glDisableClientState(GLenum(GL_VERTEX_ARRAY))
		glDisableClientState(GLenum(GL_TEXTURE_COORD_ARRAY))
		//drawimmediate();
	}

	func drawDecals(shadowTexture shadowtexture: GLuint, bloodTexture bloodtexture: GLuint, secondBloodTexture bloodtexture2: GLuint, breakTexture breaktexture: GLuint) {
		guard preferences.decalsEnabled && modelType == .Decals else {
			return
		}
		var lastType: DecalType? = nil
		//int lasttype = -1;
		var blend = true;
		
		//const float viewdistsquared=viewdistance*viewdistance;
		
		glEnable(GLenum(GL_BLEND))
		glDisable(GLenum(GL_LIGHTING))
		glDisable(GLenum(GL_CULL_FACE))
		glBlendFunc(GLenum(GL_SRC_ALPHA), GLenum(GL_ONE_MINUS_SRC_ALPHA))
		glDepthMask(0);
		//if(numdecals>max_model_decals)numdecals=max_model_decals;
		for var decal in decals {
			if (decal.type == .BloodFast && decal.aliveTime<2) {
				decal.aliveTime = 2;
			}
			
			if decal.type == .Shadow && decal.type != lastType {
				glBindTexture(GLenum(GL_TEXTURE_2D), shadowtexture);
				if !blend {
					blend=true;
					glAlphaFunc(GLenum(GL_GREATER), 0.0001);
					glBlendFunc(GLenum(GL_SRC_ALPHA), GLenum(GL_ONE_MINUS_SRC_ALPHA))
				}
			} else if decal.type == .Break && decal.type != lastType {
				glBindTexture(GLenum(GL_TEXTURE_2D), breaktexture);
				if !blend {
					blend=true;
					glAlphaFunc(GLenum(GL_GREATER), 0.0001);
					glBlendFunc(GLenum(GL_SRC_ALPHA), GLenum(GL_ONE_MINUS_SRC_ALPHA))
				}
			} else if (decal.type == .Blood || decal.type == .BloodSlow) && decal.type != lastType {
				glBindTexture(GLenum(GL_TEXTURE_2D), bloodtexture);
				if blend {
					blend=false;
					glAlphaFunc(GLenum(GL_GREATER), 0.15);
					glBlendFunc(GLenum(GL_ONE),GLenum(GL_ZERO))
				}
			} else if (decal.type == .BloodFast) && decal.type != lastType {
				glBindTexture(GLenum(GL_TEXTURE_2D), bloodtexture2);
				if blend {
					blend=false;
					glAlphaFunc(GLenum(GL_GREATER), 0.15);
					glBlendFunc(GLenum(GL_ONE), GLenum(GL_ZERO))
				}
			}
			switch decal.type {
			case .Shadow:
				glColor4f(1,1,1,decal.opacity);
				
			case .Break:
				glColor4f(1,1,1,decal.opacity);
				if decal.aliveTime > 58 {
					glColor4f(1,1,1, decal.opacity * (60 - decal.aliveTime)/2);
				}
				
			case .Blood, .BloodFast, .BloodSlow:
				glColor4f(1,1,1,decal.opacity);
				if decal.aliveTime < 4 {
					glColor4f(1,1,1,decal.opacity * decal.aliveTime * 0.25);
				}
				if decal.aliveTime > 58 {
					glColor4f(1, 1, 1, decal.opacity*(60-decal.aliveTime)/2);
				}
				
			default:
				print("Unknown decal type: \(decal.type.rawValue)")
			}
			lastType = decal.type
			
			glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_S), GL_CLAMP);
			glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_T), GL_CLAMP);
			
			// Select The Modelview Matrix
			glMatrixMode(GLenum(GL_MODELVIEW))
			glPushMatrix();
			glBegin(GLenum(GL_TRIANGLES))
			for j in 0..<3 {
				glTexCoord2f(decal.textureCoordinates[j][0], decal.textureCoordinates[j][1]);
				glVertex3f(decal.vertex[j].x, decal.vertex[j].y, decal.vertex[j].z);
			}
			glEnd();
			glPopMatrix();
		}
		
		var delDecalPos = Set<Int>()
		for (i, var decal) in decals.enumerate().reverse() {
			decal.aliveTime+=multiplier;
			if decal.type == .BloodSlow {
				decal.aliveTime-=multiplier*2/3;
			}
			if decal.type == .BloodFast {
				decal.aliveTime+=multiplier*4;
			}
			if decal.type == .Shadow {
				delDecalPos.insert(i)
				continue
			}
			if (decal.type == .Blood || decal.type == .BloodFast || decal.type == .BloodSlow) && decal.aliveTime >= 60 {
				delDecalPos.insert(i)
			}
		}
		glAlphaFunc(GLenum(GL_GREATER), 0.0001);
		glBlendFunc(GLenum(GL_SRC_ALPHA), GLenum(GL_ONE_MINUS_SRC_ALPHA))
		
		for i in Array(delDecalPos).sort().reverse() {
			removeDecal(i)
		}
	}
	
	// MARK: - Decals
	func removeDecal(which: Int) {
		guard preferences.decalsEnabled && modelType == .Decals else {
			return
		}
		decals.removeAtIndex(which)
	}
	
	func makeDecal(type atype: DecalType, `where` loc: float3, size: Float, opacity: Float, rotation: Float) {
		guard preferences.decalsEnabled && modelType == .Decals else {
			return
		}
		//float placex,placez;
		var rot = float3()
		//XYZ rot;
		//static XYZ point,point1,point2;
		//float distance;
		var aDecal = Decal()
		decals.reserveCapacity(max_model_decals)
		
		func validateDecal(bDecal: Decal) -> Bool {
			if !(bDecal.textureCoordinates[0][0] < 0 && bDecal.textureCoordinates[1][0] < 0 && bDecal.textureCoordinates[2][0] < 0) {
				if !(bDecal.textureCoordinates[0][1] < 0 && bDecal.textureCoordinates[1][1] < 0 && bDecal.textureCoordinates[2][1] < 0) {
					if !(bDecal.textureCoordinates[0][0] > 1 && bDecal.textureCoordinates[1][0] > 1 && bDecal.textureCoordinates[2][0] > 1) {
						if !(bDecal.textureCoordinates[0][1] > 1 && bDecal.textureCoordinates[1][1] > 1 && bDecal.textureCoordinates[2][1] > 1) {
							return true
						}
					}
				}
			}
			return false
		}
		
		if opacity > 0 {
			if (findDistancefast(loc, boundingSphereCenter) < (boundingSphereRadius + size) * (boundingSphereRadius + size)) {
				for (i, triangle) in triangles.enumerate() {
					let distance: Float = {
						var stage1: Float = (faceNormals[i].x * loc.x)
						stage1 += (faceNormals[i].y * loc.y)
						stage1 += (faceNormals[i].z * loc.z)
						var stage2: Float = (faceNormals[i].x * vertex[Int(triangle.vertex.0)].x)
						stage2 += (faceNormals[i].y * vertex[Int(triangle.vertex.0)].y)
						stage2 += (faceNormals[i].z * vertex[Int(triangle.vertex.0)].z)
						
						return abs(stage1 - stage2)
					}()
					if distance < 0.02 && abs(faceNormals[i].y) > abs(faceNormals[i].x) && abs(faceNormals[i].y) > abs(faceNormals[i].z) {
						aDecal.position=loc;
						aDecal.type = atype;
						aDecal.rotation = rotation;
						aDecal.aliveTime=0;
						aDecal.opacity=opacity-distance/10;
						
						if aDecal.opacity > 0 {
							var placex=vertex[Int(triangle.vertex.0)].x;
							var placez=vertex[Int(triangle.vertex.0)].z;
							
							aDecal.textureCoordinates[0][0]=(placex-loc.x)/(size)/2+0.5;
							aDecal.textureCoordinates[0][1]=(placez-loc.z)/(size)/2+0.5;
							
							aDecal.vertex[0].x=placex;
							aDecal.vertex[0].z=placez;
							aDecal.vertex[0].y=vertex[Int(triangle.vertex.0)].y;
							
							
							placex=vertex[Int(triangle.vertex.1)].x;
							placez=vertex[Int(triangle.vertex.1)].z;
							
							aDecal.textureCoordinates[1][0]=(placex-loc.x)/(size)/2+0.5;
							aDecal.textureCoordinates[1][1]=(placez-loc.z)/(size)/2+0.5;
							
							aDecal.vertex[1].x=placex;
							aDecal.vertex[1].z=placez;
							aDecal.vertex[1].y=vertex[Int(triangle.vertex.1)].y;
							
							
							placex=vertex[Int(triangle.vertex.2)].x;
							placez=vertex[Int(triangle.vertex.2)].z;
							
							aDecal.textureCoordinates[2][0]=(placex-loc.x)/(size)/2+0.5;
							aDecal.textureCoordinates[2][1]=(placez-loc.z)/(size)/2+0.5;
							
							aDecal.vertex[2].x=placex;
							aDecal.vertex[2].z=placez;
							aDecal.vertex[2].y=vertex[Int(triangle.vertex.2)].y;
							
							if validateDecal(aDecal) {
								if aDecal.rotation != 0 {
									for j in 0..<3 {
										rot.y=0;
										rot.x=aDecal.textureCoordinates[j][0]-0.5;
										rot.z=aDecal.textureCoordinates[j][1]-0.5;
										rot = SwiftLugaru.rotate(rot, byAngles: (0, -aDecal.rotation, 0));
										aDecal.textureCoordinates[j][0]=rot.x+0.5;
										aDecal.textureCoordinates[j][1]=rot.z+0.5;
									}
								}
								if decals.count < max_model_decals - 1 {
									decals.append(aDecal)
								}
							}
						}
					} else if (distance < 0.02 && abs(faceNormals[i].x) > abs(faceNormals[i].y) && abs(faceNormals[i].x) > abs(faceNormals[i].z)) {
						aDecal.position=loc;
						aDecal.type=atype;
						aDecal.rotation=rotation;
						aDecal.aliveTime=0;
						aDecal.opacity=opacity-distance/10;
						
						if aDecal.opacity > 0 {
							var placex=vertex[Int(triangle.vertex.0)].y;
							var placez=vertex[Int(triangle.vertex.0)].z;
							
							aDecal.textureCoordinates[0][0]=(placex-loc.y)/(size)/2+0.5;
							aDecal.textureCoordinates[0][1]=(placez-loc.z)/(size)/2+0.5;
							
							aDecal.vertex[0].x=vertex[Int(triangle.vertex.0)].x;
							aDecal.vertex[0].z=placez;
							aDecal.vertex[0].y=placex;
							
							
							placex=vertex[Int(triangle.vertex.1)].y;
							placez=vertex[Int(triangle.vertex.1)].z;
							
							aDecal.textureCoordinates[1][0]=(placex-loc.y)/(size)/2+0.5;
							aDecal.textureCoordinates[1][1]=(placez-loc.z)/(size)/2+0.5;
							
							aDecal.vertex[1].x=vertex[Int(triangle.vertex.1)].x;
							aDecal.vertex[1].z=placez;
							aDecal.vertex[1].y=placex;
							
							
							placex=vertex[Int(triangle.vertex.2)].y;
							placez=vertex[Int(triangle.vertex.2)].z;
							
							aDecal.textureCoordinates[2][0]=(placex-loc.y)/(size)/2+0.5;
							aDecal.textureCoordinates[2][1]=(placez-loc.z)/(size)/2+0.5;
							
							aDecal.vertex[2].x=vertex[Int(triangle.vertex.2)].x;
							aDecal.vertex[2].z=placez;
							aDecal.vertex[2].y=placex;
							
							if validateDecal(aDecal) {
								if aDecal.rotation != 0 {
									for j in 0..<3 {
										rot.y=0;
										rot.x=aDecal.textureCoordinates[j][0]-0.5;
										rot.z=aDecal.textureCoordinates[j][1]-0.5;
										rot = SwiftLugaru.rotate(rot, byAngles: (0,-aDecal.rotation,0));
										aDecal.textureCoordinates[j][0]=rot.x+0.5;
										aDecal.textureCoordinates[j][1]=rot.z+0.5;
									}
								}
								if decals.count < max_model_decals-1 {
									decals.append(aDecal)
								}
							}
						}
					} else if (distance < 0.02 && abs(faceNormals[i].z) > abs(faceNormals[i].y) && abs(faceNormals[i].z) > abs(faceNormals[i].x)) {
						aDecal.position=loc;
						aDecal.type=atype;
						aDecal.rotation=rotation;
						aDecal.aliveTime=0;
						aDecal.opacity=opacity-distance/10;
						
						if aDecal.opacity > 0 {
							var placex=vertex[Int(triangle.vertex.0)].x;
							var placez=vertex[Int(triangle.vertex.0)].y;
							
							aDecal.textureCoordinates[0][0]=(placex-loc.x)/(size)/2+0.5;
							aDecal.textureCoordinates[0][1]=(placez-loc.y)/(size)/2+0.5;
							
							aDecal.vertex[0].x=placex;
							aDecal.vertex[0].z=vertex[Int(triangle.vertex.0)].z;
							aDecal.vertex[0].y=placez;
							
							
							placex=vertex[Int(triangle.vertex.1)].x;
							placez=vertex[Int(triangle.vertex.1)].y;
							
							aDecal.textureCoordinates[1][0]=(placex-loc.x)/(size)/2+0.5;
							aDecal.textureCoordinates[1][1]=(placez-loc.y)/(size)/2+0.5;
							
							aDecal.vertex[1].x=placex;
							aDecal.vertex[1].z=vertex[Int(triangle.vertex.1)].z;
							aDecal.vertex[1].y=placez;
							
							
							placex=vertex[Int(triangle.vertex.2)].x;
							placez=vertex[Int(triangle.vertex.2)].y;
							
							aDecal.textureCoordinates[2][0]=(placex-loc.x)/(size)/2+0.5;
							aDecal.textureCoordinates[2][1]=(placez-loc.y)/(size)/2+0.5;
							
							aDecal.vertex[2].x=placex;
							aDecal.vertex[2].z=vertex[Int(triangle.vertex.2)].z;
							aDecal.vertex[2].y=placez;
							
							if validateDecal(aDecal) {
								if aDecal.rotation != 0 {
									for j in 0..<3 {
										rot.y=0;
										rot.x=aDecal.textureCoordinates[j][0]-0.5;
										rot.z=aDecal.textureCoordinates[j][1]-0.5;
										rot = SwiftLugaru.rotate(rot, byAngles: (0,-aDecal.rotation,0))
										aDecal.textureCoordinates[j][0]=rot.x+0.5;
										aDecal.textureCoordinates[j][1]=rot.z+0.5;
									}
								}
								if decals.count < max_model_decals-1 {
									decals.append(aDecal)
								}
							}
						}
					}
				}
			}
		}
	}

}
