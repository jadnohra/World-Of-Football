#include "WERenderLoadContext.h"

#include "../texture/WERenderTextureSource.h"
#include "../material/WERenderMaterialSource.h"

namespace WE {

RenderLoadContext::RenderLoadContext() {
}

RenderLoadContext::~RenderLoadContext() {
}

void RenderLoadContext::destroy() {

	mMatSource.destroy();
	mTexSource.destroy();
	mRenderer.destroy();
}

CoordSysConvPool& RenderLoadContext::convPool() {

	return mConvPool;
}

Renderer* RenderLoadContext::rendererPtr() {

	return mRenderer.ptr();
}

RenderTextureSource* RenderLoadContext::textureSourcePtr() {

	return mTexSource.ptr();
}

RenderMaterialSource* RenderLoadContext::materialSourcePtr() {

	return mMatSource.ptr();
}

Renderer& RenderLoadContext::renderer() {

	return mRenderer.dref();
}

RenderTextureSource& RenderLoadContext::textureSource() {

	return mTexSource.dref();
}

RenderMaterialSource& RenderLoadContext::materialSource() {

	return mMatSource.dref();
}


void RenderLoadContext::setRenderer(Renderer* pRenderer) {

	mRenderer = pRenderer;
}

void RenderLoadContext::setTextureSource(RenderTextureSource* pSource) {

	mTexSource = pSource;
}

void RenderLoadContext::setMaterialSource(RenderMaterialSource* pSource) {

	mMatSource = pSource;
}

}