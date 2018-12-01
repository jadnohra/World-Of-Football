#ifndef _WERenderLoadContextImpl_h
#define _WERenderLoadContextImpl_h

#include "WERenderLoadContext.h"
#include "../texture/WERenderTextureSourceImpl.h"
#include "../material/WERenderMaterialSourceImpl.h"

namespace WE {

	class RenderLoadContextImpl : public RenderLoadContext {
	public:

		RenderLoadContextImpl();
		~RenderLoadContextImpl();

	protected:

		RenderTexturePool mTexturePool;
		RenderTextureSourceImpl mTextureSourceImpl;

		RenderMaterialPool mMaterialPool;
		RenderMaterialSourceImpl mMaterialSourceImpl;
	};

}

#endif