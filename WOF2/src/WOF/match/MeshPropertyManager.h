#ifndef h_WOF_MeshPropertyManager
#define h_WOF_MeshPropertyManager

#pragma warning( push )
#pragma warning( disable : 4311 )

#include "WE3/helper/WEPropertyManagerT.h"
#include "WE3/mesh/WEMesh.h"
#include "WE3/skeleton/WESkeletonAnimMap.h"
using namespace WE;

namespace WOF {

	struct SkeletalMeshProperty {

		SkeletonAnimMap mAnimMap;
		//HardRef<DataChunk> mSkelPropChunk;
	};

	

	class SkeletalMeshPropertyManager : public PropertyManagerT<Mesh, SkeletalMeshProperty> {
	};

}

#pragma warning( pop )

#endif