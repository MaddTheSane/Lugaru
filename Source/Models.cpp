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

#include "Game.h"
#include "Models.h"
#include "Globals.h"

using namespace std;
using namespace simd;

int Model::LineCheck(XYZ &p1,XYZ &p2, XYZ &p, const XYZ &move, const float rotate)
{
    static float olddistance;
    int firstintersecting = -1;
    XYZ point;
    
    p1 = p1 - move;
    p2 = p2 - move;
    if (rotate) {
        p1 = DoRotation(p1, 0, -rotate, 0);
        p2 = DoRotation(p2, 0, -rotate, 0);
    }
    if (!sphere_line_intersection(p1, p2, boundingspherecenter, boundingsphereradius))
        return -1;
    
    for (int j = 0; j < TriangleNum; j++) {
        int intersecting = LineFacetd(p1, p2, vertex[Triangles[j].vertex[0]], vertex[Triangles[j].vertex[1]], vertex[Triangles[j].vertex[2]], facenormals[j], point);
        float distance = distance_squared(point, p1);
        if ((distance < olddistance || firstintersecting == -1) && intersecting) {
            olddistance = distance;
            firstintersecting = j;
            p = point;
        }
    }
    
    if (rotate)
        p = DoRotation(p, 0, rotate, 0);
    p += move;
    return firstintersecting;
}

int Model::LineCheckSlide(XYZ &p1,XYZ &p2, XYZ &p, const XYZ &move, const float rotate)
{
    static float olddistance;
    int firstintersecting = -1;
    XYZ point;

    p1 = p1 - move;
    p2 = p2 - move;
    if (!sphere_line_intersection(p1, p2, boundingspherecenter, boundingsphereradius))
        return -1;
    if (rotate) {
        p1 = DoRotation(p1, 0, -rotate, 0);
        p2 = DoRotation(p2, 0, -rotate, 0);
    }

    for (int j = 0; j < TriangleNum; j++) {
        int intersecting = LineFacetd(p1, p2, vertex[Triangles[j].vertex[0]], vertex[Triangles[j].vertex[1]], vertex[Triangles[j].vertex[2]], facenormals[j], point);
        float distance = distance_squared(point, p1);
        if ((distance < olddistance || firstintersecting == -1) && intersecting) {
            olddistance = distance;
            firstintersecting = j;
        }
    }

    float distance = abs((facenormals[firstintersecting].x * p2.x) + (facenormals[firstintersecting].y * p2.y) + (facenormals[firstintersecting].z * p2.z) - ((facenormals[firstintersecting].x * vertex[Triangles[firstintersecting].vertex[0]].x) + (facenormals[firstintersecting].y * vertex[Triangles[firstintersecting].vertex[0]].y) + (facenormals[firstintersecting].z * vertex[Triangles[firstintersecting].vertex[0]].z)));
    p2 -= facenormals[firstintersecting] * distance;

    if (rotate)
        p2 = DoRotation(p2, 0, rotate, 0);
    p2 += move;
    return firstintersecting;
}

int Model::LineCheckPossible(XYZ &p1,XYZ &p2, XYZ &p, const XYZ &move, const float rotate)
{
    static float olddistance;
    int firstintersecting = -1;
    XYZ point;

    p1 = p1 - move;
    p2 = p2 - move;
    if (!sphere_line_intersection(p1, p2, boundingspherecenter, boundingsphereradius))
        return -1;
    if (rotate) {
        p1 = DoRotation(p1, 0, -rotate, 0);
        p2 = DoRotation(p2, 0, -rotate, 0);
    }

    if (numpossible > 0 && numpossible < TriangleNum)
        for (int j = 0; j < numpossible; j++) {
            if (possible[j] >= 0 && possible[j] < TriangleNum) {
                int intersecting = LineFacetd(p1, p2, vertex[Triangles[possible[j]].vertex[0]], vertex[Triangles[possible[j]].vertex[1]], vertex[Triangles[possible[j]].vertex[2]], facenormals[possible[j]], point);
                float distance= distance_squared(point, p1);
                if ((distance < olddistance || firstintersecting == -1) && intersecting) {
                    olddistance = distance;
                    firstintersecting = possible[j];
                    p = point;
                }
            }
        }

    if (rotate)
        p = DoRotation(p, 0, rotate, 0);
    p += move;
    return firstintersecting;
}

int Model::LineCheckSlidePossible(XYZ &p1,XYZ &p2, XYZ &p, const XYZ &move, const float rotate)
{
    static float olddistance;
    int firstintersecting = -1;
    XYZ point;

    p1 = p1 - move;
    p2 = p2 - move;
    if (!sphere_line_intersection(p1, p2, boundingspherecenter, boundingsphereradius))
        return -1;
    if (rotate) {
        p1 = DoRotation(p1, 0, -rotate, 0);
        p2 = DoRotation(p2, 0, -rotate, 0);
    }

    if (numpossible) {
        for (int j = 0; j < numpossible; j++) {
            if (possible[j] >= 0 && possible[j] < TriangleNum) {
                int intersecting = LineFacetd(p1, p2, vertex[Triangles[possible[j]].vertex[0]], vertex[Triangles[possible[j]].vertex[1]], vertex[Triangles[possible[j]].vertex[2]], facenormals[possible[j]], point);
                float distance= distance_squared(point, p1);
                if ((distance < olddistance || firstintersecting == -1) && intersecting) {
                    olddistance = distance;
                    firstintersecting = possible[j];
                }
            }
        }
    }

    if (firstintersecting > 0) {
        float distance = abs((facenormals[firstintersecting].x * p2.x) + (facenormals[firstintersecting].y * p2.y) + (facenormals[firstintersecting].z * p2.z) - ((facenormals[firstintersecting].x * vertex[Triangles[firstintersecting].vertex[0]].x) + (facenormals[firstintersecting].y * vertex[Triangles[firstintersecting].vertex[0]].y) + (facenormals[firstintersecting].z * vertex[Triangles[firstintersecting].vertex[0]].z)));
        p2 -= facenormals[firstintersecting] * distance;
    }

    if (rotate)
        p2 = DoRotation(p2, 0, rotate, 0);
    p2 += move;
    return firstintersecting;
}

int Model::SphereCheck(XYZ &p1,const float radius, XYZ &p, const XYZ &move, const float rotate)
{
    static float olddistance;
    int firstintersecting = -1;
    XYZ point;
    
    //XYZ oldp1=p1;
    p1 = p1 - move;
    if (rotate)
        p1 = DoRotation(p1, 0, -rotate, 0);
    if (distsq(p1, boundingspherecenter) > radius * radius + boundingsphereradius * boundingsphereradius)
        return -1;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < TriangleNum; j++) {
            int intersecting = 0;
            float distance = abs((facenormals[j].x * p1.x) + (facenormals[j].y * p1.y) + (facenormals[j].z * p1.z) - ((facenormals[j].x * vertex[Triangles[j].vertex[0]].x) + (facenormals[j].y * vertex[Triangles[j].vertex[0]].y) + (facenormals[j].z * vertex[Triangles[j].vertex[0]].z)));
            if (distance < radius) {
                point = p1 - facenormals[j] * distance;
                if (PointInTriangle( &point, facenormals[j], &vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[1]], &vertex[Triangles[j].vertex[2]]))
                    intersecting = 1;
                if (!intersecting)
                    intersecting = sphere_line_intersection(vertex[Triangles[j].vertex[0]], vertex[Triangles[j].vertex[1]], p1, radius);
                if (!intersecting)
                    intersecting = sphere_line_intersection(vertex[Triangles[j].vertex[1]], vertex[Triangles[j].vertex[2]], p1, radius);
                if (!intersecting)
                    intersecting = sphere_line_intersection(vertex[Triangles[j].vertex[0]], vertex[Triangles[j].vertex[2]], p1, radius);
                if (intersecting) {
                    p1 += facenormals[j] * (distance - radius);
                    /*start=*p1;
                    end=*p1;
                    end.y-=radius;
                    if(LineFacetd(&start,&end,&vertex[Triangles[j].vertex[0]],&vertex[Triangles[j].vertex[1]],&vertex[Triangles[j].vertex[2]],&facenormals[j],&point)){
                    p1->y=point.y+radius;
                    }*/
                }
            }
            if ((distance < olddistance || firstintersecting == -1) && intersecting) {
                olddistance = distance;
                firstintersecting = j;
                p = point;
            }
        }
    }
    if (rotate) {
        p = DoRotation(p, 0, rotate, 0);
        p1 = DoRotation(p1, 0, rotate, 0);
    }
    p += move;
    p1 += move;
    return firstintersecting;
}

