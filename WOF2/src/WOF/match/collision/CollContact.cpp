#include "CollContact.h"

#include "WE3/math/WESpan.h"
using namespace WE;

#include "float.h"

namespace WOF { namespace match {


bool CollContact::contactOBB(const OBB& b0, const OBB& b1, Vector3& normal, float& depth, const bool& normalToB0, const int& ignoreAxis) {

	Span s0[15];
    Span s1[15];
	const Vector* paxis[6];
    Vector axis[9];
	bool ignore[9];
	float aLenSq;

	paxis[0] = &MOBBAxis(b0, 0);
	if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 0), b0, b1, s0[0], s1[0]))
         return false;
	paxis[1] = &MOBBAxis(b0, 1);
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 1), b0, b1, s0[1], s1[1]))
         return false;
	paxis[2] = &MOBBAxis(b0, 2);
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b0, 2), b0, b1, s0[2], s1[2]))
         return false;

	paxis[3] = &MOBBAxis(b1, 0);
	if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 0), b0, b1, s0[3], s1[3]))
         return false;
	paxis[4] = &MOBBAxis(b1, 1);
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 1), b0, b1, s0[4], s1[4]))
         return false;
	paxis[5] = &MOBBAxis(b1, 2);
    if (!_axisOverlap_OBB_OBB(MOBBAxis(b1, 2), b0, b1, s0[5], s1[5]))
         return false;

	const Vector& ignoreVector = MOBBAxis(b0, ignoreAxis);

	ignore[0] = (0 == ignoreAxis);
	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 0), axis[0]);
    if (!_axisOverlap_OBB_OBB(axis[0], b0, b1, s0[6], s1[6]))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 1), axis[1]);
    ignore[1] = ((ignoreVector == axis[1]) || ignoreVector.isInverse(axis[1]));
	if (!_axisOverlap_OBB_OBB(axis[1], b0, b1, s0[7], s1[7]))
         return false;

	MOBBAxis(b0, 0).cross(MOBBAxis(b1, 2), axis[2]);
	ignore[2] = ((ignoreVector == axis[2]) || ignoreVector.isInverse(axis[2]));
    if (!_axisOverlap_OBB_OBB(axis[2], b0, b1, s0[8], s1[8]))
         return false;


	ignore[3] = ignore[1];
	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 0), axis[3]);
    if (!_axisOverlap_OBB_OBB(axis[3], b0, b1, s0[9], s1[9]))
         return false;

	ignore[4] = (1 == ignoreAxis);
	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 1), axis[4]);
    if (!_axisOverlap_OBB_OBB(axis[4], b0, b1, s0[10], s1[10]))
         return false;

	MOBBAxis(b0, 1).cross(MOBBAxis(b1, 2), axis[5]);
	ignore[5] = ((ignoreVector == axis[5]) || ignoreVector.isInverse(axis[5]));
    if (!_axisOverlap_OBB_OBB(axis[5], b0, b1, s0[11], s1[11]))
         return false;


	ignore[6] = ignore[2];
	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 0), axis[6]);
    if (!_axisOverlap_OBB_OBB(axis[6], b0, b1, s0[12], s1[12]))
         return false;

	ignore[7] = ignore[5];
	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 1), axis[7]);
    if (!_axisOverlap_OBB_OBB(axis[7], b0, b1, s0[13], s1[13]))
         return false;

	ignore[8] = (2 == ignoreAxis);
	MOBBAxis(b0, 2).cross(MOBBAxis(b1, 2), axis[8]);
    if (!_axisOverlap_OBB_OBB(axis[8], b0, b1, s0[14], s1[14]))
         return false;


	int i;
	int j;
	
	float dSq;

	float minALenSq;
	float minDSq = FLT_MAX;
	int minPhase;
	int minI;

	for(i = 0; i < 6; ++i) {

		if (i == ignoreAxis) {

			continue;
		}

		dSq = spanResolveOverlap(s0[i], s1[i]);
		dSq *= dSq;

		paxis[i]->magSquared(aLenSq);

		dSq /= aLenSq;

		if (dSq < minDSq) {

			minPhase = 0;
			minALenSq = aLenSq;
			minI = i;
			minDSq = dSq;
		}
	}

	for(j = 0; i < 15; ++i, ++j) {

		if (ignore[j]) {

			continue;
		}

		dSq = spanResolveOverlap(s0[i], s1[i]);
		dSq *= dSq;

		axis[j].magSquared(aLenSq);

		dSq /= aLenSq;

		if (dSq < minDSq) {

			minPhase = 1;
			minALenSq = aLenSq;
			minI = i;
			minDSq = dSq;
		}
	}


	depth = sqrtf(minDSq);

	if (minPhase == 0) {

		paxis[minI]->div(sqrtf(minALenSq), normal);

	} else {

		j = minI - 6;
		axis[j].div(sqrtf(minALenSq), normal);
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


bool CollContact::checkContactAndPenetration(const OBB& b0, const TriangleEx1& tri, Vector3& normal, float& depth, const bool& normalToB0) {

	Span s0[13];
    Span s1[13];
	const Vector* paxis[4];
    Vector axis[9];
	float aLenSq;

	paxis[0] = &tri.normal;
	if (!_axisOverlap_OBB_Tri(tri.normal, b0, tri, s0[0], s1[0]))
         return false;

	paxis[1] = &MOBBAxis(b0, 0);
	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 0), b0, tri, s0[1], s1[1]))
         return false;

	paxis[2] = &MOBBAxis(b0, 1);
	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 1), b0, tri, s0[2], s1[2]))
         return false;

	paxis[3] = &MOBBAxis(b0, 2);
	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 2), b0, tri, s0[3], s1[3]))
         return false;

	Vector edge[3];

	tri.computeEdges(edge);

	MOBBAxis(b0, 0).cross(edge[0], axis[0]);
    if (!_axisOverlap_OBB_Tri(axis[0], b0, tri, s0[4], s1[4]))
         return false;

	MOBBAxis(b0, 0).cross(edge[1], axis[1]);
    if (!_axisOverlap_OBB_Tri(axis[1], b0, tri, s0[5], s1[5]))
         return false;

	MOBBAxis(b0, 0).cross(edge[2], axis[2]);
    if (!_axisOverlap_OBB_Tri(axis[2], b0, tri, s0[6], s1[6]))
         return false;


	MOBBAxis(b0, 1).cross(edge[0], axis[3]);
    if (!_axisOverlap_OBB_Tri(axis[3], b0, tri, s0[7], s1[7]))
         return false;

	MOBBAxis(b0, 1).cross(edge[1], axis[4]);
    if (!_axisOverlap_OBB_Tri(axis[4], b0, tri, s0[8], s1[8]))
         return false;

	MOBBAxis(b0, 1).cross(edge[2], axis[5]);
    if (!_axisOverlap_OBB_Tri(axis[5], b0, tri, s0[9], s1[9]))
         return false;


	MOBBAxis(b0, 2).cross(edge[0], axis[6]);
    if (!_axisOverlap_OBB_Tri(axis[6], b0, tri, s0[10], s1[10]))
         return false;

	MOBBAxis(b0, 2).cross(edge[1], axis[7]);
    if (!_axisOverlap_OBB_Tri(axis[7], b0, tri, s0[11], s1[11]))
         return false;

	MOBBAxis(b0, 2).cross(edge[2], axis[8]);
    if (!_axisOverlap_OBB_Tri(axis[8], b0, tri, s0[12], s1[12]))
         return false;


	int i;
	int j;
	
	float dSq;

	float minALenSq;
	float minDSq = FLT_MAX;
	int minPhase;
	int minI;

	for(i = 0; i < 4; ++i) {

		dSq = spanResolveOverlap(s0[i], s1[i]);
		dSq *= dSq;

		paxis[i]->magSquared(aLenSq);

		dSq /= aLenSq;

		if (dSq < minDSq) {

			minPhase = 0;
			minALenSq = aLenSq;
			minI = i;
			minDSq = dSq;
		}
	}

	for(j = 0; i < 13; ++i, ++j) {

		dSq = spanResolveOverlap(s0[i], s1[i]);
		dSq *= dSq;

		axis[j].magSquared(aLenSq);

		dSq /= aLenSq;

		if (dSq < minDSq) {

			minPhase = 1;
			minALenSq = aLenSq;
			minI = i;
			minDSq = dSq;
		}
	}

	depth = sqrtf(minDSq);

	if (minPhase == 0) {

		paxis[minI]->div(sqrtf(minALenSq), normal);

	} else {

		j = minI - 4;
		axis[j].div(sqrtf(minALenSq), normal);
	}

	//use as temp var
	Vector& diff = axis[0];

	AAB b1(true);
	tri.addTo(b1);
	Vector triCenter;
	b1.center(triCenter);


	if (normalToB0) {

		triCenter.subtract(MOBBCenter(b0), diff);

	} else {

		MOBBCenter(b0).subtract(triCenter, diff);
	}

	if (diff.dot(normal) > 0.0f) {

        normal.negate();
	}

	return true;
}

