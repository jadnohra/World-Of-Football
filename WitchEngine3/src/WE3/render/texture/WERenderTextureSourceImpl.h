#ifndef _WERenderTextureSourceImpl_h
#define _WERenderTextureSourceImpl_h

#include "WERenderTextureSource.h"
#include "WERenderTexturePool.h"

namespace WE {

	class RenderTextureSourceImpl : public RenderTextureSource {
	public:

		RenderTextureSourceImpl(RenderTexturePool* pPool = NULL);
		~RenderTextureSourceImpl();

		bool setPool(RenderTexturePool* pPool);
		RenderTexturePool* getPool();

		virtual RenderTexture* get(RenderTextureLoadInfo& loadInfo);

	protected:

		RenderTexturePool* mpPool;
	};

}

#endif