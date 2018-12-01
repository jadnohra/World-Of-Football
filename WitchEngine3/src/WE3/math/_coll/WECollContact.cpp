#include "WECollContact.h"

#include "../WESpan.h"
#include "sweptEllipsoid/SweptEllipsoid.h"

#include "float.h"

namespace WE {

/*
void contact(const Sphere& s0, 
				const Vector& normal, 
				const Point& v0, const Point& v1, const Point& v2, Contact& contact) {

	
	contact.normal = normal;
	project(v0, v1, normal, s0.center, contact.point);

	Vector radiusVect = normal;
	radiusVect *= s0.radius;
	s0.center.add(radiusVect, contact.forcePoint);

	Vector dist;
	s0.center.subtract(contact.point, dist);

	contact.penetration = s0.radius - dist.mag();

	if (contact.penetration < 0.0f) {
		contact.penetration = 0.0f;
	}
}
*/

/*
void contact(const Sphere& sphere, const TriangleEx1& tri, Vector& contactNormal) {
}
*/


//bool PolygonSweptSphereIntersect(const Plane &inPlane, const Vector2 *inVertices, int inNumVertices, const Vector3 &inBegin, const Vector3 &inDelta, float inRadius, Vector3 &outPoint, float &outFraction);

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



/*
bool sweptContact(float sphereRadius, const Vector3& startPos, const Vector3& posDiff, TriangleEx1& staticTri, SweptContact& contact, float e) {

	WE_3rdp_SweptEllipsoid::Plane inPlane;
	WE_3rdp_SweptEllipsoid::Vector2 inVertices[3];
	WE_3rdp_SweptEllipsoid::Vector3 inBegin;
	WE_3rdp_SweptEllipsoid::Vector3 inDelta;
	WE_3rdp_SweptEllipsoid::Vector3 outPoint;
	bool intersect;

	triToPlane(staticTri, inPlane);

	WE_3rdp_SweptEllipsoid::Vector3 u;
	WE_3rdp_SweptEllipsoid::Vector3 v;

	inPlane.GetBasisVectors(u, v);

	inPlane.sConvertWorldToPlane(u, v, staticTri.vertices[0], inVertices[0]);
	inPlane.sConvertWorldToPlane(u, v, staticTri.vertices[1], inVertices[1]);
	inPlane.sConvertWorldToPlane(u, v, staticTri.vertices[2], inVertices[2]);
	
	inBegin = startPos;
	inDelta = posDiff;

	intersect = PolygonSweptSphereIntersect(inPlane, inVertices, 3, inBegin, inDelta, sphereRadius, outPoint, contact.sweptFraction);
	
	if (intersect == false) {
		return false;
	}

	outPoint.copyTo(contact.staticContactPoint);



	Vector centerPosOnColl;
	

	contact.sweptDistance = posDiff;
	contact.sweptDistance *= contact.sweptFraction;
	centerPosOnColl = startPos;
	centerPosOnColl.add(contact.sweptDistance);	

	float centerToPointDist;

	outPoint.subtract(centerPosOnColl, contact.sweptObjectContactPointVector);

	contact.sweptObjectContactPointVector.normalize(contact.contactNormal, centerToPointDist);
	contact.contactNormal *= -1.0f;

	if (equalF(centerToPointDist, radius, e) == false) {

		contact.sweptObjectContactPointVector *= sphereRadius / centerToPointDist;
	
	} 
	
	

	return true;
}
*/


bool CollContact::sweptContactTri(const float& sphereRadius, const Vector3& startPos, const Vector3& posDiff, SweptContact& contact) {

	WE_3rdp_SweptEllipsoid::Plane inPlane(true);
	Vector2NoInit inVertices[3];
	WE_3rdp_SweptEllipsoid::Vector3 inBegin(true);
	WE_3rdp_SweptEllipsoid::Vector3 inDelta(true);
	WE_3rdp_SweptEllipsoid::Vector3 outPoint(true);
	
	Vector3* vertices = contact.vertices;

	inBegin = startPos;
	inDelta = posDiff;

	triToPlane(vertices, contact.normal, inPlane);

	WE_3rdp_SweptEllipsoid::Vector3 u;
	WE_3rdp_SweptEllipsoid::Vector3 v;

	inPlane.GetBasisVectors(u, v);

	inPlane.sConvertWorldToPlane(u, v, vertices[0], inVertices[0]);
	inPlane.sConvertWorldToPlane(u, v, vertices[1], inVertices[1]);
	inPlane.sConvertWorldToPlane(u, v, vertices[2], inVertices[2]);
	
	
	if (WE_3rdp_SweptEllipsoid::PolygonSweptSphereIntersect(inPlane, inVertices, 3, inBegin, inDelta, sphereRadius, outPoint, contact.sweptFraction, contact.sweptContactInfo)) {

		contact.vertexCount = 3;
		outPoint.copyTo(contact.contactPoint);
		return true;
	}
	
	return false;
}


bool CollContact::sweptContact(const float& sphereRadius, const Vector3& startPos, const Vector3& posDiff, OBB& staticOBB, SweptContact& contact) {

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

	inBegin = startPos;
	inDelta = posDiff;

	staticOBB.toPoints(obbPoints);

	for (int face = 0; face < 6; ++face) {

		OBB::getFaceCW(face, obbFace);

		triToPlane(obbPoints[obbFace[0]], obbPoints[obbFace[1]], obbPoints[obbFace[2]], true, temp, inPlane);

		inPlane.GetBasisVectors(u, v);

		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[0]], inVertices[0]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[1]], inVertices[1]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[2]], inVertices[2]);
		inPlane.sConvertWorldToPlane(u, v, obbPoints[obbFace[3]], inVertices[3]);

		if (WE_3rdp_SweptEllipsoid::PolygonSweptSphereIntersect(inPlane, inVertices, 4, inBegin, inDelta, sphereRadius, outPoint, contact.sweptFraction, contact.sweptContactInfo)) {

			contact.vertexCount = 4;

			contact.vertices[0] = obbPoints[obbFace[0]];
			contact.vertices[1] = obbPoints[obbFace[1]];
			contact.vertices[2] = obbPoints[obbFace[2]];
			contact.vertices[3] = obbPoints[obbFace[3]];

			contact.normal = inPlane.mNormal;
			outPoint.copyTo(contact.contactPoint);
			return true;
		}
	}

	return false;

}

