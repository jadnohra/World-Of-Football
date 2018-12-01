#include "WEContactDepth.h"

#include "float.h"
#include "WESpan.h"
#include "WEIntersect.h"

namespace WE {


bool getContactDepthSwept(const Vector3& dynMoveDir, const float& moveDist, VolumeStruct& dynObj, VolumeStruct& staticObj, Vector3& normal, float* pDepth, bool useSimplifiedTest) {

	switch (dynObj.type) {

		case V_AAB:
			return getContactDepthSwept(dynMoveDir, moveDist, dynObj.toAAB(), staticObj, normal, pDepth, useSimplifiedTest);
		default:
			assrt(false);
	}

	return false;
}

bool getContactDepthSwept(const Vector3& dynMoveDir, const float& moveDist, const AAB& dynObj, VolumeStruct& staticObj, Vector3& normal, float* pDepth, bool useSimplifiedTest) {

	switch (staticObj.type) {

		case V_AAB:
			return getContactDepthSwept(dynMoveDir, moveDist, dynObj, staticObj.toAAB(), normal, pDepth);
		default:
			assrt(false);
	}

	return false;
}

template<typename T1, typename T2>
inline bool intersectDepthCheckAxis(const Vector3& ref, const T1& dynObj, const T2& staticObj, const Vector*& pAxis, Span& s0, Span& s1, bool& valid, int& index, const int& filterOutAxis) {

	if (!_axisOverlap(ref, dynObj, staticObj, s0, s1))
		return false;

	if (ref.el[filterOutAxis] == 0.0f) {

		pAxis = &ref;
		valid = true;

	} else {

		valid = false;
	}

	++index;

	return true;
}

template<typename T1, typename T2>
inline bool intersectDepthCheckAxis(const Vector3& ref, const T1& dynObj, const T2& staticObj, const Vector*& pAxis, Span& s0, Span& s1, bool& valid, int& index) {

	if (!_axisOverlap(ref, dynObj, staticObj, s0, s1))
		return false;

	pAxis = &ref;
	++index;

	return true;
}

template<typename T1, typename T2>
inline bool intersectDepthCheckAxis(const Vector3& cross1, const Vector3& cross2, const T1& dynObj, const T2& staticObj, Vector3& axis, Span& s0, Span& s1, bool& valid, int& index, int& axisIndex, const int& filterOutAxis) {

	cross1.cross(cross2, axis);

	if (!_axisOverlap(axis, dynObj, staticObj, s0, s1))
		return false;

	if (axis.el[filterOutAxis] == 0.0f) {

		valid = true;

	} else {

		valid = false;
	}

	++index;
	++axisIndex;

	return true;
}

template<typename T1, typename T2>
inline bool intersectDepthCheckAxis(const Vector3& cross1, const Vector3& cross2, const T1& dynObj, const T2& staticObj, Vector3& axis, Span& s0, Span& s1, bool& valid, int& index, int& axisIndex) {

	cross1.cross(cross2, axis);

	if (!_axisOverlap(axis, dynObj, staticObj, s0, s1))
		return false;

	++index;
	++axisIndex;

	return true;
}

inline float testSpanFreeOverlap(const Span& dynSpan, const Span& staticSpan, const bool& moveInPositiveDir) {
		
	if (moveInPositiveDir) {

		assrt(staticSpan.max - dynSpan.min >= 0.0f);
		return staticSpan.max - dynSpan.min;

	} else {

		assrt(dynSpan.max - staticSpan.min >= 0.0f);
		return dynSpan.max - staticSpan.min;
	}
}

bool getContactDepth(const OBB& dynObj, const TriangleEx1& staticObj, Vector3& normal, float& depth, const int& filterOutAxis, bool considerTriEdges, const Vector3* pNormalGuide, float guideAngleSinSpread) {

	if (filterOutAxis < 0 || filterOutAxis > 2) {
		
		assrt(false);
		return false;
	}

	
	int index = 0;
	int axisIndex = 0;

	const int pAxisCount = 4;

	Span s0[13];
    Span s1[13];
	const Vector3* pAxis[pAxisCount];
    Vector3 axis[9];
	bool valid[13];
	
	if (!intersectDepthCheckAxis(staticObj.normal, dynObj, staticObj, pAxis[index], s0[index], s1[index], valid[index], index, filterOutAxis)) return false;
	if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 0), dynObj, staticObj, pAxis[index], s0[index], s1[index], valid[index], index, filterOutAxis)) return false;
	if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 1), dynObj, staticObj, pAxis[index], s0[index], s1[index], valid[index], index, filterOutAxis)) return false;
	if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 2), dynObj, staticObj, pAxis[index], s0[index], s1[index], valid[index], index, filterOutAxis)) return false;


	if (considerTriEdges) {

		Vector edge[3];
		staticObj.computeEdges(edge);

		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 0), edge[0], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 0), edge[1], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 0), edge[2], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;

		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 1), edge[0], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 1), edge[1], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 1), edge[2], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;

		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 2), edge[0], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 2), edge[1], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, 2), edge[2], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;

	} else {

		Vector edge[3];
		staticObj.computeEdges(edge);

		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, filterOutAxis), edge[0], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, filterOutAxis), edge[1], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
		if (!intersectDepthCheckAxis(MOBBAxis(dynObj, filterOutAxis), edge[2], dynObj, staticObj, axis[axisIndex], s0[index], s1[index], valid[index], index, axisIndex, filterOutAxis)) return false;
	}

	bool found = false;

	float dSq;
	float aLenSq;
	bool posAxis;

	float minDSq = FLT_MAX;
	bool minPosAxis;


	for(int i = 0; i < index; ++i) {

		if (valid[i]) {

			Vector3 selectedAxis = i < pAxisCount ? *pAxis[i] : axis[i - pAxisCount];

			selectedAxis.magSquared(aLenSq);
			selectedAxis.div(sqrtf(aLenSq));

			bool checked = false;

			if (pNormalGuide) {

				float dotResult = pNormalGuide->dot(selectedAxis);

				if (dotResult >= 0.0f) {

					posAxis = true;

				} else {

					posAxis = false;
					dotResult = -dotResult;
				}

				if (fabs(1.0f - dotResult) > guideAngleSinSpread)
					continue;

				if (dotResult != 0.0f) {

					dSq = testSpanFreeOverlap(s0[i], s1[i], posAxis);

					checked = true;
				} 
			} 
			
			if (!checked) {

				dSq = testSpanFreeOverlap(s0[i], s1[i], true);

				float temp = testSpanFreeOverlap(s0[i], s1[i], false);

				if (temp < dSq) {

					dSq = temp;
					posAxis = false;

				} else {

					posAxis = true;
				}
			}

			dSq *= dSq;
			dSq /= aLenSq;

			if (dSq < minDSq) {

				found = true;

				normal = selectedAxis;
				minDSq = dSq;
				minPosAxis = posAxis;
			}
		}
	}

	if (!found) {

		return false;
	}

	depth = sqrtf(minDSq);
	
	if (!minPosAxis)
		normal.negate();

	return true;
}

