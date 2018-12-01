#ifndef _WEIntersection_h
#define _WEIntersection_h

#include "WEVector.h"

namespace WE {

	struct SphereSegIntersection {

		SphereSegIntersection();

		void empty();
		bool init(const Point& sphereCenter, const float& sphereRadius, const Point& segmentA, const Point& segmentB, bool considerInternalPoints);

		inline const unsigned int& getPointCount() { return pointCount; }
		inline const Point& getPoint(const unsigned int& index) { return points[index]; }
		inline const bool& isInternalPoint(const unsigned int& index) { return isInternal[index]; }

		unsigned int pointCount;
		Point points[2];
		bool isInternal[2];
	};

	

}

#endif