bool CollContact::getPenetrationDepth(const OBB& b0, const TriangleEx1& tri, const Vector3& inNormal, float& depth) {

	Span s0;
    Span s1;

	const Vector3& axis = inNormal;

	if (!_axisOverlap_OBB_Tri(axis, b0, tri, s0, s1)) 
		return false;

	float dSq = spanResolveOverlap(s0, s1);
	dSq *= dSq;

	float aLenSq;

	axis.magSquared(aLenSq);
	dSq /= aLenSq;

	depth = sqrtf(dSq);

	return true;
}


bool CollContact::getPenetration(const OBB& b0, const TriangleEx1& tri, Vector3& normal, float& depth, const bool& normalToB0, const int& ignoreAxis, const bool& considerTriEdges, const Vector3* pAllowedDirection) {

	Span s0[13];
    Span s1[13];
    Vector axis[13];
	float aLenSq;
	bool valid[13];

	axis[0] = tri.normal;
	axis[0].el[ignoreAxis] = 0.0f;
	valid[0] = _axisOverlap_OBB_Tri(axis[0], b0, tri, s0[0], s1[0]);

	axis[1] = MOBBAxis(b0, 0);
	axis[1].el[ignoreAxis] = 0.0f;
	valid[1] = _axisOverlap_OBB_Tri(axis[1], b0, tri, s0[1], s1[1]);

	axis[2] = MOBBAxis(b0, 1);
	axis[2].el[ignoreAxis] = 0.0f;
	valid[2] = _axisOverlap_OBB_Tri(axis[2], b0, tri, s0[2], s1[2]);

	axis[3] = MOBBAxis(b0, 2);
	axis[3].el[ignoreAxis] = 0.0f;
	valid[3] = _axisOverlap_OBB_Tri(axis[3], b0, tri, s0[3], s1[3]);
	
	int count = 4;

	if (considerTriEdges) {

		count = 13;

		Vector edge[3];

		tri.computeEdges(edge);

		MOBBAxis(b0, 0).cross(edge[0], axis[4]);
		axis[4].el[ignoreAxis] = 0.0f;
		valid[4] =_axisOverlap_OBB_Tri(axis[4], b0, tri, s0[4], s1[4]);


		MOBBAxis(b0, 0).cross(edge[1], axis[5]);
		axis[5].el[ignoreAxis] = 0.0f;
		valid[5] =_axisOverlap_OBB_Tri(axis[5], b0, tri, s0[5], s1[5]);

		MOBBAxis(b0, 0).cross(edge[2], axis[6]);
		axis[6].el[ignoreAxis] = 0.0f;
		valid[6] =_axisOverlap_OBB_Tri(axis[6], b0, tri, s0[6], s1[6]);


		MOBBAxis(b0, 1).cross(edge[0], axis[7]);
		axis[7].el[ignoreAxis] = 0.0f;
		valid[7] =_axisOverlap_OBB_Tri(axis[7], b0, tri, s0[7], s1[7]);

		MOBBAxis(b0, 1).cross(edge[1], axis[8]);
		axis[8].el[ignoreAxis] = 0.0f;
		valid[8] =_axisOverlap_OBB_Tri(axis[8], b0, tri, s0[8], s1[8]);

		MOBBAxis(b0, 1).cross(edge[2], axis[9]);
		axis[9].el[ignoreAxis] = 0.0f;
		valid[9] =_axisOverlap_OBB_Tri(axis[9], b0, tri, s0[9], s1[9]);


		MOBBAxis(b0, 2).cross(edge[0], axis[10]);
		axis[10].el[ignoreAxis] = 0.0f;
		valid[10] =_axisOverlap_OBB_Tri(axis[10], b0, tri, s0[10], s1[10]);

		MOBBAxis(b0, 2).cross(edge[1], axis[11]);
		axis[11].el[ignoreAxis] = 0.0f;
		valid[11] =_axisOverlap_OBB_Tri(axis[11], b0, tri, s0[11], s1[11]);

		MOBBAxis(b0, 2).cross(edge[2], axis[12]);
		axis[12].el[ignoreAxis] = 0.0f;
		valid[12] =_axisOverlap_OBB_Tri(axis[12], b0, tri, s0[12], s1[12]);

	} else {

		if (ignoreAxis < 3) {
		
			count += 3;

			Vector edge[3];

			tri.computeEdges(edge);


			MOBBAxis(b0, ignoreAxis).cross(edge[0], axis[4]);
			axis[4].el[ignoreAxis] = 0.0f;
			valid[4] =_axisOverlap_OBB_Tri(axis[4], b0, tri, s0[4], s1[4]);

			MOBBAxis(b0, ignoreAxis).cross(edge[1], axis[5]);
			axis[5].el[ignoreAxis] = 0.0f;
			valid[5] =_axisOverlap_OBB_Tri(axis[5], b0, tri, s0[5], s1[5]);

			MOBBAxis(b0, ignoreAxis).cross(edge[2], axis[6]);
			axis[6].el[ignoreAxis] = 0.0f;
			valid[6] =_axisOverlap_OBB_Tri(axis[6], b0, tri, s0[6], s1[6]);
		}

	}

	int i;
	
	float dSq;

	float minALenSq;
	float minDSq = FLT_MAX;
	int minI;

	for(i = 0; i < count; ++i) {

		if (valid[i]) {

			if (pAllowedDirection && pAllowedDirection->dot(axis[i]) < 0.0f) {

				continue;
			}

			dSq = spanResolveOverlap(s0[i], s1[i]);
			dSq *= dSq;

			axis[i].magSquared(aLenSq);

			dSq /= aLenSq;

			if (dSq < minDSq) {

				minALenSq = aLenSq;
				minI = i;
				minDSq = dSq;
			}
		}
	}

	if (minDSq == FLT_MAX) {

		return false;
	}

	depth = sqrtf(minDSq);

	axis[minI].div(sqrtf(minALenSq), normal);

	//use as temp var
	Vector& diff = axis[0];

	AAB b1(true);
	tri.addTo(b1);
	Vector triCenter;
	b1.center(triCenter);


	if (normalToB0) {

		triCenter.subtract(MOBBCenter(b0), diff);

	} else {

		MOBBCenter(b0).subtract(triCenter, diff);
	}

	if (diff.dot(normal) > 0.0f) {

        normal.negate();
	}

	return true;
}



