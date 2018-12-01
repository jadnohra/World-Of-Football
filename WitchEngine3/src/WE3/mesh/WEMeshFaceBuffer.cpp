#include "WEMeshFaceBuffer.h"

namespace WE {

MeshFaceBuffer::MeshFaceBuffer() {
}

MeshFaceBuffer::~MeshFaceBuffer() {
}

RenderUINT MeshFaceBuffer::getFaceCount() {

	return mFaces.size;
}

bool MeshFaceBuffer::create(RenderUINT faceCount) {

	mFaces.setSize(faceCount);

	MeshFace* faces = mFaces.el;

	for (RenderUINT f = 0, i = 0; f < faceCount; f++, i += 3) {

		faces[f].mBaseIndex = i;
		faces[f].mMaterialIndex = MeshMaterialIndex::Null;
		faces[f].mMatrixGroupIndex = SceneTransformIndex::Null;
	}

	return true;
}


MeshFaceArrayBase& MeshFaceBuffer::arrayBase() {

	return mFaces;
}


void MeshFaceBuffer::batchSetMaterialIndex(MeshMaterialIndex::Type matIndex) {

	for (RenderUINT i = 0; i < mFaces.size; i++) {

		mFaces.el[i].mMaterialIndex = matIndex;
	}
}

void MeshFaceBuffer::batchSetMatrixGroupIndex(MeshMatrixGroupIndex::Type matGroupIndex) {

	for (RenderUINT i = 0; i < mFaces.size; i++) {

		mFaces.el[i].mMatrixGroupIndex = matGroupIndex;
	}
}

}