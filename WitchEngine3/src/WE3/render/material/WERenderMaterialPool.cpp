#include "WERenderMaterialPool.h"

namespace WE {

RenderMaterialPool::RenderMaterialPool() {
}

RenderMaterialPool::~RenderMaterialPool() {
}

RefPoolObjectBase* RenderMaterialPool::create(RefPoolObjectLoadInfo& loadInfo) {

	return RenderMaterialCreator::create((RenderMaterialLoadInfo&) loadInfo);
}

RenderMaterial* RenderMaterialPool::get(RenderMaterialLoadInfo& loadInfo) {

	return (RenderMaterial*) baseGet(loadInfo);
}


}