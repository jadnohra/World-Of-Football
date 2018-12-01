#include "WEDistance.h"

namespace WE {

	float distanceSq(const CtVector3 &a, const CtVector3 &b) {

		float dx = a.x - b.x;
		float dy = a.y - b.y;
		float dz = a.z - b.z;
		return dx*dx + dy*dy + dz*dz;
	}

	float distanceSq(const CtVector2 &a, const CtVector2 &b) {

		float dx = a.x - b.x;
		float dy = a.y - b.y;
		return dx*dx + dy*dy;
	}

	float distanceSq(const CtVector3 &a, const CtVector3 &b, int dim1, int dim2) {

		float d1 = a.el[dim1] - b.el[dim1];
		float d2 = a.el[dim2] - b.el[dim2];
		return d1*d1 + d2*d2;
	}
	
	float distance(const CtVector3 &a, const CtVector3 &b) {

		return sqrtf(distanceSq(a, b));
	}

	float distance(const CtVector2 &a, const CtVector2 &b) {

		return sqrtf(distanceSq(a, b));
	}

	float distance(const CtVector3 &a, const CtVector3 &b, int dim1, int dim2) {

		return sqrtf(distanceSq(a, b, dim1, dim2));
	}

	float distanceU(const CtVector3& segmentA, const CtVector3& segmentB, const CtVector3& point) {
	
		return ((point.x - segmentA.x)*(segmentB.x - segmentA.x) + (point.y - segmentA.y)*(segmentB.y - segmentA.y) + (point.z - segmentA.z)*(segmentB.z - segmentA.z))
			/ ((segmentB.x - segmentA.x)*(segmentB.x - segmentA.x) + (segmentB.y - segmentA.y)*(segmentB.y - segmentA.y) + (segmentB.z - segmentA.z)*(segmentB.z - segmentA.z));
	}

	float distanceU(const CtVector2& segmentA, const CtVector2& segmentB, const CtVector2& point) {
	
		return ((point.x - segmentA.x)*(segmentB.x - segmentA.x) + (point.y - segmentA.y)*(segmentB.y - segmentA.y))
			/ ((segmentB.x - segmentA.x)*(segmentB.x - segmentA.x) + (segmentB.y - segmentA.y)*(segmentB.y - segmentA.y));
	}

	float distanceSq(const CtVector3& segmentA, const CtVector3& segmentB, const CtVector3& point, Vector3& closestPt, float& u) {
	
		u = distanceU(segmentA, segmentB, point);

		lerp(segmentA, segmentB, u, closestPt);

		return distanceSq(closestPt, point);
	}

	float distanceSq(const CtVector2& segmentA, const CtVector2& segmentB, const CtVector2& point, Vector2& closestPt, float& u) {
	
		u = distanceU(segmentA, segmentB, point);

		lerp(segmentA, segmentB, u, closestPt);

		return distanceSq(closestPt, point);
	}

	float distanceSq(const CtVector3& segmentA, const CtVector3& segmentB, const CtVector3& point, bool& isBetweenPoints) {
	
		Vector3 closestPt;
		float u;
		float ret = distanceSq(segmentA, segmentB, point, closestPt, u);

		isBetweenPoints = (u >= 0.0f) && (u <= 1.0f);

		return ret;
	}

	float distanceSq(const CtVector2& segmentA, const CtVector2& segmentB, const CtVector2& point, bool& isBetweenPoints) {
	
		Vector2 closestPt;
		float u;
		float ret = distanceSq(segmentA, segmentB, point, closestPt, u);

		isBetweenPoints = (u >= 0.0f) && (u <= 1.0f);

		return ret;
	}

	float distance(const CtVector3& segmentA, const CtVector3& segmentB, const CtVector3& point, bool& isBetweenPoints) {

		return sqrtf(distanceSq(segmentA, segmentB, point, isBetweenPoints));
	}

	float distance(const CtVector2& segmentA, const CtVector2& segmentB, const CtVector2& point, bool& isBetweenPoints) {

		return sqrtf(distanceSq(segmentA, segmentB, point, isBetweenPoints));
	}

	float distanceLineU(const CtVector3& lineOrig, const CtVector3& lineDir, const CtVector3& point) {

		Vector3 segB;

		lineOrig.addMultiplication(lineDir, 1.0f, segB);
		return distanceU(lineOrig, segB, point);
	}

