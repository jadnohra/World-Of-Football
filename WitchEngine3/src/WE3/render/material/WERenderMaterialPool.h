#ifndef _WERenderMaterialPool_h
#define _WERenderMaterialPool_h

#include "WERenderMaterialCreator.h"
#include "../pool/WERenderObjectPool.h"

namespace WE {

	class RenderMaterialPool : public RefObjectPool {
	public:

		RenderMaterialPool();
		~RenderMaterialPool();

		RenderMaterial* get(RenderMaterialLoadInfo& loadInfo);

	protected:

		virtual RefPoolObjectBase* create(RefPoolObjectLoadInfo& loadInfo);
	};

}

#endif