void selectIfMinDepthImpl_UnitNormal(const float& resolveDist, const Vector3& normal, const Span& dynSpan, const Span& staticSpan, float& minDSq, Vector3& minNormal, bool& minNormalIsUnit, float& minDepth) {

	const float& d = resolveDist;
	float dSq = d * d;

	if (dSq < minDSq) {

		minDSq = dSq;
		minDepth = d;
		minNormal = normal;
		minNormalIsUnit = true;
	}
}

void selectIfMinDepthImpl(const float& resolveDist, const Vector3& normal, const Span& dynSpan, const Span& staticSpan, float& minDSq, Vector3& minNormal, bool& minNormalIsUnit, float& minDepth) {

	const float& d = resolveDist;
	float dSq = d * d;

	float magSq = normal.magSquared();
	dSq /= magSq;

	if (dSq < minDSq) {

		minDSq = dSq;
		minDepth = d;
		minNormal = normal;
		minNormalIsUnit = false;
	}
}

inline void selectIfMinDepth_UnitNormal(const Vector3& normal, const Span& dynSpan, const Span& staticSpan, float& minDSq, Vector3& minNormal, bool& minNormalIsUnit, float& minDepth) {

	selectIfMinDepthImpl_UnitNormal(spanResolveDynOverlap(dynSpan, staticSpan), normal, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
}

inline void selectIfMinDepth(const Vector3& normal, const Span& dynSpan, const Span& staticSpan, float& minDSq, Vector3& minNormal, bool& minNormalIsUnit, float& minDepth) {

	selectIfMinDepthImpl(spanResolveDynOverlap(dynSpan, staticSpan), normal, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
}

inline void selectIfMinDepth_UnitNormal_moveFwd(const Vector3& normal, const Span& dynSpan, const Span& staticSpan, float& minDSq, Vector3& minNormal, bool& minNormalIsUnit, float& minDepth) {

	selectIfMinDepthImpl_UnitNormal(spanResolveDynOverlap_moveFwd(dynSpan, staticSpan), normal, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
}

inline void selectIfMinDepth_moveFwd(const Vector3& normal, const Span& dynSpan, const Span& staticSpan, float& minDSq, Vector3& minNormal, bool& minNormalIsUnit, float& minDepth) {

	selectIfMinDepthImpl(spanResolveDynOverlap_moveFwd(dynSpan, staticSpan), normal, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
}

inline void selectIfMinDepth_UnitNormal_moveBack(const Vector3& normal, const Span& dynSpan, const Span& staticSpan, float& minDSq, Vector3& minNormal, bool& minNormalIsUnit, float& minDepth) {

	selectIfMinDepthImpl_UnitNormal(spanResolveDynOverlap_moveBack(dynSpan, staticSpan), normal, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
}

inline void selectIfMinDepth_moveBack(const Vector3& normal, const Span& dynSpan, const Span& staticSpan, float& minDSq, Vector3& minNormal, bool& minNormalIsUnit, float& minDepth) {

	selectIfMinDepthImpl(spanResolveDynOverlap_moveBack(dynSpan, staticSpan), normal, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
}


bool getContactDepth(const AAB& dynObj, const TriangleEx1& staticObj, Vector3& minNormal, float* pDepth, bool considerTriEdges) {

	assrt(false);
	//this is flawed, check the AAB vs. AAB getContactDepth fction to see the correct way

	Span dynSpan;
	Span staticSpan;

	float minDepth;
	float minDSq = FLT_MAX;
	bool minNormalIsUnit;

	if (!_axisOverlap(staticObj.normal, dynObj, staticObj, dynSpan, staticSpan)) 
		return false;
	selectIfMinDepth_UnitNormal(staticObj.normal, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	if (!_axisOverlap(AAB::axis[0], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal(AAB::axis[0], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (!_axisOverlap(AAB::axis[1], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal(AAB::axis[1], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (!_axisOverlap(AAB::axis[2], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal(AAB::axis[2], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (considerTriEdges) {

		Vector axis;
		Vector edge[3];

		staticObj.computeEdges(edge);

		AAB::axis[0].cross(edge[0], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		AAB::axis[0].cross(edge[1], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		AAB::axis[0].cross(edge[2], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);


		AAB::axis[1].cross(edge[0], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		AAB::axis[1].cross(edge[1], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		AAB::axis[1].cross(edge[2], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);


		AAB::axis[2].cross(edge[0], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		AAB::axis[2].cross(edge[1], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		AAB::axis[2].cross(edge[2], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	}

	//minDepth already has the correct sign because spanFreeOverlap was used
	//to calculate it
	if (minDepth < 0.0f) {

		minNormal.negate();
		minDepth = -minDepth;
	}

	if (!minNormalIsUnit) {

		float mag;

		minNormal.normalize(mag);
		minDepth /= mag;
	}

	if (pDepth)
		*pDepth = minDepth;

	return true;
}


bool getContactDepth(const OBB& dynObj, const TriangleEx1& staticObj, Vector3& minNormal, float* pDepth, bool considerTriEdges) {

	assrt(false);
	//this is flawed, check the AAB vs. AAB getContactDepth fction to see the correct way

	Span dynSpan;
	Span staticSpan;

	float minDepth;
	float minDSq = FLT_MAX;
	bool minNormalIsUnit;

	if (!_axisOverlap(staticObj.normal, dynObj, staticObj, dynSpan, staticSpan)) 
		return false;
	selectIfMinDepth_UnitNormal(staticObj.normal, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	if (!_axisOverlap(MOBBAxis(dynObj, 0), dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal(MOBBAxis(dynObj, 0), dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (!_axisOverlap(MOBBAxis(dynObj, 1), dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal(MOBBAxis(dynObj, 1), dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (!_axisOverlap(MOBBAxis(dynObj, 2), dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal(MOBBAxis(dynObj, 2), dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (considerTriEdges) {

		Vector axis;
		Vector edge[3];

		staticObj.computeEdges(edge);

		MOBBAxis(dynObj, 0).cross(edge[0], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		MOBBAxis(dynObj, 0).cross(edge[1], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		MOBBAxis(dynObj, 0).cross(edge[2], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);


		MOBBAxis(dynObj, 1).cross(edge[0], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		MOBBAxis(dynObj, 1).cross(edge[1], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		MOBBAxis(dynObj, 1).cross(edge[2], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);


		MOBBAxis(dynObj, 2).cross(edge[0], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		MOBBAxis(dynObj, 2).cross(edge[1], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

		MOBBAxis(dynObj, 2).cross(edge[2], axis);
		if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;
		selectIfMinDepth(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	}

	//minDepth already has the correct sign because spanFreeOverlap was used
	//to calculate it
	if (minDepth < 0.0f) {

		minNormal.negate();
		minDepth = -minDepth;
	}

	if (!minNormalIsUnit) {

		float mag;

		minNormal.normalize(mag);
		minDepth /= mag;
	}

	if (pDepth)
		*pDepth = minDepth;

	return true;
}

bool getContactDepth(const AAB& dynObj, const AAB& staticObj, Vector3& minNormal, float* pDepth) {

	Span dynSpan;
	Span staticSpan;

	float minDepth;
	float minDSq = FLT_MAX;
	bool minNormalIsUnit;

	Vector3 negAxis;

	if (!_axisOverlap(AAB::axis[0], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(AAB::axis[0], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	AAB::axis[0].negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	if (!_axisOverlap(AAB::axis[1], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(AAB::axis[1], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	AAB::axis[1].negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (!_axisOverlap(AAB::axis[2], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(AAB::axis[2], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	AAB::axis[2].negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);


	if (minDSq == FLT_MAX)
		return false;

	//minDepth already has the correct sign because spanFreeOverlap was used
	//to calculate it
	if (minDepth < 0.0f) {

		minNormal.negate();
		minDepth = -minDepth;
	}

	if (!minNormalIsUnit) {

		float mag;

		minNormal.normalize(mag);
		minDepth /= mag;
	}

	if (pDepth)
		*pDepth = minDepth;

	return true;
}


bool getContactDepth(const AAB& dynObj, const OBB& staticObj, Vector3& minNormal, float* pDepth) {

	/*
		not 100% sure about this one
	*/

	Span dynSpan;
	Span staticSpan;

	float minDepth;
	float minDSq = FLT_MAX;
	bool minNormalIsUnit;

	Vector3 negAxis;

	if (!_axisOverlap(AAB::axis[0], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(AAB::axis[0], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	AAB::axis[0].negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	if (!_axisOverlap(AAB::axis[1], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(AAB::axis[1], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	AAB::axis[1].negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (!_axisOverlap(AAB::axis[2], dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(AAB::axis[2], dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	AAB::axis[2].negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);


	if (!_axisOverlap(MOBBAxis(staticObj, 0), dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(MOBBAxis(staticObj, 0), dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	MOBBAxis(staticObj, 0).negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	if (!_axisOverlap(MOBBAxis(staticObj, 1), dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(MOBBAxis(staticObj, 1), dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	MOBBAxis(staticObj, 1).negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	if (!_axisOverlap(MOBBAxis(staticObj, 2), dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(MOBBAxis(staticObj, 2), dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	MOBBAxis(staticObj, 2).negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);

	Vector axis;

	MOBBAxis(staticObj, 0).cross(AAB::axis[0], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	MOBBAxis(staticObj, 0).cross(AAB::axis[1], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	MOBBAxis(staticObj, 0).cross(AAB::axis[2], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	

	MOBBAxis(staticObj, 1).cross(AAB::axis[0], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	MOBBAxis(staticObj, 1).cross(AAB::axis[1], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	MOBBAxis(staticObj, 1).cross(AAB::axis[2], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	

	MOBBAxis(staticObj, 2).cross(AAB::axis[0], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	MOBBAxis(staticObj, 2).cross(AAB::axis[1], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	
	MOBBAxis(staticObj, 2).cross(AAB::axis[2], axis);
	if (!_axisOverlap(axis, dynObj, staticObj, dynSpan, staticSpan))
         return false;
	selectIfMinDepth_UnitNormal_moveFwd(axis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	axis.negate(negAxis);
	dynSpan.negateAndSwap();
	staticSpan.negateAndSwap();
	selectIfMinDepth_UnitNormal_moveFwd(negAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	

	if (minDSq == FLT_MAX)
		return false;

	//minDepth already has the correct sign because spanFreeOverlap was used
	//to calculate it
	if (minDepth < 0.0f) {

		minNormal.negate();
		minDepth = -minDepth;
	}

	if (!minNormalIsUnit) {

		float mag;

		minNormal.normalize(mag);
		minDepth /= mag;
	}

	if (pDepth)
		*pDepth = minDepth;

	return true;
}

/*
bool getContactDepthSwept(const Vector3& dynMoveDir, const float& moveDist, const AAB& dynObj, const AAB& staticObj, Vector3& minNormal, float* pDepth) {

	Span dynSpan;
	Span staticSpan;

	float minDepth;
	float minDSq = FLT_MAX;
	bool minNormalIsUnit;

	Vector3 axis;

	{
		const Vector3& testAxis = AAB::axis[0];

		if (dynMoveDir.dot(testAxis) > 0.0f)
			testAxis.negate(axis);
		else
			axis = testAxis;

		if (!_axisOverlap(testAxis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;

		selectIfMinDepth_UnitNormal_moveFwd(testAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	}

	{
		const Vector3& testAxis = AAB::axis[1];

		if (dynMoveDir.dot(testAxis) > 0.0f)
			testAxis.negate(axis);
		else
			axis = testAxis;

		if (!_axisOverlap(testAxis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;

		selectIfMinDepth_UnitNormal_moveFwd(testAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	}

	{
		const Vector3& testAxis = AAB::axis[2];

		if (dynMoveDir.dot(testAxis) > 0.0f)
			testAxis.negate(axis);
		else
			axis = testAxis;

		if (!_axisOverlap(testAxis, dynObj, staticObj, dynSpan, staticSpan))
			 return false;

		selectIfMinDepth_UnitNormal_moveFwd(testAxis, dynSpan, staticSpan, minDSq, minNormal, minNormalIsUnit, minDepth);
	}


	if (minDSq == FLT_MAX)
		return false;

	//minDepth already has the correct sign because spanFreeOverlap was used
	//to calculate it
	if (minDepth < 0.0f) {

		minNormal.negate();
		minDepth = -minDepth;
	}

	if (!minNormalIsUnit) {

		float mag;

		minNormal.normalize(mag);
		minDepth /= mag;
	}

	if (pDepth)
		*pDepth = minDepth;

	return true;
}
*/

bool getContactDepthSwept(const Vector3& dynMoveDir, const float& moveDist, const AAB& dynObj, const AAB& staticObj, Vector3& minNormal, float* pDepth) {

	/*
		Adavanced SAT, Support Functions

		also see bookmarks Reseach/GJK1

		We are constructing the CS (configuration space or minkowsky sum A-B)
		and ray tracing from the origin (which is inside CS) 
		against the move swept direction to see the amount of backup needed to translate
		B to make it a zero depth intersection

		http://docs.google.com/Doc?docid=dcm3hb4r_35c2tc98&hl=en
		http://docs.google.com/Doc?id=dcm3hb4r_17d9jwjt&pli=1
	*/

	//minkowski sum A-B
	AAB AminB;

	AminB.min.el[0] = staticObj.min.el[0] - dynObj.max.el[0];
	AminB.max.el[0] = staticObj.max.el[0] - dynObj.min.el[0];

	AminB.min.el[1] = staticObj.min.el[1] - dynObj.max.el[1];
	AminB.max.el[1] = staticObj.max.el[1] - dynObj.min.el[1];

	AminB.min.el[2] = staticObj.min.el[2] - dynObj.max.el[2];
	AminB.max.el[2] = staticObj.max.el[2] - dynObj.min.el[2];

	Span segSpan;
	Vector3 segPt2; 

	dynMoveDir.mul(-moveDist, segPt2);

	if (!intersectSegment(AminB, Vector3::kZero, segPt2, segSpan)) {

		return false;
	}

	AAB collTimeDynObj;
	Vector3 collTimeBackOffset;
	float backOffsetMag;

	backOffsetMag = moveDist * segSpan.max;
	dynMoveDir.mul(-backOffsetMag, collTimeBackOffset);

	dynObj.min.add(collTimeBackOffset, collTimeDynObj.min);
	dynObj.max.add(collTimeBackOffset, collTimeDynObj.max);

	if (!getContactDepth(collTimeDynObj, staticObj, minNormal, pDepth)) {

		backOffsetMag = backOffsetMag - 0.01f * backOffsetMag;
		dynMoveDir.mul(-backOffsetMag, collTimeBackOffset);

		dynObj.min.add(collTimeBackOffset, collTimeDynObj.min);
		dynObj.max.add(collTimeBackOffset, collTimeDynObj.max);

		if (!getContactDepth(collTimeDynObj, staticObj, minNormal, pDepth))
			return false;
	}

	if (pDepth) {

		Span dynSpan;
		Span staticSpan;

		if (_axisOverlap(minNormal, collTimeDynObj, staticObj, dynSpan, staticSpan)) { 

			*pDepth = spanResolveDynOverlap_moveFwd(dynSpan, staticSpan);

		} else {

			//should never happen
			assrt(false);
		}

	}

	return true;
}


bool getContactDepthSwept(const Vector3& dynMoveDir, const float& moveDist, const AAB& dynObj, const OBB& staticObj, Vector3& minNormal, float* pDepth) {

	/*
		Adavanced SAT, Support Functions

		also see bookmarks Reseach/GJK1

		We are constructing the CS (configuration space or minkowsky sum A-B)
		and ray tracing from the origin (which is inside CS) 
		against the move swept direction to see the amount of backup needed to translate
		B to make it a zero depth intersection

		http://docs.google.com/Doc?docid=dcm3hb4r_35c2tc98&hl=en
		http://docs.google.com/Doc?id=dcm3hb4r_17d9jwjt&pli=1
	*/

	//minkowski sum A-B
	OBB AminB;

	//all whats left is to construct AminB .... then this will be usable (testable)
	//BUT thats not trivial!! thats y GJK exists, to escape having to build it
	//cggmwww.csie.nctu.edu.tw/seminar/III_06/2006_04_27.ppt
	assrt(false);
	return false;
	/*
	AminB.min.el[0] = staticObj.min.el[0] - dynObj.max.el[0];
	AminB.max.el[0] = staticObj.max.el[0] - dynObj.min.el[0];

	AminB.min.el[1] = staticObj.min.el[1] - dynObj.max.el[1];
	AminB.max.el[1] = staticObj.max.el[1] - dynObj.min.el[1];

	AminB.min.el[2] = staticObj.min.el[2] - dynObj.max.el[2];
	AminB.max.el[2] = staticObj.max.el[2] - dynObj.min.el[2];
	*/

	Span segSpan;
	Vector3 segPt2; 

	dynMoveDir.mul(-moveDist, segPt2);

	if (!intersectSegment(AminB, Vector3::kZero, segPt2, segSpan)) {

		return false;
	}

	AAB collTimeDynObj;
	Vector3 collTimeBackOffset;
	float backOffsetMag;

	backOffsetMag = moveDist * segSpan.max;
	dynMoveDir.mul(-backOffsetMag, collTimeBackOffset);

	dynObj.min.add(collTimeBackOffset, collTimeDynObj.min);
	dynObj.max.add(collTimeBackOffset, collTimeDynObj.max);

	if (!getContactDepth(collTimeDynObj, staticObj, minNormal, pDepth)) {

		backOffsetMag = backOffsetMag - 0.01f * backOffsetMag;
		dynMoveDir.mul(-backOffsetMag, collTimeBackOffset);

		dynObj.min.add(collTimeBackOffset, collTimeDynObj.min);
		dynObj.max.add(collTimeBackOffset, collTimeDynObj.max);

		if (!getContactDepth(collTimeDynObj, staticObj, minNormal, pDepth))
			return false;
	}

	if (pDepth) {

		Span dynSpan;
		Span staticSpan;

		if (_axisOverlap(minNormal, collTimeDynObj, staticObj, dynSpan, staticSpan)) { 

			*pDepth = spanResolveDynOverlap_moveFwd(dynSpan, staticSpan);

		} else {

			//should never happen
			assrt(false);
		}

	}

	return true;
}

}