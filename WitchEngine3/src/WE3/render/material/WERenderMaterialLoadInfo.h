#ifndef _WERenderMaterialLoadInfo_h
#define _WERenderMaterialLoadInfo_h

#include "../../data/WEDataSourcePool.h"
#include "../pool/WERenderPoolObjectLoadInfo.h"

namespace WE {

	struct RenderMaterialLoadInfo : RenderPoolObjectLoadInfo {

		RefWrap<DataChunk> matChunk;

		RenderMaterialLoadInfo(RenderLoadContext* pRenderObjSource);
	};

	
	class RenderMaterialLoadInfoLoader_v1 {
	public:

		static bool load(DataChunk& resolvedMatChunk, RenderMaterialLoadInfo& loadInfo);
	};

}

#endif