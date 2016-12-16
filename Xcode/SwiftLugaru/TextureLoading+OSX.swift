//
//  TextureLoading.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/25/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Cocoa
import OpenGL.GL
import OpenGL.GL.GLU
import OpenGL.GL.Ext


func loadTexture(_ fileURL: URL, textureID: inout GLuint, mipmap: Bool, hasAlpha: Bool) {
	guard let sourcefile = NSImage(contentsOf:fileURL) else {
		return
	}
	
	guard let sourceTiff = sourcefile.tiffRepresentation, let imgRep = NSBitmapImageRep(data: sourceTiff) else {
		return
	}
	
	print("Loading texture... " + fileURL.path)
	
	do {
		let type: GLint
		//Alpha channel?
		if !imgRep.hasAlpha {
			type = GL_RGB
		} else {
			type = GL_RGBA
		}
		
		glPixelStorei(GLenum(GL_UNPACK_ALIGNMENT), 1)
		
		if textureID == 0 {
			glGenTextures(1, &textureID)
		}
		glBindTexture(GLenum(GL_TEXTURE_2D), textureID)
		glTexImage2D(GLenum(GL_TEXTURE_2D), 0, type, GLsizei(imgRep.pixelsWide), GLsizei(imgRep.pixelsHigh), 0, GLenum(imgRep.hasAlpha ? GL_RGBA : GL_RGB), GLenum(GL_UNSIGNED_BYTE), imgRep.bitmapData);
		//ATI workaround!
		glEnable(GLenum(GL_TEXTURE_2D))
		glGenerateMipmap(GLenum(GL_TEXTURE_2D));  //Generate mipmaps now!!!
		glTexEnvi(GLenum(GL_TEXTURE_ENV), GLenum(GL_TEXTURE_ENV_MODE), GL_MODULATE)
		
		glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_S), GL_REPEAT)
		glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_T), GL_REPEAT)
		glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MAG_FILTER), GL_LINEAR)
		if mipmap {
			if preferences.trilinearFiltering {
				glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MIN_FILTER), GL_LINEAR_MIPMAP_LINEAR)
			} else {
				glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MIN_FILTER), GL_LINEAR_MIPMAP_NEAREST)
			}
		} else {
			glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MIN_FILTER), GL_LINEAR)
		}
	}
}
