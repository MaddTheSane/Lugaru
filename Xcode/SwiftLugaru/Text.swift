//
//  Text.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/24/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation
import OpenGL.GL
import OpenGL.GL.GLU
import OpenGL.GL.Ext

final class Text {
	private(set) var fontTexture: GLuint = 0
	private var base: GLuint = 0

	func loadFontTexture(textureURL: NSURL) {
		loadTexture(textureURL, textureID: &fontTexture, mipmap: false, hasAlpha: false)
		
		if base != 0 {
			glDeleteLists(base, 512);
			base = 0;
		}

	}
	
	/// Build Our Font Display List
	func buildFont() {
		if base != 0  {
			Swift.print("Font already created...");
			return;
		}
		
		// Creating 256 Display Lists
		base=glGenLists(512);
		// Select Our Font Texture
		glBindTexture(GLenum(GL_TEXTURE_2D), fontTexture);
		// Loop Through All 256 Lists
		for loop in 0 ..< GLuint(512) {
			/// Holds Our X Character Coord
			let cx: Float
			/// Holds Our Y Character Coord
			let cy: Float
			if loop < 256 {
				// X Position Of Current Character
				cx=Float(loop%16)/16.0;
				// Y Position Of Current Character
				cy=Float(loop/16)/16.0;
			} else {
				// X Position Of Current Character
				cx=Float((loop-256)%16)/16.0;
				// Y Position Of Current Character
				cy=Float((loop-256)/16)/16.0;
			}
			// Start Building A List
			glNewList(base+loop, GLenum(GL_COMPILE));
			// Use A Quad For Each Character
			glBegin(GLenum(GL_QUADS))
			// Texture Coord (Bottom Left)
			glTexCoord2f(cx,1-cy-0.0625+0.001);
			// Vertex Coord (Bottom Left)
			glVertex2i(0,0);
			// Texture Coord (Bottom Right)
			glTexCoord2f(cx+0.0625,1-cy-0.0625+0.001);
			// Vertex Coord (Bottom Right)
			glVertex2i(16,0);
			// Texture Coord (Top Right)
			glTexCoord2f(cx+0.0625,1-cy-0.001);
			// Vertex Coord (Top Right)
			glVertex2i(16,16);
			// Texture Coord (Top Left)
			glTexCoord2f(cx,1-cy - +0.001);
			// Vertex Coord (Top Left)
			glVertex2i(0,16);
			// Done Building Our Quad (Character)
			glEnd();
			if loop < 256 {
				// Move To The Right Of The Character
				glTranslated(10,0,0);
			} else {
				// Move To The Right Of The Character
				glTranslated(8,0,0);
			}
			// Done Building The Display List
			glEndList();
		}
	}
	
	deinit {
		if base != 0 {
			glDeleteLists(base, 512);
			base = 0;
		}
		if fontTexture != 0 {
			glDeleteTextures(1, &fontTexture);
		}
	}
	
	func print(x x: GLfloat, y: GLfloat, string: UnsafePointer<CChar>, set: UInt32, size: GLfloat, width: GLfloat, height: GLfloat, start: Int = 0, end: Int? = nil) {
		glPrint(x: x, y: y, string: string, set: set, size: size, width: width, height: height, start: start, end: end ?? Int(strlen(string)), offset: 0);
	}
	
	func printOutline(x x: GLfloat, y: GLfloat, string: UnsafePointer<CChar>, set: UInt32, size: GLfloat, width: GLfloat, height: GLfloat, start: Int = 0, end: Int? = nil) {
		glPrint(x: x, y: y, string: string, set: set, size: size, width: width, height: height, start: start, end: end ?? Int(strlen(string)), offset: 256);
	}

	
	func printOutlined(red r: GLfloat = 1, green g: GLfloat = 1, blue b: GLfloat = 1, x: GLfloat, y: GLfloat, string: UnsafePointer<CChar>, set: UInt32, size: GLfloat, width: GLfloat, height: GLfloat) {
		glColor4f(0,0,0,1);
		printOutline(x: x-2*size,  y: y-2*size, string: string,  set: set,  size: size*2.5/2,  width: width,  height: height);
		glColor4f(r,g,b,1);
		print( x: x,  y: y, string: string,  set: set,  size: size,  width: width,  height: height);

	}
	
	/// Where The Printing Happens
	private func glPrint(x x: GLfloat, y: GLfloat, string: UnsafePointer<CChar>, set aSet: UInt32, size: GLfloat, width: GLfloat, height: GLfloat, start: Int, end: Int, offset: UInt32) {
		var set = aSet
		if set > 1 {
			set = 1
		}
		
		glTexEnvi(GLenum(GL_TEXTURE_ENV), GLenum(GL_TEXTURE_ENV_MODE), GL_MODULATE);
		// Select Our Font Texture
		glBindTexture(GLenum(GL_TEXTURE_2D), fontTexture);
		// Disables Depth Testing
		glDisable(GLenum(GL_DEPTH_TEST));
		glDisable(GLenum(GL_LIGHTING))
		glEnable(GLenum(GL_BLEND))
		glBlendFunc(GLenum(GL_SRC_ALPHA), GLenum(GL_ONE_MINUS_SRC_ALPHA))
		// Select The Projection Matrix
		glMatrixMode(GLenum(GL_PROJECTION))
		// Store The Projection Matrix
		glPushMatrix();
		do {
			// Reset The Projection Matrix
			glLoadIdentity();
			// Set Up An Ortho Screen
			glOrtho(0,GLdouble(width),0,GLdouble(height),-100,100);
			// Select The Modelview Matrix
			glMatrixMode(GLenum(GL_MODELVIEW))
			// Store The Modelview Matrix
			glPushMatrix();
			do {
				glLoadIdentity();
				// Position The Text (0,0 - Bottom Left)
				glTranslatef(x,y,0);
				// Reset The Modelview Matrix
				glScalef(size,size,1);
				// Choose The Font Set (0 or 1)
				glListBase(base-32+(128*set) + offset);
				// Write The Text To The Screen
				glCallLists(Int32(end-start),GLenum(GL_BYTE),string.advancedBy(start));
				// Select The Projection Matrix
				glMatrixMode(GLenum(GL_PROJECTION));
			}
			// Restore The Old Projection Matrix
			glPopMatrix();
			// Select The Modelview Matrix
			glMatrixMode(GLenum(GL_MODELVIEW))
		}
		// Restore The Old Projection Matrix
		glPopMatrix();
		// Enables Depth Testing
		glEnable(GLenum(GL_DEPTH_TEST))
		glTexEnvi(GLenum(GL_TEXTURE_ENV), GLenum(GL_TEXTURE_ENV_MODE), GL_MODULATE)
		
	}
	/*
void glPrint(float x, float y, char *string, int set, float size, float width, float height);
void glPrintOutline(float x, float y, char *string, int set, float size, float width, float height);
void glPrint(float x, float y, char *string, int set, float size, float width, float height,int start,int end);
void glPrintOutline(float x, float y, char *string, int set, float size, float width, float height,int start,int end);
void glPrintOutlined(float x, float y, char *string, int set, float size, float width, float height);
void glPrintOutlined(float r, float g, float b, float x, float y, char *string, int set, float size, float width, float height);
*/
}
