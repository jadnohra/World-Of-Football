#ifndef _WEIntersect_h
#define _WEIntersect_h

#include "WESpan.h"
#include "WEFrustum.h"

namespace WE {

	/*

			| OBB | AAB | Sph | Tri | Seg. | Ray | Fru.
		---------------------------------------------------
		OBB	|  *  |	 *	|  *  |	 *  |      |     |  *
			-----------------------------------------------
		AAB	|  *  |	 *	|  *  |	 *  |	   |     |
			-----------------------------------------------
		Sph	|  *  |	 *	|  *  |	 *  |  *   |     | 
			-----------------------------------------------
		Tri	|  *  |	 *	|  *  |	 *  |      | *   |
			-----------------------------------------------
	    Seg.|     |	  	|  *  |	    |      |     |
		    -----------------------------------------------
        Ray |     |	  	|     |	 *  |      |     |
		    -----------------------------------------------
		Fru |  *  |	  	|     |	 *  |      |     | 
		    -----------------------------------------------

	*/

	/*
		In general, when testing for separation of two
		convex polyhedra P0 and P1, the set of potential
		separating axes consists of the face normals of P0,
		the face normals of P1, and cross products E0xE1
		where E0 is an edge of P0 and E1 is an edge of P1.
		A floating-point implementation should pay attention
		to whenever E0xE1 is nearly the zero vector. 
	*/

	/*
			The axis-aligned rectangular  box is a very simple convex polyhedron.
		The non-axis-aligned frustum is a slightly less simple convex
		polyhedron--still a hexahedron, (with near and far clip planes).  For
		perspective projection, the frustum is not completely rectangular,
		(but it IS what is called a "right-rectangular frustum").

		Several recent threads discuss fast tests for intersection of convex
		polyhedra based on Stefan Gottschalk's separating axis theorem--see
		his Siggraph 96 paper for a statement of it.

		For the box and the frustum, you would have to test on at most 26
		potential separating axes. If you are willing to fail,  rarely, to
		cull a box that is completely outside the frustum, then you can limit
		the testing to 8 potential separating axes. To gain confidence that
		the 8-axis test would only rarely fail to cull a cullable box,  try
		constructing non-intersecting box-frustum pairs that are not separated
		by a plane parallel to a face of one or the other. Not culling a
		cullable box does not hurt the picture, but means that you spend more
		time in clipping. I'd guess, for most worlds, it would still be a win
		over the 26-axis test.

		Constructing the code to carry out the separating axis tests is an
		exercise is elementary vector algebra. 
	*/

	/*
		The axes you must test are :

		1. Face normals of the view frustum (5 axes)
		2. Face normals of the OOB (3 axes)
		3. Cross product between all edge directions of the view frustum (6
		vectors) with all edge directions of the cube (3 vectors). This is 18
		different axes to test. 
	*/


	bool intersect(const AAB &b0, const AAB &b1);
	bool intersect(const AAB &b0, const AAB &b1, float e);
	bool intersect(const AAB &b0, const Sphere& sphere);
	bool intersect(const AAB &b0, const Sphere& sphere, float e);
	bool intersect(const AAB& b0, const TriangleEx1& tri);
	bool intersect(const AAB& b0, const TriangleEx1& tri, float e);
	

	bool intersect(const OBB &b0, const OBB &b1);
	bool intersect(const OBB &b0, const OBB &b1, float e);
	bool intersect(const OBB& b0, const TriangleEx1& tri);
	bool intersect(const OBB& b0, const TriangleEx1& tri, float e);
	
	bool intersect(const OBB &b0, const Sphere &sphere);
	bool intersect(const OBB &b0, const Sphere &sphere, float e);
	bool intersect(const OBB& b0, const AAB& b1);
	bool intersect(const OBB& b0, const AAB& b1, float e);

	bool intersect(const Sphere& s0, const Sphere& s1);
	bool intersect(const Sphere& s0, const Sphere& s1, float e);
	bool intersect(const TriangleEx1& tri0, const TriangleEx1& tri1);
	bool intersect(const TriangleEx1& tri0, const TriangleEx1& tri1, float e);
	bool intersect(const Sphere& s0, const TriangleEx1& tri, bool tryEarlyOut = true);
	bool intersect(const Sphere& s0, const TriangleEx1& tri, float e, bool tryEarlyOut = true);


	bool intersect(const Point& sphereCenter, float sphereRadius, const Point& segmentA, const Point& segmentB);

	bool intersect(Frustum& f, const OBB& b);
	
	bool intersect(const Plane& pl0, const Vector3& lineOrig, const Vector3& lineDir, float& u, Vector3* pIntersection = NULL);
	bool intersectAAPlane(const Vector3& planePos, int normalDim, const Vector3& lineOrig, const Vector3& lineDir, float& u, Vector3* pIntersection = NULL);
	bool intersectPlane(const Vector3& planePos, const Vector3& planeNormal, const Vector3& lineOrig, const Vector3& lineDir, float& u, Vector3* pIntersection);
	bool intersectSegPlane(const Vector3& planePos, const Vector3& planeNormal, const Vector3& segPt1, const Vector3& segPt2, float& u, Vector3* pIntersection);
	
	bool intersect(const AAB& b, const Vector3& lineOrig, const Vector3& lineDir, Span& intersectionInterval); //intersectionInterval uses intervals on line
	bool intersectSegment(const AAB& b, const Vector3& segPt1, const Vector3& segPt2, Span& intersectionInterval); //intersectionInterval between 0.0f (segPt1) and 1.0f (segPt2). 
	bool intersect(const OBB& b, const Vector3& lineOrig, const Vector3& lineDir, Span& intersectionInterval); //intersectionInterval uses intervals on line
	bool intersectSegment(const OBB& b, const Vector3& segPt1, const Vector3& segPt2, Span& intersectionInterval); //intersectionInterval between 0.0f (segPt1) and 1.0f (segPt2). 

	bool intersect(const OBB& b0, const OBB& b1, Vector3& normal, float& depth, const bool& normalToB0);

	bool intersectAASlab(const int& normalDim, const float& min, const float& max, const Vector3& lineOrig, const Vector3& lineDir, Span& intersection);
	bool intersectSlab(const Vector3& slabDir, const float& slabMin, const float& slabMax, const Vector3& lineOrig, const Vector3& lineDir, Span& intersection);
	bool intersectAASlab(const int& normalDim, const float& min, const float& max, const Vector2& lineOrig, const Vector2& lineDir, Span& intersection);
	bool intersectSlab(const Vector2& slabDir, const float& slabMin, const float& slabMax, const Vector2& lineOrig, const Vector2& lineDir, Span& intersection);
	bool intersectSlab1D(const float& min, const float& max, const float& lineStart, const float& slope, Span& intersection);

	bool intersectSlab1D(const float& min, const float& max, const float& point);
	bool intersectSlab1D(const float& min, const float& max, const float& point, Span& intersection);

	bool intersectSlab(const Vector3& slabDir, const float& slabMin, const float& slabMax, const Vector3& point, Span& intersection);
	bool intersectSlab(const Vector3& slabDir, const float& slabMin, const float& slabMax, const Vector3& point);
	bool intersectSlab(const Vector2& slabDir, const float& slabMin, const float& slabMax, const Vector2& point, Span& intersection);
	bool intersectSlab(const Vector2& slabDir, const float& slabMin, const float& slabMax, const Vector2& point);
}

#endif