CollContact::CollIteratorOBB::CollIteratorOBB() : inPlane(true), inBegin(true), inDelta(true) {
}

bool CollContact::collIterStart(CollIteratorOBB& iter, const float& sphereRadius, const Vector3& startPos, const Vector3& posDiff, OBB& staticOBB) {

	iter.inBegin = startPos;
	iter.inDelta = posDiff;

	iter.sphereRadius = sphereRadius;
	iter.face = 0;

	staticOBB.toPoints(iter.obbPoints);

	return true;
}

bool CollContact::collIterNext_sweptContact(CollIteratorOBB& iter, SweptContact& contact) {

	while (iter.face < 6) {

		int obbFace[4];
		Vector3 temp[2];
		
		WE_3rdp_SweptEllipsoid::Vector3 outPoint(true);

		WE_3rdp_SweptEllipsoid::Vector3 u;
		WE_3rdp_SweptEllipsoid::Vector3 v;

		OBB::getFaceCW(iter.face++, obbFace);

		triToPlane(iter.obbPoints[obbFace[0]], iter.obbPoints[obbFace[1]], iter.obbPoints[obbFace[2]], true, temp, iter.inPlane);

		iter.inPlane.GetBasisVectors(u, v);

		iter.inPlane.sConvertWorldToPlane(u, v, iter.obbPoints[obbFace[0]], iter.inVertices[0]);
		iter.inPlane.sConvertWorldToPlane(u, v, iter.obbPoints[obbFace[1]], iter.inVertices[1]);
		iter.inPlane.sConvertWorldToPlane(u, v, iter.obbPoints[obbFace[2]], iter.inVertices[2]);
		iter.inPlane.sConvertWorldToPlane(u, v, iter.obbPoints[obbFace[3]], iter.inVertices[3]);

		if (WE_3rdp_SweptEllipsoid::PolygonSweptSphereIntersect(iter.inPlane, iter.inVertices, 4, iter.inBegin, iter.inDelta, iter.sphereRadius, outPoint, contact.sweptFraction, contact.sweptContactInfo)) {

			contact.vertexCount = 4;

			contact.vertices[0] = iter.obbPoints[obbFace[0]];
			contact.vertices[1] = iter.obbPoints[obbFace[1]];
			contact.vertices[2] = iter.obbPoints[obbFace[2]];
			contact.vertices[3] = iter.obbPoints[obbFace[3]];

			contact.normal = iter.inPlane.mNormal;
			outPoint.copyTo(contact.contactPoint);
			return true;
		}
	}

	return false;
}

void CollContact::collIterEnd(CollIteratorOBB& iter) {
}


