#include "WEIntersect.h"

#include "../WEMacros.h"
#include "WEDistance.h"
#include "intersect/WETriIntersect.h"
#include "float.h"

namespace WE {

bool intersect(const OBB &b0, const OBB &b1) {

	if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 0), b0, b1))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 1), b0, b1))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 2), b0, b1))
         return false;

	if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 0), b0, b1))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 1), b0, b1))
         return false;
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 2), b0, b1))
         return false;

	Vector axis;

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
   
    return true;
}


bool intersect(const OBB &b0, const OBB &b1, float e) {

	if (!_axisOverlap_e_OBB_OBB(MOBBAxis(b0, 0), b0, b1, e))
         return false;
    if (!_axisOverlap_e_OBB_OBB(MOBBAxis(b0, 1), b0, b1, e))
         return false;
    if (!_axisOverlap_e_OBB_OBB(MOBBAxis(b0, 2), b0, b1, e))
         return false;

	if (!_axisOverlap_e_OBB_OBB(MOBBAxis(b1, 0), b0, b1, e))
         return false;
    if (!_axisOverlap_e_OBB_OBB(MOBBAxis(b1, 1), b0, b1, e))
         return false;
    if (!_axisOverlap_e_OBB_OBB(MOBBAxis(b1, 2), b0, b1, e))
         return false;

	Vector axis;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 0), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 1), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 2), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;


	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 0), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 1), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 2), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;


	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 0), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 1), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 2), axis);
    if (!_axisOverlap_e_OBB_OBB(axis, b0, b1, e))
         return false;
   
    return true;
}

