#include "WEMeshMaterial.h"


namespace WE {


MeshMaterial::MeshMaterial() {
}

MeshMaterial::~MeshMaterial() {
}

StringHash MeshMaterial::getMaterialName() {

	return mNameHash;
}

}
