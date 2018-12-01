#ifndef _WEMeshLoadInfo_h
#define _WEMeshLoadInfo_h

#include "../data/WEDataSourcePool.h"
#include "../pool/WERefPoolObjectLoadInfo.h"
#include "../loadContext/WELoadContext.h"
#include "../coordSys/WECoordSys.h"
#include "WEMeshGeometryTransform.h"

namespace WE {

	struct MeshLoadInfo : RefPoolObjectLoadInfo {

		SoftPtr<LoadContext> objSource;
		DataSourceChunk meshChunk;
		CoordSys geometryCoordSys;

		MeshLoadInfo(LoadContext* pObjSource);
	};

	
	class MeshLoadInfoLoader_v1 {
	public:

		static bool load(DataSourceChunk& meshChunk, MeshLoadInfo& loadInfo);
	};

}

#endif