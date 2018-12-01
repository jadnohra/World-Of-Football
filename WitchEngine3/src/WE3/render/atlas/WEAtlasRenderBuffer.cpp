#include "WEAtlasRenderBuffer.h"

#include "WEAtlasInternalBuffer.h"

namespace WE {

AtlasRenderBuffer::AtlasRenderBuffer() : mpPart(NULL) {
}


AtlasRenderBuffer::~AtlasRenderBuffer() {

	//taken care of in RenderBufferAtlas release functions
}

bool AtlasRenderBuffer::lockIB(Renderer& renderer, const RenderUINT& offsetBytes, const RenderUINT& sizeBytes, void** ppData) {

	return mBuffer->lockIB(renderer, mpPart, offsetBytes, sizeBytes, ppData);
}

bool AtlasRenderBuffer::unlockIB(Renderer& renderer, const RenderUINT& offsetBytes, const RenderUINT& sizeBytes, void* pData) {

	return mBuffer->unlockIB(renderer, mpPart, offsetBytes, sizeBytes, pData);
}

}

