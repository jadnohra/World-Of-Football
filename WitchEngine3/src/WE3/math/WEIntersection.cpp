#include "WEIntersection.h"

#include "WEIntersect.h"
#include "WEDistance.h"

namespace WE {	

SphereSegIntersection::SphereSegIntersection() : pointCount(0) {
}

void SphereSegIntersection::empty() {

	pointCount = 0;
}

bool SphereSegIntersection::init(const Point& sphereCenter, const float& sphereRadius, const Point& segA, const Point& segB, bool considerInternalPoints) {

	pointCount = 0;

	float radiusSq = sphereRadius * sphereRadius;

	if (considerInternalPoints) {

		if (distanceSq(sphereCenter, segA) <= radiusSq) {

			points[pointCount] = segA;
			isInternal[pointCount] = true;

			++pointCount;
		}


		if (distanceSq(sphereCenter, segB) <= radiusSq) {

			points[pointCount] = segB;
			isInternal[pointCount] = true;

			++pointCount;
		}

		if (pointCount == 2)
			return true;
	} 

	if (!intersect(sphereCenter, sphereRadius, segA, segB))
		return pointCount > 0;

	float a = distanceSq(segA, segB);
	float b = 2.0f * (
						(segB.x - segA.x) * (segA.x - sphereCenter.x) 
						+ (segB.y - segA.y) * (segA.y - sphereCenter.y) 
						+ (segB.z - segA.z) * (segA.z - sphereCenter.z) 
						);
	float c = sphereCenter.dot(sphereCenter) + segA.dot(segA) - 2.0f * (sphereCenter.dot(segA)) - radiusSq;
	
	float bSqMin4AC = b * b - 4.0f * a * c;

	if (bSqMin4AC < 0.0f) {

		return pointCount > 0;

	} else if (bSqMin4AC > 0.0f) {

		float expr = sqrtf(bSqMin4AC);

		float u1 = (-b - expr) / 2.0f * a;
		float u2 = (-b + expr) / 2.0f * a;

		pointCount = 0; //precision problems friendly

		lerp(segA, segB, u1, points[pointCount]);
		isInternal[pointCount] = false;
		++pointCount;

		lerp(segA, segB, u2, points[pointCount]);
		isInternal[pointCount] = false;
		++pointCount;

		return true;

	} else {

		float u = (-b) / 2.0f * a;
		
		lerp(segA, segB, u, points[pointCount]);
		isInternal[pointCount] = false;
		++pointCount;
	}

	return pointCount > 0;
}


}