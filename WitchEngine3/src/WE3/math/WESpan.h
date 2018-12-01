#ifndef _WESpan_h
#define _WESpan_h

#include "WEVector.h"
#include "WEOBB.h"
#include "WESphere.h"
#include "WETriangle.h"
#include "WEFrustum.h"
#include "../WEAssert.h"
#include "../WEMacros.h"
#include "WEVector2.h"

namespace WE {

	struct Span : Vector2 {

		/*
		union {
			struct {
				float min, max;
			};
			struct {
				float el[2];
			};
		};
		*/

		Span() {}
		Span(const float& val) { min = val; max = val; }
		Span(const float& _min, const float& _max) { min = _min; max = _max; }

		inline void order() { if (min > max)  swapt(min, max); }
		inline void negate() { min = -min; max = -max; }
		inline void negateAndSwap() { float temp = min; min = -max; max = -temp; }
	};
	
	inline bool spanOverlap(const float& min0, const float& max0, const float& min1, const float& max1) {
		return !(min0 > max1 || max0 < min1);
	}

	inline bool spanOverlap(const Span& span0, const Span& span1) {
		return !(span0.min > span1.max || span0.max < span1.min);
	}

	inline bool spanOverlap(const Span& span0, const float& span1) {
		return (span0.min <= span1 && span0.max >= span1);
	}

	inline bool spanOverlap(const float& min0, const float& max0, const float& min1, const float& max1, const float& e) {
		return !(min0 - e > max1 || max0 + e < min1);
	}

	inline bool spanOverlap(const Span& span0, const Span& span1, const float& e) {
		return !(span0.min - e > span1.max || span0.max + e < span1.min);
	}

	inline bool spanOverlap(const Span& span0, const float& span1, const float& e) {
		return (span0.min - e <= span1 && span0.max + e >= span1);
	}

	inline bool spanOverlap(float& span0Min, float& span0Max, const float& span1) {
		return (span0Min <= span1 && span0Max >= span1);
	}

	inline float _span(const Vector& axis, const Vector& pt) {

		return axis.dot(pt);
	}

	inline float _span(Axis axis, const Vector& pt) {

		return pt.el[axis];
	}

	void computeSpan(const Vector& axis, const OBB& box, float& spanMin, float& spanMax);
	void computeSpan(const Vector& axis, const Triangle& tri, float& spanMin, float& spanMax);
	void computeSpan(const Vector& axis, const Sphere& sphere, float& spanMin, float& spanMax);
	void computeSpan(const Vector& axis, const AAB& box, float& spanMin, float& spanMax);
	void computeSpan(const Vector& axis, const Point& pt, float& span);
	void computeSpan(const Vector& axis, const Point& segmentA, const Point& segmentB, float& spanMin, float& spanMax);

	inline void computeSpan(const Vector& axis, Frustum& frustum, float& spanMin, float& spanMax) {

		frustum.computeSpan(axis, spanMin, spanMax);
	}

	void computeSpan(Axis axis, const OBB& box, float& spanMin, float& spanMax);
	void computeSpan(Axis axis, const Triangle& tri, float& spanMin, float& spanMax);
	void computeSpan(Axis axis, const Sphere& sphere, float& spanMin, float& spanMax);
	void computeSpan(Axis axis, const AAB& box, float& spanMin, float& spanMax);
	void computeSpan(Axis axis, const Point& pt, float& span);
	void computeSpan(Axis axis, const Point& segmentA, const Point& segmentB, float& spanMin, float& spanMax);
	

	template <class T0, class T1>
	bool _axisOverlap(const Vector& axis, const T0& o0, const T1& o1) {

		Span s0, s1;

		computeSpan(axis, o0, s0.min, s0.max);
		computeSpan(axis, o1, s1.min, s1.max);

		return spanOverlap(s0, s1);
	}

