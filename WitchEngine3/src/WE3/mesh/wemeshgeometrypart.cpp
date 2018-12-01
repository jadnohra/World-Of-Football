#include "WEMeshGeometryPart.h"

#include "../WEMacros.h"
#include "WEMeshMatrixGroupHelper.h"

namespace WE {


MeshGeometryPart::MeshGeometryPart() {
}

MeshGeometryPart::~MeshGeometryPart() {

	destroy(true, true);
}

void MeshGeometryPart::destroy(bool geomBuffers, bool faceBuffer) {

	if (geomBuffers) {

		mIB.destroy();
		mVB.destroy();

		faceBuffer = true;
	}

	if (faceBuffer) {

		mFB.destroy();
	}
}

bool MeshGeometryPart::createVB() {

	if (mVB.isValid()) {

		assrt(false);
		return false;
	}

	MMemNew(mVB.ptrRef(), MeshGeometryVertexBuffer());
	
	return mVB.isValid();
}

bool MeshGeometryPart::createIB() {

	if (mIB.isValid()) {

		assrt(false);
		return false;
	}

	MMemNew(mIB.ptrRef(), MeshGeometryIndexBuffer());
	
	return mIB.isValid();
}

/*
void* MeshGeometryPart::createVB(const RenderElementProfile& profile, RenderUINT elementCount) {

	if (mVB.isValid()) {

		assrt(false);
		return NULL;
	}

	MMemNew(mVB.ptrRef(), MeshGeometryVertexBuffer(&profile, elementCount));
	
	return mVB->bufferPtr();
}

void* MeshGeometryPart::createIB(const RenderElementProfile& profile, RenderUINT elementCount) {

	if (mIB.isValid()) {

		assrt(false);
		return NULL;
	}

	MMemNew(mIB.ptrRef(), MeshGeometryIndexBuffer(&profile, elementCount));
	
	return mIB->bufferPtr();
}
*/

bool MeshGeometryPart::hasVB() {

	return (mVB.isValid());
}

bool MeshGeometryPart::hasIB() {

	return (mIB.isValid());
}


bool MeshGeometryPart::hasFB() {

	return (mFB.isValid());
}

MeshGeometryVertexBuffer& MeshGeometryPart::getVB() {

	return mVB.dref();
}

MeshGeometryIndexBuffer& MeshGeometryPart::getIB() {

	return mIB.dref();
}

MeshFaceBuffer& MeshGeometryPart::getFB() {

	return mFB.dref();
}


MeshGeometryVertexBuffer* MeshGeometryPart::getVBPtr() {

	return mVB.ptr();
}

MeshGeometryIndexBuffer* MeshGeometryPart::getIBPtr() {

	return mIB.ptr();
}

MeshFaceBuffer* MeshGeometryPart::getFBPtr() {

	return mFB.ptr();
}

/*
void* MeshGeometryPart::getVBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount) {

	return NULL;
	//return mpVB->bufferPtr(pProfile, elementCount);
}

void* MeshGeometryPart::getIBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount) {

	return NULL;
	//return mpIB->bufferPtr(pProfile, elementCount);
}
*/


bool MeshGeometryPart::createFB() {

	if (mFB.isValid()) {

		assrt(false);
		return false;
	}

	if (mIB.isNull()) {

		assrt(false);
		return false;
	}

	RenderUINT faceCount = mIB->getElementCount() / 3;

	MMemNew(mFB.ptrRef(), MeshFaceBuffer());
	mFB->create(faceCount);
	
	return mFB.isValid();
}


void MeshGeometryPart::applyCoordSys(CoordSysConvPool& convPool, const CoordSys& coordSys) {

	SoftRef<CoordSysConv> conv;

	if (mVB.isValid()) {
		
		conv = convPool.getConverterFor(mVB->getCoordSys(), coordSys, false);
		mVB->applyConverter(conv.ptr(), &coordSys);
	}

	if (mIB.isValid()) {
		
		conv = convPool.getConverterFor(mIB->getCoordSys(), coordSys, false);
		mIB->applyConverter(conv.ptr(), &coordSys);
	}
}

void MeshGeometryPart::applyTransform(GeometryVertexTransform* pTrans) {

	if (mVB.isValid()) {
		
		mVB->applyTransform(pTrans);
	}
}

void MeshGeometryPart::reanchor(float* anchoringOffset) {

	if (mVB.isValid()) {
		
		mVB->reanchor(anchoringOffset);
	}
}

}