int Model::SphereCheckPossible(XYZ &p1,const float radius, const XYZ &move, const float rotate)
{
    static float olddistance;
    int firstintersecting = -1;
    XYZ point;
    
    XYZ oldp1 = p1;
    p1 = p1 - move;
    
    numpossible=0;

    if (rotate)
        p1 = DoRotation(p1, 0, -rotate, 0);
    if (distsq(p1, boundingspherecenter) > radius * radius + boundingsphereradius * boundingsphereradius) {
        p1 = oldp1;
        return -1;
    }

    for (int j = 0; j < TriangleNum; j++) {
        int intersecting = 0;
        float distance = abs((facenormals[j].x * p1.x) + (facenormals[j].y * p1.y) + (facenormals[j].z * p1.z) - ((facenormals[j].x * vertex[Triangles[j].vertex[0]].x) + (facenormals[j].y * vertex[Triangles[j].vertex[0]].y) + (facenormals[j].z * vertex[Triangles[j].vertex[0]].z)));
        if (distance < radius) {
            point = p1 - facenormals[j] * distance;
            if (PointInTriangle( &point, facenormals[j], &vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[1]], &vertex[Triangles[j].vertex[2]]))
                intersecting = 1;
            if (!intersecting)
                intersecting = sphere_line_intersection(vertex[Triangles[j].vertex[0]], vertex[Triangles[j].vertex[1]], p1, radius);
            if (!intersecting)
                intersecting = sphere_line_intersection(vertex[Triangles[j].vertex[1]], vertex[Triangles[j].vertex[2]], p1, radius);
            if (!intersecting)
                intersecting = sphere_line_intersection(vertex[Triangles[j].vertex[0]], vertex[Triangles[j].vertex[2]], p1, radius);
            if (intersecting) {
                //if(j>=0&&j<TriangleNum)
                possible[numpossible] = j;
                numpossible++;
            }
        }
        if ((distance < olddistance || firstintersecting == -1) && intersecting) {
            olddistance = distance;
            firstintersecting = j;
        }
    }
    if (rotate)
        p1 = DoRotation(p1, 0, rotate, 0);
    p1 += move;
    return firstintersecting;
}


void Model::UpdateBoundingSphere()
{
    boundingsphereradius = 0;
    for (int i = 0; i < vertexNum; i++) {
        for (int j = 0; j < vertexNum; j++) {
            if (j != i && distsq(&vertex[j], &vertex[i]) / 2 > boundingsphereradius) {
                boundingsphereradius = distsq(&vertex[j], &vertex[i]) / 2;
                boundingspherecenter = (vertex[i] + vertex[j]) / 2;
            }
        }
    }
    boundingsphereradius = fast_sqrt(boundingsphereradius);
}

void Model::UpdateVertexArray()
{
    if (type != normaltype && type != decalstype)
        return;
    if (!flat)
        for (int i = 0; i < TriangleNum; i++) {
            const int j = i * 24;
            vArray[j + 0] = Triangles[i].gx[0];
            vArray[j + 1] = Triangles[i].gy[0];
            vArray[j + 2] = normals[Triangles[i].vertex[0]].x;
            vArray[j + 3] = normals[Triangles[i].vertex[0]].y;
            vArray[j + 4] = normals[Triangles[i].vertex[0]].z;
            vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
            vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
            vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

            vArray[j + 8] = Triangles[i].gx[1];
            vArray[j + 9] = Triangles[i].gy[1];
            vArray[j + 10] = normals[Triangles[i].vertex[1]].x;
            vArray[j + 11] = normals[Triangles[i].vertex[1]].y;
            vArray[j + 12] = normals[Triangles[i].vertex[1]].z;
            vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
            vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
            vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

            vArray[j + 16] = Triangles[i].gx[2];
            vArray[j + 17] = Triangles[i].gy[2];
            vArray[j + 18] = normals[Triangles[i].vertex[2]].x;
            vArray[j + 19] = normals[Triangles[i].vertex[2]].y;
            vArray[j + 20] = normals[Triangles[i].vertex[2]].z;
            vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
            vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
            vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
        }
    if (flat)
        for (int i = 0; i < TriangleNum; i++) {
            const int j = i * 24;
            vArray[j + 0] = Triangles[i].gx[0];
            vArray[j + 1] = Triangles[i].gy[0];
            vArray[j + 2] = facenormals[i].x * -1;
            vArray[j + 3] = facenormals[i].y * -1;
            vArray[j + 4] = facenormals[i].z * -1;
            vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
            vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
            vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

            vArray[j + 8] = Triangles[i].gx[1];
            vArray[j + 9] = Triangles[i].gy[1];
            vArray[j + 10] = facenormals[i].x * -1;
            vArray[j + 11] = facenormals[i].y * -1;
            vArray[j + 12] = facenormals[i].z * -1;
            vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
            vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
            vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

            vArray[j + 16] = Triangles[i].gx[2];
            vArray[j + 17] = Triangles[i].gy[2];
            vArray[j + 18] = facenormals[i].x * -1;
            vArray[j + 19] = facenormals[i].y * -1;
            vArray[j + 20] = facenormals[i].z * -1;
            vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
            vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
            vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;

        }
}

void Model::UpdateVertexArrayNoTex()
{
    if (type != normaltype && type != decalstype)
        return;
    if (!flat)
        for (int i = 0; i < TriangleNum; i++) {
            const int j = i * 24;
            vArray[j + 2] = normals[Triangles[i].vertex[0]].x;
            vArray[j + 3] = normals[Triangles[i].vertex[0]].y;
            vArray[j + 4] = normals[Triangles[i].vertex[0]].z;
            vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
            vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
            vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

            vArray[j + 10] = normals[Triangles[i].vertex[1]].x;
            vArray[j + 11] = normals[Triangles[i].vertex[1]].y;
            vArray[j + 12] = normals[Triangles[i].vertex[1]].z;
            vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
            vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
            vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

            vArray[j + 18] = normals[Triangles[i].vertex[2]].x;
            vArray[j + 19] = normals[Triangles[i].vertex[2]].y;
            vArray[j + 20] = normals[Triangles[i].vertex[2]].z;
            vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
            vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
            vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
        }
    if (flat)
        for (int i = 0; i < TriangleNum; i++) {
            const int j = i * 24;
            vArray[j + 2] = facenormals[i].x * -1;
            vArray[j + 3] = facenormals[i].y * -1;
            vArray[j + 4] = facenormals[i].z * -1;
            vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
            vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
            vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

            vArray[j + 10] = facenormals[i].x * -1;
            vArray[j + 11] = facenormals[i].y * -1;
            vArray[j + 12] = facenormals[i].z * -1;
            vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
            vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
            vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

            vArray[j + 18] = facenormals[i].x * -1;
            vArray[j + 19] = facenormals[i].y * -1;
            vArray[j + 20] = facenormals[i].z * -1;
            vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
            vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
            vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
        }
}

