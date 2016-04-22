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


#ifndef _QUATERNIONS_H_
#define _QUATERNIONS_H_

#ifndef WIN32
#pragma mark -
#endif

//#include "Carbon.h"
#include <simd/simd.h>
#include <cmath>
#include "PhysicsMath.h"
#include "gamegl.h"

/**> Quaternion Structures <**/
#define PI      3.14159265355555897932384626
#define RADIANS 0
#define DEGREES 1
#define deg2rad .0174532925

//using namespace std;
typedef float Matrix_t [4][4];
typedef simd::float3 euler;
//struct euler
//{
//	float x, y, z;
//};
struct angle_axis
{
	float x, y, z, angle;
};

typedef simd::float4 quaternion;

typedef simd::float3 XYZ;

/*********************> Quaternion Function definition <********/
quaternion To_Quat(int Degree_Flag, euler Euler);
quaternion To_Quat(angle_axis Ang_Ax);
quaternion To_Quat(Matrix_t m);
angle_axis Quat_2_AA(quaternion Quat);
void Quat_2_Matrix(quaternion Quat, Matrix_t m);
XYZ Quat2Vector(quaternion Quat);

inline void CrossProduct(XYZ *P, XYZ *Q, XYZ *V);
inline void CrossProduct(XYZ P, XYZ Q, XYZ *V);
inline void Normalise(XYZ &vectory);
inline float normaldotproduct(XYZ point1, XYZ point2);
inline float fast_sqrt (register float arg);
bool PointInTriangle(const XYZ *p, const XYZ normal, const XYZ *p1, const XYZ *p2, const XYZ *p3);
bool LineFacet(XYZ p1,XYZ p2,XYZ pa,XYZ pb,XYZ pc,XYZ *p);
float LineFacetd(const XYZ &p1,const XYZ &p2,const XYZ &pa,const XYZ &pb,const XYZ &pc, const XYZ &n, XYZ &p);
float LineFacetd(const XYZ &p1, const XYZ &p2, const XYZ &pa,const XYZ &pb,const XYZ &pc,XYZ *p);
bool PointInTriangle(Vector *p, Vector normal, float p11, float p12, float p13, float p21, float p22, float p23, float p31, float p32, float p33);
bool LineFacet(Vector p1,Vector p2,Vector pa,Vector pb,Vector pc,Vector *p);
inline void ReflectVector(XYZ *vel, const XYZ *n);
inline void ReflectVector(XYZ *vel, const XYZ &n);
inline XYZ DoRotation(XYZ thePoint, float xang, float yang, float zang);
inline XYZ DoRotationRadian(XYZ thePoint, float xang, float yang, float zang);
inline float findLengthfast(XYZ *point1);
inline float findDistancefast(XYZ *point1, XYZ *point2);
inline float findDistancefast(const XYZ &point1, const XYZ &point2);
inline float findDistancefastflat(XYZ *point1, XYZ *point2);
bool sphere_line_intersection (
							   float x1, float y1 , float z1,
							   float x2, float y2 , float z2,
							   float x3, float y3 , float z3, float r );
bool sphere_line_intersection (
							   const XYZ &p1, const XYZ &p2, const XYZ &p3, const float r );
inline bool DistancePointLine( XYZ *Point, XYZ *LineStart, XYZ *LineEnd, float *Distance, XYZ *Intersection );


inline void Normalise(XYZ &vectory) {
	vectory = simd::normalize(vectory);
}

inline void CrossProduct(XYZ *P, XYZ *Q, XYZ *V){
	V->x = P->y * Q->z - P->z * Q->y;
	V->y = P->z * Q->x - P->x * Q->z;
	V->z = P->x * Q->y - P->y * Q->x;
}

inline void CrossProduct(XYZ P, XYZ Q, XYZ *V){
	V->x = P.y * Q.z - P.z * Q.y;
	V->y = P.z * Q.x - P.x * Q.z;
	V->z = P.x * Q.y - P.y * Q.x;
}

inline float fast_sqrt (register float arg)
{	
#if PLATFORM_MACOSX
	// Can replace with slower return std::sqrt(arg);
	register float result;

	if (arg == 0.0) return 0.0;

	asm {
		frsqrte		result,arg			// Calculate Square root
	}	

	// Newton Rhapson iterations.
	result = result + 0.5 * result * (1.0 - arg * result * result);
	result = result + 0.5 * result * (1.0 - arg * result * result);

	return result * arg;
#else
	return sqrt( arg);
#endif
}

