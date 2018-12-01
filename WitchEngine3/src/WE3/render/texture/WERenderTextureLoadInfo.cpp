#include "WERenderTextureLoadInfo.h"

namespace WE {

RenderTextureLoadInfo::RenderTextureLoadInfo(RenderLoadContext* pRenderObjSource) : RenderPoolObjectLoadInfo(pRenderObjSource) {

	filePath = NULL;
	mipFilter = NULL;
}

RenderTextureLoadInfoContainer::RenderTextureLoadInfoContainer(RenderLoadContext* pRenderObjSource) : RenderTextureLoadInfo(pRenderObjSource) {
}

void RenderTextureLoadInfoContainer::update() {

	filePath = reserved[0].c_tstr();
	mipFilter = reserved[1].c_tstr();
}

bool RenderTextureLoadInfoLoader_v1::load(DataChunk& chunk, RenderTextureLoadInfoContainer& loadInfo) {

	BufferString tempStr;

	chunk.getAttribute(L"mipFilter", loadInfo.reserved[1]);

	loadInfo.genMipmaps = (loadInfo.reserved[1].length() != 0);
	chunk.scanAttribute(tempStr, L"genMipmaps", loadInfo.genMipmaps);

	{

		DataSourceTranslationContext ctx;
		ctx.start(&chunk, true);

		chunk.getAttribute(L"file", loadInfo.reserved[0]);

		if (chunk.resolveFilePath(loadInfo.reserved[0]) == false) {

			return false;
		}

		if (loadInfo.needsPoolId()) {

			bool poolIdSet = chunk.getAttribute(L"poolId", tempStr);

			if (poolIdSet) {

				if (tempStr.equals(L"auto")) {

					loadInfo.poolId = loadInfo.reserved[0].hash() + loadInfo.reserved[1].hash() + (StringHash) loadInfo.genMipmaps;

				} else {

					loadInfo.poolId = tempStr.hash();
				}


			} else {

				loadInfo.poolState = Pool_NonePoolable;
			}
		}
	}

	loadInfo.update();

	return true;
}

}