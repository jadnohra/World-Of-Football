#include "WEMeshInstance.h"


namespace WE {

bool MeshInstance::init(Mesh& mesh, RenderLoadContext& renderLoadContext) {

	return mesh.initInstance(*this, renderLoadContext);
}

bool MeshInstance::isInited() {

	return mMesh.isValid();
}

/*
bool MeshInstance::hasSkeletonInstance() {

	return mSkelInst.isValid();
}
*/

Mesh* MeshInstance::getMesh() {

	return mMesh.ptr();
}

void MeshInstance::cloneTo(MeshInstance& clone, bool cloneMaterial) {

	clone.mMesh = mMesh.ptr();

	if (cloneMaterial) {

		cloneMaterialInstanceArray(mMaterialInstances, clone.mMaterialInstances);
	}
}

/*
void MeshInstance::useWorldMatrix(Matrix4x3Base* pExternalWorldMatrix) {

	unsetWorldMatrix();

	if (pExternalWorldMatrix != NULL) {

		mWorldMatrix.set(pExternalWorldMatrix, Ptr_SoftRef);

	} else {

		Matrix4x3Base* pOwnMatrix;
		MMemNew(pOwnMatrix, Matrix4x3Base);

		mWorldMatrix.set(pOwnMatrix, Ptr_HardRef);
	}

	
	if (mSkelInst.isValid()) {

		mSkelInst->useWorldMatrix(mWorldMatrix.ptr());
	}
}

void MeshInstance::unsetWorldMatrix() {

	mWorldMatrix.destroy();
}

bool MeshInstance::hasWorldMatrix() {

	return mWorldMatrix.isValid();
}

bool MeshInstance::setWorldMatrix(Matrix4x3Base* pWorldMatrix, bool updateVolumes) {

	if (pWorldMatrix != NULL) {

		mWorldMatrix.dref() = *pWorldMatrix;
	}


	if (mSkelInst.isValid()) {

		//signal matrix change
		mSkelInst->setWorldMatrix(NULL);

		if (updateVolumes) {

			return mSkelInst->update();
		}
	}

	return false;
}

bool MeshInstance::signalExtrernalWorldMatrixChanged(bool updateVolumes) {

	if (mSkelInst.isValid()) {

		//signal matrix change
		mSkelInst->setWorldMatrix(NULL);

		if (updateVolumes) {

			return mSkelInst->update();
		}
	}

	return false;
}

void MeshInstance::nonSkeletalSignalExtrernalWorldMatrixChanged() {
}

bool MeshInstance::skeletalSignalExtrernalWorldMatrixChanged(bool updateVolumes) {

	//signal matrix change
	mSkelInst->setWorldMatrix(NULL);

	if (updateVolumes) {

		return mSkelInst->update();
	}

	return false;
}


bool MeshInstance::createSkeletonInstance(Skeleton& skel) {

	MMemNew(mSkelInst.ptrRef(), SkeletonInstance());
	
	mSkelInst->bind(&skel);
	
	if (mWorldMatrix.isValid()) {

		mSkelInst->useWorldMatrix(mWorldMatrix.ptr());
	}

	return true;
}

SkeletonInstance* MeshInstance::getSkeletonInstance() {

	return mSkelInst.ptr();
}
*/



/*
void MeshInstance::renderExtras(Renderer& renderer, bool renderSkelVolume, bool renderSkelBoneVolumes) {

	if (mSkelInst.isValid()) {

		if (renderSkelVolume) {

			renderer.draw(mSkelInst->getLocalDynamicAAB(), mWorldMatrix.ptr(), &(RenderColor::kGreen));
		}

		if (renderSkelBoneVolumes) {

			mSkelInst->renderBoundingBoxes(renderer);
		}
	}
}
*/

}