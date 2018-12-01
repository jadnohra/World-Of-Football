#include "WERendererD3D9Texture.h"

#include "WERendererD3D9.h"

namespace WE {

RendererD3D9_Texture::RendererD3D9_Texture() : pTex(NULL), mLoadSizeWidth(0.0f), mLoadSizeHeight(0.0f) {
}

RendererD3D9_Texture::~RendererD3D9_Texture() {

	MSafeRelease(pTex);
}

bool RendererD3D9_Texture::getLoadSize(unsigned int& width, unsigned int& height) {

	width = mLoadSizeWidth;
	height = mLoadSizeHeight;

	return mLoadSizeWidth > 0.0f;
}

bool RendererD3D9_Texture::getSize(unsigned int& width, unsigned int& height) {

	D3DSURFACE_DESC desc;

	if (pTex != NULL && (pTex->GetLevelDesc(0, &desc) == D3D_OK)) {

		width = desc.Width;
		height = desc.Height;

		return true;
	}

	return false;
}

void RendererD3D9::setTexture(RenderTexture* pTexture, int stage) {

	if (pTexture) {

		RendererD3D9_Texture& tex = RendererD3D9_convert(pTexture);

		MCheckD3DStmt(mpDevice->SetTexture(stage, tex.pTex));

	} else {

		MCheckD3DStmt(mpDevice->SetTexture(stage, NULL));
	}
}

RenderTexture* RendererD3D9::createTextureFromNative(IDirect3DTexture9* pNativeTex, unsigned int* pLoadWidth, unsigned int* pLoadHeight) {

	RendererD3D9_Texture* pRet = NULL;

	if (pNativeTex) {

		MMemNew(pRet, RendererD3D9_Texture());
		pNativeTex->AddRef();
		pRet->pTex = pNativeTex;
		pRet->mLoadSizeWidth = pLoadWidth ? *pLoadWidth : 0.0f;
		pRet->mLoadSizeHeight = pLoadHeight ? *pLoadHeight : 0.0f;
	}

	return pRet;
}

}