#include "WEMesh.h"

#include "../WEMacros.h"
#include "WEMeshMatrixGroupHelper.h"

namespace WE {


Mesh::Mesh() 
	: mpVB(NULL), mpIB(NULL), mpFB(NULL) {
}

Mesh::~Mesh() {

	destroy(true, true, true);
}

void Mesh::destroy(bool sourceBuffers, bool faceBuffer, bool renderBuffers) {

	if (sourceBuffers) {

		MMemDelete(mpIB);
		MMemDelete(mpVB);

		faceBuffer = true;
	}

	if (faceBuffer) {

		MMemDelete(mpFB);
	}

	if (renderBuffers) {
	}

}


void* Mesh::createVB(const RenderElementProfile& profile, RenderUINT elementCount) {

	if (mpVB) {

		assert(false);
		return NULL;
	}

	MMemNew(mpVB, MeshGeometryVertexBuffer(&profile, elementCount));
	
	return mpVB->bufferPtr();
}

void* Mesh::createIB(const RenderElementProfile& profile, RenderUINT elementCount) {

	if (mpIB) {

		assert(false);
		return NULL;
	}

	MMemNew(mpIB, MeshGeometryIndexBuffer(&profile, elementCount));
	
	return mpIB->bufferPtr();
}

bool Mesh::hasVB() {

	return (mpVB != NULL);
}

bool Mesh::hasIB() {

	return (mpIB != NULL);
}

bool Mesh::hasFB() {

	return (mpFB != NULL);
}

bool Mesh::isRenderReady() {

	return false;
}

void* Mesh::getVBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount) {

	return NULL;
	//return mpVB->bufferPtr(pProfile, elementCount);
}

void* Mesh::getIBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount) {

	return NULL;
	//return mpIB->bufferPtr(pProfile, elementCount);
}

void Mesh::createFB() {

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



bool Mesh::prepareForRendering(Renderer& renderer, CoordSysConvPool& convFactory) {

	if (isRenderReady() || ((mpVB == false) && (mpIB == false))) {

		assert(false);
		//TODO create empty buffers
		return true;
	}

	if (mpFB == NULL) {

		assert(false);
		return false;
	}

	Ptr<MeshGeometryVertexBuffer> renderReadyVB;
	Ptr<MeshGeometryIndexBuffer> renderReadyIB;

	if (MeshMatrixGroupHelper::getRenderReadyBuffers(renderer, convFactory, mGeomSemantics, *mpVB, *mpIB, *mpFB, renderReadyVB, renderReadyIB, mRenderMatrixGroups) == false) {

		assert(false);
		return false;
	}

		
	

	return false;
}


}