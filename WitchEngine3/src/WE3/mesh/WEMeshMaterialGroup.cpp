#include "WEMeshMaterialGroup.h"

#include "WEMeshInstance.h"


namespace WE {

MeshMaterialGroup::MeshMaterialGroup() {
}

MeshMaterialGroup::~MeshMaterialGroup() {
}

bool MeshMaterialGroup::setMaterialCount(MeshMaterialIndex::Type count) {

	if (mMaterials.size != 0) {

		return false;
	}

	mMaterials.setSize(count);

	return true;
}

MeshMaterialIndex::Type MeshMaterialGroup::getMaterialCount() {

	return mMaterials.size;
}
		
bool MeshMaterialGroup::createMaterial(MeshMaterialIndex::Type index, MeshMaterial** ppMaterial) {

	if (mMaterials.el[index] != NULL) {

		return false;
	}

	MMemNew(mMaterials.el[index], MeshMaterial());

	if (ppMaterial) {

		*ppMaterial = mMaterials.el[index];
	}

	return true;
}


MeshMaterial& MeshMaterialGroup::getMaterial(MeshMaterialIndex::Type index) {

	return dref<MeshMaterial>(mMaterials.el[index]);
}

MeshMaterialIndex::Type MeshMaterialGroup::findMaterialIndex(const TCHAR* name) {

	StringHash nameHash = String::hash(name);

	for (MeshMaterialIndex i = 0; i < mMaterials.size; i++) {

		if (mMaterials.el[i] && (mMaterials.el[i]->mNameHash == nameHash)) {

			return i;
		}
	}

	return MeshMaterialIndex::Null;
}

void MeshMaterialGroup::prepareForRenderer(RenderLoadContext& renderLoadContext, MeshMaterialIndex::Type index) {

	mMaterials.el[index]->mRenderMaterial->prepareForRenderer(renderLoadContext);
}

bool MeshMaterialGroup::set(Renderer& renderer, MeshMaterialIndex::Type index, MeshMaterialInstance* pMatInstance) {

	return mMaterials.el[index]->mRenderMaterial->set(renderer, pMatInstance);
}

void MeshMaterialGroup::unset(Renderer& renderer, MeshMaterialIndex::Type index, MeshMaterialInstance* pMatInstance) {

	mMaterials.el[index]->mRenderMaterial->unset(renderer, pMatInstance);
}

void MeshMaterialGroup::createInstanceMaterials(RenderLoadContext& renderLoadContext, MeshInstance& inst) {

	inst.mMaterialInstances.setSize(mMaterials.size);

	for (MeshMaterialIndex i = 0; i < mMaterials.size; i++) {

		if (mMaterials.el[i]->mRenderMaterial.isValid()) {

			inst.mMaterialInstances.el[i] = mMaterials.el[i]->mRenderMaterial->createInstance(renderLoadContext, 0);
		}
	}

}

}