void Model::UpdateVertexArrayNoTexNoNorm()
{
    if (type != normaltype && type != decalstype)
        return;
    for (int i = 0; i < TriangleNum; i++) {
        const int j = i * 24;
        vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
        vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
        vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

        vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
        vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
        vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

        vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
        vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
        vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
    }
}

bool Model::loadnotex(const char *filename )
{
    FILE *tfile;

    //~ int oldvertexNum, oldTriangleNum;
    //~ oldvertexNum = vertexNum;
    //~ oldTriangleNum = TriangleNum;

    type = notextype;
    color = 0;

    tfile = fopen( ConvertFileName(filename), "rb" );
    // read model settings

    fseek(tfile, 0, SEEK_SET);
    funpackf(tfile, "Bs Bs", &vertexNum, &TriangleNum);

    // read the model data
    deallocate();

    numpossible = 0;

    owner = (int*)malloc(sizeof(int) * vertexNum);
    possible = (int*)malloc(sizeof(int) * TriangleNum);
    vertex = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
    Triangles = (TexturedTriangle*)malloc(sizeof(TexturedTriangle) * TriangleNum);
    vArray = (GLfloat*)malloc(sizeof(GLfloat) * TriangleNum * 24);

    for (int i = 0; i < vertexNum; i++) {
        float tmpx, tmpy, tmpz;
        funpackf(tfile, "Bf Bf Bf", &tmpx, &tmpy, &tmpz);
        vertex[i] = XYZ{tmpx, tmpy, tmpz};
    }

    for (int i = 0; i < TriangleNum; i++) {
        //funpackf(tfile, "Bi Bi Bi", &Triangles[i].vertex[0], &Triangles[i].vertex[1], &Triangles[i].vertex[2]);
        short vertex[ 6];
        funpackf(tfile, "Bs Bs Bs Bs Bs Bs", &vertex[ 0], &vertex[ 1], &vertex[ 2], &vertex[ 3], &vertex[ 4], &vertex[ 5]);
        Triangles[i].vertex[ 0] = vertex[ 0];
        Triangles[i].vertex[ 1] = vertex[ 2];
        Triangles[i].vertex[ 2] = vertex[ 4];
        funpackf(tfile, "Bf Bf Bf", &Triangles[i].gx[0], &Triangles[i].gx[1], &Triangles[i].gx[2]);
        funpackf(tfile, "Bf Bf Bf", &Triangles[i].gy[0], &Triangles[i].gy[1], &Triangles[i].gy[2]);
    }

    fclose(tfile);

    UpdateVertexArray();

    for (int i = 0; i < vertexNum; i++) {
        owner[i] = -1;
    }

    UpdateBoundingSphere();

    return 1;
}


bool Model::load(const char *filename, bool texture )
{
    FILE *tfile;

    LOGFUNC;

    LOG(std::string("Loading model...") + filename);

    if (visibleloading)
        Game::LoadingScreen();

    //~ int oldvertexNum, oldTriangleNum;
    //~ oldvertexNum = vertexNum;
    //~ oldTriangleNum = TriangleNum;

    type = normaltype;
    color = 0;

    tfile = fopen( ConvertFileName(filename), "rb" );
    // read model settings


    fseek(tfile, 0, SEEK_SET);
    funpackf(tfile, "Bs Bs", &vertexNum, &TriangleNum);

    // read the model data
    deallocate();

    numpossible = 0;

    owner = (int*)malloc(sizeof(int) * vertexNum);
    possible = (int*)malloc(sizeof(int) * TriangleNum);
    vertex = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
    normals = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
    facenormals = (XYZ*)malloc(sizeof(XYZ) * TriangleNum);
    Triangles = (TexturedTriangle*)malloc(sizeof(TexturedTriangle) * TriangleNum);
    vArray = (GLfloat*)malloc(sizeof(GLfloat) * TriangleNum * 24);

    for (int i = 0; i < vertexNum; i++) {
        float tmpx, tmpy, tmpz;
        funpackf(tfile, "Bf Bf Bf", &tmpx, &tmpy, &tmpz);
        vertex[i] = XYZ{tmpx, tmpy, tmpz};
    }

    for (int i = 0; i < TriangleNum; i++) {
        //funpackf(tfile, "Bi Bi Bi", &Triangles[i].vertex[0], &Triangles[i].vertex[1], &Triangles[i].vertex[2]);
        short vertex[ 6];
        funpackf(tfile, "Bs Bs Bs Bs Bs Bs", &vertex[ 0], &vertex[ 1], &vertex[ 2], &vertex[ 3], &vertex[ 4], &vertex[ 5]);
        Triangles[i].vertex[ 0] = vertex[ 0];
        Triangles[i].vertex[ 1] = vertex[ 2];
        Triangles[i].vertex[ 2] = vertex[ 4];
        funpackf(tfile, "Bf Bf Bf", &Triangles[i].gx[0], &Triangles[i].gx[1], &Triangles[i].gx[2]);
        funpackf(tfile, "Bf Bf Bf", &Triangles[i].gy[0], &Triangles[i].gy[1], &Triangles[i].gy[2]);
    }

    modelTexture.xsz = 0;

    fclose(tfile);

    UpdateVertexArray();

    for (int i = 0; i < vertexNum; i++) {
        owner[i] = -1;
    }

    UpdateBoundingSphere();

    return 1;
}

bool Model::loaddecal(const char *filename, bool texture )
{
    FILE *tfile;

    LOGFUNC;

    // Changing the filename so that its more os specific
    char * FixedFN = ConvertFileName(filename);

    LOG(std::string("Loading decal...") + FixedFN);

    //~ int oldvertexNum, oldTriangleNum;
    //~ oldvertexNum = vertexNum;
    //~ oldTriangleNum = TriangleNum;

    type = decalstype;
    numdecals = 0;
    color = 0;

    tfile = fopen( FixedFN, "rb" );
    // read model settings


    fseek(tfile, 0, SEEK_SET);
    funpackf(tfile, "Bs Bs", &vertexNum, &TriangleNum);

    // read the model data

    deallocate();

    numpossible = 0;

    owner = (int*)malloc(sizeof(int) * vertexNum);
    possible = (int*)malloc(sizeof(int) * TriangleNum);
    vertex = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
    normals = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
    facenormals = (XYZ*)malloc(sizeof(XYZ) * TriangleNum);
    Triangles = (TexturedTriangle*)malloc(sizeof(TexturedTriangle) * TriangleNum);
    vArray = (GLfloat*)malloc(sizeof(GLfloat) * TriangleNum * 24);


    for (int i = 0; i < vertexNum; i++) {
        float tmpx, tmpy, tmpz;
        funpackf(tfile, "Bf Bf Bf", &tmpx, &tmpy, &tmpz);
        vertex[i] = XYZ{tmpx, tmpy, tmpz};
    }

    for (int i = 0; i < TriangleNum; i++) {
        //funpackf(tfile, "Bi Bi Bi", &Triangles[i].vertex[0], &Triangles[i].vertex[1], &Triangles[i].vertex[2]);
        short vertex[ 6];
        funpackf(tfile, "Bs Bs Bs Bs Bs Bs", &vertex[ 0], &vertex[ 1], &vertex[ 2], &vertex[ 3], &vertex[ 4], &vertex[ 5]);
        Triangles[i].vertex[ 0] = vertex[ 0];
        Triangles[i].vertex[ 1] = vertex[ 2];
        Triangles[i].vertex[ 2] = vertex[ 4];
        funpackf(tfile, "Bf Bf Bf", &Triangles[i].gx[0], &Triangles[i].gx[1], &Triangles[i].gx[2]);
        funpackf(tfile, "Bf Bf Bf", &Triangles[i].gy[0], &Triangles[i].gy[1], &Triangles[i].gy[2]);
    }


    modelTexture.xsz = 0;

    fclose(tfile);

    UpdateVertexArray();

    for (int i = 0; i < vertexNum; i++) {
        owner[i] = -1;
    }

    UpdateBoundingSphere();

    //allow decals
    if (!decaltexcoords) {
        decaltexcoords = (float***)malloc(sizeof(float**)*max_model_decals);
        for (int i = 0; i < max_model_decals; i++) {
            decaltexcoords[i] = (float**)malloc(sizeof(float*) * 3);
            for (int j = 0; j < 3; j++) {
                decaltexcoords[i][j] = (float*)malloc(sizeof(float) * 2);
            }
        }
        //if(decalvertex)free(decalvertex);
        decalvertex = (XYZ**)malloc(sizeof(XYZ*)*max_model_decals);
        for (int i = 0; i < max_model_decals; i++) {
            decalvertex[i] = (XYZ*)malloc(sizeof(XYZ) * 3);
        }

        decaltype = (int*)malloc(sizeof(int) * max_model_decals);
        decalopacity = (float*)malloc(sizeof(float) * max_model_decals);
        decalrotation = (float*)malloc(sizeof(float) * max_model_decals);
        decalalivetime = (float*)malloc(sizeof(float) * max_model_decals);
        decalposition = (XYZ*)malloc(sizeof(XYZ) * max_model_decals);
    }

    return 1;
}

