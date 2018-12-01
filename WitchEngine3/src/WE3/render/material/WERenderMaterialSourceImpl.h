#ifndef _WERenderMaterialSourceImpl_h
#define _WERenderMaterialSourceImpl_h

#include "WERenderMaterialSource.h"
#include "WERenderMaterialPool.h"

namespace WE {

	class RenderMaterialSourceImpl : public RenderMaterialSource {
	public:

		RenderMaterialSourceImpl(RenderMaterialPool* pPool = NULL);
		~RenderMaterialSourceImpl();

		bool setPool(RenderMaterialPool* pPool);
		RenderMaterialPool* getPool();

		virtual RenderMaterial* get(RenderMaterialLoadInfo& loadInfo);

	protected:

		RenderMaterialPool* mpPool;
	};

}

#endif