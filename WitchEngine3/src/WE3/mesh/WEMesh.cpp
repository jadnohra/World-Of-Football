#include "WEMesh.h"

#include "../WEMacros.h"

#include "WEMeshMatrixGroupHelper.h"
#include "WEMeshInstance.h"

namespace WE {


Mesh::Mesh() {
}

Mesh::~Mesh() {

	destroy(true, true, true, true, true);
}


void Mesh::destroy(bool geometry, bool renderGeometry, bool materialGroup, bool skeleton, bool physicalProxy) {

	mGeometry.conditionalDestroy(geometry);
	mRenderGeometry.conditionalDestroy(renderGeometry);
	mMaterialGroup.conditionalDestroy(materialGroup);
	mSkeleton.conditionalDestroy(skeleton);
	mPhysicalProxy.conditionalDestroy(physicalProxy);
}

bool Mesh::isLoaded() {

	return (hasGeometry() || hasRenderGeometry());
}

bool Mesh::hasMaterialGroup() {

	return (mMaterialGroup.isValid());
}

bool Mesh::createMaterialGroup() {

	if (mMaterialGroup.isValid()) {

		return false;
	}

	MMemNew(mMaterialGroup.ptrRef(), MeshMaterialGroup);
	return true;
}

MeshMaterialGroup& Mesh::getMaterialGroup() {

	return mMaterialGroup.dref();
}

MeshMaterialGroup* Mesh::getMaterialGroupPtr() {

	return mMaterialGroup.ptr();
}


bool Mesh::hasGeometry() {

	return mGeometry.isValid();
}

bool Mesh::createGeometry() {

	if (mGeometry.isValid()) {

		return false;
	}

	MMemNew(mGeometry.ptrRef(), MeshGeometry);
	return true;
}

MeshGeometry& Mesh::getGeometry() {

	return mGeometry.dref();
}

bool Mesh::prepareForRendering(RenderLoadContext& renderLoadContext) {

	if (hasRenderGeometry() == false) {

		return createRenderGeometry(renderLoadContext);
	}

	return true;
}

bool Mesh::hasRenderGeometry() {

	return mRenderGeometry.isValid();
}

bool Mesh::createRenderGeometry(RenderLoadContext& renderLoadContext) {

	if (hasRenderGeometry() || (hasGeometry() == false)) {

		assrt(false);
		return false;
	}

	MMemNew(mRenderGeometry.ptrRef(), MeshRenderGeometry());
	
	if (!mRenderGeometry->create(renderLoadContext, getGeometry(), mMaterialGroup.ptr())) {

		mRenderGeometry.destroy();
		return false;
	}

	if (mSkeleton.isValid()) {

		mSkeleton->prepareForRenderer(renderLoadContext.renderer(), renderLoadContext.convPool());
	}

	return true;
}

MeshRenderGeometry& Mesh::getRenderGeometry() {

	return mRenderGeometry.dref();
}

bool Mesh::hasSkeleton() {

	return mSkeleton.isValid();
}

bool Mesh::createSkeleton() {

	if (hasSkeleton()) {

		assrt(false);
		return false;
	}

	MMemNew(mSkeleton.ptrRef(), Skeleton());

	return true;
}

Skeleton& Mesh::getSkeleton() {

	return mSkeleton.dref();
}


bool Mesh::hasPhysicalProxy() {

	return mPhysicalProxy.isValid();
}

bool Mesh::createPhysicalProxy() {

	if (hasPhysicalProxy()) {

		assrt(false);
		return false;
	}

	MMemNew(mPhysicalProxy.ptrRef(), MeshPhysicalProxy());

	return true;
}

MeshPhysicalProxy& Mesh::getPhysicalProxy() {

	return mPhysicalProxy.dref();
}


bool Mesh::initInstance(MeshInstance& inst, RenderLoadContext& renderLoadContext) {

	if (hasRenderGeometry() == false) {

		return true;
	}

	if (inst.mMesh.isValid()) {

		assrt(false);
		return false;
	}
	
	inst.mMesh = this;

	if (mMaterialGroup.isValid()) {

		mMaterialGroup->createInstanceMaterials(renderLoadContext, inst);
	}

	/*
	if (mSkeleton.isValid()) { 
		
		inst.createSkeletonInstance(mSkeleton.dref());
	}
	*/

	return true;
}

void Mesh::render(Renderer& renderer, MeshInstance& instance, const Matrix4x3Base& worldMatrix, RenderPassEnum pass) {

	mRenderGeometry->render(renderer, mMaterialGroup.ptr(), &(instance.mMaterialInstances), &worldMatrix, pass);
}

void Mesh::render(Renderer& renderer, MeshInstance& instance, SkeletonInstance& skelInst, RenderPassEnum pass) {

	mRenderGeometry->render(renderer, mMaterialGroup.ptr(), &(instance.mMaterialInstances), skelInst.getWorldTransformArray().el, pass);
}


}