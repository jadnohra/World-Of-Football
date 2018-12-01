#include "WEDynamicIntersect.h"

#include "../WESpan.h"
#include "../../WEPtr.h"


namespace WE {


inline void triToPlane(const Vector3* vertices, const Vector3& normal, WE_3rdp_SweptEllipsoid::Plane& inPlane) {

	inPlane.mNormal = normal;
	inPlane.mConstant = -normal.dot(vertices[0]);
}

inline void triToPlane(const Vector3& p1, const Vector3& p2, const Vector3& p3, const bool& CW, Vector3 temp[2], WE_3rdp_SweptEllipsoid::Plane& inPlane) {

	p2.subtract(p1, temp[0]);
	p3.subtract(p1, temp[1]);

	if (CW) {
	
		temp[0].cross(temp[1], inPlane.mNormal);

	} else {

		temp[1].cross(temp[0], inPlane.mNormal);
	}

	inPlane.mNormal.normalize();
	inPlane.mConstant = -inPlane.mNormal.dot(p1);
}

bool sweptIntersect(const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff, 
						const Vector3* triVertices, const Vector3& triNormal, 
						Point& intersectionPoint, SweptIntersection& sweptInter, TriIntersection& triInter) {

	WE_3rdp_SweptEllipsoid::Plane inPlane(true);
	Vector2NoInit inVertices[3];
	WE_3rdp_SweptEllipsoid::Vector3 inBegin(true);
	WE_3rdp_SweptEllipsoid::Vector3 inDelta(true);
	WE_3rdp_SweptEllipsoid::Vector3 outPoint(true);
	
	const Vector3* vertices = triVertices;

	inBegin = sweptSphereStartPos;
	inDelta = sweptSpherePosDiff;

	triToPlane(vertices, triNormal, inPlane);

	WE_3rdp_SweptEllipsoid::Vector3 u;
	WE_3rdp_SweptEllipsoid::Vector3 v;

	inPlane.GetBasisVectors(u, v);

	inPlane.sConvertWorldToPlane(u, v, vertices[0], inVertices[0]);
	inPlane.sConvertWorldToPlane(u, v, vertices[1], inVertices[1]);
	inPlane.sConvertWorldToPlane(u, v, vertices[2], inVertices[2]);
	
	
	if (WE_3rdp_SweptEllipsoid::PolygonSweptSphereIntersect(inPlane, inVertices, 3, inBegin, inDelta, sweptSphereRadius, outPoint, sweptInter.sweptFraction, triInter)) {

		outPoint.copyTo(intersectionPoint);
		return true;
	}
	
	
	return false;
}


bool sweptIntersect(const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff, 
						const Vector3* triVertices, const Vector3& triNormal, 
						Point& intersectionPoint, SweptIntersection& sweptInter, PlanarFaceIntersectionEx4& triInter) {

	WE_3rdp_SweptEllipsoid::Plane inPlane(true);
	Vector2NoInit inVertices[3];
	WE_3rdp_SweptEllipsoid::Vector3 inBegin(true);
	WE_3rdp_SweptEllipsoid::Vector3 inDelta(true);
	WE_3rdp_SweptEllipsoid::Vector3 outPoint(true);
	
	const Vector3* vertices = triVertices;

	inBegin = sweptSphereStartPos;
	inDelta = sweptSpherePosDiff;

	triToPlane(vertices, triNormal, inPlane);

	WE_3rdp_SweptEllipsoid::Vector3 u;
	WE_3rdp_SweptEllipsoid::Vector3 v;

	inPlane.GetBasisVectors(u, v);

	inPlane.sConvertWorldToPlane(u, v, vertices[0], inVertices[0]);
	inPlane.sConvertWorldToPlane(u, v, vertices[1], inVertices[1]);
	inPlane.sConvertWorldToPlane(u, v, vertices[2], inVertices[2]);
	
	
	if (WE_3rdp_SweptEllipsoid::PolygonSweptSphereIntersect(inPlane, inVertices, 3, inBegin, inDelta, sweptSphereRadius, outPoint, sweptInter.sweptFraction, triInter)) {

		triInter.pointCount = 3;

		triInter.points[0] = vertices[0];
		triInter.points[1] = vertices[1];
		triInter.points[2] = vertices[2];

		outPoint.copyTo(intersectionPoint);
		return true;
	}
	
	
	return false;
}

/*
bool sweptIntersect(const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff, 
						const OBB& vol, 
						Point& intersectionPoint, SweptIntersection& sweptInter, PlanarFaceIntersectionEx4& faceInter) {

	Vector obbPoints[8];
	int obbFace[4];
	Vector3 temp[2];

	WE_3rdp_SweptEllipsoid::Plane inPlane(true);
	Vector2NoInit inVertices[4];

	WE_3rdp_SweptEllipsoid::Vector3 inBegin(true);
	WE_3rdp_SweptEllipsoid::Vector3 inDelta(true);
	WE_3rdp_SweptEllipsoid::Vector3 outPoint(true);
	
	WE_3rdp_SweptEllipsoid::Vector3 u;
	WE_3rdp_SweptEllipsoid::Vector3 v;

	inBegin = sweptSphereStartPos;
	inDelta = sweptSpherePosDiff;

	vol.toPoints(obbPoints);

	for (int face = 0; face < 6; ++face) {

		OBB::getFaceCW(face, obbFace);

		triToPlane(obbPoints[obbFace[0]], obbPoints[obbFace[1]], obbPoints[obbFace[2]], true, temp, inPlane);

		inPlane.GetBasisVectors(u, v);

		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[0]], inVertices[0]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[1]], inVertices[1]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[2]], inVertices[2]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[3]], inVertices[3]);

		if (WE_3rdp_SweptEllipsoid::PolygonSweptSphereIntersect(inPlane, inVertices, 4, inBegin, inDelta, sweptSphereRadius, outPoint, sweptInter.sweptFraction, faceInter)) {

			faceInter.points[0] = obbPoints[obbFace[0]];
			faceInter.points[1] = obbPoints[obbFace[1]];
			faceInter.points[2] = obbPoints[obbFace[2]];
			faceInter.points[3] = obbPoints[obbFace[3]];

			faceInter.normal = inPlane.mNormal;

			outPoint.copyTo(intersectionPoint);
			return true;
		}
	}

	return false;
}
*/

SweptIntersectionTesterOBB::SweptIntersectionTesterOBB(const OBB& vol, const float& _sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff) 
	: faceIndex(0), inPlane(true), inBegin(true), inDelta(true), outPoint(true) {
	
	inBegin = sweptSphereStartPos;
	inDelta = sweptSpherePosDiff;

	sweptSphereRadius = _sweptSphereRadius;

	vol.toPoints(obbPoints);
}

bool SweptIntersectionTesterOBB::next(bool& didIntersect, Point& intersectionPoint, SweptIntersection& sweptInter, PlanarFaceIntersectionEx4& faceInter) {

	if (faceIndex >= 6)
		return false;


	OBB::getFaceCW(faceIndex, obbFace);

	triToPlane(obbPoints[obbFace[0]], obbPoints[obbFace[1]], obbPoints[obbFace[2]], true, temp, inPlane);

	inPlane.GetBasisVectors(u, v);

	inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[0]], inVertices[0]);
	inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[1]], inVertices[1]);
	inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[2]], inVertices[2]);
	inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[3]], inVertices[3]);

	if (WE_3rdp_SweptEllipsoid::PolygonSweptSphereIntersect(inPlane, inVertices, 4, inBegin, inDelta, sweptSphereRadius, outPoint, sweptInter.sweptFraction, faceInter)) {

		faceInter.pointCount = 4;

		faceInter.points[0] = obbPoints[obbFace[0]];
		faceInter.points[1] = obbPoints[obbFace[1]];
		faceInter.points[2] = obbPoints[obbFace[2]];
		faceInter.points[3] = obbPoints[obbFace[3]];

		faceInter.normal = inPlane.mNormal;

		outPoint.copyTo(intersectionPoint);
		didIntersect = true;

	} else {

		didIntersect = false;
	}


	++faceIndex;

	return true;
}

