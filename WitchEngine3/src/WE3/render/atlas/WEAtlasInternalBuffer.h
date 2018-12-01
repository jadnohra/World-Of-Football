#ifndef _WEAtlasInternalBuffer_h
#define _WEAtlasInternalBuffer_h

#include "../../WERef.h"
#include "../../WETL/WETLArray.h"
#include "../WERenderer.h"

#include "WEAtlasRenderBuffer.h"
#include "WEAtlasBufferPart.h"

namespace WE {


	class AtlasInternalBuffer : public Ref {
	protected:

		typedef IndexT<USHORT> BufferPartIndex;
		typedef WETL::MemAllocT<RenderUINT, RenderUINT, true> MemBuffer;
		typedef WETL::CountedArray<AtlasBufferPart, false, BufferPartIndex::Type, WETL::ResizeExact> CountedFreeBufferParts;
		typedef WETL::CountedPtrArray<AtlasBufferPart*, false, BufferPartIndex::Type, WETL::ResizeExact> CountedBufferParts;
				
		
		struct CreationData {

			RenderBufferUsage mUsage;
			MemBuffer mMemBuffer;
			CountedFreeBufferParts mFreeParts;
		};

		static RenderUINT mMaxSizeIB;

		RefWrap<RenderBuffer> mRenderBuffer;
		CountedBufferParts mBufferParts;

		CreationData* mpCreationData;

	public:		
		
		static void setGlobalIBMaxSizeBytes(RenderUINT maxBytes);
		static RenderUINT getGlobalIBMaxSizeBytes();

		AtlasInternalBuffer();
		~AtlasInternalBuffer();

		bool isCompatibleWithIB(bool bit32, RenderBufferUsage& usage);
		bool canAccomodateIB(bool bit32, RenderUINT elementCount);
		AtlasBufferPart* accomodateIB(bool bit32, RenderBufferUsage& usage, RenderUINT elementCount);
		void releaseIB(AtlasBufferPart*& pPart);

		bool lockIB(Renderer& renderer, AtlasBufferPart* pBufferPart, const RenderUINT& offsetBytes, const RenderUINT& sizeBytes, void** ppData);
		bool unlockIB(Renderer& renderer, AtlasBufferPart* pBufferPart, const RenderUINT& offsetBytes, const RenderUINT& sizeBytes, void* pData);
		
		bool hasRenderIB();
		bool createRenderIB(Renderer& renderer);
		
		bool hasSystemMemData();
		void discardSystemMemData();

		bool isEmpty();
		bool isUnusable();

		void debug();

	protected:

		bool canAccomodateIB(RenderUINT elementCount, bool bit32, bool& recycled, BufferPartIndex::Type& recycleIndex, RenderUINT& newSizeBytes);
		
		void useRecycledPart(BufferPartIndex::Type recycledIndex, AtlasBufferPart* pUserPart);
		void addEmptyPart(RenderUINT offset, RenderUINT size);
		bool findMergableEmptyPart(BufferPartIndex::Type selfIndex, RenderUINT& offset, RenderUINT& size, BufferPartIndex::Type& mergableIndex, bool fixOffsetAndSize);
	};

}

#endif