bool CollContact::contactOBB(const OBB& b0, const OBB& b1, Vector3& normal, float& depth, const bool& normalToB0) {

	Span s0[15];
    Span s1[15];
    Vector axis[9];
	float aLenSq;


	if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 0), b0, b1, s0[0], s1[0]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 1), b0, b1, s0[1], s1[1]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 2), b0, b1, s0[2], s1[2]))
         return false;

	if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 0), b0, b1, s0[3], s1[3]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 1), b0, b1, s0[4], s1[4]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 2), b0, b1, s0[5], s1[5]))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 0), axis[0]);
    if (!_axisOverlap_OBB_OBB(axis[0], b0, b1, s0[6], s1[6]))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 1), axis[1]);
    if (!_axisOverlap_OBB_OBB(axis[1], b0, b1, s0[7], s1[7]))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 2), axis[2]);
    if (!_axisOverlap_OBB_OBB(axis[2], b0, b1, s0[8], s1[8]))
         return false;


	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 0), axis[3]);
    if (!_axisOverlap_OBB_OBB(axis[3], b0, b1, s0[9], s1[9]))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 1), axis[4]);
    if (!_axisOverlap_OBB_OBB(axis[4], b0, b1, s0[10], s1[10]))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 2), axis[5]);
    if (!_axisOverlap_OBB_OBB(axis[5], b0, b1, s0[11], s1[11]))
         return false;


	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 0), axis[6]);
    if (!_axisOverlap_OBB_OBB(axis[6], b0, b1, s0[12], s1[12]))
         return false;

	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 1), axis[7]);
    if (!_axisOverlap_OBB_OBB(axis[7], b0, b1, s0[13], s1[13]))
         return false;

	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 2), axis[8]);
    if (!_axisOverlap_OBB_OBB(axis[8], b0, b1, s0[14], s1[14]))
         return false;
   
	int i;
	int j;
	
	
	float min;
    float max;
	float dSq;

	float minALenSq;
	float minDSq = FLT_MAX;
	int minPhase;
	int minI;

	for(i = 0; i < 3; ++i) {

		min = (s1[i].min > s0[i].min) ? s1[i].min : s0[i].min;
        max = (s1[i].max < s0[i].max)? s1[i].max : s0[i].max;

		dSq = max - min;
		dSq *= dSq;

		MOBBAxis(b0, i).magSquared(aLenSq);

		dSq /= aLenSq;

		if (dSq < minDSq) {

			minPhase = 0;
			minALenSq = aLenSq;
			minI = i;
			minDSq = dSq;
		}
	}

	for(j = 0;i < 6; ++i, ++j) {

		min = (s1[i].min > s0[i].min) ? s1[i].min : s0[i].min;
        max = (s1[i].max < s0[i].max)? s1[i].max : s0[i].max;

		dSq = max - min;
		dSq *= dSq;

		MOBBAxis(b1, j).magSquared(aLenSq);

		dSq /= aLenSq;

		if (dSq < minDSq) {

			minPhase = 1;
			minALenSq = aLenSq;
			minI = i;
			minDSq = dSq;

			
		}
	}

	for(j = 0; i < 15; ++i, ++j) {

		min = (s1[i].min > s0[i].min) ? s1[i].min : s0[i].min;
        max = (s1[i].max < s0[i].max)? s1[i].max : s0[i].max;

		dSq = max - min;
		dSq *= dSq;

		axis[j].magSquared(aLenSq);

		dSq /= aLenSq;

		if (dSq < minDSq) {

			minPhase = 2;
			minALenSq = aLenSq;
			minI = i;
			minDSq = dSq;
		}
	}


	depth = sqrtf(minDSq);

	switch (minPhase) {
		case 0:
			MOBBAxis(b0, minI).mul(sqrtf(minALenSq), normal);
			break;
		case 1:
			MOBBAxis(b1, minI - 3).mul(sqrtf(minALenSq), normal);
			break;
		case 2:
			j = minI - 6;
			axis[j].mul(sqrtf(minALenSq), normal);
			break;
	}

	//use as temp var
	Vector& diff = axis[0];

	if (normalToB0) {

		MOBBCenter(b1).subtract(MOBBCenter(b0), diff);

	} else {

		MOBBCenter(b0).subtract(MOBBCenter(b1), diff);
	}

	if (diff.dot(normal) > 0.0f) {

        normal.negate();
	}

    return true;
}

