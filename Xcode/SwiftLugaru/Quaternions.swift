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

