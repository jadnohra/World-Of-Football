#ifndef _WEAtlasRenderBuffer_h
#define _WEAtlasRenderBuffer_h

#include "../../WERef.h"
#include "../WERenderer.h"

#include "WEAtlasBufferPart.h"


namespace WE {

	class AtlasInternalBuffer;
	
	class AtlasRenderBuffer {
	friend class RenderBufferAtlas;
	friend class AtlasInternalBuffer;
	public:

		AtlasRenderBuffer();
		~AtlasRenderBuffer();

		bool lockIB(Renderer& renderer, const RenderUINT& offsetBytes, const RenderUINT& sizeBytes, void** ppData);
		bool unlockIB(Renderer& renderer, const RenderUINT& offsetBytes, const RenderUINT& sizeBytes, void* pData);

	protected:

		RefWrap<AtlasInternalBuffer> mBuffer;
		AtlasBufferPart* mpPart;
	};

}

#endif