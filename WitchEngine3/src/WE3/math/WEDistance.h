#ifndef _WEDistance_h
#define _WEDistance_h

#include "WEVector.h"
#include "WEVector2.h"
#include "WETriangle.h"

namespace WE {

	inline bool isBetweenSegPoints(const float& u) { return (u >= 0.0f) && (u <= 1.0f); }

	float distanceSq(const CtVector3 &a, const CtVector3 &b);
	float distance(const CtVector3 &a, const CtVector3 &b);

	float distanceSq(const CtVector3 &a, const CtVector3 &b, int dim1, int dim2);
	float distance(const CtVector3 &a, const CtVector3 &b, int dim1, int dim2);

	float distanceSq(const CtVector3& segmentA, const CtVector3& segmentB, const CtVector3& point, bool& isBetweenPoints);
	float distance(const CtVector3& segmentA, const CtVector3& segmentB, const CtVector3& point, bool& isBetweenPoints);
	float distanceSq(const CtVector3& segmentA, const CtVector3& segmentB, const CtVector3& point, Vector3& closestPt, float& u);
	float distanceU(const CtVector3& segmentA, const CtVector3& segmentB, const CtVector3& point);

	float distanceLineU(const CtVector3& lineOrig, const CtVector3& lineDir, const CtVector3& point);

	float distanceSq(const CtVector2 &a, const CtVector2 &b);
	float distance(const CtVector2 &a, const CtVector2 &b);

	float distanceSq(const CtVector2& segmentA, const CtVector2& segmentB, const CtVector2& point, bool& isBetweenPoints);
	float distance(const CtVector2& segmentA, const CtVector2& segmentB, const CtVector2& point, bool& isBetweenPoints);
	float distanceSq(const CtVector2& segmentA, const CtVector2& segmentB, const CtVector2& point, Vector2& closestPt, float& u);
	float distanceU(const CtVector2& segmentA, const CtVector2& segmentB, const CtVector2& point);


	void project(const CtVector3& planeA, const CtVector3& planeB, const CtVector3& planeN, const CtVector3& point, Vector3& projection); 
	void project(const Vector3& ref, const Vector3& projected, Vector3& projection); 

	bool coplanarPointInsideTriangle(const TriangleEx1& tri, const CtVector3& point);
	bool coplanarPointInsideTriangle(const TriangleEx1& tri, const CtVector3& point, float e);
	
}

#endif