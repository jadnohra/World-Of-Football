#ifndef _WEMeshLoader_Ogre_h
#define _WEMeshLoader_Ogre_h

#include "WEMesh.h"
#include "WEMeshGeometryPartArray.h"
#include "../data/WEDataSourcePool.h"
#include "../render/loadContext/WERenderLoadContext.h"

namespace WE {

	class MeshLoader_Ogre {
	public:

		static bool load(Mesh& mesh, DataSourcePool& dataPool, DataSourceChunk meshChunk, RenderLoadContext& renderLoadContext, const CoordSys& geometryCoordSys);
	};
}

#endif