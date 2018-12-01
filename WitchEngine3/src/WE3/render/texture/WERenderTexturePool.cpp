#include "WERenderTexturePool.h"

namespace WE {

RenderTexturePool::RenderTexturePool() {
}

RenderTexturePool::~RenderTexturePool() {
}

RefPoolObjectBase* RenderTexturePool::create(RefPoolObjectLoadInfo& loadInfo) {

	return RenderTextureCreator::createAndLoad((RenderTextureLoadInfo&) loadInfo);
}

RenderTexture* RenderTexturePool::get(RenderTextureLoadInfo& loadInfo) {

	return (RenderTexture*) baseGet(loadInfo);
}


}