bool Model::loadraw(char *filename )
{
    FILE *tfile;

    LOGFUNC;

    LOG(std::string("Loading raw...") + filename);

    //~ int oldvertexNum, oldTriangleNum;
    //~ oldvertexNum = vertexNum;
    //~ oldTriangleNum = TriangleNum;

    type = rawtype;
    color = 0;

    tfile = fopen( ConvertFileName(filename), "rb" );
    // read model settings


    fseek(tfile, 0, SEEK_SET);
    funpackf(tfile, "Bs Bs", &vertexNum, &TriangleNum);

    // read the model data
    deallocate();

    numpossible = 0;

    owner = (int*)malloc(sizeof(int) * vertexNum);
    possible = (int*)malloc(sizeof(int) * TriangleNum);
    vertex = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
    Triangles = (TexturedTriangle*)malloc(sizeof(TexturedTriangle) * TriangleNum);
    vArray = (GLfloat*)malloc(sizeof(GLfloat) * TriangleNum * 24);


    for (int i = 0; i < vertexNum; i++) {
        float tmpx, tmpy, tmpz;
        funpackf(tfile, "Bf Bf Bf", &tmpx,&tmpy,&tmpz);
        vertex[i] = XYZ{tmpx, tmpy, tmpz};
    }

    for (int i = 0; i < TriangleNum; i++) {
        //funpackf(tfile, "Bi Bi Bi", &Triangles[i].vertex[0], &Triangles[i].vertex[1], &Triangles[i].vertex[2]);
        short vertex[ 6];
        funpackf(tfile, "Bs Bs Bs Bs Bs Bs", &vertex[ 0], &vertex[ 1], &vertex[ 2], &vertex[ 3], &vertex[ 4], &vertex[ 5]);
        Triangles[i].vertex[ 0] = vertex[ 0];
        Triangles[i].vertex[ 1] = vertex[ 2];
        Triangles[i].vertex[ 2] = vertex[ 4];
        funpackf(tfile, "Bf Bf Bf", &Triangles[i].gx[0], &Triangles[i].gx[1], &Triangles[i].gx[2]);
        funpackf(tfile, "Bf Bf Bf", &Triangles[i].gy[0], &Triangles[i].gy[1], &Triangles[i].gy[2]);
    }


    fclose(tfile);

    for (int i = 0; i < vertexNum; i++) {
        owner[i] = -1;
    }

    return 1;
}


void Model::UniformTexCoords()
{
    for (int i = 0; i < TriangleNum; i++) {
        Triangles[i].gy[0] = vertex[Triangles[i].vertex[0]].y;
        Triangles[i].gy[1] = vertex[Triangles[i].vertex[1]].y;
        Triangles[i].gy[2] = vertex[Triangles[i].vertex[2]].y;
        Triangles[i].gx[0] = vertex[Triangles[i].vertex[0]].x;
        Triangles[i].gx[1] = vertex[Triangles[i].vertex[1]].x;
        Triangles[i].gx[2] = vertex[Triangles[i].vertex[2]].x;
    }
    UpdateVertexArray();
}


void Model::FlipTexCoords()
{
    for (int i = 0; i < TriangleNum; i++) {
        Triangles[i].gy[0] = -Triangles[i].gy[0];
        Triangles[i].gy[1] = -Triangles[i].gy[1];
        Triangles[i].gy[2] = -Triangles[i].gy[2];
    }
    UpdateVertexArray();
}

void Model::ScaleTexCoords(float howmuch)
{
    for (int i = 0; i < TriangleNum; i++) {
        Triangles[i].gx[0] *= howmuch;
        Triangles[i].gx[1] *= howmuch;
        Triangles[i].gx[2] *= howmuch;
        Triangles[i].gy[0] *= howmuch;
        Triangles[i].gy[1] *= howmuch;
        Triangles[i].gy[2] *= howmuch;
    }
    UpdateVertexArray();
}

void Model::Scale(float xscale, float yscale, float zscale)
{
    for (int i = 0; i < vertexNum; i++) {
        vertex[i] *= {xscale, yscale, zscale};
    }
    UpdateVertexArray();
    UpdateBoundingSphere();
}

void Model::ScaleNormals(float xscale, float yscale, float zscale)
{
	if(type!=normaltype&&type!=decalstype)
        return;
	for (int i = 0; i < vertexNum; i++) {
		normals[i] *= {xscale, yscale, zscale};
	}
	for (int i = 0; i < TriangleNum; i++) {
		facenormals[i] *= {xscale, yscale, zscale};
	}
	UpdateVertexArray();
}

void Model::Translate(float xtrans, float ytrans, float ztrans)
{
    for (int i = 0; i < vertexNum; i++) {
        vertex[i] += {xtrans, ytrans, ztrans};
    }
    UpdateVertexArray();
    UpdateBoundingSphere();
}

void Model::Rotate(float xang, float yang, float zang)
{
    for (int i = 0; i < vertexNum; i++) {
        vertex[i] = DoRotation(vertex[i], xang, yang, zang);
    }
    UpdateVertexArray();
    UpdateBoundingSphere();
}


void Model::CalculateNormals(bool facenormalise)
{
    if (visibleloading)
        Game::LoadingScreen();
    if (type != normaltype && type != decalstype)
        return;

    for (int i = 0; i < vertexNum; i++) {
        normals[i].x = 0;
        normals[i].y = 0;
        normals[i].z = 0;
    }

    for (int i = 0; i < TriangleNum; i++) {
        XYZ l_vect_b1 = vertex[Triangles[i].vertex[1]] - vertex[Triangles[i].vertex[0]];
        XYZ l_vect_b2 = vertex[Triangles[i].vertex[2]] - vertex[Triangles[i].vertex[0]];
        facenormals[i] = cross(l_vect_b1, l_vect_b2);

        normals[Triangles[i].vertex[0]] += facenormals[i];
        normals[Triangles[i].vertex[1]] += facenormals[i];
        normals[Triangles[i].vertex[2]] += facenormals[i];
        
        if (facenormalise)
            Normalise(facenormals[i]);
    }
    for (int i = 0; i < vertexNum; i++) {
        Normalise(normals[i]);
        normals[i] *= -1;
    }
    UpdateVertexArrayNoTex();
}

