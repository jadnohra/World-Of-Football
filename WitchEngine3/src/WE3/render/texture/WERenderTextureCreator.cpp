#include "WERenderTextureCreator.h"

#include "../../binding/directx/9/d3dx9/WED3DX9TextureCreator.h"
#include "../../binding/directx/9/d3d9/renderer/WERendererD3D9.h"

namespace WE {

String RenderTextureCreator::mDebugTexturePath;

void RenderTextureCreator::setDebugTexturePath(const TCHAR* path) {

	mDebugTexturePath.assign(path);
}

RenderTexture* RenderTextureCreator::createAndLoad(RenderTextureLoadInfo& loadInfo) {

	Renderer& renderer = loadInfo.renderLoadContext->renderer();

	if (tcharCompare(renderer.getName(), RendererD3D9::kName) != 0) {

		assrt(false);
		return NULL;
	}

	RenderTexture* pRet = D3DX9TextureCreator::createAndLoad(loadInfo.filePath, loadInfo.genMipmaps, loadInfo.mipFilter, (RendererD3D9&) renderer, &loadInfo.width, &loadInfo.height);

	if (pRet == NULL) {

		log(L"Texture Not Found [%s]", loadInfo.filePath);

		if (mDebugTexturePath.isValid()) {

			pRet = D3DX9TextureCreator::createAndLoad(mDebugTexturePath.c_tstr(), loadInfo.genMipmaps, loadInfo.mipFilter, (RendererD3D9&) renderer);

			if (pRet == NULL) {
				
				log(L"Debug Texture Not Found [%s]", mDebugTexturePath.c_tstr());
			}

		} else {

			assrt(false);
		}

		
	}


	return pRet;
}


}