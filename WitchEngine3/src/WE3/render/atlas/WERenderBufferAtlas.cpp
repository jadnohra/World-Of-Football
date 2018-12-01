#include "WERenderBufferAtlas.h"

namespace WE {


RenderBufferAtlas::RenderBufferAtlas() {
}

RenderBufferAtlas::~RenderBufferAtlas() {
}


AtlasRenderBuffer* RenderBufferAtlas::getIB(Renderer& renderer, bool bit32, RenderBufferUsage& usage, RenderUINT elementCount, bool createRenderIB, bool discardSystemMemData) {

	AtlasInternalBuffer* pIntBuffer = NULL;
	AtlasBufferPart* pPart = NULL;
	AtlasRenderBuffer* pRet = NULL;

	for (InternalBufferIndex i = 0; i < mInternalIBs.count; ++i) {

		if (mInternalIBs.el[i]->isCompatibleWithIB(bit32, usage)) {

			pIntBuffer = mInternalIBs.el[i];
			pPart = mInternalIBs.el[i]->accomodateIB(bit32, usage, elementCount);

			if (pPart) {

				break;
			}
		}
	}

	if (pPart == NULL) {

		MMemNew(pIntBuffer, AtlasInternalBuffer());
		
		pPart = pIntBuffer->accomodateIB(bit32, usage, elementCount);

		if (pPart == NULL) {

			MMemDelete(pIntBuffer);

		} else {

			mInternalIBs.addOne(pIntBuffer);
			//pIntBuffer->Release();
		}

	}

	if (pPart == NULL) {

		assrt(false);
		return NULL;
	}

	if (createRenderIB && (pIntBuffer->hasRenderIB() == false)) {

		pIntBuffer->createRenderIB(renderer);
	}
	if (discardSystemMemData && pIntBuffer->hasSystemMemData()) {

		pIntBuffer->discardSystemMemData();
	}
	
	MMemNew(pRet, AtlasRenderBuffer());

	pRet->mBuffer.set(pIntBuffer, true);
	pRet->mpPart = pPart;

	return pRet;
}

void RenderBufferAtlas::releaseIB(Renderer& renderer, AtlasRenderBuffer*& pBuffer) {

	//InternalBufferIndex::Type index;

	//if (mInternalIBs.searchFor(pBuffer->mBuffer.ptr(), 0, mInternalIBs.count, index)) {

	RefWrap<AtlasInternalBuffer> checkBuffer;
	checkBuffer.set(pBuffer->mBuffer.ptr(), true);

	pBuffer->mBuffer->releaseIB(pBuffer->mpPart);
	MMemDelete(pBuffer);

	if (checkBuffer.isValid() && (checkBuffer->GetRefCount() == 2) && checkBuffer->isUnusable()) {

		InternalBufferIndex::Type index;

		if (mInternalIBs.searchFor(checkBuffer.ptr(), 0, mInternalIBs.count, index)) {

			mInternalIBs.removeSwapWithLast(index, true);
		}
	}
	checkBuffer.destroy();

	//}
}

void RenderBufferAtlas::releaseUnusedRenderIBs() {

	for (InternalBufferIndex::Type i = 0; i < mInternalIBs.count; ++i) {

		if (mInternalIBs.el[i]->GetRefCount() == 1) {

			mInternalIBs.removeSwapWithLast(i, true);
			i--;
		}
	}
}
		
bool RenderBufferAtlas::lockIB(Renderer& renderer, AtlasRenderBuffer* pBuffer, RenderUINT offsetBytes, RenderUINT sizeBytes, void** ppData) {

	if (pBuffer->mBuffer->hasRenderIB()) {



	} else {
	}

	return false;
}

bool RenderBufferAtlas::unlockIB(Renderer& renderer, AtlasRenderBuffer* pBuffer) {

	return false;
}

bool RenderBufferAtlas::createRenderIBs(Renderer& renderer, bool _discardSystemMemData) {

	for (InternalBufferIndex::Type i = 0; i < mInternalIBs.count; ++i) {

		if (mInternalIBs.el[i]->hasRenderIB() == false) {
	
			if (mInternalIBs.el[i]->createRenderIB(renderer) == false) {

				assrt(false);
			} else {

				if (_discardSystemMemData) {

					mInternalIBs.el[i]->discardSystemMemData();
				}
			}
		}
	}

	return true;
}

void RenderBufferAtlas::discardSystemMemData() {

	for (InternalBufferIndex::Type i = 0; i < mInternalIBs.count; ++i) {

		mInternalIBs.el[i]->discardSystemMemData();

		if ((mInternalIBs.el[i]->GetRefCount() == 1) && mInternalIBs.el[i]->isUnusable()) {

			mInternalIBs.removeSwapWithLast(i, true);
			i--;
		}
	}
}

void RenderBufferAtlas::debug() {

	for (InternalBufferIndex::Type i = 0; i < mInternalIBs.count; ++i) {

		String::debug(L"internal buffer %u\n", i);
		mInternalIBs.el[i]->debug();
	}
}


}