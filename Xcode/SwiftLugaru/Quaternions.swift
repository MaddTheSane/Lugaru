//
//  Quaternions.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/23/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation
import simd

func normaldotproduct(point1a: float3, _ point2a: float3) -> Float {
	var point1 = normalize(point1a)
	var point2 = normalize(point2a)
	//return simd::dot(point1, point2);
	let returnvalue=(point1.x*point2.x+point1.y*point2.y+point1.z*point2.z);
	return returnvalue;
}

func ReflectVector(inout vel: float3, _ n: float3)
{
	let dotprod = dot(n,vel);
	let vn = n * dotprod;
	let vt = vel - vn;
	
	vel = vt - vn;
}


func rotate(thePoint1: float3, byAngles angles: (x: Float, y: Float, z: Float)) -> float3 {
	var newpoint = float3();
	var thePoint = thePoint1
	var xang = angles.x
	var yang = angles.y
	var zang = angles.z
	if xang != 0 {
		xang *= 6.283185
		xang /= 360
	}
	if yang != 0 {
		yang *= 6.283185
		yang /= 360
	}
	if zang != 0 {
		zang *= 6.283185
		zang /= 360
	}
	
	if yang != 0 {
		newpoint.z = thePoint.z * cos(yang) - thePoint.x * sin(yang);
		newpoint.x = thePoint.z * sin(yang) + thePoint.x * cos(yang);
		thePoint.z = newpoint.z;
		thePoint.x = newpoint.x;
	}
	
	if zang != 0 {
		newpoint.x = thePoint.x * cos(zang) - thePoint.y * sin(zang);
		newpoint.y = thePoint.y * cos(zang) + thePoint.x * sin(zang);
		thePoint.x = newpoint.x;
		thePoint.y = newpoint.y;
	}
	
	if xang != 0 {
		newpoint.y = thePoint.y * cos(xang) - thePoint.z * sin(xang);
		newpoint.z = thePoint.y * sin(xang) + thePoint.z * cos(xang);
		thePoint.z = newpoint.z;
		thePoint.y = newpoint.y;
	}
	
	return thePoint;
}

func distance(point Point: float3, lineStart LineStart: float3, lineEnd LineEnd: float3) -> (distance: Float, intersection: float3)?
{
	let LineMag = distance( LineEnd, LineStart );
	
	let U = ( ( ( Point.x - LineStart.x ) * ( LineEnd.x - LineStart.x ) ) +
	( ( Point.y - LineStart.y ) * ( LineEnd.y - LineStart.y ) ) +
	( ( Point.z - LineStart.z ) * ( LineEnd.z - LineStart.z ) ) ) /
	( LineMag * LineMag );
	
	if( U < 0.0 || U > 1.0 ) {
	return nil;   // closest point does not fall within the line segment
	}
	var Intersection = float3()
	Intersection.x = LineStart.x + U * ( LineEnd.x - LineStart.x );
	Intersection.y = LineStart.y + U * ( LineEnd.y - LineStart.y );
	Intersection.z = LineStart.z + U * ( LineEnd.z - LineStart.z );
	
	let Distance = distance(Point, Intersection);
	
	return (Distance, Intersection);
}

func findDistancefast(point1: float3, _ point2: float3) -> Float {
	return((point1.x-point2.x)*(point1.x-point2.x)+(point1.y-point2.y)*(point1.y-point2.y)+(point1.z-point2.z)*(point1.z-point2.z));
}

private func square(num: Float) -> Float {
	return num * num
}

