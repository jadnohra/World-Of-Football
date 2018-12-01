#include "WERendererD3D9ResourceManager.h"

namespace WE {

RendererD3D9_ResourceManager::RendererD3D9_ResourceManager() {
}

RendererD3D9_ResourceManager::~RendererD3D9_ResourceManager() {
}

void RendererD3D9_ResourceManager::init(ResourceIndex::Type count) {

	mVolatileResources.reserve(count);
}

void RendererD3D9_ResourceManager::add(RendererD3D9_VolatileResource* pResource) {

	mVolatileResources.addOne(pResource);
}


bool RendererD3D9_ResourceManager::remove(RendererD3D9_VolatileResource* pResource) {

	for (ResourceIndex i = 0; i < mVolatileResources.count; ++i) {

		if (mVolatileResources.el[i] == pResource) {

			mVolatileResources.removeIndex(i);
			return true;
		}
	}

	return false;
}

void RendererD3D9_ResourceManager::onDeviceState(IDirect3DDevice9* pDevice, RendererD3D9_DeviceState state) {

	for (ResourceIndex i = 0; i < mVolatileResources.count; ++i) {

		mVolatileResources.el[i]->onDeviceState(pDevice, state);
	}
}

}