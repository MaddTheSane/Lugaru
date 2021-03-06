/*
Copyright (C) 2003, 2010 - Wolfire Games

This file is part of Lugaru.

Lugaru is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _MODELS_H_
#define _MODELS_H_

/**> Model Loading <**/
//
// Model Maximums
//
#include "gamegl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "Terrain.h"
#include "binio.h"
#include "Quaternions.h"
#include "Texture.h"

//
// Textures List
//
typedef struct {
    long xsz, ysz;
    GLubyte *txt;
} ModelTexture;

//
// Model Structures
//

class TexturedTriangle
{
public:
    short vertex[3];
    float gx[3], gy[3];
};

#define max_model_decals 300

typedef enum ModelType : int ModelType; enum ModelType : int {
	ModelTypeNothing = 0,
	ModelTypeNoTexture,
	ModelTypeRaw,
	ModelTypeDecals,
	ModelTypeNormal,
};

#define nothing ModelTypeNothing
#define normaltype ModelTypeNormal
#define notextype ModelTypeNoTexture
#define rawtype ModelTypeRaw
#define decalstype ModelTypeDecals

class Model
{
public:
    short vertexNum, TriangleNum;
    bool hastexture;

    ModelType type, oldtype;

    int* possible;
    int* owner;
    XYZ* vertex;
    XYZ* normals;
    XYZ* facenormals;
    TexturedTriangle* Triangles;
    GLfloat* vArray;

    /*int possible[max_model_vertex];
    int owner[max_textured_triangle];
    XYZ vertex[max_model_vertex];
    XYZ normals[max_model_vertex];
    XYZ facenormals[max_textured_triangle];
    TexturedTriangle Triangles[max_textured_triangle];
    GLfloat vArray[max_textured_triangle*24];*/

    Texture textureptr;
    ModelTexture modelTexture;
    int numpossible;
    bool color;

    XYZ boundingspherecenter;
    float boundingsphereradius;

    float*** decaltexcoords;
    XYZ** decalvertex;
    int* decaltype;
    float* decalopacity;
    float* decalrotation;
    float* decalalivetime;
    XYZ* decalposition;

    /*float decaltexcoords[max_model_decals][3][2];
    XYZ decalvertex[max_model_decals][3];
    int decaltype[max_model_decals];
    float decalopacity[max_model_decals];
    float decalrotation[max_model_decals];
    float decalalivetime[max_model_decals];
    XYZ decalposition[max_model_decals];*/

    int numdecals;

    bool flat;

    void DeleteDecal(int which);
    void MakeDecal(int atype, XYZ *where, float *size, float *opacity, const float rotation);
    void MakeDecal(int atype, XYZ where, float size, float opacity, const float rotation);
    void drawdecals(Texture shadowtexture, Texture bloodtexture, Texture bloodtexture2, Texture breaktexture);
    int SphereCheck(XYZ &p1,const float radius, XYZ &p, const XYZ &move, const float rotate);
    int SphereCheckPossible(XYZ &p1,const float radius, const XYZ &move, const float rotate);
    int LineCheck(XYZ &p1,XYZ &p2, XYZ &p, const XYZ &move, const float rotate);
    int LineCheckSlide(XYZ &p1,XYZ &p2, XYZ &p, const XYZ &move, const float rotate);
    int LineCheckPossible(XYZ &p1,XYZ &p2, XYZ &p, const XYZ &move, const float rotate);
    int LineCheckSlidePossible(XYZ &p1,XYZ &p2, XYZ &p, const XYZ &move, const float rotate);
    void UpdateVertexArray();
    void UpdateVertexArrayNoTex();
    void UpdateVertexArrayNoTexNoNorm();
    bool loadnotex(const char *filename);
    bool loadraw(char *filename);
    bool load(const char *filename, bool texture);
    bool loaddecal(const char *filename, bool texture);
    void Scale(float xscale, float yscale, float zscale);
    void FlipTexCoords();
    void UniformTexCoords();
    void ScaleTexCoords(float howmuch);
    void ScaleNormals(float xscale, float yscale, float zscale);
    void Translate(float xtrans, float ytrans, float ztrans);
    void CalculateNormals(bool facenormalise);
    void draw();
    void drawdifftex(GLuint texture);
    void drawdifftex(Texture texture);
    void drawimmediate();
    void drawdiffteximmediate(GLuint texture);
    void Rotate(float xang, float yang, float zang);
    ~Model();
    void deallocate();
    Model();

private:
    void UpdateBoundingSphere();
};

#endif
