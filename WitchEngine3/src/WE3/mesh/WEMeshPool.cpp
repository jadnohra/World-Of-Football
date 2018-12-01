#include "WEMeshPool.h"

namespace WE {

MeshPool::MeshPool() {
}

MeshPool::~MeshPool() {
}

RefPoolObjectBase* MeshPool::create(RefPoolObjectLoadInfo& loadInfo) {

	return MeshCreator::create((MeshLoadInfo&) loadInfo);
}

Mesh* MeshPool::get(MeshLoadInfo& loadInfo) {

	return (Mesh*) baseGet(loadInfo);
}


}