#include "WEMeshSource.h"

namespace WE {


Mesh* MeshSource::getFromDataChunk(LoadContext& objSource, DataSourceChunk meshChunk, const CoordSys& geometryCoordSys, const RefPoolObjectPoolInfo* pPoolInfo, int loaderVersion) {

	if (meshChunk.isNull()) {

		return NULL;
	}

	MeshLoadInfo loadInfo(&objSource);

	if (MeshLoadInfoLoader_v1::load(meshChunk, loadInfo) == false) {

		return NULL;
	}

	loadInfo.geometryCoordSys.set(geometryCoordSys);
	loadInfo.injectPoolState(pPoolInfo);

	return get(loadInfo);
}

}