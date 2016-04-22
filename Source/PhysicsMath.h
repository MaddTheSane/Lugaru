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

#ifndef _PHYSICSMATH_H_
#define _PHYSICSMATH_H_

//#include <Carbon.h>

#include <cmath>
#include "MacCompatibility.h"
#include <simd/simd.h>

//------------------------------------------------------------------------//
// Misc. Constants
//------------------------------------------------------------------------//

float	const	pi	= 3.14159265f;
float	const	g	= -32.174f;		// acceleration due to gravity, ft/s^2
float	const	rho = 0.0023769f;	// desity of air at sea level, slugs/ft^3
float	const	tol = 0.0000000001f;		// float type tolerance 


//------------------------------------------------------------------------//
// Misc. Functions
//------------------------------------------------------------------------//
inline	float	DegreesToRadians(float deg);
inline	float	RadiansToDegrees(float rad);

inline	float	DegreesToRadians(float deg)
{
	return deg * pi / 180.0f;
}

inline	float	RadiansToDegrees(float rad)
{	
	return rad * 180.0f / pi;
}

//------------------------------------------------------------------------//
// Vector Class and vector functions
//------------------------------------------------------------------------//
typedef simd::float3 Vector;

//float Magnitude(Vector vect);


// triple scalar product (u dot (v cross w))
inline	float TripleScalarProduct(Vector u, Vector v, Vector w)
{
	return float(	(u.x * (v.y*w.z - v.z*w.y)) +
		(u.y * (-v.x*w.z + v.z*w.x)) +
		(u.z * (v.x*w.y - v.y*w.x)) );
	//return u*(v^w);

}



//------------------------------------------------------------------------//
// Matrix Class and matrix functions
//------------------------------------------------------------------------//

typedef simd::float3x3 Matrix3x3;

//------------------------------------------------------------------------//
// Quaternion Class and Quaternion functions
//------------------------------------------------------------------------//

class Quaternion {
public:
	float	n;	// number (scalar) part
	Vector	v;	// vector part: v.x, v.y, v.z

	inline Quaternion(void)
	{
		n = 0;
		v.x = 0;
		v.y =  0;
		v.z = 0;
	}
	inline Quaternion(float e0, float e1, float e2, float e3)
	{
		n = e0;
		v.x = e1;
		v.y = e2;
		v.z = e3;
	}

	inline float Magnitude(void)
	{
		return (float) sqrt(n*n + v.x*v.x + v.y*v.y + v.z*v.z);
	}
	
	inline Vector GetVector(void)
	{
		return (Vector){v.x, v.y, v.z};
	}
	
	inline float GetScalar(void)
	{
		return n;
	}
	
	inline Quaternion operator+=(Quaternion q)
	{
		n += q.n;
		v.x += q.v.x;
		v.y += q.v.y;
		v.z += q.v.z;
		return *this;
	}
	
	inline Quaternion operator-=(Quaternion q)
	{
		n -= q.n;
		v.x -= q.v.x;
		v.y -= q.v.y;
		v.z -= q.v.z;
		return *this;
	}
	
	inline Quaternion operator*=(float s)
	{
		n *= s;
		v.x *= s;
		v.y *= s;
		v.z *= s;
		return *this;
	}
	
	inline Quaternion operator/=(float s)
	{
		n /= s;
		v.x /= s;
		v.y /= s;
		v.z /= s;
		return *this;
	}
	Quaternion	operator~(void) const { return Quaternion(n, -v.x, -v.y, -v.z);}
};

inline	Quaternion operator+(Quaternion q1, Quaternion q2);
inline	Quaternion operator-(Quaternion q1, Quaternion q2);
inline	Quaternion operator*(Quaternion q1, Quaternion q2);
inline	Quaternion operator*(Quaternion q, float s);
inline	Quaternion operator*(float s, Quaternion q);
inline	Quaternion operator*(Quaternion q, Vector v);
inline	Quaternion operator*(Vector v, Quaternion q);
inline	Quaternion operator/(Quaternion q, float s);
inline	float QGetAngle(Quaternion q);
inline	Vector QGetAxis(Quaternion q);
inline	Quaternion QRotate(Quaternion q1, Quaternion q2);
inline	Vector	QVRotate(Quaternion q, Vector v);
inline	Quaternion	MakeQFromEulerAngles(float x, float y, float z);
inline	Vector	MakeEulerAnglesFromQ(Quaternion q);


/*inline	Quaternion	Quaternion::operator~()
{
return Quaternion(n, -v.x, -v.y, -v.z);
}*/

inline	Quaternion operator+(Quaternion q1, Quaternion q2)
{
	return	Quaternion(	q1.n + q2.n,
		q1.v.x + q2.v.x,
		q1.v.y + q2.v.y,
		q1.v.z + q2.v.z);
}

inline	Quaternion operator-(Quaternion q1, Quaternion q2)
{
	return	Quaternion(	q1.n - q2.n,
		q1.v.x - q2.v.x,
		q1.v.y - q2.v.y,
		q1.v.z - q2.v.z);
}

