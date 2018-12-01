#ifndef _WEMeshSource_h
#define _WEMeshSource_h

#include "WEMeshCreator.h"

namespace WE {

	class MeshSource {
	public:

		virtual Mesh* get(MeshLoadInfo& loadInfo) = 0;

		Mesh* getFromDataChunk(LoadContext& objSource, DataSourceChunk meshChunk, const CoordSys& geometryCoordSys, const RefPoolObjectPoolInfo* pPoolInfo = NULL, int loaderVersion = 1);
	};
}

#endif