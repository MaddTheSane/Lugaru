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

#include <cmath>
#include "Quaternions.h"

using namespace simd;
using namespace std;

// Functions
quaternion Quat_Mult(quaternion q1, quaternion q2)
{
    quaternion QResult;
    float a, b, c, d, e, f, g, h;
    a = (q1.w + q1.x) * (q2.w + q2.x);
    b = (q1.z - q1.y) * (q2.y - q2.z);
    c = (q1.w - q1.x) * (q2.y + q2.z);
    d = (q1.y + q1.z) * (q2.w - q2.x);
    e = (q1.x + q1.z) * (q2.x + q2.y);
    f = (q1.x - q1.z) * (q2.x - q2.y);
    g = (q1.w + q1.y) * (q2.w - q2.z);
    h = (q1.w - q1.y) * (q2.w + q2.z);
    QResult.w = b + (-e - f + g + h) / 2;
    QResult.x = a - (e + f + g + h) / 2;
    QResult.y = c + (e - f + g - h) / 2;
    QResult.z = d + (e - f - g + h) / 2;
    return QResult;
}



quaternion To_Quat(Matrix_t m)
{
    // From Jason Shankel, (C) 2000.
    quaternion Quat = {0};

    double Tr = m[0][0] + m[1][1] + m[2][2] + 1.0, fourD;
    double q[4] = {0, 0, 0, 0};

    if (Tr >= 1.0) {
        fourD = 2.0 * fast_sqrt(Tr);
        q[3] = fourD / 4.0;
        q[0] = (m[2][1] - m[1][2]) / fourD;
        q[1] = (m[0][2] - m[2][0]) / fourD;
        q[2] = (m[1][0] - m[0][1]) / fourD;
    } else {
        int i;
        if (m[0][0] > m[1][1]) {
            i = 0;
        } else {
            i = 1;
        }
        if (m[2][2] > m[i][i]) {
            i = 2;
        }
        int j = (i + 1) % 3;
        int k = (j + 1) % 3;
        fourD = 2.0 * fast_sqrt(m[i][i] - m[j][j] - m[k][k] + 1.0);
        q[i] = fourD / 4.0;
        q[j] = (m[j][i] + m[i][j]) / fourD;
        q[k] = (m[k][i] + m[i][k]) / fourD;
        q[3] = (m[j][k] - m[k][j]) / fourD;
    }

    Quat.x = q[0];
    Quat.y = q[1];
    Quat.z = q[2];
    Quat.w = q[3];
    return Quat;
}

