#ifndef _WEMeshCreator_h
#define _WEMeshCreator_h

#include "../data/WEDataSourcePool.h"
#include "WEMesh.h"

namespace WE {

	class MeshCreator {
	public:

		static Mesh* create(DataSourcePool& dataFactory, DataSourceChunk& meshChunk);
	};

}

#endif