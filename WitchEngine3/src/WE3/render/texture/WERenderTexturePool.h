#ifndef _WERenderTexturePool_h
#define _WERenderTexturePool_h

#include "WERenderTextureCreator.h"
#include "../pool/WERenderObjectPool.h"
#include "../../WETL/WETLArray.h"

namespace WE {

	class RenderTexturePool : public RefObjectPool {
	public:

		RenderTexturePool();
		~RenderTexturePool();

		RenderTexture* get(RenderTextureLoadInfo& loadInfo);

	protected:

		virtual RefPoolObjectBase* create(RefPoolObjectLoadInfo& loadInfo);
	};

}

#endif