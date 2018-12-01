#include "WERenderTextureSource.h"

namespace WE {

RenderTexture* RenderTextureSource::getFromDataChunk(DataChunk& texChunk, RenderLoadContext& renderLoadContext, 
													 const RefPoolObjectPoolInfo* pPoolInfo, int loaderVersion) {

	RenderTextureLoadInfoContainer loadInfo(&renderLoadContext);
	
	if (RenderTextureLoadInfoLoader_v1::load(texChunk, loadInfo) == false) {

		return NULL;
	}

	loadInfo.injectPoolState(pPoolInfo);

	return get(loadInfo);
}

}