inline	Quaternion operator*(Quaternion q1, Quaternion q2)
{
	return	Quaternion(	q1.n*q2.n - q1.v.x*q2.v.x - q1.v.y*q2.v.y - q1.v.z*q2.v.z,
		q1.n*q2.v.x + q1.v.x*q2.n + q1.v.y*q2.v.z - q1.v.z*q2.v.y,
		q1.n*q2.v.y + q1.v.y*q2.n + q1.v.z*q2.v.x - q1.v.x*q2.v.z,
		q1.n*q2.v.z + q1.v.z*q2.n + q1.v.x*q2.v.y - q1.v.y*q2.v.x);							
}

inline	Quaternion operator*(Quaternion q, float s)
{
	return	Quaternion(q.n*s, q.v.x*s, q.v.y*s, q.v.z*s);
}

inline	Quaternion operator*(float s, Quaternion q)
{
	return	Quaternion(q.n*s, q.v.x*s, q.v.y*s, q.v.z*s);
}

inline	Quaternion operator*(Quaternion q, Vector v)
{
	return	Quaternion(	-(q.v.x*v.x + q.v.y*v.y + q.v.z*v.z),
		q.n*v.x + q.v.y*v.z - q.v.z*v.y,
		q.n*v.y + q.v.z*v.x - q.v.x*v.z,
		q.n*v.z + q.v.x*v.y - q.v.y*v.x);
}

inline	Quaternion operator*(Vector v, Quaternion q)
{
	return	Quaternion(	-(q.v.x*v.x + q.v.y*v.y + q.v.z*v.z),
		q.n*v.x + q.v.z*v.y - q.v.y*v.z,
		q.n*v.y + q.v.x*v.z - q.v.z*v.x,
		q.n*v.z + q.v.y*v.x - q.v.x*v.y);
}

inline	Quaternion operator/(Quaternion q, float s)
{
	return	Quaternion(q.n/s, q.v.x/s, q.v.y/s, q.v.z/s);
}

inline	float QGetAngle(Quaternion q)
{
	return	(float) (2*acos(q.n));
}

inline	Vector QGetAxis(Quaternion q)
{
	Vector v;
	float m;

	v = q.GetVector();
	m = simd::length(v);

	if (m <= tol)
		return Vector();
	else
		return v/m;	
}

inline	Quaternion QRotate(Quaternion q1, Quaternion q2)
{
	return	q1*q2*(~q1);
}

inline	Vector	QVRotate(Quaternion q, Vector v)
{
	Quaternion t;

	t = q*v*(~q);

	return	t.GetVector();
}

inline	Quaternion	MakeQFromEulerAngles(float x, float y, float z)
{
	Quaternion	q;
	double	roll = DegreesToRadians(x);
	double	pitch = DegreesToRadians(y);
	double	yaw = DegreesToRadians(z);

	double	cyaw, cpitch, croll, syaw, spitch, sroll;
	double	cyawcpitch, syawspitch, cyawspitch, syawcpitch;

	cyaw = cos(0.5 * yaw);
	cpitch = cos(0.5 * pitch);
	croll = cos(0.5 * roll);
	syaw = sin(0.5 * yaw);
	spitch = sin(0.5 * pitch);
	sroll = sin(0.5 * roll);

	cyawcpitch = cyaw*cpitch;
	syawspitch = syaw*spitch;
	cyawspitch = cyaw*spitch;
	syawcpitch = syaw*cpitch;

	q.n = (float) (cyawcpitch * croll + syawspitch * sroll);
	q.v.x = (float) (cyawcpitch * sroll - syawspitch * croll); 
	q.v.y = (float) (cyawspitch * croll + syawcpitch * sroll);
	q.v.z = (float) (syawcpitch * croll - cyawspitch * sroll);

	return q;
}

inline	Vector	MakeEulerAnglesFromQ(Quaternion q)
{
	double	r11, r21, r31, r32, r33, r12, r13;
	double	q00, q11, q22, q33;
	double	tmp;
	Vector	u;

	q00 = q.n * q.n;
	q11 = q.v.x * q.v.x;
	q22 = q.v.y * q.v.y;
	q33 = q.v.z * q.v.z;

	r11 = q00 + q11 - q22 - q33;
	r21 = 2 * (q.v.x*q.v.y + q.n*q.v.z);
	r31 = 2 * (q.v.x*q.v.z - q.n*q.v.y);
	r32 = 2 * (q.v.y*q.v.z + q.n*q.v.x);
	r33 = q00 - q11 - q22 + q33;

	tmp = fabs(r31);
	if(tmp > 0.999999)
	{
		r12 = 2 * (q.v.x*q.v.y - q.n*q.v.z);
		r13 = 2 * (q.v.x*q.v.z + q.n*q.v.y);

		u.x = RadiansToDegrees(0.0f); //roll
		u.y = RadiansToDegrees((float) (-(pi/2) * r31/tmp)); // pitch
		u.z = RadiansToDegrees((float) atan2(-r12, -r31*r13)); // yaw
		return u;
	}

	u.x = RadiansToDegrees((float) atan2(r32, r33)); // roll
	u.y = RadiansToDegrees((float) asin(-r31));		 // pitch
	u.z = RadiansToDegrees((float) atan2(r21, r11)); // yaw
	return u;
}

#endif