inline float normaldotproduct(XYZ point1, XYZ point2){
	GLfloat returnvalue;
	Normalise(point1);
	Normalise(point2);
	//return simd::dot(point1, point2);
	returnvalue=(point1.x*point2.x+point1.y*point2.y+point1.z*point2.z);
	return returnvalue;
}

inline void ReflectVector(XYZ *vel, const XYZ *n)
{
    ReflectVector(vel, *n);
}

inline void ReflectVector(XYZ *vel, const XYZ &n)
{
	float dotprod=simd::dot(n,*vel);
	XYZ vn = n * dotprod;
	XYZ vt = *vel - vn;

	*vel = vt - vn;
}

inline float findLengthfast(XYZ *point1){
	return((point1->x)*(point1->x)+(point1->y)*(point1->y)+(point1->z)*(point1->z));
}

inline float findDistancefast(XYZ *point1, XYZ *point2){
	return((point1->x-point2->x)*(point1->x-point2->x)+(point1->y-point2->y)*(point1->y-point2->y)+(point1->z-point2->z)*(point1->z-point2->z));
}

inline float findDistancefast(const XYZ &point1, const XYZ &point2){
	return((point1.x-point2.x)*(point1.x-point2.x)+(point1.y-point2.y)*(point1.y-point2.y)+(point1.z-point2.z)*(point1.z-point2.z));
}

inline float findDistancefastflat(XYZ *point1, XYZ *point2){
	return((point1->x-point2->x)*(point1->x-point2->x)+(point1->z-point2->z)*(point1->z-point2->z));
}

inline XYZ DoRotation(XYZ thePoint, float xang, float yang, float zang){
	static XYZ newpoint;
	if(xang){
		xang*=6.283185f;
		xang/=360;
	}
	if(yang){
		yang*=6.283185f;
		yang/=360;
	}
	if(zang){
		zang*=6.283185f;
		zang/=360;
	}


	if(yang){
		newpoint.z=thePoint.z*cos(yang)-thePoint.x*sin(yang);
		newpoint.x=thePoint.z*sin(yang)+thePoint.x*cos(yang);
		thePoint.z=newpoint.z;
		thePoint.x=newpoint.x;
	}

	if(zang){
		newpoint.x=thePoint.x*cos(zang)-thePoint.y*sin(zang);
		newpoint.y=thePoint.y*cos(zang)+thePoint.x*sin(zang);
		thePoint.x=newpoint.x;
		thePoint.y=newpoint.y;
	}

	if(xang){
		newpoint.y=thePoint.y*cos(xang)-thePoint.z*sin(xang);
		newpoint.z=thePoint.y*sin(xang)+thePoint.z*cos(xang);
		thePoint.z=newpoint.z;
		thePoint.y=newpoint.y;
	}

	return thePoint;
}

inline float square(const float f ) { return (f*f) ;}

inline bool sphere_line_intersection (
									  float x1, float y1 , float z1,
									  float x2, float y2 , float z2,
									  float x3, float y3 , float z3, float r )
{

	// x1,y1,z1  P1 coordinates (point of line)
	// x2,y2,z2  P2 coordinates (point of line)
	// x3,y3,z3, r  P3 coordinates and radius (sphere)
	// x,y,z   intersection coordinates
	//
	// This function returns a pointer array which first index indicates
	// the number of intersection point, followed by coordinate pairs.

	float a, b, c, i ;

	if(x1>x3+r&&x2>x3+r)
		return false;
	if(x1<x3-r&&x2<x3-r)
		return false;
	if(y1>y3+r&&y2>y3+r)
		return false;
	if(y1<y3-r&&y2<y3-r)
		return false;
	if(z1>z3+r&&z2>z3+r)
		return false;
	if(z1<z3-r&&z2<z3-r)
		return false;
	a =  square(x2 - x1) + square(y2 - y1) + square(z2 - z1);
	b =  2* ( (x2 - x1)*(x1 - x3)
		+ (y2 - y1)*(y1 - y3)
		+ (z2 - z1)*(z1 - z3) );
	c =  square(x3) + square(y3) +
		square(z3) + square(x1) +
		square(y1) + square(z1) -
		2* ( x3*x1 + y3*y1 + z3*z1 ) - square(r);
	i =   b * b - 4 * a * c;

	if ( i < 0.0 )
	{
		// no intersection
		return false;
	}
	return true;
}

