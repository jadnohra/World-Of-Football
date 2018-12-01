#include "WERenderTextureSourceImpl.h"

namespace WE {

RenderTextureSourceImpl::RenderTextureSourceImpl(RenderTexturePool* pPool) 
	: mpPool(pPool) {
}

RenderTextureSourceImpl::~RenderTextureSourceImpl() {
}

bool RenderTextureSourceImpl::setPool(RenderTexturePool* pPool) {

	assrt((mpPool == NULL) || (mpPool->getObjectCount() == 0));

	mpPool = pPool;

	return true;
}

RenderTexturePool* RenderTextureSourceImpl::getPool() {

	return mpPool;
}

RenderTexture* RenderTextureSourceImpl::get(RenderTextureLoadInfo& loadInfo) {

	if (mpPool) {

		return mpPool->get(loadInfo);
	} 

	return RenderTextureCreator::createAndLoad(loadInfo);
}


}