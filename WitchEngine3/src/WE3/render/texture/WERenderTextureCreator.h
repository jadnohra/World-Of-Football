#ifndef _WERenderTextureCreator_h
#define _WERenderTextureCreator_h

#include "../WERenderer.h"
#include "WERenderTextureLoadInfo.h"

namespace WE {

	

	class RenderTextureCreator {
	public:

		static RenderTexture* createAndLoad(RenderTextureLoadInfo& loadInfo);
		static void setDebugTexturePath(const TCHAR* path = NULL);

	protected:

		static String mDebugTexturePath;
	};
}

#endif