inline bool sphere_line_intersection (
									  const XYZ &p1, const XYZ &p2, const XYZ &p3, const float r )
{

	// x1,p1->y,p1->z  P1 coordinates (point of line)
	// p2->x,p2->y,p2->z  P2 coordinates (point of line)
	// p3->x,p3->y,p3->z, r  P3 coordinates and radius (sphere)
	// x,y,z   intersection coordinates
	//
	// This function returns a pointer array which first index indicates
	// the number of intersection point, followed by coordinate pairs.

	float a, b, c, i ;

	if(p1.x>p3.x+r&&p2.x>p3.x+r)
		return false;
	if(p1.x<p3.x-r&&p2.x<p3.x-r)
		return false;
	if(p1.y>p3.y+r&&p2.y>p3.y+r)
		return false;
	if(p1.y<p3.y-r&&p2.y<p3.y-r)
		return false;
	if(p1.z>p3.z+r&&p2.z>p3.z+r)
		return false;
	if(p1.z<p3.z-r&&p2.z<p3.z-r)
		return false;
	a =  square(p2.x - p1.x) + square(p2.y - p1.y) + square(p2.z - p1.z);
	b =  2* ( (p2.x - p1.x)*(p1.x - p3.x)
		+ (p2.y - p1.y)*(p1.y - p3.y)
		+ (p2.z - p1.z)*(p1.z - p3.z) ) ;
	c =  square(p3.x) + square(p3.y) +
		square(p3.z) + square(p1.x) +
		square(p1.y) + square(p1.z) -
		2* ( p3.x*p1.x + p3.y*p1.y + p3.z*p1.z ) - square(r) ;
	i =   b * b - 4 * a * c ;

	if ( i < 0.0 )
	{
		// no intersection
		return false;
	}
	return true;
}

inline XYZ DoRotationRadian(XYZ thePoint, float xang, float yang, float zang){
	XYZ newpoint;
	XYZ oldpoint = thePoint;

	if(yang!=0){
		newpoint.z=oldpoint.z*cos(yang)-oldpoint.x*sin(yang);
		newpoint.x=oldpoint.z*sin(yang)+oldpoint.x*cos(yang);
		oldpoint.z=newpoint.z;
		oldpoint.x=newpoint.x;
	}

	if(zang!=0){
		newpoint.x=oldpoint.x*cos(zang)-oldpoint.y*sin(zang);
		newpoint.y=oldpoint.y*cos(zang)+oldpoint.x*sin(zang);
		oldpoint.x=newpoint.x;
		oldpoint.y=newpoint.y;
	}

	if(xang!=0){
		newpoint.y=oldpoint.y*cos(xang)-oldpoint.z*sin(xang);
		newpoint.z=oldpoint.y*sin(xang)+oldpoint.z*cos(xang);
		oldpoint.z=newpoint.z;
		oldpoint.y=newpoint.y;	
	}

	return oldpoint;

}

inline bool DistancePointLine( XYZ *Point, XYZ *LineStart, XYZ *LineEnd, float *Distance, XYZ *Intersection )
{
	float U;

	float LineMag = simd::distance( *LineEnd, *LineStart );

	U = ( ( ( Point->x - LineStart->x ) * ( LineEnd->x - LineStart->x ) ) +
		( ( Point->y - LineStart->y ) * ( LineEnd->y - LineStart->y ) ) +
		( ( Point->z - LineStart->z ) * ( LineEnd->z - LineStart->z ) ) ) /
		( LineMag * LineMag );

	if( U < 0.0f || U > 1.0f )
		return false;   // closest point does not fall within the line segment

	Intersection->x = LineStart->x + U * ( LineEnd->x - LineStart->x );
	Intersection->y = LineStart->y + U * ( LineEnd->y - LineStart->y );
	Intersection->z = LineStart->z + U * ( LineEnd->z - LineStart->z );

	*Distance = simd::distance( *Point, *Intersection );

	return true;
}

#endif