/*
bool sweptIntersect(const float& sweptSphereRadius, const Vector3& sweptSphereStartPos, const Vector3& sweptSpherePosDiff, 
						const OBB& vol, 
						SweptIntersection_PlanarFaceIntersectionEx4::Bucket& bucket) {

	Vector obbPoints[8];
	int obbFace[4];
	Vector3 temp[2];

	WE_3rdp_SweptEllipsoid::Plane inPlane(true);
	Vector2NoInit inVertices[4];

	WE_3rdp_SweptEllipsoid::Vector3 inBegin(true);
	WE_3rdp_SweptEllipsoid::Vector3 inDelta(true);
	WE_3rdp_SweptEllipsoid::Vector3 outPoint(true);
	
	WE_3rdp_SweptEllipsoid::Vector3 u;
	WE_3rdp_SweptEllipsoid::Vector3 v;

	inBegin = sweptSphereStartPos;
	inDelta = sweptSpherePosDiff;

	vol.toPoints(obbPoints);

	SoftPtr<SweptIntersection_PlanarFaceIntersectionEx4> inter;
	bool didIntersect = false;

	for (int face = 0; face < 6; ++face) {

		OBB::getFaceCW(face, obbFace);

		triToPlane(obbPoints[obbFace[0]], obbPoints[obbFace[1]], obbPoints[obbFace[2]], true, temp, inPlane);

		inPlane.GetBasisVectors(u, v);

		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[0]], inVertices[0]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[1]], inVertices[1]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[2]], inVertices[2]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[3]], inVertices[3]);

		if (!inter.isValid()) {

			inter = &bucket.addOne();
		}

		if (WE_3rdp_SweptEllipsoid::PolygonSweptSphereIntersect(inPlane, inVertices, 4, inBegin, inDelta, sweptSphereRadius, outPoint, inter->sweptInter.sweptFraction, inter->faceInter)) {

			SweptIntersection_PlanarFaceIntersectionEx4& result = inter;

			result.faceInter.points[0] = obbPoints[obbFace[0]];
			result.faceInter.points[1] = obbPoints[obbFace[1]];
			result.faceInter.points[2] = obbPoints[obbFace[2]];
			result.faceInter.points[3] = obbPoints[obbFace[3]];

			result.faceInter.normal = inPlane.mNormal;

			outPoint.copyTo(result.intersectionPoint);
			didIntersect = true;

			inter.destroy();
		}
	}

	if (inter.isValid()) {

		bucket.putBackLast();
		inter.destroy();
	}

	return didIntersect;
}
*/



}