#include "WEMeshMaterial.h"


namespace WE {


MeshMaterial::MeshMaterial() {
}

MeshMaterial::~MeshMaterial() {
}


MeshRSIndex::Type MeshMaterial::getStateCount() {

	return 0;
}


const MeshRenderState* MeshMaterial::getState(MeshRSIndex::Type index) {

	return NULL;
}

}
