#include "WERenderMaterialLoadInfo.h"

namespace WE {

RenderMaterialLoadInfo::RenderMaterialLoadInfo(RenderLoadContext* pRenderObjSource) : RenderPoolObjectLoadInfo(pRenderObjSource) {
}

bool RenderMaterialLoadInfoLoader_v1::load(DataChunk& chunk, RenderMaterialLoadInfo& loadInfo) {

	BufferString tempStr;

	if (loadInfo.needsPoolId()) {

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
	
	//loadInfo.pDataPool = &dataPool;
	loadInfo.matChunk.set(&chunk, true);

	return true;
}

}