void Model::drawimmediate()
{
    textureptr.bind();
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < TriangleNum; i++) {
        /*if(Triangles[i].vertex[0]<vertexNum&&Triangles[i].vertex[1]<vertexNum&&Triangles[i].vertex[2]<vertexNum&&Triangles[i].vertex[0]>=0&&Triangles[i].vertex[1]>=0&&Triangles[i].vertex[2]>=0){
        if(isnormal(vertex[Triangles[i].vertex[0]].x)&&isnormal(vertex[Triangles[i].vertex[0]].y)&&isnormal(vertex[Triangles[i].vertex[0]].z)
        &&isnormal(vertex[Triangles[i].vertex[1]].x)&&isnormal(vertex[Triangles[i].vertex[1]].y)&&isnormal(vertex[Triangles[i].vertex[1]].z)
        &&isnormal(vertex[Triangles[i].vertex[2]].x)&&isnormal(vertex[Triangles[i].vertex[2]].y)&&isnormal(vertex[Triangles[i].vertex[2]].z)){
        */
        glTexCoord2f(Triangles[i].gx[0], Triangles[i].gy[0]);
        if (color)
            glColor3f(normals[Triangles[i].vertex[0]].x, normals[Triangles[i].vertex[0]].y, normals[Triangles[i].vertex[0]].z);
        if (!color && !flat)
            glNormal3f(normals[Triangles[i].vertex[0]].x, normals[Triangles[i].vertex[0]].y, normals[Triangles[i].vertex[0]].z);
        if (!color && flat)
            glNormal3f(facenormals[i].x, facenormals[i].y, facenormals[i].y);
        glVertex3f(vertex[Triangles[i].vertex[0]].x, vertex[Triangles[i].vertex[0]].y, vertex[Triangles[i].vertex[0]].z);

        glTexCoord2f(Triangles[i].gx[1], Triangles[i].gy[1]);
        if (color)
            glColor3f(normals[Triangles[i].vertex[1]].x, normals[Triangles[i].vertex[1]].y, normals[Triangles[i].vertex[1]].z);
        if (!color && !flat)
            glNormal3f(normals[Triangles[i].vertex[1]].x, normals[Triangles[i].vertex[1]].y, normals[Triangles[i].vertex[1]].z);
        if (!color && flat)
            glNormal3f(facenormals[i].x, facenormals[i].y, facenormals[i].y);
        glVertex3f(vertex[Triangles[i].vertex[1]].x, vertex[Triangles[i].vertex[1]].y, vertex[Triangles[i].vertex[1]].z);

        glTexCoord2f(Triangles[i].gx[2], Triangles[i].gy[2]);
        if (color)
            glColor3f(normals[Triangles[i].vertex[2]].x, normals[Triangles[i].vertex[2]].y, normals[Triangles[i].vertex[2]].z);
        if (!color && !flat)
            glNormal3f(normals[Triangles[i].vertex[2]].x, normals[Triangles[i].vertex[2]].y, normals[Triangles[i].vertex[2]].z);
        if (!color && flat)
            glNormal3f(facenormals[i].x, facenormals[i].y, facenormals[i].y);
        glVertex3f(vertex[Triangles[i].vertex[2]].x, vertex[Triangles[i].vertex[2]].y, vertex[Triangles[i].vertex[2]].z);
        //}
        //}
    }
    glEnd();
}

void Model::draw()
{
    if (type != normaltype && type != decalstype)
        return;

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if (!color)
        glInterleavedArrays( GL_T2F_N3F_V3F, 8 * sizeof(GLfloat), &vArray[0]);
    if (color)
        glInterleavedArrays( GL_T2F_C3F_V3F, 8 * sizeof(GLfloat), &vArray[0]);
    textureptr.bind();

#if PLATFORM_MACOSX
    glLockArraysEXT( 0, TriangleNum * 3);
#endif
    glDrawArrays(GL_TRIANGLES, 0, TriangleNum * 3);
#if PLATFORM_MACOSX
    glUnlockArraysEXT();
#endif


    if (!color)
        glDisableClientState(GL_NORMAL_ARRAY);
    if (color)
        glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //drawimmediate();
}

//TODO: phase out in favor of Texture
void Model::drawdifftex(GLuint texture)
{
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    if (!color)
        glInterleavedArrays( GL_T2F_N3F_V3F, 8 * sizeof(GLfloat), &vArray[0]);
    if (color)
        glInterleavedArrays( GL_T2F_C3F_V3F, 8 * sizeof(GLfloat), &vArray[0]);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );


#ifndef WIN32
    glLockArraysEXT( 0, TriangleNum * 3);
#endif
    glDrawArrays(GL_TRIANGLES, 0, TriangleNum * 3);
#ifndef WIN32
    glUnlockArraysEXT();
#endif


    if (!color)
        glDisableClientState(GL_NORMAL_ARRAY);
    if (color)
        glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //drawdiffteximmediate(texture);
}

void Model::drawdifftex(Texture texture)
{
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    if (!color)
        glInterleavedArrays( GL_T2F_N3F_V3F, 8 * sizeof(GLfloat), &vArray[0]);
    if (color)
        glInterleavedArrays( GL_T2F_C3F_V3F, 8 * sizeof(GLfloat), &vArray[0]);

    texture.bind();
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );


#ifndef WIN32
    glLockArraysEXT( 0, TriangleNum * 3);
#endif
    glDrawArrays(GL_TRIANGLES, 0, TriangleNum * 3);
#ifndef WIN32
    glUnlockArraysEXT();
#endif


    if (!color)
        glDisableClientState(GL_NORMAL_ARRAY);
    if (color)
        glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //drawdiffteximmediate(texture);
}

void Model::drawdiffteximmediate(GLuint texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < TriangleNum; i++) {
        /*if(Triangles[i].vertex[0]<vertexNum&&Triangles[i].vertex[1]<vertexNum&&Triangles[i].vertex[2]<vertexNum&&Triangles[i].vertex[0]>=0&&Triangles[i].vertex[1]>=0&&Triangles[i].vertex[2]>=0){
        if(isnormal(vertex[Triangles[i].vertex[0]].x)&&isnormal(vertex[Triangles[i].vertex[0]].y)&&isnormal(vertex[Triangles[i].vertex[0]].z)
        &&isnormal(vertex[Triangles[i].vertex[1]].x)&&isnormal(vertex[Triangles[i].vertex[1]].y)&&isnormal(vertex[Triangles[i].vertex[1]].z)
        &&isnormal(vertex[Triangles[i].vertex[2]].x)&&isnormal(vertex[Triangles[i].vertex[2]].y)&&isnormal(vertex[Triangles[i].vertex[2]].z)){
        */glTexCoord2f(Triangles[i].gx[0], Triangles[i].gy[0]);
        if (color)
            glColor3f(normals[Triangles[i].vertex[0]].x, normals[Triangles[i].vertex[0]].y, normals[Triangles[i].vertex[0]].z);
        if (!color && !flat)
            glNormal3f(normals[Triangles[i].vertex[0]].x, normals[Triangles[i].vertex[0]].y, normals[Triangles[i].vertex[0]].z);
        if (!color && flat)
            glNormal3f(facenormals[i].x, facenormals[i].y, facenormals[i].y);
        glVertex3f(vertex[Triangles[i].vertex[0]].x, vertex[Triangles[i].vertex[0]].y, vertex[Triangles[i].vertex[0]].z);

        glTexCoord2f(Triangles[i].gx[1], Triangles[i].gy[1]);
        if (color)
            glColor3f(normals[Triangles[i].vertex[1]].x, normals[Triangles[i].vertex[1]].y, normals[Triangles[i].vertex[1]].z);
        if (!color && !flat)
            glNormal3f(normals[Triangles[i].vertex[1]].x, normals[Triangles[i].vertex[1]].y, normals[Triangles[i].vertex[1]].z);
        if (!color && flat)
            glNormal3f(facenormals[i].x, facenormals[i].y, facenormals[i].y);
        glVertex3f(vertex[Triangles[i].vertex[1]].x, vertex[Triangles[i].vertex[1]].y, vertex[Triangles[i].vertex[1]].z);

        glTexCoord2f(Triangles[i].gx[2], Triangles[i].gy[2]);
        if (color)
            glColor3f(normals[Triangles[i].vertex[2]].x, normals[Triangles[i].vertex[2]].y, normals[Triangles[i].vertex[2]].z);
        if (!color && !flat)
            glNormal3f(normals[Triangles[i].vertex[2]].x, normals[Triangles[i].vertex[2]].y, normals[Triangles[i].vertex[2]].z);
        if (!color && flat)
            glNormal3f(facenormals[i].x, facenormals[i].y, facenormals[i].y);
        glVertex3f(vertex[Triangles[i].vertex[2]].x, vertex[Triangles[i].vertex[2]].y, vertex[Triangles[i].vertex[2]].z);
        //}
        //}
    }
    glEnd();
}

