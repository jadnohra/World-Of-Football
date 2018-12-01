#include "WEAtlasInternalBuffer.h"

#include "../../WEString.h"

namespace WE {

RenderUINT AtlasInternalBuffer::mMaxSizeIB = (RenderUINT) -1;

void AtlasInternalBuffer::setGlobalIBMaxSizeBytes(RenderUINT maxBytes) {

	mMaxSizeIB = maxBytes;
}

RenderUINT AtlasInternalBuffer::getGlobalIBMaxSizeBytes() {

	return mMaxSizeIB;
}

AtlasInternalBuffer::AtlasInternalBuffer() : mpCreationData(NULL) {
}

AtlasInternalBuffer::~AtlasInternalBuffer() {

	discardSystemMemData();
}

bool AtlasInternalBuffer::isEmpty() {

	return (mBufferParts.count == 0);
}

bool AtlasInternalBuffer::isUnusable() {

	return (isEmpty() && (hasSystemMemData() == false));
}

bool AtlasInternalBuffer::canAccomodateIB(RenderUINT elementCount, bool bit32, bool& recycled, BufferPartIndex::Type& recycleIndex, RenderUINT& newSizeBytes) {

	bool recycledOnly = mRenderBuffer.isValid();

	recycled = false;

	RenderUINT currSizeBytes;
	RenderUINT accSizeBytes;

	if (mpCreationData == NULL) {

		currSizeBytes = 0;
		accSizeBytes = elementCount;

		accSizeBytes = elementCount;
		if (checkMul(accSizeBytes, bit32 ? sizeof(UINT32) : sizeof(UINT16)) == false) {
	
			return false;
		}

	} else {

		if (mpCreationData->mMemBuffer.toSizeBytesCheck(elementCount, accSizeBytes) == false) {

			return false;
		}
		
		RenderUINT minFoundSizeBytes = mMaxSizeIB;
		bool found;

		found = false;
		for (BufferPartIndex::Type i = 0; i < mpCreationData->mFreeParts.count; ++i) {

			AtlasBufferPart& part = mpCreationData->mFreeParts.el[i];

			if (part.sizeBytes >= accSizeBytes) {

				if (part.sizeBytes < minFoundSizeBytes) {

					minFoundSizeBytes = minFoundSizeBytes;

					recycleIndex = i;
					found = true;
				} 
			}
		}

		if (found) {

			recycled = true;
			return true;
		}

		currSizeBytes = mpCreationData->mMemBuffer.size;
	}
		

	if (recycledOnly && (recycled == false)) {

		return false;
	}

	newSizeBytes = currSizeBytes;
	if (checkOp<RenderUINT>(newSizeBytes, +accSizeBytes) == false) {

		return false;
	}

	if (newSizeBytes >= mMaxSizeIB) {

		return false;
	}

	return true;
}

bool AtlasInternalBuffer::canAccomodateIB(bool bit32, RenderUINT elementCount) {

	RenderUINT newSizeBytes;
	bool recycled;
	BufferPartIndex::Type recycleIndex;

	return canAccomodateIB(elementCount, bit32, recycled, recycleIndex, newSizeBytes);
}

bool AtlasInternalBuffer::isCompatibleWithIB(bool bit32, RenderBufferUsage& usage) {

	if (mpCreationData == NULL) {
	
		return mRenderBuffer.isNull();
	} 

	if (mpCreationData->mUsage.equals(usage) == false) {

		return false;
	}

	return true;
}


bool AtlasInternalBuffer::findMergableEmptyPart(BufferPartIndex::Type selfIndex, RenderUINT& offset, RenderUINT& size, BufferPartIndex::Type& mergableIndex, bool fixOffsetAndSize) {

	if (mpCreationData == NULL) {

		return false;
	}

	RenderUINT end = offset + size;

	for (BufferPartIndex::Type i = 0; i < mpCreationData->mFreeParts.count; ++i) {

		if (i != selfIndex) {

			AtlasBufferPart& part = mpCreationData->mFreeParts.el[i];

			if (part.offsetBytes == end) {

				if (fixOffsetAndSize) {

					size += part.sizeBytes;
				}

				mergableIndex = i;
				return true;
				
			} else if (part.offsetBytes + part.sizeBytes == offset) {

				if (fixOffsetAndSize) {

					offset = part.offsetBytes;
					size += part.sizeBytes;
				}

				mergableIndex = i;
				return true;
			}
		}
	}

	return false;
}

void AtlasInternalBuffer::addEmptyPart(RenderUINT offset, RenderUINT size) {

	if (mpCreationData == NULL) {

		assrt(false);
		return;
	}

	BufferPartIndex::Type mergeIndex;
	
	if (findMergableEmptyPart((BufferPartIndex::Type) -1, offset, size, mergeIndex, true)) {

		//remove part to be merged
		mpCreationData->mFreeParts.removeSwapWithLast(mergeIndex, true);

		//add new part
		addEmptyPart(offset, size);

		return;
	}

	//no mergers
	AtlasBufferPart& part = mpCreationData->mFreeParts.addOne();

	part.offsetBytes = offset;
	part.sizeBytes = size;
}

void AtlasInternalBuffer::useRecycledPart(BufferPartIndex::Type recycledIndex, AtlasBufferPart* pUserPart) {

	AtlasBufferPart& part = mpCreationData->mFreeParts.el[recycledIndex];


	BufferPartIndex::Type mergeIndex;
	bool useFirstPart = true;
	
	if (findMergableEmptyPart(recycledIndex, part.offsetBytes, part.sizeBytes, mergeIndex, false)) {

		if (mpCreationData->mFreeParts.el[mergeIndex].offsetBytes < part.offsetBytes) {
			
			useFirstPart = false;
		}
	}

	if (part.sizeBytes > pUserPart->sizeBytes) {
		
		AtlasBufferPart emptyPart;

		if (useFirstPart) {

			pUserPart->offsetBytes = part.offsetBytes;

			emptyPart.offsetBytes = part.offsetBytes + pUserPart->sizeBytes;
			emptyPart.sizeBytes = part.sizeBytes - pUserPart->sizeBytes;

		} else {

			pUserPart->offsetBytes = part.offsetBytes + (part.sizeBytes - pUserPart->sizeBytes);

			emptyPart.offsetBytes = part.offsetBytes;
			emptyPart.sizeBytes = part.sizeBytes - pUserPart->sizeBytes;
		}

		mpCreationData->mFreeParts.removeSwapWithLast(recycledIndex, true);
		addEmptyPart(emptyPart.offsetBytes, emptyPart.sizeBytes);

	} else {

		assrt(part.sizeBytes == pUserPart->sizeBytes);

		mpCreationData->mFreeParts.removeSwapWithLast(recycledIndex, true);
	}
}

AtlasBufferPart* AtlasInternalBuffer::accomodateIB(bool bit32, RenderBufferUsage& usage, RenderUINT elementCount) {

	RenderUINT newSizeBytes;
	bool recycled;
	BufferPartIndex::Type recycledIndex;

	if (canAccomodateIB(elementCount, bit32, recycled, recycledIndex, newSizeBytes) == false) {

		return NULL;
	}

	if (mpCreationData == NULL) {

		MMemNew(mpCreationData, CreationData());

		mpCreationData->mUsage = usage;
		mpCreationData->mMemBuffer.setStride(bit32 ? sizeof(UINT32) : sizeof(UINT16), false);
	}

	AtlasBufferPart* pPart;
	MMemNew(pPart, AtlasBufferPart());

	mpCreationData->mMemBuffer.toSizeBytesCheck(elementCount, pPart->sizeBytes);

	mBufferParts.addOne(pPart);

	if (recycled) {

		useRecycledPart(recycledIndex, pPart);

	} else {

		pPart->offsetBytes = mpCreationData->mMemBuffer.size;
		mpCreationData->mMemBuffer.resize(newSizeBytes);
	}

	return pPart;
}

void AtlasInternalBuffer::releaseIB(AtlasBufferPart*& pPart) {

	BufferPartIndex::Type partIndex;

	if (mBufferParts.searchFor(pPart, 0, mBufferParts.count, partIndex)) {
		
		if (mpCreationData) {
			
			addEmptyPart(pPart->offsetBytes, pPart->sizeBytes);
		}

		mBufferParts.removeSwapWithLast(partIndex, true);

		pPart = NULL;

	} else {

		assrt(false);
		//??MMemDelete(pPart);
	}

}

bool AtlasInternalBuffer::hasRenderIB() {

	return (mRenderBuffer.isValid());
}

bool AtlasInternalBuffer::createRenderIB(Renderer& renderer) {

	if (mRenderBuffer.isValid() || (mpCreationData == NULL)) {

		return false;
	}

	if (renderer.createIB((mpCreationData->mMemBuffer.getStride() == sizeof(UINT32)), 
		mpCreationData->mUsage, mpCreationData->mMemBuffer.size, &(mRenderBuffer.pObject))) {

		//mRenderBuffer.pObject->AddRef();

		void* pData;
		renderer.lockIB(mRenderBuffer.ptr(), 0, mpCreationData->mMemBuffer.size, &pData);
		memcpy(pData, mpCreationData->mMemBuffer.el, mpCreationData->mMemBuffer.size);
		renderer.unlockIB(mRenderBuffer.ptr());

		return true;
	}

	return false;
}

bool AtlasInternalBuffer::lockIB(Renderer& renderer, AtlasBufferPart* pBufferPart, 
								 const RenderUINT& offsetBytes, const RenderUINT& sizeBytes, void** ppData) {

	RenderBuffer* pRenderBuffer = mRenderBuffer.ptr();

	if (pRenderBuffer) {

		renderer.lockIB(pRenderBuffer, pBufferPart->offsetBytes + offsetBytes, sizeBytes, ppData);

	} else {

		*ppData = (void*) (mpCreationData->mMemBuffer.el + pBufferPart->offsetBytes + offsetBytes);
	}

	return true;
}

bool AtlasInternalBuffer::unlockIB(Renderer& renderer, AtlasBufferPart* pBufferPart, 
								   const RenderUINT& offsetBytes, const RenderUINT& sizeBytes, void* pData) {

	RenderBuffer* pRenderBuffer = mRenderBuffer.ptr();

	if (pRenderBuffer) {

		renderer.unlockIB(pRenderBuffer);

		if (mpCreationData) {

			void* pDest = (void*) (mpCreationData->mMemBuffer.el + pBufferPart->offsetBytes + offsetBytes);

			memcpy(pDest, pData, sizeBytes);
		}
	} 

	return true;
}

bool AtlasInternalBuffer::hasSystemMemData() {

	return (mpCreationData != NULL);
}

void AtlasInternalBuffer::discardSystemMemData() {

	MMemDelete(mpCreationData);
}

void AtlasInternalBuffer::debug() {

	String::debug(L"RenderBuffer %s, SysMemData %s\n", mRenderBuffer.isValid() ? L"yes": L"no", mpCreationData ? L"yes": L"no");
	String::debug(L"Used\n");

	for (BufferPartIndex i = 0; i < mBufferParts.count; i++) {

		AtlasBufferPart* pPart = mBufferParts.el[i];

		if (pPart) {
			
			String::debug(L"%u, %u\n", pPart->offsetBytes, pPart->sizeBytes);
		}
	}

	if (mpCreationData) {

		String::debug(L"Free\n");

		for (BufferPartIndex::Type i = 0; i < mpCreationData->mFreeParts.count; ++i) {

			AtlasBufferPart& part = mpCreationData->mFreeParts.el[i];

			String::debug(L"%u, %u\n", part.offsetBytes, part.sizeBytes);
		}
	}
}

}