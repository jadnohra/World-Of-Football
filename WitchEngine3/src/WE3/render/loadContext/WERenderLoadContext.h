#ifndef _WERenderLoadContext_h
#define _WERenderLoadContext_h

#include "../WERenderer.h"
#include "../../coordSys/WECoordSysConvPool.h"

namespace WE {

	class RenderTextureSource;
	class RenderMaterialSource;


	class RenderLoadContext {
	public:

		RenderLoadContext();
		~RenderLoadContext();

		void destroy();

		Renderer* rendererPtr();
		RenderTextureSource* textureSourcePtr();
		RenderMaterialSource* materialSourcePtr();

		Renderer& renderer();
		RenderTextureSource& textureSource();
		RenderMaterialSource& materialSource();
		
		void setRenderer(Renderer* pRenderer);
		void setTextureSource(RenderTextureSource* pSource);
		void setMaterialSource(RenderMaterialSource* pSource);

		CoordSysConvPool& convPool();

	protected:

		CoordSysConvPool mConvPool;

		HardRef<Renderer> mRenderer;
		SoftPtr<RenderTextureSource> mTexSource;
		SoftPtr<RenderMaterialSource> mMatSource;
	};

}

#endif