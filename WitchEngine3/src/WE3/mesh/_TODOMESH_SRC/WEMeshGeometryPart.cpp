#include "WEMeshGeometryPart.h"

#include "../WEMacros.h"
#include "WEMeshMatrixGroupHelper.h"

namespace WE {


MeshGeometryPart::MeshGeometryPart() 
	: mpVB(NULL), mpIB(NULL), mpFB(NULL) {
}

MeshGeometryPart::~MeshGeometryPart() {

	destroy(true, true);
}

void MeshGeometryPart::destroy(bool geomBuffers, bool faceBuffer) {

	if (geomBuffers) {

		MMemDelete(mpIB);
		MMemDelete(mpVB);

		faceBuffer = true;
	}

	if (faceBuffer) {

		MMemDelete(mpFB);
	}
}

MeshGeometryVertexBuffer* MeshGeometryPart::createVB() {

	if (mpVB) {

		assert(false);
		return NULL;
	}

	MMemNew(mpVB, MeshGeometryVertexBuffer());
	
	return mpVB;
}

MeshGeometryIndexBuffer* MeshGeometryPart::createIB() {

	if (mpIB) {

		assert(false);
		return NULL;
	}

	MMemNew(mpIB, MeshGeometryIndexBuffer());
	
	return mpIB;
}


void* MeshGeometryPart::createVB(const RenderElementProfile& profile, RenderUINT elementCount) {

	if (mpVB) {

		assert(false);
		return NULL;
	}

	MMemNew(mpVB, MeshGeometryVertexBuffer(&profile, elementCount));
	
	return mpVB->bufferPtr();
}

void* MeshGeometryPart::createIB(const RenderElementProfile& profile, RenderUINT elementCount) {

	if (mpIB) {

		assert(false);
		return NULL;
	}

	MMemNew(mpIB, MeshGeometryIndexBuffer(&profile, elementCount));
	
	return mpIB->bufferPtr();
}

bool MeshGeometryPart::hasVB() {

	return (mpVB != NULL);
}

bool MeshGeometryPart::hasIB() {

	return (mpIB != NULL);
}

bool MeshGeometryPart::hasFB() {

	return (mpFB != NULL);
}


void* MeshGeometryPart::getVBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount) {

	return NULL;
	//return mpVB->bufferPtr(pProfile, elementCount);
}

void* MeshGeometryPart::getIBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount) {

	return NULL;
	//return mpIB->bufferPtr(pProfile, elementCount);
}

void MeshGeometryPart::createFB() {

	if ((mpFB != NULL) || (mpIB == NULL)) {

		assert(false);
		return;
	}

	RenderUINT faceCount = mpIB->getElementCount() / 3;

	MMemNew(mpFB, StaticMeshFaceArray());
	
	mpFB->setSize(faceCount);

	MeshFace* faces = mpFB->el;

	for (RenderUINT f = 0, i = 0; f < faceCount; f++, i += 3) {

		faces[f].mBaseIndex = i;
		faces[f].mMaterialIndex = MeshMaterialIndex::Null;
		faces[f].mMatrixGroupIndex = MeshMatrixIndex::Null;
	}
}




}