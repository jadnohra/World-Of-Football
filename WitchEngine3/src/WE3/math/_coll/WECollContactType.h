#ifndef _WECollContactType_h
#define _WECollContactType_h

#include "../WETriangle.h"
#include "../dynamicIntersect/WEDynamicIntersect.h"

namespace WE {

	/*
	enum SweptContactType {

		PlanarFaceIntersection::T_Face = 0, PlanarFaceIntersection::T_Edge, PlanarFaceIntersection::T_Point
	};

	struct SweptContactInfo {

		SweptContactType type;
		short edge[2];
	};
	*/

	typedef TriIntersection SweptContactInfo;

	struct SweptContact {

		int vertexCount;
		Vector3 vertices[4];
		Vector3 normal;

		Point contactPoint; //static contactPoint
		float sweptFraction; 
		SweptContactInfo sweptContactInfo;
	};
}

#endif