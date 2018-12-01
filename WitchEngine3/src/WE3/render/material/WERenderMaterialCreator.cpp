#include "WERenderMaterialCreator.h"

namespace WE {

RenderMaterial* RenderMaterialCreator::create(RenderMaterialLoadInfo& loadInfo) {

	RenderMaterial* pRet = NULL;

	//loadInfo.pDataPool->resolvePossiblyRemoteChunk(loadInfo.matChunk, true, NULL);

	if (loadInfo.matChunk.isValid()) {

		MMemNew(pRet, RenderMaterial());

		if (pRet->init(loadInfo.matChunk.dref()) == false) {

			MMemDelete(pRet);
		}
	}

	return pRet;
}

}