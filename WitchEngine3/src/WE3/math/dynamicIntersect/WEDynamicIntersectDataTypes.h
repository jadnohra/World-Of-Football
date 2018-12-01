#ifndef _WEDynamicIntersectDataTypes_h
#define _WEDynamicIntersectDataTypes_h

#include "../../WEDataTypes.h"
#include "../WEVector.h"
#include "../WEOBB.h"

namespace WE {

	struct SweptIntersection {

		float sweptFraction; 
	};

	struct PlanarFaceIntersection {

		enum Type {

			T_Face = 0, T_Edge, T_Point
		};

		Type type;
		UHALFINT edge[2];
	};

	typedef PlanarFaceIntersection TriIntersection;

	template<int PointCountT>
	struct PlanarFaceIntersectionEx : public PlanarFaceIntersection {

		Vector3 normal;

		int pointCount;
		Point points[PointCountT];

		PlanarFaceIntersectionEx() : pointCount(PointCountT) {}
	};

	typedef PlanarFaceIntersectionEx<3> PlanarFaceIntersectionEx3;
	typedef PlanarFaceIntersectionEx<4> PlanarFaceIntersectionEx4;

}

#endif