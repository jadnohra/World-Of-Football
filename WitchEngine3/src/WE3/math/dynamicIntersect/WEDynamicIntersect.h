#ifndef _WEDynamicIntersect_h
#define _WEDynamicIntersect_h

#include "../../WEDataTypes.h"
#include "../WEVector.h"
#include "../WEOBB.h"
#include "../intersect/WEIntersectionBucket.h"
#include "sweptEllipsoid/SweptEllipsoid.h"
#include "WEDynamicIntersectDataTypes.h"

namespace WE {

	class Vector2NoInit : public WE_3rdp_SweptEllipsoid::Vector2 {
	public:

		Vector2NoInit() : WE_3rdp_SweptEllipsoid::Vector2(true) {};
	};

	bool sweptIntersect(const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff, 
						const Vector3* triVertices, const Vector3& triNormal, 
						Point& intersectionPoint, SweptIntersection& sweptInter, TriIntersection& triInter);

	bool sweptIntersect(const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff, 
						const Vector3* triVertices, const Vector3& triNormal, 
						Point& intersectionPoint, SweptIntersection& sweptInter, PlanarFaceIntersectionEx4& triInter);
	
	struct SweptIntersectionTesterOBB {

		SweptIntersectionTesterOBB(const OBB& vol, const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff);
		
		bool next(bool& didIntersect, Point& intersectionPoint, SweptIntersection& sweptInter, PlanarFaceIntersectionEx4& faceInter);

		int faceIndex;

		float sweptSphereRadius;

		Vector3 obbPoints[8];
		int obbFace[4];
		Vector3 temp[2];

		WE_3rdp_SweptEllipsoid::Plane inPlane;
		Vector2NoInit inVertices[4];

		WE_3rdp_SweptEllipsoid::Vector3 inBegin;
		WE_3rdp_SweptEllipsoid::Vector3 inDelta;
		WE_3rdp_SweptEllipsoid::Vector3 outPoint;
		
		WE_3rdp_SweptEllipsoid::Vector3 u;
		WE_3rdp_SweptEllipsoid::Vector3 v;
	};

	/*
	bool sweptIntersect(const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff, 
						const OBB& vol, 
						Point& intersectionPoint, SweptIntersection& sweptInter, PlanarFaceIntersectionEx4& faceInter);


	struct SweptIntersection_PlanarFaceIntersectionEx4 {

		Point intersectionPoint;
		SweptIntersection sweptInter;
		PlanarFaceIntersectionEx4 faceInter;

		typedef IntersectionBucket<SweptIntersection_PlanarFaceIntersectionEx4> Bucket;
	};

	bool sweptIntersect(const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff, 
						const OBB& vol, 
						SweptIntersection_PlanarFaceIntersectionEx4::Bucket& bucket);
	*/


}


#endif