bool intersect(const OBB &b0, const AAB &b1) {

	if (!_axisOverlap_OBB_AAB(MOBBAxis(b0, 0), b0, b1))
         return false;
    if (!_axisOverlap_OBB_AAB(MOBBAxis(b0, 1), b0, b1))
         return false;
    if (!_axisOverlap_OBB_AAB(MOBBAxis(b0, 2), b0, b1))
         return false;

	if (!_axisOverlap_OBB_AAB(AAB::axis[0], b0, b1))
         return false;
    if (!_axisOverlap_OBB_AAB(AAB::axis[1], b0, b1))
         return false;
    if (!_axisOverlap_OBB_AAB(AAB::axis[2], b0, b1))
         return false;

	Vector axis;

	MOBBAxis(b0, 0).cross(AAB::axis[0], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 0).cross(AAB::axis[1], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 0).cross(AAB::axis[2], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;


	MOBBAxis(b0, 1).cross(AAB::axis[0], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 1).cross(AAB::axis[1], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 1).cross(AAB::axis[2], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;


	MOBBAxis(b0, 2).cross(AAB::axis[0], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 2).cross(AAB::axis[1], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;

	MOBBAxis(b0, 2).cross(AAB::axis[2], axis);
    if (!_axisOverlap_OBB_AAB(axis, b0, b1))
         return false;
   
    return true;
}


bool intersect(const OBB &b0, const AAB &b1, float e) {

	if (!_axisOverlap_e_OBB_AAB(MOBBAxis(b0, 0), b0, b1, e))
         return false;
    if (!_axisOverlap_e_OBB_AAB(MOBBAxis(b0, 1), b0, b1, e))
         return false;
    if (!_axisOverlap_e_OBB_AAB(MOBBAxis(b0, 2), b0, b1, e))
         return false;

	if (!_axisOverlap_e_OBB_AAB(AAB::axis[0], b0, b1, e))
         return false;
    if (!_axisOverlap_e_OBB_AAB(AAB::axis[1], b0, b1, e))
         return false;
    if (!_axisOverlap_e_OBB_AAB(AAB::axis[2], b0, b1, e))
         return false;

	Vector axis;

	MOBBAxis(b0, 0).cross(AAB::axis[0], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 0).cross(AAB::axis[1], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 0).cross(AAB::axis[2], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;


	MOBBAxis(b0, 1).cross(AAB::axis[0], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 1).cross(AAB::axis[1], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 1).cross(AAB::axis[2], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;


	MOBBAxis(b0, 2).cross(AAB::axis[0], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 2).cross(AAB::axis[1], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;

	MOBBAxis(b0, 2).cross(AAB::axis[2], axis);
    if (!_axisOverlap_e_OBB_AAB(axis, b0, b1, e))
         return false;
   
    return true;
}




bool intersect(const OBB& b0, const TriangleEx1& tri) {

	
	if (!_axisOverlap_OBB_Tri(tri.normal, b0, tri))
         return false;

	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 0), b0, tri))
         return false;

	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 1), b0, tri))
         return false;

	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 2), b0, tri))
         return false;

	Vector axis;
	Vector edge[3];

	tri.computeEdges(edge);

	MOBBAxis(b0, 0).cross(edge[0], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;

	MOBBAxis(b0, 0).cross(edge[1], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;

	MOBBAxis(b0, 0).cross(edge[2], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;


	MOBBAxis(b0, 1).cross(edge[0], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;

	MOBBAxis(b0, 1).cross(edge[1], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;

	MOBBAxis(b0, 1).cross(edge[2], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;


	MOBBAxis(b0, 2).cross(edge[0], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;

	MOBBAxis(b0, 2).cross(edge[1], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;

	MOBBAxis(b0, 2).cross(edge[2], axis);
    if (!_axisOverlap_OBB_Tri(axis, b0, tri))
         return false;

	return true;
}


bool intersect(const OBB& b0, const TriangleEx1& tri, float e) {

	
	if (!_axisOverlap_e_OBB_Tri(tri.normal, b0, tri, e))
         return false;

	if (!_axisOverlap_e_OBB_Tri(MOBBAxis(b0, 0), b0, tri, e))
         return false;

	if (!_axisOverlap_e_OBB_Tri(MOBBAxis(b0, 1), b0, tri, e))
         return false;

	if (!_axisOverlap_e_OBB_Tri(MOBBAxis(b0, 2), b0, tri, e))
         return false;

	Vector axis;
	Vector edge[3];

	tri.computeEdges(edge);

	MOBBAxis(b0, 0).cross(edge[0], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;

	MOBBAxis(b0, 0).cross(edge[1], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;

	MOBBAxis(b0, 0).cross(edge[2], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;


	MOBBAxis(b0, 1).cross(edge[0], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;

	MOBBAxis(b0, 1).cross(edge[1], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;

	MOBBAxis(b0, 1).cross(edge[2], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;


	MOBBAxis(b0, 2).cross(edge[0], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;

	MOBBAxis(b0, 2).cross(edge[1], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;

	MOBBAxis(b0, 2).cross(edge[2], axis);
    if (!_axisOverlap_e_OBB_Tri(axis, b0, tri, e))
         return false;

	return true;
}

bool intersect(const AAB &b0, const AAB &b1) {

	if (!_axisOverlap_AAB_AAB(AAB::axis[0], b0, b1))
         return false;

	if (!_axisOverlap_AAB_AAB(AAB::axis[1], b0, b1))
         return false;

	if (!_axisOverlap_AAB_AAB(AAB::axis[2], b0, b1))
         return false;
	
	return true;
}


bool intersect(const AAB &b0, const AAB &b1, float e) {

	if (!_axisOverlap_e_AAB_AAB(AAB::axis[0], b0, b1, e))
         return false;

	if (!_axisOverlap_e_AAB_AAB(AAB::axis[1], b0, b1, e))
         return false;

	if (!_axisOverlap_e_AAB_AAB(AAB::axis[2], b0, b1, e))
         return false;
	
	return true;
}

bool intersect(const AAB &b0, const Sphere& sphere) {

	float s, d = 0; 

    for(int i = 0; i < 3; i++) {

		if( sphere.center[i] < b0.min.el[i] ) {

            s = sphere.center[i] - b0.min.el[i];
            d += s*s; 

        } else if( sphere.center[i] > b0.max.el[i] ) {

            s = sphere.center[i] - b0.max.el[i];
            d += s*s; 
        }

    }

	return d <= sphere.radius * sphere.radius;
}

bool intersect(Frustum &f0, const OBB &b1) {

	f0.updateNormalsEdges();

	//1. Face normals of the view frustum (5 axes)
	if (!_axisOverlap_Frustum_OBB(f0.normals[Frustum::N_Far], f0, b1))
         return false;

	if (!_axisOverlap_Frustum_OBB(f0.normals[Frustum::N_Top], f0, b1))
		return false;

	if (!_axisOverlap_Frustum_OBB(f0.normals[Frustum::N_Left], f0, b1))
		return false;

	if (!_axisOverlap_Frustum_OBB(f0.normals[Frustum::N_Bottom], f0, b1))
		return false;

	if (!_axisOverlap_Frustum_OBB(f0.normals[Frustum::N_Right], f0, b1))
		return false;
	
	//2. Face normals of the OOB (3 axes)
	if (!_axisOverlap_Frustum_OBB(MOBBAxis(b1, 0), f0, b1))
         return false;
    if (!_axisOverlap_Frustum_OBB(MOBBAxis(b1, 1), f0, b1))
         return false;
    if (!_axisOverlap_Frustum_OBB(MOBBAxis(b1, 2), f0, b1))
         return false;

	Vector3 normal;

	//could be unrolled with templates programming 
	//but would that be better or worse??
	for (int fi = 0; fi < Frustum::EdgeCount; ++fi) {

		for (int bi = 0; bi < 3; ++bi) {

			MOBBAxis(b1, bi).cross(f0.edges[fi], normal);

			if (!_axisOverlap_Frustum_OBB(normal, f0, b1))
				return false;
		}
	}

	return true;

}

/*
bool intersect(const Frustum &f0, const OBB &b1) {


	//The axes you must test are :
//
//		1. Face normals of the view frustum (5 axes)
//		2. Face normals of the OOB (3 axes)
//		3. Cross product between all edge directions of the view frustum (6
//		vectors) with all edge directions of the cube (3 vectors). This is 18
//		different axes to test. 


	//1. Face normals of the view frustum (5 axes)
	if (!_axisOverlap_Frustum_OBB(f0.dVector(), f0, b1))
         return false;

	Vector3 edge[6];
	Vector3 normal;

	Vector3 nearPoint;
	Vector3 offset;
	Vector3 tempVect;

	int edgeIndex = 2;

	f0.getNearPoint(nearPoint);

	
	{
		
		f0.uVector().mul(f0.uBound(), offset);
		nearPoint.add(offset, tempVect);
		tempVect.subtract(f0.origin(), edge[edgeIndex]);

		edge[edgeIndex].cross(f0.lVector(), normal);
		++edgeIndex;

		if (!_axisOverlap_Frustum_OBB(normal, f0, b1))
         return false;
	}

	{
		f0.lVector().mul(f0.lBound(), offset);
		nearPoint.add(offset, tempVect);
		tempVect.subtract(f0.origin(), edge[edgeIndex]);

		edge[edgeIndex].cross(f0.uVector(), normal);
		++edgeIndex;

		if (!_axisOverlap_Frustum_OBB(normal, f0, b1))
         return false;
	}
	
	{
		f0.uVector().mul(f0.uBound(), offset);
		nearPoint.subtract(offset, tempVect);
		tempVect.subtract(f0.origin(), edge[edgeIndex]);

		edge[edgeIndex].cross(f0.lVector(), normal);
		++edgeIndex;

		if (!_axisOverlap_Frustum_OBB(normal, f0, b1))
         return false;
	}

	{
		f0.lVector().mul(f0.lBound(), offset);
		nearPoint.subtract(offset, tempVect);
		tempVect.subtract(f0.origin(), edge[edgeIndex]);

		edge[edgeIndex].cross(f0.uVector(), normal);
		++edgeIndex;

		if (!_axisOverlap_Frustum_OBB(normal, f0, b1))
         return false;
	}

	//2. Face normals of the OOB (3 axes)
	if (!_axisOverlap_Frustum_OBB(MOBBAxis(b1, 0), f0, b1))
         return false;
    if (!_axisOverlap_Frustum_OBB(MOBBAxis(b1, 1), f0, b1))
         return false;
    if (!_axisOverlap_Frustum_OBB(MOBBAxis(b1, 2), f0, b1))
         return false;
	

	edge[0] = f0.lVector();
	edge[1] = f0.uVector();

	//could be unrolled with templates programming 
	//but would that be better or worse??
	for (int fi = 0; fi < 6; ++fi) {

		for (int bi = 0; bi < 3; ++bi) {

			MOBBAxis(b1, bi).cross(edge[fi], normal);

			if (!_axisOverlap_Frustum_OBB(normal, f0, b1))
				return false;
		}
	}

	return true;
}
*/

bool intersect(const AAB &b0, const Sphere& sphere, float e) {

	float s, d = 0; 

    for (int i = 0; i < 3; i++) {

		if( sphere.center[i] < b0.min.el[i] ) {

            s = sphere.center[i] - b0.min.el[i];
            d += s*s; 

        } else if( sphere.center[i] > b0.max.el[i] ) {

            s = sphere.center[i] - b0.max.el[i];
            d += s*s; 
        }

    }

	return d <= (sphere.radius + e) * (sphere.radius + e);
}

float squaredDistance(const OBB &b0, const Point& point) {

	Point d;

	point.subtract(MOBBCenter(b0), d);

    float squaredDistance = 0.0f;
	float coord;
    for (int i = 0; i < 3; i++) {

		coord = d.dot(MOBBAxis(b0, i));

		if ( coord < -MOBBExtents(b0).el[i] )
            squaredDistance += (coord + MOBBExtents(b0).el[i]) * (coord + MOBBExtents(b0).el[i]);
        else if ( coord > MOBBExtents(b0).el[i] )
            squaredDistance += (coord - MOBBExtents(b0).el[i])*(coord - MOBBExtents(b0).el[i]);
    } 

	return squaredDistance;
}

bool intersect(const OBB &b0, const Sphere &sphere) {

	return squaredDistance(b0, sphere.center) <= (sphere.radius * sphere.radius);
}

bool intersect(const OBB &b0, const Sphere &sphere, float e) {

	return squaredDistance(b0, sphere.center) <= ((sphere.radius + e) * (sphere.radius + e));
}

bool intersect(const AAB& b0, const TriangleEx1& tri) {

	
	if (!_axisOverlap_AAB_Tri(tri.normal, b0, tri))
         return false;

	if (!_axisOverlap_AAB_Tri(AAB::axis[0], b0, tri))
         return false;

	if (!_axisOverlap_AAB_Tri(AAB::axis[1], b0, tri))
         return false;

	if (!_axisOverlap_AAB_Tri(AAB::axis[2], b0, tri))
         return false;

	Vector axis;
	Vector edge[3];

	tri.computeEdges(edge);

	AAB::axis[0].cross(edge[0], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;

	AAB::axis[0].cross(edge[1], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;

	AAB::axis[0].cross(edge[2], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;


	AAB::axis[1].cross(edge[0], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;

	AAB::axis[1].cross(edge[1], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;

	AAB::axis[1].cross(edge[2], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;


	AAB::axis[2].cross(edge[0], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;

	AAB::axis[2].cross(edge[1], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;

	AAB::axis[2].cross(edge[2], axis);
    if (!_axisOverlap_AAB_Tri(axis, b0, tri))
         return false;

	return true;
}

bool intersect(const AAB& b0, const TriangleEx1& tri, float e) {

	
	if (!_axisOverlap_e_AAB_Tri(tri.normal, b0, tri, e))
         return false;

	if (!_axisOverlap_e_AAB_Tri(AAB::axis[0], b0, tri, e))
         return false;

	if (!_axisOverlap_e_AAB_Tri(AAB::axis[1], b0, tri, e))
         return false;

	if (!_axisOverlap_e_AAB_Tri(AAB::axis[2], b0, tri, e))
         return false;

	Vector axis;
	Vector edge[3];

	tri.computeEdges(edge);

	AAB::axis[0].cross(edge[0], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;

	AAB::axis[0].cross(edge[1], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;

	AAB::axis[0].cross(edge[2], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;


	AAB::axis[1].cross(edge[0], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;

	AAB::axis[1].cross(edge[1], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;

	AAB::axis[1].cross(edge[2], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;


	AAB::axis[2].cross(edge[0], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;

	AAB::axis[2].cross(edge[1], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;

	AAB::axis[2].cross(edge[2], axis);
    if (!_axisOverlap_e_AAB_Tri(axis, b0, tri, e))
         return false;

	return true;
}

bool intersect(const Sphere& s0, const Sphere& s1) {

	float dSq = distanceSq(s0.center, s1.center);

	return dSq <= (s0.radius + s1.radius) * (s0.radius + s1.radius);
}

bool intersect(const Sphere& s0, const Sphere& s1, float e) {

	float dSq = distanceSq(s0.center, s1.center);

	return dSq <= (s0.radius + s1.radius) * ((s0.radius + e) + (s1.radius + e));
}

bool intersect(const TriangleEx1& tri0, const TriangleEx1& tri1) {


	return (tri_tri_intersect(tri0.vertices[0].el, tri0.vertices[1].el , tri0.vertices[2].el,
								tri1.vertices[0].el, tri1.vertices[1].el , tri1.vertices[2].el) != 0);
}

bool intersect(const TriangleEx1& tri0, const TriangleEx1& tri1, float e) {


	return (tri_tri_intersect(tri0.vertices[0].el, tri0.vertices[1].el , tri0.vertices[2].el,
								tri1.vertices[0].el, tri1.vertices[1].el , tri1.vertices[2].el) != 0);
}

inline bool _isInside(const Sphere& s0, const Point& pt, float& rSq, float& dSq) {

	dSq = distanceSq(s0.center, pt);

	return (dSq <= rSq);
}


inline float _roughTriangleRadius(const Point& v0, const Point& v1, const Point& v2) {

	return MMax(
		(v0.x > v1.x) ? (v0.x - v1.x) : (v1.x - v0.x)
		+ (v0.y > v1.y) ? (v0.y - v1.y) : (v1.y - v0.y)
		+ (v0.z > v1.z) ? (v0.z - v1.z) : (v1.z - v0.z)
		,
		(v0.x > v2.x) ? (v0.x - v2.x) : (v2.x - v0.x)
		+ (v0.y > v2.y) ? (v0.y - v2.y) : (v2.y - v0.y)
		+ (v0.z > v2.z) ? (v0.z - v2.z) : (v2.z - v0.z)
		);
}


bool intersect(const Sphere& s0, const TriangleEx1& tri, bool tryEarlyOut) {

	
	float triSpanPoint = tri.vertices[0].dot(tri.normal);

	Span s;
	computeSpan(tri.normal, s0, s.min, s.max);


	if (MBetweenMinMaxIncl(triSpanPoint, s.min, s.max) == false) {
		return false;
	}


	if (tryEarlyOut) {

		//take a guess at a separating axis
		Vector axis;
		tri.vertices[0].subtract(s0.center, axis);

		if (_axisOverlap(axis, s0, tri) == false) {
			return false;
		}
	}


	

	//sphere does intersect tri plane, more checks

	//adaptive checking
	//float triRadius = _roughTriangleRadius(v0, v1, v2);
	
	float rSq = s0.radius * s0.radius;
	float dSq;
	bool between;


	if (_isInside(s0, tri.vertices[0], rSq, dSq)) {
		return true;
	}
	dSq = distanceSq(tri.vertices[0], tri.vertices[1], s0.center, between);
	if (between && (dSq <= rSq)) {
		return true;
	}


	if (_isInside(s0, tri.vertices[1], rSq, dSq)) {
		return true;
	}
	dSq = distanceSq(tri.vertices[1], tri.vertices[2], s0.center, between);
	if (between && (dSq <= rSq)) {
		return true;
	}


	if (_isInside(s0, tri.vertices[2], rSq, dSq)) {
		return true;
	}
	dSq = distanceSq(tri.vertices[2], tri.vertices[0], s0.center, between);
	if (between && (dSq <= rSq)) {
		return true;
	}
	

	//last case sphere projection inside large triangle
	Point proj;

	project(tri.vertices[0], tri.vertices[1], tri.normal, s0.center, proj);
	if (coplanarPointInsideTriangle(tri, proj)) {
		return true;
	}


	return false;
}


bool intersect(const Sphere& s0, const TriangleEx1& tri, float e, bool tryEarlyOut) {

	
	float triSpanPoint = tri.vertices[0].dot(tri.normal);

	Span s;
	computeSpan(tri.normal, s0, s.min, s.max);


	if (MBetweenMinMaxIncl_e(triSpanPoint, s.min, s.max, e) == false) {
		return false;
	}


	if (tryEarlyOut) {

		//take a guess at a separating axis
		Vector axis;
		tri.vertices[0].subtract(s0.center, axis);

		if (_axisOverlap_e(axis, s0, tri, e) == false) {
			return false;
		}
	}


	

	//sphere does intersect tri plane, more checks

	//adaptive checking
	//float triRadius = _roughTriangleRadius(v0, v1, v2);
	
	/*
	float rSq = (s0.radius + e) * (s0.radius + e);
	float dSq;
	bool between;


	if (_isInside(s0, tri.vertices[0], rSq, dSq)) {
		return true;
	}
	dSq = distanceSq(tri.vertices[0], tri.vertices[1], s0.center, between);
	if (between && (dSq <= rSq)) {
		return true;
	}


	if (_isInside(s0, tri.vertices[1], rSq, dSq)) {
		return true;
	}
	dSq = distanceSq(tri.vertices[1], tri.vertices[2], s0.center, between);
	if (between && (dSq <= rSq)) {
		return true;
	}


	if (_isInside(s0, tri.vertices[2], rSq, dSq)) {
		return true;
	}
	dSq = distanceSq(tri.vertices[2], tri.vertices[0], s0.center, between);
	if (between && (dSq <= rSq)) {
		return true;
	}
	*/
	

	//last case sphere projection inside large triangle
	Point proj;

	project(tri.vertices[0], tri.vertices[1], tri.normal, s0.center, proj);
	if (coplanarPointInsideTriangle(tri, proj, 0.3f)) {
		return true;
	}


	return false;
}
/*
int LineSphereIntersection(Sphere sphere, Line3d line, float t[2])
{
float a, b, c, discrm;
Vector3D pMinusC = l.origin - sphere.center;
a = Dot(line.direction, line.direction);
b = 2 * Dot(l.direction, pMinusC);
c = Dot(pMinusC, pMinusC) - sphere.radius * sphere.radius;
discrm = b * b - 4 * a * c;
if (discrm > 0) {
t[0] = (-b + sqrt(discrm)) / (2 * a);
t[1] = (-b - sqrt(discrm)) / (2 * a);
return 2;
} else if (discrm == 0) {
// The line is tangent to the sphere
t[0] = -b / (2 * a);
return 1;
} else {
return 0;
}
}
*/

/*
bool intersect(const Point& sphereCenter, float sphereRadius, const Point& segmentA, const Point& segmentB) {
	
	short intCount;
	float t[2];

	float a, b, c, discrm;
	Vector3 bMinusA;
	Vector3 pMinusC;
	Vector3 lineDir;

	segmentB.subtract(segmentA, bMinusA);
	//bMinusA.normalize(lineDir);
	lineDir = bMinusA;
		
	segementA.subtract(sphereCenter, pMinusC);
	a = lineDir.dot(lineDir);
	b = 2.0f * lineDir.dot(pMinusC);
	c = pMinusC.dot(pMinusC) - sphereRadius * sphereRadius;

	discrm = b * b - 4.0f * a * c;

	if (discrm > 0) {
		
		intCount = 2;
		
		float sqrt_discrm = sqrtf(discrm);

		t[0] = (-b + sqrt_discrm) / (2.0f * a);
		t[1] = (-b - sqrt_discrm) / (2.0f * a);

	} else if (discrm == 0) {

		intCount = 1;

		t[0] = -b / (2 * a);

	} else {

		intCount = 0;
	}

		
	return (intCount != 0);
}
*/

bool intersect(const Point& sphereCenter, float sphereRadius, const Point& segmentA, const Point& segmentB) {

	Vector3 cMinusP;
	Vector3 bMinusA;

	segmentB.subtract(segmentA, bMinusA);
	//bMinusA.normalize(lineDir);
	//lineDir = bMinusA;

	sphereCenter.subtract(segmentA, cMinusP);

	float u = cMinusP.dot(bMinusA) / bMinusA.dot(bMinusA);

	if ((u < 0) && (u > 1)) {

		return false;
	}

	Point closestPoint;

	bMinusA.mul(u, closestPoint);
	closestPoint += segmentA;

	if (distanceSq(sphereCenter, closestPoint) < sphereRadius * sphereRadius) {

		return true;
	}

	return false;
}

bool intersectSegPlane(const Vector3& planePos, const Vector3& planeNormal, const Vector3& segPt1, const Vector3& segPt2, float& u, Vector3* pIntersection) {

	Vector3 lineDir;

	segPt2.subtract(segPt1, lineDir);

	return (intersectPlane(planePos, planeNormal, segPt1, lineDir, u, pIntersection)
			&& u >= 0.0f && u <= 1.0f);
}

bool intersectPlane(const Vector3& planePos, const Vector3& planeNormal, const Vector3& lineOrig, const Vector3& lineDir, float& u, Vector3* pIntersection) {

	/*
	The equation of a plane (points P are on the plane with normal N and point P3 on the plane) can be written as

		N dot (P - P3) = 0

		The equation of the line (points P on the line passing through points P1 and P2) can be written as

		P = P1 + u (P2 - P1)

		The intersection of these two occurs when

		N dot (P1 + u (P2 - P1)) = N dot P3

		Solving for u gives

		Note

			* If the denominator is 0 then the normal to the plane is perpendicular to the line. Thus the line is either parallel to the plane and there are no solutions or the line is on the plane in which case there are an infinite number of solutions

			* If it is necessary to determine the intersection of the line segment between P1 and P2 then just check that u is between 0 and 1.

	*/

	float coeff = planeNormal.dot(lineDir);

	if (coeff == 0.0f)
		return false;

	Vector3 temp;

	planePos.subtract(lineOrig, temp);

	u = (planeNormal.dot(temp)) / coeff;

	if (pIntersection) {

		lineDir.mul(u, *pIntersection);
		pIntersection->add(lineOrig);
	}

	return true;
}

bool intersectAAPlane(const Vector3& planePos, int normalDim, const Vector3& lineOrig, const Vector3& lineDir, float& u, Vector3* pIntersection) {

	float coeff = lineDir[normalDim];

	if (coeff == 0.0f)
		return false;

	Vector3 temp;

	planePos.subtract(lineOrig, temp);

	u = (temp[normalDim]) / coeff;

	if (pIntersection) {

		lineDir.mul(u, *pIntersection);
		pIntersection->add(lineOrig);
	}

	return true;
}


bool intersect(const Plane& pl0, const Vector3& lineOrig, const Vector3& lineDir, float& u, Vector3* pIntersection) {

	return intersectPlane(pl0.position, pl0.normal, lineOrig, lineDir, u, pIntersection);
}

bool intersectSlab1D(const float& min, const float& max, 
						const float& lineStart, const float& slope, 
						Span& intersection) {
	
	if (slope == 0.0f) {

		if (MBetweenMinMaxIncl(lineStart, min, max)) {

			intersection.min = -FLT_MAX;
			intersection.max = FLT_MAX;

			return true;

		} else {

			return false;
		}
	}

	intersection.min = (min - lineStart) / slope;
	intersection.max = (max - lineStart) / slope;
	
	intersection.order();

	return true;
}

bool intersectSlab1D(const float& min, const float& max, 
						const float& point) {
	
	return MBetweenMinMaxIncl(point, min, max);
}

bool intersectSlab1D(const float& min, const float& max, 
						const float& point, Span& intersection) {
	
	if (intersectSlab1D(min, max, point)) {

		intersection.min = point;
		intersection.max = point;
	}

	return false;
}

bool intersectSlab(const Vector3& slabDir, const float& slabMin, const float& slabMax, 
					const Vector3& lineOrig, const Vector3& lineDir, Span& intersection) {

	float lineStart = lineOrig.dot(slabDir);
	float lineSlope = lineDir.dot(slabDir);

	return intersectSlab1D(slabMin, slabMax, lineStart, lineSlope, intersection);
}

bool intersectSlab(const Vector3& slabDir, const float& slabMin, const float& slabMax, 
					const Vector3& point, Span& intersection) {

	float pointVal = point.dot(slabDir);

	return intersectSlab1D(slabMin, slabMax, pointVal, intersection);
}

bool intersectSlab(const Vector3& slabDir, const float& slabMin, const float& slabMax, 
					const Vector3& point) {

	float pointVal = point.dot(slabDir);

	return intersectSlab1D(slabMin, slabMax, pointVal);
}

bool intersectSlab(const Vector2& slabDir, const float& slabMin, const float& slabMax, 
					const Vector2& point, Span& intersection) {

	float pointVal = point.dot(slabDir);

	return intersectSlab1D(slabMin, slabMax, pointVal, intersection);
}

bool intersectSlab(const Vector2& slabDir, const float& slabMin, const float& slabMax, 
					const Vector2& point) {

	float pointVal = point.dot(slabDir);

	return intersectSlab1D(slabMin, slabMax, pointVal);
}

bool intersectSlab(const Vector2& slabDir, const float& slabMin, const float& slabMax, 
					const Vector2& lineOrig, const Vector2& lineDir, Span& intersection) {

	float lineStart = lineOrig.dot(slabDir);
	float lineSlope = lineDir.dot(slabDir);

	return intersectSlab1D(slabMin, slabMax, lineStart, lineSlope, intersection);
}


bool intersectAASlab(const int& normalDim, const float& min, const float& max, const Vector3& lineOrig, const Vector3& lineDir, Span& intersection) {

	float coeff = lineDir[normalDim];

	if (coeff == 0.0f) {

		if (MBetweenMinMaxIncl(lineOrig.el[normalDim], min, max)) {

			intersection.min = -FLT_MAX;
			intersection.max = FLT_MAX;

			return true;

		} else {

			return false;
		}
	}

	intersection.min = (min - lineOrig.el[normalDim]) / coeff;
	intersection.max = (max - lineOrig.el[normalDim]) / coeff;
	
	intersection.order();

	return true;
}


bool intersectAASlab(const int& normalDim, const float& min, const float& max, const Vector2& lineOrig, const Vector2& lineDir, Span& intersection) {

	float coeff = lineDir[normalDim];

	if (coeff == 0.0f) {

		if (MBetweenMinMaxIncl(lineOrig.el[normalDim], min, max)) {

			intersection.min = -FLT_MAX;
			intersection.max = FLT_MAX;

			return true;

		} else {

			return false;
		}
	}

	intersection.min = (min - lineOrig.el[normalDim]) / coeff;
	intersection.max = (max - lineOrig.el[normalDim]) / coeff;
	
	intersection.order();

	return true;
}

bool intersect(const AAB& b, const Vector3& lineOrig, const Vector3& lineDir, Span& intersectionInterval) {

	Span& span = intersectionInterval;
	Span tempSpan;

	if (!intersectAASlab(0, b.min.el[0], b.max.el[0], lineOrig, lineDir, span))
		return false;

	if (!intersectAASlab(1, b.min.el[1], b.max.el[1], lineOrig, lineDir, tempSpan))
		return false;

	if (!spanCheckGetOverlap(span, tempSpan, span)) {

		return false;
	}

	if (!intersectAASlab(2, b.min.el[2], b.max.el[2], lineOrig, lineDir, tempSpan))
		return false;

	if (!spanCheckGetOverlap(span, tempSpan, span)) {

		return false;
	}

	return true;
}

bool intersect(const OBB& b, const Vector3& lineOrig, const Vector3& lineDir, Span& intersectionInterval) {

	Span& span = intersectionInterval;
	Span tempSpan;

	float centerDot;

	centerDot = MOBBCenter(b).dot(MOBBAxis(b, 0));

	if (!intersectSlab(MOBBAxis(b, 0), centerDot - MOBBExtents(b).el[0], centerDot + MOBBExtents(b).el[0], 
							lineOrig, lineDir, span))
		return false;

	centerDot = MOBBCenter(b).dot(MOBBAxis(b, 0));

	if (!intersectSlab(MOBBAxis(b, 1), centerDot - MOBBExtents(b).el[1], centerDot + MOBBExtents(b).el[1], 
							lineOrig, lineDir, tempSpan))
		return false;

	if (!spanCheckGetOverlap(span, tempSpan, span)) {

		return false;
	}

	centerDot = MOBBCenter(b).dot(MOBBAxis(b, 1));

	if (!intersectSlab(MOBBAxis(b, 2), centerDot - MOBBExtents(b).el[2], centerDot + MOBBExtents(b).el[2], 
							lineOrig, lineDir, tempSpan))
		return false;

	if (!spanCheckGetOverlap(span, tempSpan, span)) {

		return false;
	}

	return true;
}

bool intersectSegment(const AAB& b, const Vector3& segPt1, const Vector3& segPt2, Span& intersectionInterval) {

	Vector3 lineDir;

	segPt2.subtract(segPt1, lineDir);

	if (!intersect(b, segPt1, lineDir, intersectionInterval))
		return false;

	Span segmentSpan(0.0f, 1.0f);

	if (!spanCheckGetOverlap(intersectionInterval, segmentSpan, intersectionInterval)) {

		return false;
	}

	return true;
}

bool intersectSegment(const OBB& b, const Vector3& segPt1, const Vector3& segPt2, Span& intersectionInterval) {

	Vector3 lineDir;

	segPt2.subtract(segPt1, lineDir);

	if (!intersect(b, segPt1, lineDir, intersectionInterval))
		return false;

	Span segmentSpan(0.0f, 1.0f);

	if (!spanCheckGetOverlap(intersectionInterval, segmentSpan, intersectionInterval)) {

		return false;
	}

	return true;
}

bool intersect(const OBB& b0, const OBB& b1, Vector3& normal, float& depth, const bool& normalToB0) {

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

}