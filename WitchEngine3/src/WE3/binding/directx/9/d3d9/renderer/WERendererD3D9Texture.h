#ifndef _WERendererD3D9Texture_h
#define _WERendererD3D9Texture_h

#include "includeRenderer.h"
#include "../includeD3D9.h"


namespace WE {

	class RendererD3D9;
	
	class RendererD3D9_Texture : public RenderTexture {
	public:

		IDirect3DTexture9* pTex;
		
		RendererD3D9_Texture();
		virtual ~RendererD3D9_Texture();

		virtual bool getSize(unsigned int& width, unsigned int& height);
		virtual bool getLoadSize(unsigned int& width, unsigned int& height);

	public:

		unsigned int mLoadSizeWidth;
		unsigned int mLoadSizeHeight;
	};

		
	inline void RendererD3D9_convert(const RenderTexture* from, RendererD3D9_Texture*& to) {

		to = (RendererD3D9_Texture*) from;
	}
	
	inline RendererD3D9_Texture& RendererD3D9_convert(const RenderTexture* from) {

		return dref<RendererD3D9_Texture>((RendererD3D9_Texture*) from);
	}

}

#endif