void Model::drawdecals(Texture shadowtexture, Texture bloodtexture, Texture bloodtexture2, Texture breaktexture)
{
    if (decals) {
        if (type != decalstype)
            return;
        //~ static float distancemult;
        int lasttype = -1;
        //~ static float viewdistsquared;
        bool blend = true;

        //~ viewdistsquared = viewdistance * viewdistance;

        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(0);
        if (numdecals > max_model_decals)
            numdecals = max_model_decals;
        for (int i = 0; i < numdecals; i++) {
            if (decaltype[i] == blooddecalfast && decalalivetime[i] < 2)
                decalalivetime[i] = 2;

            if (decaltype[i] == shadowdecal && decaltype[i] != lasttype) {
                shadowtexture.bind();
                if (!blend) {
                    blend = 1;
                    glAlphaFunc(GL_GREATER, 0.0001);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                }
            }
            if (decaltype[i] == breakdecal && decaltype[i] != lasttype) {
                breaktexture.bind();
                if (!blend) {
                    blend = 1;
                    glAlphaFunc(GL_GREATER, 0.0001);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                }
            }
            if ((decaltype[i] == blooddecal || decaltype[i] == blooddecalslow) && decaltype[i] != lasttype) {
                bloodtexture.bind();
                if (blend) {
                    blend = 0;
                    glAlphaFunc(GL_GREATER, 0.15);
                    glBlendFunc(GL_ONE, GL_ZERO);
                }
            }
            if ((decaltype[i] == blooddecalfast) && decaltype[i] != lasttype) {
                bloodtexture2.bind();
                if (blend) {
                    blend = 0;
                    glAlphaFunc(GL_GREATER, 0.15);
                    glBlendFunc(GL_ONE, GL_ZERO);
                }
            }
            if (decaltype[i] == shadowdecal) {
                glColor4f(1, 1, 1, decalopacity[i]);
            }
            if (decaltype[i] == breakdecal) {
                glColor4f(1, 1, 1, decalopacity[i]);
                if (decalalivetime[i] > 58)
                    glColor4f(1, 1, 1, decalopacity[i] * (60 - decalalivetime[i]) / 2);
            }
            if ((decaltype[i] == blooddecal || decaltype[i] == blooddecalfast || decaltype[i] == blooddecalslow)) {
                glColor4f(1, 1, 1, decalopacity[i]);
                if (decalalivetime[i] < 4)
                    glColor4f(1, 1, 1, decalopacity[i]*decalalivetime[i]*.25);
                if (decalalivetime[i] > 58)
                    glColor4f(1, 1, 1, decalopacity[i] * (60 - decalalivetime[i]) / 2);
            }
            lasttype = decaltype[i];
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glBegin(GL_TRIANGLES);
            for (int j = 0; j < 3; j++) {
                glTexCoord2f(decaltexcoords[i][j][0], decaltexcoords[i][j][1]);
                glVertex3f(decalvertex[i][j].x, decalvertex[i][j].y, decalvertex[i][j].z);
            }
            glEnd();
            glPopMatrix();
        }
        for (int i = numdecals - 1; i >= 0; i--) {
            decalalivetime[i] += multiplier;
            if (decaltype[i] == blooddecalslow)
                decalalivetime[i] -= multiplier * 2 / 3;
            if (decaltype[i] == blooddecalfast)
                decalalivetime[i] += multiplier * 4;
            if (decaltype[i] == shadowdecal)
                DeleteDecal(i);
            if ((decaltype[i] == blooddecal || decaltype[i] == blooddecalfast || decaltype[i] == blooddecalslow) && decalalivetime[i] >= 60)
                DeleteDecal(i);
        }
        glAlphaFunc(GL_GREATER, 0.0001);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void Model::DeleteDecal(int which)
{
    if (decals) {
        if (type != decalstype)
            return;
        decaltype[which] = decaltype[numdecals - 1];
        decalposition[which] = decalposition[numdecals - 1];
        for (int i = 0; i < 3; i++) {
            decalvertex[which][i] = decalvertex[numdecals - 1][i];
            decaltexcoords[which][i][0] = decaltexcoords[numdecals - 1][i][0];
            decaltexcoords[which][i][1] = decaltexcoords[numdecals - 1][i][1];
        }
        decalrotation[which] = decalrotation[numdecals - 1];
        decalalivetime[which] = decalalivetime[numdecals - 1];
        decalopacity[which] = decalopacity[numdecals - 1];
        numdecals--;
    }
}

void Model::MakeDecal(int atype, XYZ *where,float *size, float *opacity, const float rotation)
{
    if (decals) {
        if (type != decalstype)
            return;

        float placex, placez;
        XYZ rot;
        //static XYZ point,point1,point2;
        float distance;

        if (*opacity > 0) {
            if (distsq(where, &boundingspherecenter) < (boundingsphereradius + *size) * (boundingsphereradius + *size)) {
                for (int i = 0; i < TriangleNum; i++) {
                    if (facenormals[i].y < -.1 && (vertex[Triangles[i].vertex[0]].y < where->y || vertex[Triangles[i].vertex[1]].y < where->y || vertex[Triangles[i].vertex[2]].y < where->y)) {
                        decalposition[numdecals] = *where;
                        decaltype[numdecals] = atype;
                        decalrotation[numdecals] = rotation;
                        decalalivetime[numdecals] = 0;
                        distance = abs(((facenormals[i].x * where->x) + (facenormals[i].y * where->y) + (facenormals[i].z * where->z) - ((facenormals[i].x * vertex[Triangles[i].vertex[0]].x) + (facenormals[i].y * vertex[Triangles[i].vertex[0]].y) + (facenormals[i].z * vertex[Triangles[i].vertex[0]].z))) / facenormals[i].y);
                        decalopacity[numdecals] = *opacity - distance / 10;

                        if (decalopacity[numdecals > 0]) {
                            placex = vertex[Triangles[i].vertex[0]].x;
                            placez = vertex[Triangles[i].vertex[0]].z;

                            decaltexcoords[numdecals][0][0] = (placex - where->x) / (*size) / 2 + .5;
                            decaltexcoords[numdecals][0][1] = (placez - where->z) / (*size) / 2 + .5;

                            decalvertex[numdecals][0].x = placex;
                            decalvertex[numdecals][0].z = placez;
                            decalvertex[numdecals][0].y = vertex[Triangles[i].vertex[0]].y;


                            placex = vertex[Triangles[i].vertex[1]].x;
                            placez = vertex[Triangles[i].vertex[1]].z;

                            decaltexcoords[numdecals][1][0] = (placex - where->x) / (*size) / 2 + .5;
                            decaltexcoords[numdecals][1][1] = (placez - where->z) / (*size) / 2 + .5;

                            decalvertex[numdecals][1].x = placex;
                            decalvertex[numdecals][1].z = placez;
                            decalvertex[numdecals][1].y = vertex[Triangles[i].vertex[1]].y;


                            placex = vertex[Triangles[i].vertex[2]].x;
                            placez = vertex[Triangles[i].vertex[2]].z;

                            decaltexcoords[numdecals][2][0] = (placex - where->x) / (*size) / 2 + .5;
                            decaltexcoords[numdecals][2][1] = (placez - where->z) / (*size) / 2 + .5;

                            decalvertex[numdecals][2].x = placex;
                            decalvertex[numdecals][2].z = placez;
                            decalvertex[numdecals][2].y = vertex[Triangles[i].vertex[2]].y;

                            if ((!(decaltexcoords[numdecals][0][0] < 0 && decaltexcoords[numdecals][1][0] < 0 && decaltexcoords[numdecals][2][0] < 0))
                                && (!(decaltexcoords[numdecals][0][1] < 0 && decaltexcoords[numdecals][1][1] < 0 && decaltexcoords[numdecals][2][1] < 0))
                                    && (!(decaltexcoords[numdecals][0][0] > 1 && decaltexcoords[numdecals][1][0] > 1 && decaltexcoords[numdecals][2][0] > 1))
                                        && (!(decaltexcoords[numdecals][0][1] > 1 && decaltexcoords[numdecals][1][1] > 1 && decaltexcoords[numdecals][2][1] > 1))) {
                                            if (decalrotation[numdecals]) {
                                                for (int j = 0; j < 3; j++) {
                                                    rot.y = 0;
                                                    rot.x = decaltexcoords[numdecals][j][0] - .5;
                                                    rot.z = decaltexcoords[numdecals][j][1] - .5;
                                                    rot = DoRotation(rot, 0, -decalrotation[numdecals], 0);
                                                    decaltexcoords[numdecals][j][0] = rot.x + .5;
                                                    decaltexcoords[numdecals][j][1] = rot.z + .5;
                                                }
                                            }
                                            if (numdecals < max_model_decals - 1)
                                                numdecals++;
                                        }
                        }
                    }
                }
            }
        }
    }
}

void Model::MakeDecal(int atype, XYZ where, float size, float opacity, float rotation)
{
    if (decals) {
        if (type != decalstype)
            return;

        float placex, placez;
        XYZ rot;
        //static XYZ point,point1,point2;
        float distance;

        if (opacity > 0)
            if (distsq(&where, &boundingspherecenter) < (boundingsphereradius + size) * (boundingsphereradius + size))
                for (int i = 0; i < TriangleNum; i++) {
                    distance = abs(((facenormals[i].x * where.x) + (facenormals[i].y * where.y) + (facenormals[i].z * where.z) - ((facenormals[i].x * vertex[Triangles[i].vertex[0]].x) + (facenormals[i].y * vertex[Triangles[i].vertex[0]].y) + (facenormals[i].z * vertex[Triangles[i].vertex[0]].z))));
                    if (distance < .02 && abs(facenormals[i].y) > abs(facenormals[i].x) && abs(facenormals[i].y) > abs(facenormals[i].z)) {
                        decalposition[numdecals] = where;
                        decaltype[numdecals] = atype;
                        decalrotation[numdecals] = rotation;
                        decalalivetime[numdecals] = 0;
                        decalopacity[numdecals] = opacity - distance / 10;

                        if (decalopacity[numdecals > 0]) {
                            placex = vertex[Triangles[i].vertex[0]].x;
                            placez = vertex[Triangles[i].vertex[0]].z;

                            decaltexcoords[numdecals][0][0] = (placex - where.x) / (size) / 2 + .5;
                            decaltexcoords[numdecals][0][1] = (placez - where.z) / (size) / 2 + .5;

                            decalvertex[numdecals][0].x = placex;
                            decalvertex[numdecals][0].z = placez;
                            decalvertex[numdecals][0].y = vertex[Triangles[i].vertex[0]].y;


                            placex = vertex[Triangles[i].vertex[1]].x;
                            placez = vertex[Triangles[i].vertex[1]].z;

                            decaltexcoords[numdecals][1][0] = (placex - where.x) / (size) / 2 + .5;
                            decaltexcoords[numdecals][1][1] = (placez - where.z) / (size) / 2 + .5;

                            decalvertex[numdecals][1].x = placex;
                            decalvertex[numdecals][1].z = placez;
                            decalvertex[numdecals][1].y = vertex[Triangles[i].vertex[1]].y;


                            placex = vertex[Triangles[i].vertex[2]].x;
                            placez = vertex[Triangles[i].vertex[2]].z;

                            decaltexcoords[numdecals][2][0] = (placex - where.x) / (size) / 2 + .5;
                            decaltexcoords[numdecals][2][1] = (placez - where.z) / (size) / 2 + .5;

                            decalvertex[numdecals][2].x = placex;
                            decalvertex[numdecals][2].z = placez;
                            decalvertex[numdecals][2].y = vertex[Triangles[i].vertex[2]].y;

                            if (!(decaltexcoords[numdecals][0][0] < 0 && decaltexcoords[numdecals][1][0] < 0 && decaltexcoords[numdecals][2][0] < 0))
                                if (!(decaltexcoords[numdecals][0][1] < 0 && decaltexcoords[numdecals][1][1] < 0 && decaltexcoords[numdecals][2][1] < 0))
                                    if (!(decaltexcoords[numdecals][0][0] > 1 && decaltexcoords[numdecals][1][0] > 1 && decaltexcoords[numdecals][2][0] > 1))
                                        if (!(decaltexcoords[numdecals][0][1] > 1 && decaltexcoords[numdecals][1][1] > 1 && decaltexcoords[numdecals][2][1] > 1)) {
                                            if (decalrotation[numdecals]) {
                                                for (int j = 0; j < 3; j++) {
                                                    rot.y = 0;
                                                    rot.x = decaltexcoords[numdecals][j][0] - .5;
                                                    rot.z = decaltexcoords[numdecals][j][1] - .5;
                                                    rot = DoRotation(rot, 0, -decalrotation[numdecals], 0);
                                                    decaltexcoords[numdecals][j][0] = rot.x + .5;
                                                    decaltexcoords[numdecals][j][1] = rot.z + .5;
                                                }
                                            }
                                            if (numdecals < max_model_decals - 1)
                                                numdecals++;
                                        }
                        }
                    } else if (distance < .02 && abs(facenormals[i].x) > abs(facenormals[i].y) && abs(facenormals[i].x) > abs(facenormals[i].z)) {
                        decalposition[numdecals] = where;
                        decaltype[numdecals] = atype;
                        decalrotation[numdecals] = rotation;
                        decalalivetime[numdecals] = 0;
                        decalopacity[numdecals] = opacity - distance / 10;

                        if (decalopacity[numdecals > 0]) {
                            placex = vertex[Triangles[i].vertex[0]].y;
                            placez = vertex[Triangles[i].vertex[0]].z;

                            decaltexcoords[numdecals][0][0] = (placex - where.y) / (size) / 2 + .5;
                            decaltexcoords[numdecals][0][1] = (placez - where.z) / (size) / 2 + .5;

                            decalvertex[numdecals][0].x = vertex[Triangles[i].vertex[0]].x;
                            decalvertex[numdecals][0].z = placez;
                            decalvertex[numdecals][0].y = placex;


                            placex = vertex[Triangles[i].vertex[1]].y;
                            placez = vertex[Triangles[i].vertex[1]].z;

                            decaltexcoords[numdecals][1][0] = (placex - where.y) / (size) / 2 + .5;
                            decaltexcoords[numdecals][1][1] = (placez - where.z) / (size) / 2 + .5;

                            decalvertex[numdecals][1].x = vertex[Triangles[i].vertex[1]].x;
                            decalvertex[numdecals][1].z = placez;
                            decalvertex[numdecals][1].y = placex;


                            placex = vertex[Triangles[i].vertex[2]].y;
                            placez = vertex[Triangles[i].vertex[2]].z;

                            decaltexcoords[numdecals][2][0] = (placex - where.y) / (size) / 2 + .5;
                            decaltexcoords[numdecals][2][1] = (placez - where.z) / (size) / 2 + .5;

                            decalvertex[numdecals][2].x = vertex[Triangles[i].vertex[2]].x;
                            decalvertex[numdecals][2].z = placez;
                            decalvertex[numdecals][2].y = placex;

                            if (!(decaltexcoords[numdecals][0][0] < 0 && decaltexcoords[numdecals][1][0] < 0 && decaltexcoords[numdecals][2][0] < 0))
                                if (!(decaltexcoords[numdecals][0][1] < 0 && decaltexcoords[numdecals][1][1] < 0 && decaltexcoords[numdecals][2][1] < 0))
                                    if (!(decaltexcoords[numdecals][0][0] > 1 && decaltexcoords[numdecals][1][0] > 1 && decaltexcoords[numdecals][2][0] > 1))
                                        if (!(decaltexcoords[numdecals][0][1] > 1 && decaltexcoords[numdecals][1][1] > 1 && decaltexcoords[numdecals][2][1] > 1)) {
                                            if (decalrotation[numdecals]) {
                                                for (int j = 0; j < 3; j++) {
                                                    rot.y = 0;
                                                    rot.x = decaltexcoords[numdecals][j][0] - .5;
                                                    rot.z = decaltexcoords[numdecals][j][1] - .5;
                                                    rot = DoRotation(rot, 0, -decalrotation[numdecals], 0);
                                                    decaltexcoords[numdecals][j][0] = rot.x + .5;
                                                    decaltexcoords[numdecals][j][1] = rot.z + .5;
                                                }
                                            }
                                            if (numdecals < max_model_decals - 1)
                                                numdecals++;
                                        }
                        }
                    } else if (distance < .02 && abs(facenormals[i].z) > abs(facenormals[i].y) && abs(facenormals[i].z) > abs(facenormals[i].x)) {
                        decalposition[numdecals] = where;
                        decaltype[numdecals] = atype;
                        decalrotation[numdecals] = rotation;
                        decalalivetime[numdecals] = 0;
                        decalopacity[numdecals] = opacity - distance / 10;

                        if (decalopacity[numdecals > 0]) {
                            placex = vertex[Triangles[i].vertex[0]].x;
                            placez = vertex[Triangles[i].vertex[0]].y;

                            decaltexcoords[numdecals][0][0] = (placex - where.x) / (size) / 2 + .5;
                            decaltexcoords[numdecals][0][1] = (placez - where.y) / (size) / 2 + .5;

                            decalvertex[numdecals][0].x = placex;
                            decalvertex[numdecals][0].z = vertex[Triangles[i].vertex[0]].z;
                            decalvertex[numdecals][0].y = placez;


                            placex = vertex[Triangles[i].vertex[1]].x;
                            placez = vertex[Triangles[i].vertex[1]].y;

                            decaltexcoords[numdecals][1][0] = (placex - where.x) / (size) / 2 + .5;
                            decaltexcoords[numdecals][1][1] = (placez - where.y) / (size) / 2 + .5;

                            decalvertex[numdecals][1].x = placex;
                            decalvertex[numdecals][1].z = vertex[Triangles[i].vertex[1]].z;
                            decalvertex[numdecals][1].y = placez;


                            placex = vertex[Triangles[i].vertex[2]].x;
                            placez = vertex[Triangles[i].vertex[2]].y;

                            decaltexcoords[numdecals][2][0] = (placex - where.x) / (size) / 2 + .5;
                            decaltexcoords[numdecals][2][1] = (placez - where.y) / (size) / 2 + .5;

                            decalvertex[numdecals][2].x = placex;
                            decalvertex[numdecals][2].z = vertex[Triangles[i].vertex[2]].z;
                            decalvertex[numdecals][2].y = placez;

                            if (!(decaltexcoords[numdecals][0][0] < 0 && decaltexcoords[numdecals][1][0] < 0 && decaltexcoords[numdecals][2][0] < 0))
                                if (!(decaltexcoords[numdecals][0][1] < 0 && decaltexcoords[numdecals][1][1] < 0 && decaltexcoords[numdecals][2][1] < 0))
                                    if (!(decaltexcoords[numdecals][0][0] > 1 && decaltexcoords[numdecals][1][0] > 1 && decaltexcoords[numdecals][2][0] > 1))
                                        if (!(decaltexcoords[numdecals][0][1] > 1 && decaltexcoords[numdecals][1][1] > 1 && decaltexcoords[numdecals][2][1] > 1)) {
                                            if (decalrotation[numdecals]) {
                                                for (int j = 0; j < 3; j++) {
                                                    rot.y = 0;
                                                    rot.x = decaltexcoords[numdecals][j][0] - .5;
                                                    rot.z = decaltexcoords[numdecals][j][1] - .5;
                                                    rot = DoRotation(rot, 0, -decalrotation[numdecals], 0);
                                                    decaltexcoords[numdecals][j][0] = rot.x + .5;
                                                    decaltexcoords[numdecals][j][1] = rot.z + .5;
                                                }
                                            }
                                            if (numdecals < max_model_decals - 1)
                                                numdecals++;
                                        }
                        }
                    }
                }
    }
}

Model::~Model()
{
    deallocate();
    textureptr.destroy();
}

void Model::deallocate()
{
    if (owner)
        free(owner);
    owner = 0;

    if (possible)
        free(possible);
    possible = 0;

    if (vertex)
        free(vertex);
    vertex = 0;

    if (normals)
        free(normals);
    normals = 0;

    if (facenormals)
        free(facenormals);
    facenormals = 0;

    if (Triangles)
        free(Triangles);
    Triangles = 0;

    if (vArray)
        free(vArray);
    vArray = 0;


    //allow decals
    if (decaltexcoords) {
        for (int i = 0; i < max_model_decals; i++) {
            for (int j = 0; j < 3; j++) {
                free(decaltexcoords[i][j]);
            }
            free(decaltexcoords[i]);
        }
        free(decaltexcoords);
    }
    decaltexcoords = 0;


    if (decalvertex) {
        for (int i = 0; i < max_model_decals; i++) {
            free(decalvertex[i]);
        }
        free(decalvertex);
    }
    decalvertex = 0;


    free(decaltype);
    decaltype = 0;

    free(decalopacity);
    decalopacity = 0;

    free(decalrotation);
    decalrotation = 0;

    free(decalalivetime);
    decalalivetime = 0;

    free(decalposition);
    decalposition = 0;

};

Model::Model()
{
    vertexNum = 0, TriangleNum = 0;
    hastexture = 0;

    type = ModelTypeNothing, oldtype = ModelTypeNothing;

    possible = 0;
    owner = 0;
    vertex = 0;
    normals = 0;
    facenormals = 0;
    Triangles = 0;
    vArray = 0;

    memset(&modelTexture, 0, sizeof(modelTexture));
    numpossible = 0;
    color = 0;

    boundingspherecenter = 0;
    boundingsphereradius = 0;

    decaltexcoords = 0;
    decalvertex = 0;
    decaltype = 0;
    decalopacity = 0;
    decalrotation = 0;
    decalalivetime = 0;
    decalposition = 0;

    numdecals = 0;

    flat = 0;

    type = nothing;
}

