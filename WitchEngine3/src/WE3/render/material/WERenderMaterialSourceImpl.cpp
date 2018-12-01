#include "WERenderMaterialSourceImpl.h"

namespace WE {

RenderMaterialSourceImpl::RenderMaterialSourceImpl(RenderMaterialPool* pPool) 
	: mpPool(pPool) {
}

RenderMaterialSourceImpl::~RenderMaterialSourceImpl() {
}

bool RenderMaterialSourceImpl::setPool(RenderMaterialPool* pPool) {

	assrt((mpPool == NULL) || (mpPool->getObjectCount() == 0));

	mpPool = pPool;

	return true;
}

RenderMaterialPool* RenderMaterialSourceImpl::getPool() {

	return mpPool;
}

RenderMaterial* RenderMaterialSourceImpl::get(RenderMaterialLoadInfo& loadInfo) {

	if (mpPool) {

		return mpPool->get(loadInfo);
	} 

	return RenderMaterialCreator::create(loadInfo);
}


}