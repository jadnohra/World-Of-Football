#include "WED3DX9TextureCreator.h"

#include "includeD3DX9.h"

namespace WE {


RenderTexture* D3DX9TextureCreator::createAndLoad(const TCHAR* filePath, bool genMipmaps, const TCHAR* mipFilter, RendererD3D9& renderer, unsigned int* pWidth, unsigned int* pHeight) {

	IDirect3DTexture9* pNativeTex = NULL;
	RenderTexture* pRet;

	D3DXIMAGE_INFO info;

	if (D3DXCreateTextureFromFileEx(renderer.getDevice(), filePath, 
										D3DX_DEFAULT, D3DX_DEFAULT, 
										genMipmaps ? D3DX_DEFAULT : 1,
										0, 
										D3DFMT_UNKNOWN, 
										D3DPOOL_MANAGED, 
										D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 
										D3DX_FILTER_BOX, 
										0, 
										&info, NULL, &pNativeTex) 
										!= D3D_OK) {
		return NULL;
	}

	if (pWidth) {

		*pWidth = info.Width;
	}

	if (pHeight) {

		*pHeight = info.Height; 
	}

	pRet = renderer.createTextureFromNative(pNativeTex, &info.Width, &info.Height);
	pNativeTex->Release();

	return pRet;
}


}