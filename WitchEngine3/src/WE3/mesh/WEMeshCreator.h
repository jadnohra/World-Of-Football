#ifndef _WEMeshCreator_h
#define _WEMeshCreator_h

#include "WEMesh.h"
#include "WEMeshLoadInfo.h"

namespace WE {

	class MeshCreator {
	public:

		static Mesh* create(MeshLoadInfo& loadInfo);
	};

}

#endif