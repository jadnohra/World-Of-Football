#ifndef _WECollFace_h
#define _WECollFace_h

#include "../../math/WETriangle.h"
#include "../../math/WEMatrix.h"


namespace WE { 

	typedef Matrix4x3Base CollFaceTransform;

	struct CollFace : IndexedTriangleEx1 {
	};


	struct CollVertex : public Point {
	};

	inline void collTransform(const CollFaceTransform& transf, CollFace& face) {
		
		transf.transformVector(face.normal);
		face.normal.normalize();
	}

	inline void collTransform(const CollFaceTransform& transf, CollVertex& vertex) {

		transf.transformPoint(vertex);
	}
}

#endif