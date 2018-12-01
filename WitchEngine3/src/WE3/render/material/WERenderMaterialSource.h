#ifndef _WERenderMaterialSource_h
#define _WERenderMaterialSource_h

#include "WERenderMaterialCreator.h"

namespace WE {

	class RenderMaterialSource {
	public:

		virtual RenderMaterial* get(RenderMaterialLoadInfo& loadInfo) = 0;

		RenderMaterial* getFromDataChunk(DataChunk& resolvedMatChunk, RenderLoadContext& renderLoadContext, const RefPoolObjectPoolInfo* pPoolInfo = NULL, int loaderVersion = 1);
		RenderMaterial* getFromDataChunk(DataSourcePool& dataPool, DataSourceChunk matChunk, RenderLoadContext& renderLoadContext, const RefPoolObjectPoolInfo* pPoolInfo = NULL, int loaderVersion = 1);
	};
}

#endif