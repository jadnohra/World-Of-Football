#ifndef _WERenderTextureLoadInfo_h
#define _WERenderTextureLoadInfo_h

#include "../../data/WEDataSourcePool.h"
#include "../pool/WERenderPoolObjectLoadInfo.h"

namespace WE {

	struct RenderTextureLoadInfo : RenderPoolObjectLoadInfo {

		const TCHAR* filePath;
		bool genMipmaps;
		const TCHAR* mipFilter;

		unsigned int width;
		unsigned int height;

		RenderTextureLoadInfo(RenderLoadContext* pRenderObjSource);
	};

	struct RenderTextureLoadInfoContainer : RenderTextureLoadInfo {

		String reserved[2];

		RenderTextureLoadInfoContainer(RenderLoadContext* pRenderObjSource);

		void update();
	};

	class RenderTextureLoadInfoLoader_v1 {
	public:

		static bool load(DataChunk& texChunk, RenderTextureLoadInfoContainer& loadInfo);
	};

}

#endif