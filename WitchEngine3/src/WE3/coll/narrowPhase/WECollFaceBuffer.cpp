#include "WECollFaceBuffer.h"

namespace WE {

CollFaceBuffer::CollFaceBuffer() {
}

CollFaceBuffer::~CollFaceBuffer() {
}

void CollFaceBuffer::createFB(CollPrimIndex faceCount) {

	mFB.resize(faceCount);
}

void CollFaceBuffer::createVB(CollPrimIndex vertexCount) {

	mVB.resize(vertexCount);
}

CollPrimIndex CollFaceBuffer::getFaceCount() {

	return mFB.size;
}

CollFace* CollFaceBuffer::faceArrayPtr() {

	return mFB.el;
}

CollPrimIndex CollFaceBuffer::getVertexCount() {

	return mVB.size;
}

CollVertex* CollFaceBuffer::vertexArrayPtr() {

	return mVB.el;
}

void CollFaceBuffer::transform(const CollFaceTransform& transf) {

	for (CollPrimIndex i = 0; i < mVB.size; i++) {

		collTransform(transf, mVB.el[i]);
	}

	for (CollPrimIndex i = 0; i < mFB.size; i++) {

		collTransform(transf, mFB.el[i]);
	}
}

}