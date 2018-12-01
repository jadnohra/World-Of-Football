#include "WEMeshLoadInfo.h"

namespace WE {

MeshLoadInfo::MeshLoadInfo(LoadContext* pObjSource) : RefPoolObjectLoadInfo() {

	objSource = pObjSource;
}

bool MeshLoadInfoLoader_v1::load(DataSourceChunk& meshChunk, MeshLoadInfo& loadInfo) {

	BufferString tempStr;

	loadInfo.meshChunk.set(meshChunk);
	loadInfo.objSource->dataSourcePool().resolvePossiblyRemoteChunk(loadInfo.meshChunk, true, NULL);

	if (loadInfo.needsPoolId()) {

		DataChunk& chunk = loadInfo.meshChunk.chunk();
		bool poolIdSet = chunk.getAttribute(L"poolId", tempStr);

		if (poolIdSet) {

			if (tempStr.equals(L"auto")) {

				loadInfo.poolId = chunk.generatePathId(true, true);
			
			} else {

				loadInfo.poolId = tempStr.hash();
			}


		} else {

			loadInfo.poolState = Pool_NonePoolable;
		}
	}
	

	return true;
}

}