/*
bool CollContact::contactOBB(const OBB& b0, const OBB& b1, Vector3& normal, float& depth, const bool& normalToB0) {

	Span s0[15];
    Span s1[15];
    Vector axis[9];
	

	if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 0), b0, b1, s0[0], s1[0]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 1), b0, b1, s0[1], s1[1]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 2), b0, b1, s0[2], s1[2]))
         return false;

	if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 0), b0, b1, s0[3], s1[3]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 1), b0, b1, s0[4], s1[4]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 2), b0, b1, s0[5], s1[5]))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 0), axis[0]);
    if (!_axisOverlap_OBB_OBB(axis[0], b0, b1, s0[6], s1[6]))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 1), axis[1]);
    if (!_axisOverlap_OBB_OBB(axis[1], b0, b1, s0[7], s1[7]))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 2), axis[2]);
    if (!_axisOverlap_OBB_OBB(axis[2], b0, b1, s0[8], s1[8]))
         return false;


	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 0), axis[3]);
    if (!_axisOverlap_OBB_OBB(axis[3], b0, b1, s0[9], s1[9]))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 1), axis[4]);
    if (!_axisOverlap_OBB_OBB(axis[4], b0, b1, s0[10], s1[10]))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 2), axis[5]);
    if (!_axisOverlap_OBB_OBB(axis[5], b0, b1, s0[11], s1[11]))
         return false;


	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 0), axis[6]);
    if (!_axisOverlap_OBB_OBB(axis[6], b0, b1, s0[12], s1[12]))
         return false;

	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 1), axis[7]);
    if (!_axisOverlap_OBB_OBB(axis[7], b0, b1, s0[13], s1[13]))
         return false;

	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 2), axis[8]);
    if (!_axisOverlap_OBB_OBB(axis[8], b0, b1, s0[14], s1[14]))
         return false;
   
	int i;
	int j;
	
	float min;
    float max;
	float d;

	float minD = FLT_MAX;
	int minI;

	for(i = 0; i < 3; ++i) {

		min = (s1[i].min > s0[i].min) ? s1[i].min : s0[i].min;
        max = (s1[i].max < s0[i].max)? s1[i].max : s0[i].max;

		d = max - min;
		
		if (d < minD) {

			minI = i;
			minD = d;

			normal = MOBBAxis(b0, i);
		}
	}

	for(j = 0; i < 6; ++i, ++j) {

		min = (s1[i].min > s0[i].min) ? s1[i].min : s0[i].min;
        max = (s1[i].max < s0[i].max) ? s1[i].max : s0[i].max;

		d = max - min;
		
		if (d < minD) {

			minI = i;
			minD = d;

			normal = MOBBAxis(b1, j);
		}
	}

	for(j = 0; i < 15; ++i, ++j) {

		min = (s1[i].min > s0[i].min) ? s1[i].min : s0[i].min;
        max = (s1[i].max < s0[i].max) ? s1[i].max : s0[i].max;

		d = max - min;

		if (d < minD && (axis[j].magSquared() > 1.0E-8f)) {

			minI = i;
			minD = d;
		}
	}


	depth = minD;

	if (minI >= 6) {
		
		j = minI - 6;

		normal = axis[j];
	}

	//use as temp var
	Vector& diff = axis[0];

	if (normalToB0) {

		MOBBCenter(b1).subtract(MOBBCenter(b0), diff);

	} else {

		MOBBCenter(b0).subtract(MOBBCenter(b1), diff);
	}

	if (diff.dot(normal) > 0.0f) {

        normal.negate();
	}

	//log.log(LOG_FORCE, true, true, L"%d", minI);

    return true;
}


bool CollContact::contactOBBMainAxisOnly(const OBB& b0, const OBB& b1, Vector3& normal, float& depth, const bool& normalToB0) {

	Span s0[6];
    Span s1[6];
	

	if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 0), b0, b1, s0[0], s1[0]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 1), b0, b1, s0[1], s1[1]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 2), b0, b1, s0[2], s1[2]))
         return false;

	if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 0), b0, b1, s0[3], s1[3]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 1), b0, b1, s0[4], s1[4]))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 2), b0, b1, s0[5], s1[5]))
         return false;

	Vector3 axis;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 0), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 1), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 2), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;


	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 0), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 1), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 2), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;


	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 0), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 1), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 2), axis);
    if (!_axisOverlap_OBB_OBB(axis, b0, b1))
         return false;
   
	int i;
	int j;
	
	float min;
    float max;
	float d;

	float minD = FLT_MAX;
	int minI;

	for(i = 0; i < 3; ++i) {

		min = (s1[i].min > s0[i].min) ? s1[i].min : s0[i].min;
        max = (s1[i].max < s0[i].max)? s1[i].max : s0[i].max;

		d = max - min;
		
		if (d < minD) {

			minI = i;
			minD = d;

			normal = MOBBAxis(b0, i);
		}
	}

	for(j = 0; i < 6; ++i, ++j) {

		min = (s1[i].min > s0[i].min) ? s1[i].min : s0[i].min;
        max = (s1[i].max < s0[i].max) ? s1[i].max : s0[i].max;

		d = max - min;
		
		if (d < minD) {

			minI = i;
			minD = d;

			normal = MOBBAxis(b1, j);
		}
	}

	depth = minD;

	//use as temp var
	Vector& diff = axis;

	if (normalToB0) {

		MOBBCenter(b1).subtract(MOBBCenter(b0), diff);

	} else {

		MOBBCenter(b0).subtract(MOBBCenter(b1), diff);
	}

	if (diff.dot(normal) > 0.0f) {

        normal.negate();
	}

	//log.log(LOG_FORCE, true, true, L"%d", minI);

    return true;
}
*/

}