func pointInTriangle(p: float3, normal: float3, _ p1: float3, _ p2: float3, _ p3: float3) -> Bool
{
	var bInter = false;
	let pointv = [p.x, p.y, p.z]
	let p1v = [p1.x, p1.y, p1.z]
	let p2v = [p2.x, p2.y, p2.z]
	let p3v = [p3.x, p3.y, p3.z]
	
	var i = 0
	var j = 0
	let max = Swift.max(abs(normal.x), abs(normal.y), abs(normal.z));
	if (max == abs(normal.x)) {i = 1; j = 2;} // y, z
	if (max == abs(normal.y)) {i = 0; j = 2;} // x, z
	if (max == abs(normal.z)) {i = 0; j = 1;} // x, y
	
	let u0 = pointv[i] - p1v[i];
	let v0 = pointv[j] - p1v[j];
	let u1 = p2v[i] - p1v[i];
	let v1 = p2v[j] - p1v[j];
	let u2 = p3v[i] - p1v[i];
	let v2 = p3v[j] - p1v[j];
	
	if (u1 > -1.0e-05 && u1 < 1.0e-05) {// == 0.0f)
		let b = u0 / u2;
		if 0.0 <= b && b <= 1.0 {
			let a = (v0 - b * v2) / v1;
			if a >= 0.0 && (( a + b ) <= 1.0) {
				bInter = true;
			}
		}
	} else {
		let b = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
		if 0.0 <= b && b <= 1.0 {
			let a = (u0 - b * u2) / u1;
			if a >= 0.0 && (( a + b ) <= 1.0 ) {
				bInter = true;
			}
		}
	}
	
	return bInter;
}

func sphereLineIntersection(p1: float3, _ p2: float3, center p3: float3, radius r: Float) -> Bool {
	
	// x1,p1->y,p1->z  P1 coordinates (point of line)
	// p2->x,p2->y,p2->z  P2 coordinates (point of line)
	// p3->x,p3->y,p3->z, r  P3 coordinates and radius (sphere)
	// x,y,z   intersection coordinates
	//
	// This function returns a pointer array which first index indicates
	// the number of intersection point, followed by coordinate pairs.
	
	if p1.x > p3.x + r && p2.x > p3.x + r {
		return false;
	} else if p1.x < p3.x - r && p2.x < p3.x - r {
		return false;
	} else if p1.y > p3.y + r && p2.y > p3.y + r {
		return false;
	} else if p1.y < p3.y - r && p2.y < p3.y - r {
		return false;
	} else if p1.z > p3.z + r && p2.z > p3.z + r {
		return false;
	} else if p1.z < p3.z - r && p2.z < p3.z - r {
		return false;
	}
	let a = square(p2.x - p1.x) + square(p2.y - p1.y) + square(p2.z - p1.z);
	let b = 2 * ( (p2.x - p1.x) * (p1.x - p3.x)
		+ (p2.y - p1.y) * (p1.y - p3.y)
		+ (p2.z - p1.z) * (p1.z - p3.z))
	let c =  square(p3.x) + square(p3.y) +
		square(p3.z) + square(p1.x) +
		square(p1.y) + square(p1.z) -
		2 * (p3.x * p1.x + p3.y * p1.y + p3.z * p1.z) - square(r)
	let i = b * b - 4 * a * c
	
	if i < 0.0 {
		// no intersection
		return false;
	}
	return true;
}

func lineFacetd(p1: float3, _ p2: float3, _ pa: float3, _ pb: float3, _ pc: float3, _ n: float3, inout p: float3) -> Float {
	//Calculate the parameters for the plane
	let d = -n.x * pa.x - n.y * pa.y - n.z * pa.z;
	
	//Calculate the position on the line that intersects the plane
	let denom = n.x * (p2.x - p1.x) + n.y * (p2.y - p1.y) + n.z * (p2.z - p1.z);
	if abs(denom) < 0.0000001 {        // Line and plane don't intersect
		return 0;
	}
	let mu = -(d + n.x * p1.x + n.y * p1.y + n.z * p1.z) / denom;
	p = p1 + mu * (p2 - p1)
	if mu < 0 || mu > 1 {   // Intersection not along line segment
		return 0;
	}
	
	if !pointInTriangle(p, normal: n, pa, pb, pc) {
		return 0;
	}
	return 1;
}
