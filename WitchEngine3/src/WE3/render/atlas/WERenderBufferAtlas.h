#ifndef _WERenderBufferAtals_h
#define _WERenderBufferAtals_h

#include "../WERenderer.h"
#include "../../WETL/WETLArray.h"

#include "WEAtlasInternalBuffer.h"


namespace WE {

	
	class RenderBufferAtlas {
	public:

		RenderBufferAtlas();
		~RenderBufferAtlas();

		AtlasRenderBuffer* getIB(Renderer& renderer, bool bit32, RenderBufferUsage& usage, RenderUINT elementCount, bool createRenderIB = false, bool discardSystemMemData = false);
		void releaseIB(Renderer& renderer, AtlasRenderBuffer*& pBuffer);
		
		bool lockIB(Renderer& renderer, AtlasRenderBuffer* pBuffer, RenderUINT offsetBytes, RenderUINT sizeBytes, void** ppData);
		bool unlockIB(Renderer& renderer, AtlasRenderBuffer* pBuffer);

		bool createRenderIBs(Renderer& renderer, bool discardSystemMemData);
		void releaseUnusedRenderIBs();

		void discardSystemMemData();

		void debug();

	protected:
		
		typedef IndexT<UINT> InternalBufferIndex;
		typedef WETL::CountedRefArray<AtlasInternalBuffer*, false, InternalBufferIndex::Type> InternalBufferArray;

		InternalBufferArray mInternalIBs;
	};
}


#endif