	template <class T0, class T1>
	bool _axisOverlap(const Vector& axis, T0& o0, const T1& o1) {

		Span s0, s1;

		computeSpan(axis, o0, s0.min, s0.max);
		computeSpan(axis, o1, s1.min, s1.max);

		return spanOverlap(s0, s1);
	}

	template <class T0, class T1>
	bool _axisOverlap_e(const Vector& axis, const T0& o0, const T1& o1, const float& e) {

		Span s0, s1;

		computeSpan(axis, o0, s0.min, s0.max);
		computeSpan(axis, o1, s1.min, s1.max);

		return spanOverlap(s0, s1, e);
	}

	template <class T0, class T1>
	bool _axisOverlap(const Vector& axis, const T0& o0, const T1& o1, Span& s0, Span& s1) {

		computeSpan(axis, o0, s0.min, s0.max);
		computeSpan(axis, o1, s1.min, s1.max);

		return spanOverlap(s0, s1);
	}

	template <class T0, class T1>
	bool _axisOverlap_e(const Vector& axis, const T0& o0, const T1& o1, const float& e, Span& s0, Span& s1) {

		computeSpan(axis, o0, s0.min, s0.max);
		computeSpan(axis, o1, s1.min, s1.max);

		return spanOverlap(s0, s1, e);
	}
	

	bool _axisOverlap_Segment_Point(const Point& segA, const Point& segB, const Point& pt);
	bool _axisOverlap_e_Segment_Point(const Point& segA, const Point& segB, const Point& pt, const float& e);
	
	#define _axisOverlap_OBB_OBB _axisOverlap<OBB, OBB>
	#define _axisOverlap_OBB_AAB _axisOverlap<OBB, AAB>
	#define _axisOverlap_AAB_AAB _axisOverlap<AAB, AAB>
	#define _axisOverlap_OBB_Tri _axisOverlap<OBB, Triangle>
	#define _axisOverlap_AAB_Tri _axisOverlap<AAB, Triangle>
	#define _axisOverlap_Point_Tri _axisOverlap<Point, Triangle>
	#define _axisOverlap_Frustum_OBB _axisOverlap<Frustum, OBB>

	#define _axisOverlap_e_OBB_OBB _axisOverlap_e<OBB, OBB>
	#define _axisOverlap_e_OBB_AAB _axisOverlap_e<OBB, AAB>
	#define _axisOverlap_e_AAB_AAB _axisOverlap_e<AAB, AAB>
	#define _axisOverlap_e_OBB_Tri _axisOverlap_e<OBB, Triangle>
	#define _axisOverlap_e_AAB_Tri _axisOverlap_e<AAB, Triangle>
	#define _axisOverlap_e_Point_Tri _axisOverlap_e<Point, Triangle>

	//overlapSpan can be safely be one of the spans
	bool spanCheckGetOverlap(const Span& span0, const Span& span1, Span& overlapSpan);
	void spanGetOverlap(const Span& span0, const Span& span1, Span& overlapSpan);

	
	//Spans must be already overlapping
	inline float spanResolveOverlap(const Span& span0, const Span& span1) {

		return MMin(span0.max - span1.min, span1.max - span0.min);
	}

	//returns the distance dynSpan must move to cancel overlap
	//the distance might be negative in that case the axis can be negated to make it positive
	inline float spanResolveDynOverlap(const Span& dynSpan, const Span& staticSpan) {

		float moveBackDist = dynSpan.max - staticSpan.min;
		float moveFwdDist = staticSpan.max - dynSpan.min;

		if (moveFwdDist < moveBackDist)
			return moveFwdDist;

		return -moveBackDist;
	}

	//returns positive number
	inline float spanResolveDynOverlap_moveFwd(const Span& dynSpan, const Span& staticSpan) {

		return staticSpan.max - dynSpan.min;
	}

	//returns negative number
	inline float spanResolveDynOverlap_moveBack(const Span& dynSpan, const Span& staticSpan) {

		return staticSpan.min - dynSpan.max;
	}
	
}

#endif