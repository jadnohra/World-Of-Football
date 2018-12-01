#include "WEMeshCreator.h"

#include "WEMeshLoader_v1.h"

namespace WE {

Mesh* MeshCreator::create(MeshLoadInfo& loadInfo) {

	Mesh* pRet = NULL;

	if (loadInfo.meshChunk.isValid()) {

		MMemNew(pRet, Mesh());

		if (MeshLoader_v1::load(*pRet, loadInfo.objSource->dataSourcePool(), loadInfo.meshChunk, loadInfo.objSource->render(), loadInfo.geometryCoordSys) == false) {

			MMemDelete(pRet);
		}
	}

	return pRet;
}

}