	void project(const CtVector3& planeA, const CtVector3& planeB, const CtVector3& planeN, const CtVector3& point, Vector3& projection) {

		/*
		Let e1 = (v2 - v1)/||v2 - v1||,  e2 = e1 X N,  q = P - v1

		The projection of P onto the plane is  v1 + (e1*q)e1 + (e2*q)e2 
		*/

		Vector e1;
		Vector e2;
		Vector q;

		planeB.subtract(planeA, e1);
		e1.normalize();

		e1.cross(planeN, e2);

		point.subtract(planeA, q);

		e1 *= e1.dot(q);
		e2 *= e2.dot(q);

		e1.add(e2, projection);
		projection.add(planeA);
	}

	
	/*
		> Can anyone tell me where I can find a robust
		> algorithm for defining if a 3D point is inside of
		> 3D triangle?

		Let M be your point, ABC your triangle,
		1/ first check if ABC is a real triangle, for this, you could calculate
		AB^AC (^ is the cross product of vectors AB and AC : if AB(x,y,z) and
		AC(u,v,w) then AB^AC(yw-zv,zu-wx,xv-yu)),
		if it is zero, then ABC are either oin the same line or two of them are
		equal

		2/ second check whether M is in the same plane as the triangle : for
		this, compute AM.(AB^AC), if it is different from zero, then M is not in
		the same plane as the triangle, and, so, cannot be inside the triangle

		3/ this done, you have to check whether M is inside ABC, for this you
		search two numbers u and v such as

		AM = u AB + v AC

		basically, this means expressing the coordinates of M in a (non
		orthogonal) base, spawned by A,B and C.

		AM.AB = u AB.AB + v AB.AC
		AM.AC = u AB.AC + v AC.AC

		solving these two equations, you get
		u = ((AP.AB)(AC.AC)-(AP.AC)(AB.AC))/((AB.AB)(AC.AC)-(AB.AC)(AB.AC))

		v = ((AP.AC)(AB.AB)-(AP.AB)(AB.AC))/((AB.AB)(AC.AC)-(AB.AC)(AB.AC))

		(AB.AC is the "dot product" : if AB(a,b,c) AC(d,e,f) AB.AC=ad+be+cf).

		if u>=0, v>=0 and u+v<=1, then the point is in the triangle, else it is
		outside. 
	*/

	bool coplanarPointInsideTriangle(const TriangleEx1& tri, const CtVector3& point) {

		float u,v;
		Vector ap, ab, ac;
		Vector apb, acc, apc, abc, abb;
		float den;

		point.subtract(tri.vertices[0], ap);
		tri.vertices[1].subtract(tri.vertices[0], ab);
		tri.vertices[2].subtract(tri.vertices[0], ac);

		apb = ap.dot(ab);
		acc = ac.dot(ac);
		apc = ap.dot(ac);
		abc = ab.dot(ac);
		abb = ab.dot(ab);

		den = 1.0f / (abb * acc - abc * abc);

		u = (apb * acc - apc * abc) * den;
		v = (apc * abb - apb * abc) * den;

		return ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f));
	}

	bool coplanarPointInsideTriangle(const TriangleEx1& tri, const CtVector3& point, float e) {

		float u,v;
		Vector ap, ab, ac;
		Vector apb, acc, apc, abc, abb;
		float den;

		point.subtract(tri.vertices[0], ap);
		tri.vertices[1].subtract(tri.vertices[0], ab);
		tri.vertices[2].subtract(tri.vertices[0], ac);

		apb = ap.dot(ab);
		acc = ac.dot(ac);
		apc = ap.dot(ac);
		abc = ab.dot(ac);
		abb = ab.dot(ab);

		den = 1.0f / (abb * acc - abc * abc);

		u = (apb * acc - apc * abc) * den;
		v = (apc * abb - apb * abc) * den;

		return ((u + e >= 0.0f) && (v + e >= 0.0f) && (u + v - (2.0f*e) <= 1.0f));
	}

	/*
	inline void _project(const Vector3& ref, const Vector3& projected, Vector3& projection) {

		float l = ref.mag();
		float dot = ref.dot(projected);
	
		if (l != 0.0f) {
			dot /= l;
		}

		ref.mul(dot, projection);
	}
	*/
}