void Quat_2_Matrix(quaternion Quat, Matrix_t m)
{
    // From the GLVelocity site (http://glvelocity.gamedev.net)
    float fW = Quat.w;
    float fX = Quat.x;
    float fY = Quat.y;
    float fZ = Quat.z;
    float fXX = fX * fX;
    float fYY = fY * fY;
    float fZZ = fZ * fZ;
    m[0][0] = 1.0f - 2.0f * (fYY + fZZ);
    m[1][0] = 2.0f * (fX * fY + fW * fZ);
    m[2][0] = 2.0f * (fX * fZ - fW * fY);
    m[3][0] = 0.0f;
    m[0][1] = 2.0f * (fX * fY - fW * fZ);
    m[1][1] = 1.0f - 2.0f * (fXX + fZZ);
    m[2][1] = 2.0f * (fY * fZ + fW * fX);
    m[3][1] = 0.0f;
    m[0][2] = 2.0f * (fX * fZ + fW * fY);
    m[1][2] = 2.0f * (fX * fZ - fW * fX);
    m[2][2] = 1.0f - 2.0f * (fXX + fYY);
    m[3][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}

quaternion To_Quat(angle_axis Ang_Ax)
{
    // From the Quaternion Powers article on gamedev.net
    static quaternion Quat;

    Quat.x = Ang_Ax.x * sin(Ang_Ax.angle / 2);
    Quat.y = Ang_Ax.y * sin(Ang_Ax.angle / 2);
    Quat.z = Ang_Ax.z * sin(Ang_Ax.angle / 2);
    Quat.w = cos(Ang_Ax.angle / 2);
    return Quat;
}

angle_axis Quat_2_AA(quaternion Quat)
{
    angle_axis Ang_Ax;
    float tw = (float)acos(Quat.w) * 2;
    float scale = (float)sin(tw / 2.0f);
    Ang_Ax.x = Quat.x / scale;
    Ang_Ax.y = Quat.y / scale;
    Ang_Ax.z = Quat.z / scale;

    Ang_Ax.angle = 2.0 * acos(Quat.w) / (float)PI * 180;
    return Ang_Ax;
}

quaternion To_Quat(int In_Degrees, euler Euler)
{
    // From the gamasutra quaternion article
    quaternion Quat;
    //If we are in Degree mode, convert to Radians
    if (In_Degrees) {
        Euler.x = Euler.x * (float)PI / 180;
        Euler.y = Euler.y * (float)PI / 180;
        Euler.z = Euler.z * (float)PI / 180;
    }
    //Calculate trig identities
    //Formerly roll, pitch, yaw
    float cr = float(cos(Euler.x / 2));
    float cp = float(cos(Euler.y / 2));
    float cy = float(cos(Euler.z / 2));
    float sr = float(sin(Euler.x / 2));
    float sp = float(sin(Euler.y / 2));
    float sy = float(sin(Euler.z / 2));

    float cpcy = cp * cy;
    float spsy = sp * sy;
    Quat.w = cr * cpcy + sr * spsy;
    Quat.x = sr * cpcy - cr * spsy;
    Quat.y = cr * sp * cy + sr * cp * sy;
    Quat.z = cr * cp * sy - sr * sp * cy;

    return Quat;
}

quaternion QNormalize(quaternion Quat)
{
	return normalize(Quat);
}

XYZ Quat2Vector(quaternion Quat)
{
	Quat = normalize(Quat);

    float fW = Quat.w;
    float fX = Quat.x;
    float fY = Quat.y;
    float fZ = Quat.z;

    XYZ tempvec;

    tempvec.x = 2.0f * (fX * fZ - fW * fY);
    tempvec.y = 2.0f * (fY * fZ + fW * fX);
    tempvec.z = 1.0f - 2.0f * (fX * fX + fY * fY);

    return tempvec;
}

bool PointInTriangle(Vector *p, Vector normal, float p11, float p12, float p13, float p21, float p22, float p23, float p31, float p32, float p33)
{
    float a, b;
    int i, j;
    float max;
    bool bInter = false;
    float pointv[3];
    float p1v[3];
    float p2v[3];
    float p3v[3];
    float normalv[3];

    pointv[0] = p->x;
    pointv[1] = p->y;
    pointv[2] = p->z;


    p1v[0] = p11;
    p1v[1] = p12;
    p1v[2] = p13;

    p2v[0] = p21;
    p2v[1] = p22;
    p2v[2] = p23;

    p3v[0] = p31;
    p3v[1] = p32;
    p3v[2] = p33;

    normalv[0] = normal.x;
    normalv[1] = normal.y;
    normalv[2] = normal.z;

    max = std::max(std::max(abs(normalv[0]), abs(normalv[1])), abs(normalv[2]));
    if (max == abs(normalv[0])) {i = 1; j = 2;} // y, z
    if (max == abs(normalv[1])) {i = 0; j = 2;} // x, z
    if (max == abs(normalv[2])) {i = 0; j = 1;} // x, y
    
    float u0 = pointv[i] - p1v[i];
    float v0 = pointv[j] - p1v[j];
    float u1 = p2v[i] - p1v[i];
    float v1 = p2v[j] - p1v[j];
    float u2 = p3v[i] - p1v[i];
    float v2 = p3v[j] - p1v[j];
    
    if (u1 > -1.0e-05f && u1 < 1.0e-05f) { // == 0.0f)
        b = u0 / u2;
        if (0.0f <= b && b <= 1.0f) {
            a = (v0 - b * v2) / v1;
            if ((a >= 0.0f) && (( a + b ) <= 1.0f))
                bInter = true;
        }
    } else {
        b = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
        if (0.0f <= b && b <= 1.0f) {
            a = (u0 - b * u2) / u1;
            if ((a >= 0.0f) && (( a + b ) <= 1.0f ))
                bInter = true;
        }
    }

    return bInter;
}

bool LineFacet(Vector p1, Vector p2, Vector pa, Vector pb, Vector pc, Vector *p)
{
    float d;
    float denom,mu;
    Vector n;

    //Calculate the parameters for the plane
    n.x = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
    n.y = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
    n.z = (pb.x - pa.x)*(pc.y - pa.y) - (pb.y - pa.y)*(pc.x - pa.x);
    n = normalize(n);
    d = - n.x * pa.x - n.y * pa.y - n.z * pa.z;

    //Calculate the position on the line that intersects the plane
    denom = n.x * (p2.x - p1.x) + n.y * (p2.y - p1.y) + n.z * (p2.z - p1.z);
    if (abs(denom) < 0.0000001)        // Line and plane don't intersect
        return false;
    mu = - (d + n.x * p1.x + n.y * p1.y + n.z * p1.z) / denom;
    p->x = p1.x + mu * (p2.x - p1.x);
    p->y = p1.y + mu * (p2.y - p1.y);
    p->z = p1.z + mu * (p2.z - p1.z);
    if (mu < 0 || mu > 1)   // Intersection not along line segment
        return false;

    if(!PointInTriangle( p, n, pa.x, pa.y, pa.z, pb.x, pb.y, pb.z, pc.x, pc.y, pc.z)) {
        return false;
    }

    return true;
}

bool PointInTriangle(const XYZ *p, const XYZ normal, const XYZ *p1, const XYZ *p2, const XYZ *p3)
{
    float a, b;
    float max;
    int i, j;
    bool bInter = false;
    float pointv[3];
    float p1v[3];
    float p2v[3];
    float p3v[3];
    float normalv[3];

    pointv[0] = p->x;
    pointv[1] = p->y;
    pointv[2] = p->z;


    p1v[0] = p1->x;
    p1v[1] = p1->y;
    p1v[2] = p1->z;

    p2v[0] = p2->x;
    p2v[1] = p2->y;
    p2v[2] = p2->z;

    p3v[0] = p3->x;
    p3v[1] = p3->y;
    p3v[2] = p3->z;

    normalv[0] = normal.x;
    normalv[1] = normal.y;
    normalv[2] = normal.z;

    max = std::max(std::max(abs(normalv[0]), abs(normalv[1])), abs(normalv[2]));
    if (max == abs(normalv[0])) {
        i = 1;    // y, z
        j = 2;
    }
    if (max == abs(normalv[1])) {
        i = 0;    // x, z
        j = 2;
    }
    if (max == abs(normalv[2])) {
        i = 0;    // x, y
        j = 1;
    }

    float u0 = pointv[i] - p1v[i];
    float v0 = pointv[j] - p1v[j];
    float u1 = p2v[i] - p1v[i];
    float v1 = p2v[j] - p1v[j];
    float u2 = p3v[i] - p1v[i];
    float v2 = p3v[j] - p1v[j];

    if (u1 > -1.0e-05f && u1 < 1.0e-05f) { // == 0.0f)
        b = u0 / u2;
        if (0.0f <= b && b <= 1.0f) {
            a = (v0 - b * v2) / v1;
            if ((a >= 0.0f) && (( a + b ) <= 1.0f))
                bInter = 1;
        }
    } else {
        b = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
        if (0.0f <= b && b <= 1.0f) {
            a = (u0 - b * u2) / u1;
            if ((a >= 0.0f) && (( a + b ) <= 1.0f ))
                bInter = 1;
        }
    }

    return bInter;
}

float LineFacetd(const XYZ &p1, const XYZ &p2, const XYZ &pa, const XYZ &pb, const XYZ &pc, XYZ *p)
{
    float d;
    float denom,mu;
    XYZ n;

    //Calculate the parameters for the plane
    n.x = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
    n.y = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
    n.z = (pb.x - pa.x)*(pc.y - pa.y) - (pb.y - pa.y)*(pc.x - pa.x);
    Normalise(n);
    d = - n.x * pa.x - n.y * pa.y - n.z * pa.z;

    //Calculate the position on the line that intersects the plane
    denom = n.x * (p2.x - p1.x) + n.y * (p2.y - p1.y) + n.z * (p2.z - p1.z);
    if (abs(denom) < 0.0000001)        // Line and plane don't intersect
        return 0;
    mu = - (d + n.x * p1.x + n.y * p1.y + n.z * p1.z) / denom;
    p->x = p1.x + mu * (p2.x - p1.x);
    p->y = p1.y + mu * (p2.y - p1.y);
    p->z = p1.z + mu * (p2.z - p1.z);
    if (mu < 0 || mu > 1)   // Intersection not along line segment
        return 0;

    if(!PointInTriangle( p, n, &pa, &pb, &pc)) {
        return 0;
    }
    return 1;
}

float LineFacetd(const XYZ &p1,const XYZ &p2,const XYZ &pa,const XYZ &pb,const XYZ &pc, const XYZ &n, XYZ &p)
{
    float d;
    float denom;
    
    //Calculate the parameters for the plane
    d = - n.x * pa.x - n.y * pa.y - n.z * pa.z;
    
    //Calculate the position on the line that intersects the plane
    denom = n.x * (p2.x - p1.x) + n.y * (p2.y - p1.y) + n.z * (p2.z - p1.z);
    if (abs(denom) < 0.0000001)        // Line and plane don't intersect
        return 0;
    float mu = - (d + n.x * p1.x + n.y * p1.y + n.z * p1.z) / denom;
    p.x = p1.x + mu * (p2.x - p1.x);
    p.y = p1.y + mu * (p2.y - p1.y);
    p.z = p1.z + mu * (p2.z - p1.z);
    if (mu < 0 || mu > 1)   // Intersection not along line segment
        return 0;
    
    if (!PointInTriangle( &p, n, &pa, &pb, &pc)) {
        return 0;
    }
    return 1;
}
