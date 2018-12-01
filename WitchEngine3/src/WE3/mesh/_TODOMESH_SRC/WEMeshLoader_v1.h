#ifndef _WEMeshLoader_v1_h
#define _WEMeshLoader_v1_h

#include "WEMesh.h"
#include "WEMeshGeometryPartArray.h"
#include "../WEDataSource.h"

namespace WE {

	class MeshLoader_v1 {
	public:

		static bool load(Mesh& mesh, DataSourcePool& dataFactory, DataSourceChunk meshChunk);
	};
}

#endif