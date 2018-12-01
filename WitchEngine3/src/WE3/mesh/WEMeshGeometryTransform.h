/*
#ifndef _WEMeshGeometryTransform_h
#define _WEMeshGeometryTransform_h

#include "../geometry/WEGeometryTransform.h"
#include "../geometry/WEGeometryAnchor.h"

namespace WE {

	struct MeshGeometryTransform {

		bool hasAnchor;
		GeometryAnchorType anchor[3];
		
		bool hasTransform;
		GeometryVertexTransform transform;

		bool isEmpty();
	};

}

#endif
*/