#include "WESpan.h"


namespace WE {

	
	void computeSpan(const Vector& axis, const Point& pt, float& span) {

		span = _span(axis, pt);
	}

	void computeSpan(Axis axis, const Point& pt, float& span) {

		span = _span(axis, pt);
	}

	void computeSpan(const Vector& axis, const Point& segmentA, const Point& segmentB, float& spanMin, float& spanMax) {

		spanMin = _span(axis, segmentA);
		float p = _span(axis, segmentB);

		if (p > spanMin) {
			spanMax = p;
		} else {
			spanMax = spanMin;
			spanMin = p;
		}
	}

	void computeSpan(Axis axis, const Point& segmentA, const Point& segmentB, float& spanMin, float& spanMax) {

		spanMin = _span(axis, segmentA);
		float p = _span(axis, segmentB);

		if (p > spanMin) {
			spanMax = p;
		} else {
			spanMax = spanMin;
			spanMin = p;
		}
	}

	
	void computeSpan(const Vector& axis, const OBB& box, float& spanMin, float& spanMax) {

		float p = _span(axis, MOBBCenter(box));
		float r = fabs(_span(axis, MOBBAxis(box, 0))) * MOBBExtents(box).x
			+ fabs(_span(axis, MOBBAxis(box, 1))) * MOBBExtents(box).y
			+ fabs(_span(axis, MOBBAxis(box, 2))) * MOBBExtents(box).z;

		spanMin = p - r;
		spanMax = p + r;
	}

	void computeSpan(Axis axis, const OBB& box, float& spanMin, float& spanMax) {

		float p = _span(axis, MOBBCenter(box));
		float r = fabs(_span(axis, MOBBAxis(box, 0))) * MOBBExtents(box).x
			+ fabs(_span(axis, MOBBAxis(box, 1))) * MOBBExtents(box).y
			+ fabs(_span(axis, MOBBAxis(box, 2))) * MOBBExtents(box).z;

		spanMin = p - r;
		spanMax = p + r;
	}


	void computeSpan(const Vector& axis, const Triangle& tri, float& spanMin, float& spanMax) {

		float d;

		d = _span(axis, tri.vertices[0]);
		spanMin = d;
		spanMax = d;

		d = _span(axis, tri.vertices[1]);
		if (d < spanMin) spanMin = d; 
		if (d > spanMax) spanMax = d; 

		d = _span(axis, tri.vertices[2]);
		if (d < spanMin) spanMin = d; 
		if (d > spanMax) spanMax = d; 
	}

	void computeSpan(Axis axis, const Triangle& tri, float& spanMin, float& spanMax) {

		float d;

		d = _span(axis, tri.vertices[0]);
		spanMin = d;
		spanMax = d;

		d = _span(axis, tri.vertices[1]);
		if (d < spanMin) spanMin = d; 
		if (d > spanMax) spanMax = d; 

		d = _span(axis, tri.vertices[2]);
		if (d < spanMin) spanMin = d; 
		if (d > spanMax) spanMax = d; 
	}

	void computeSpan(const Vector& axis, const Sphere& sphere, float& spanMin, float& spanMax) {

		float rs;
		float dc;

		dc = _span(axis, sphere.center);

		rs = axis.mag() * sphere.radius;
		spanMin = dc - rs; 
		spanMax = dc + rs; 

	}

	void computeSpan(Axis axis, const Sphere& sphere, float& spanMin, float& spanMax) {

		float rs;
		float dc;

		dc = _span(axis, sphere.center);

		rs = sphere.radius;
		spanMin = dc - rs; 
		spanMax = dc + rs; 

	}

	void computeSpan(const Vector& axis, const AAB& box, float& spanMin, float& spanMax) {

		Point center;
		Vector radius;

		box.centeredRadius(center, radius);
		
		float p = _span(axis, center);
		float r = fabs(axis.x) * radius.x
			+ fabs(axis.y) * radius.y
			+ fabs(axis.z) * radius.z;

		spanMin = p - r;
		spanMax = p + r;
	}

	void computeSpan(Axis axis, const AAB& box, float& spanMin, float& spanMax) {

		Point center;
		Vector radius;

		box.centeredRadius(center, radius);
		
		float p = _span(axis, center);
		float r = radius.el[axis];

		spanMin = p - r;
		spanMax = p + r;
	}


	bool _axisOverlap_Segment_Point(const Point& segA, const Point& segB, const Point& pt) {

		static Vector axis;
		static Span s0;
		static float s1;

		segA.subtract(segB, axis);

		computeSpan(axis, segA, segB, s0.min, s0.max);
		computeSpan(axis, pt, s1);

		return spanOverlap(s0, s1);
	}

	bool _axisOverlap_e_Segment_Point(const Point& segA, const Point& segB, const Point& pt, float e) {

		static Vector axis;
		static Span s0;
		static float s1;

		segA.subtract(segB, axis);

		computeSpan(axis, segA, segB, s0.min, s0.max);
		computeSpan(axis, pt, s1);

		return spanOverlap(s0, s1, e);
	}

	bool spanCheckGetOverlap(const Span& span0, const Span& span1, Span& overlapSpan) {

		if (spanOverlap(span0, span1)) {

			overlapSpan.min = MMax(span0.min, span1.min);
			overlapSpan.max = MMin(span0.max, span1.max);

			return true;
		}

		return false;
	}

	void spanGetOverlap(const Span& span0, const Span& span1, Span& overlapSpan) {

		overlapSpan.min = MMax(span0.min, span1.min);
		overlapSpan.max = MMin(span0.max, span1.max);
	}

}