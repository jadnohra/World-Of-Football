#include "WEMeshSourceImpl.h"

namespace WE {

MeshSourceImpl::MeshSourceImpl(MeshPool* pPool) 
	: mpPool(pPool) {
}

MeshSourceImpl::~MeshSourceImpl() {
}

bool MeshSourceImpl::setPool(MeshPool* pPool) {

	assrt((mpPool == NULL) || (mpPool->getObjectCount() == 0));

	mpPool = pPool;

	return true;
}

MeshPool* MeshSourceImpl::getPool() {

	return mpPool;
}

Mesh* MeshSourceImpl::get(MeshLoadInfo& loadInfo) {

	if (mpPool) {

		return mpPool->get(loadInfo);
	} 

	return MeshCreator::create(loadInfo);
}


}