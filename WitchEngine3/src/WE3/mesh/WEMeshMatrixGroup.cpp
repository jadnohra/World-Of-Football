#include "WEMeshMatrixGroup.h"

#include "WEMeshInstance.h"

namespace WE {

MeshMatrixGroup::MeshMatrixGroup() {

	mIsIdentityMatrixMapping = false;

	mPartIB.zero();
	mPartVB.zero();
}

MeshMatrixGroup::~MeshMatrixGroup() {
}

void MeshMatrixGroup::setMatrixCount(SceneTransformIndex count) {

	if (count) 
		mMatrixIndexArray.reserve(count);
	else 
		mMatrixIndexArray.destroy();
}

void MeshMatrixGroup::setMatrixIndex(SceneTransformIndex index, SceneTransformIndex value) {

	mMatrixIndexArray.el[index] = value;
}

SceneTransformIndex::Type MeshMatrixGroup::extractMaxMatrixIndex() {

	SceneTransformIndex::Type max = 0;

	for (SceneTransformIndex::Type i = 0; i < mMatrixIndexArray.size; i++) {

		max = MMax(mMatrixIndexArray.el[i], max);
	}

	return max;
}

void MeshMatrixGroup::setIndexedBlending(bool enable, unsigned int vertexBlendMatrixCount) {

	if (enable) {

		if (!mIndexedBlendingInfo.isValid())
			WE_MMemNew(mIndexedBlendingInfo.ptrRef(), IndexedBlendingInfo());

		mIndexedBlendingInfo->vertexBlendMatrixCount = vertexBlendMatrixCount;

	} else {

		mIndexedBlendingInfo.destroy();
	}
}

/*
void MeshMatrixGroup::set(Renderer& renderer, SceneTransformIndex::Type index, MeshInstance* pInstance) {

}
*/

}