/*
bool CollContact::getPenetration(const OBB& b0, const TriangleEx1& tri, Vector3& normal, float& depth, const bool& normalToB0, const int& ignoreAxis) {

	Span s0[12];
    Span s1[12];
	const Vector* paxis[3];
    Vector axis[9];
	float aLenSq;

	paxis[0] = &MOBBAxis(b0, 0);
	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 0), b0, tri, s0[0], s1[0]))
         return false;

	paxis[1] = &MOBBAxis(b0, 1);
	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 1), b0, tri, s0[1], s1[1]))
         return false;

	paxis[2] = &MOBBAxis(b0, 2);
	if (!_axisOverlap_OBB_Tri(MOBBAxis(b0, 2), b0, tri, s0[2], s1[2]))
         return false;

	int i;
	
	float dSq;

	float minALenSq;
	float minDSq = FLT_MAX;
	int minPhase;
	int minI;

	for(i = 0; i < 3; ++i) {

		if (i == ignoreAxis) {
			
			continue;
		}

		dSq = spanResolveOverlap(s0[i], s1[i]);
		dSq *= dSq;

		paxis[i]->magSquared(aLenSq);

		dSq /= aLenSq;

		if (dSq < minDSq) {

			minPhase = 0;
			minALenSq = aLenSq;
			minI = i;
			minDSq = dSq;
		}
	}

	depth = sqrtf(minDSq);

	paxis[minI]->mul(sqrtf(minALenSq), normal);

	//use as temp var
	Vector& diff = axis[0];

	AAB b1(true);
	tri.addTo(b1);
	Vector triCenter;
	b1.center(triCenter);


	if (normalToB0) {

		triCenter.subtract(MOBBCenter(b0), diff);

	} else {

		MOBBCenter(b0).subtract(triCenter, diff);
	}

	if (diff.dot(normal) > 0.0f) {

        normal.negate();
	}

	return true;
}
*/

} }