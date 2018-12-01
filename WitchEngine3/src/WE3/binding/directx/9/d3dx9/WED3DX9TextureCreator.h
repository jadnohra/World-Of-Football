#ifndef _WED3DX9TextureCreator_h
#define _WED3DX9TextureCreator_h

#include "../d3d9/renderer/WERendererD3D9.h"

namespace WE {

	class D3DX9TextureCreator {
	public:

		static RenderTexture* createAndLoad(const TCHAR* filePath, bool genMipmaps, const TCHAR* mipFilter, RendererD3D9& renderer, unsigned int* pWidth = NULL, unsigned int* pHeight = NULL);
	};

}

#endif