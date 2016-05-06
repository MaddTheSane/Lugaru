//
//  Texture.swift
//  Lugaru
//
//  Created by C.W. Betts on 5/5/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Cocoa
import OpenGL.GL

final class Texture {
	private var tex: TextureRes?
	
	func load(fileURL: NSURL, hasMipMap: Bool, hasAlpha: Bool) {
		tex = TextureRes(fileName: fileURL, hasMipMap: hasMipMap, hasAlpha: hasAlpha)
	}
	
	func load(fileURL: NSURL, hasMipMap: Bool, array byteArray: UnsafeMutablePointer<GLubyte>, inout skinSize: Int32) {
		tex = TextureRes(fileName: fileURL, hasMipMap: hasMipMap, array: byteArray, skinSize: &skinSize)
	}
	
	func bind() {
		if let tex = tex {
			tex.bind()
		} else {
			glBindTexture(GLenum(GL_TEXTURE_2D), 0);
		}
	}
	
	static func reloadAll() {
		TextureRes.reloadAll()
	}
}

private class TextureRes {
	var id: GLuint = 0
	let filename: NSURL
	let hasMipmap: Bool
	var hasAlpha: Bool
	var isSkin = false
	var skinsize: Int32 = 0
	var data: NSData? = nil//UnsafeMutableBufferPointer<GLubyte> = nil
	//int datalen;
	var skinData: UnsafeMutablePointer<GLubyte> = nil
	
	init(fileName _filename: NSURL, hasMipMap _hasMipmap: Bool, hasAlpha _hasAlpha: Bool) {
		filename = _filename
		hasMipmap = _hasMipmap
		hasAlpha = _hasAlpha
		
		load();
		TextureRes.list.addObject(self)
	}

	init(fileName _filename: NSURL, hasMipMap _hasMipmap: Bool, array: UnsafeMutablePointer<GLubyte>, inout  skinSize skinsizep: Int32) {
		filename = _filename
		hasMipmap = _hasMipmap
		hasAlpha = false
		isSkin = true
		
		load();
		skinsizep = skinsize;
		
		for (i, dat) in UnsafeBufferPointer(start: UnsafePointer<UInt8>(data!.bytes), count: data!.length).enumerate() {
			array[i] = dat;
		}
		skinData = array;
		TextureRes.list.addObject(self)
	}
	
	static var list = NSHashTable.weakObjectsHashTable()// Array<TextureRes>()
	
	func load() {
		//load image into 'texture' global var
		guard let image = NSImage(contentsOfURL: filename), imgData = image.TIFFRepresentation,
			texture = NSBitmapImageRep(data: imgData) else {
			return
		}
		
		//if skinData == nil {
		//	unsigned char filenamep[256];
		//	CopyCStringToPascal(ConvertFileName(filename.c_str()), filenamep);
		//	upload_image(filenamep, hasAlpha);
		//}
		
		//skinsize = texture.sizeX;
		var type = GL_RGBA;
		if (texture.bitsPerPixel == 24) {
			type = GL_RGB;
		}
		
		glPixelStorei(GLenum(GL_UNPACK_ALIGNMENT), 1);
		
		glDeleteTextures(1, &id);
		glGenTextures(1, &id);
		glTexEnvi(GLenum(GL_TEXTURE_ENV), GLenum(GL_TEXTURE_ENV_MODE), GL_MODULATE);
		
		glBindTexture(GLenum(GL_TEXTURE_2D), id);
		glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MAG_FILTER), GL_LINEAR);
		if (hasMipmap) {
			glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MIN_FILTER), (preferences.trilinearFiltering ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST));
			glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_GENERATE_MIPMAP), GL_TRUE);
		} else {
			glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MIN_FILTER), GL_LINEAR);
		}
		
		if (isSkin) {
			if skinData != nil {
				glTexImage2D(GLenum(GL_TEXTURE_2D), 0, GL_RGB, skinsize, skinsize, 0, GLenum(GL_RGB), GLenum(GL_UNSIGNED_BYTE), skinData);
			} else {
				let nb = texture.pixelsHigh * texture.pixelsWide * (texture.bitsPerPixel / 8);
				let data = NSMutableData(length: Int(nb))
				self.data = data
				//data = (GLubyte*)malloc(nb * sizeof(GLubyte));
				//datalen = 0;
				var datalen = 0
				let data2 = UnsafeMutableBufferPointer(start: UnsafeMutablePointer<UInt8>(data!.mutableBytes), count: data!.length)
				for i in 0..<nb {
					if (((i + 1) % 4) != 0 || type == GL_RGB) {
						data2[datalen] = texture.bitmapData[i]
						datalen += 1
					}
				}
				glTexImage2D(GLenum(GL_TEXTURE_2D), 0, type, GLsizei(texture.pixelsWide), GLsizei(texture.pixelsHigh), 0, GLenum(GL_RGB), GLenum(GL_UNSIGNED_BYTE), self.data!.bytes)
			}
		} else {
			glTexImage2D(GLenum(GL_TEXTURE_2D), 0, type, GLsizei(texture.pixelsWide), GLsizei(texture.pixelsHigh), 0, GLenum(type), GLenum(GL_UNSIGNED_BYTE), texture.bitmapData)
		}
	}
	
	func bind() {
		glBindTexture(GLenum(GL_TEXTURE_2D), id);
	}
	
	static func reloadAll() {
		let generator = NSFastGenerator(list)
		while let anObj = generator.next() as? TextureRes {
			anObj.id = 0
			anObj.load()
		}
	}
	
	deinit {
		glDeleteTextures(1, &id)
	}
}

