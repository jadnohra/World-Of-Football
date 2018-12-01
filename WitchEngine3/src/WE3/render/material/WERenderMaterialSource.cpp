#include "WERenderMaterialSource.h"

namespace WE {

RenderMaterial* RenderMaterialSource::getFromDataChunk(DataChunk& resolvedMatChunk, RenderLoadContext& renderLoadContext, const RefPoolObjectPoolInfo* pPoolInfo, int loaderVersion) {

	RenderMaterialLoadInfo loadInfo(&renderLoadContext);

	if (RenderMaterialLoadInfoLoader_v1::load(resolvedMatChunk, loadInfo) == false) {

		return NULL;
	}

	loadInfo.injectPoolState(pPoolInfo);

	return get(loadInfo);
}

RenderMaterial* RenderMaterialSource::getFromDataChunk(DataSourcePool& dataPool, DataSourceChunk matChunk, RenderLoadContext& renderLoadContext, const RefPoolObjectPoolInfo* pPoolInfo, int loaderVersion) {

	if (matChunk.isNull()) {

		return NULL;
	}

	dataPool.resolvePossiblyRemoteChunk(matChunk, true, NULL);
	return getFromDataChunk(matChunk.chunk(), renderLoadContext, pPoolInfo, loaderVersion);
}

}