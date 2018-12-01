#include "WEMeshMatrixGroup.h"

namespace WE {


MeshMatrixGroup::MeshMatrixGroup() {

	mPart.zero();
}

MeshMatrixGroup::~MeshMatrixGroup() {
}

void MeshMatrixGroup::setMatrixCount(MeshMatrixIndex count) {

	mMatrixIndexArray.setSize(count);
}

void MeshMatrixGroup::setMatrixIndex(MeshMatrixIndex index, MeshMatrixIndex value) {

	mMatrixIndexArray.el[index] = value;
}

void MeshMatrixGroup::setPart(RenderUINT startIndex, RenderUINT primCount) {

	mPart.startIndex = startIndex;
	mPart.primCount = primCount;
}

}