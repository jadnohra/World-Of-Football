#ifndef _WEMeshLoader_v1_h
#define _WEMeshLoader_v1_h

#include "WEMesh.h"
#include "WEMeshGeometryPartArray.h"
#include "../data/WEDataSourcePool.h"
#include "../render/loadContext/WERenderLoadContext.h"

namespace WE {

	class MeshLoader_v1 {
	public:

		static bool load(Mesh& mesh, DataSourcePool& dataPool, DataSourceChunk meshChunk, RenderLoadContext& renderLoadContext, const CoordSys& geometryCoordSys);
	};
}

#endif