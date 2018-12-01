#include "WEGeometryBuffer.h"

namespace WE {

GeometryBuffer::GeometryBuffer() {
}

GeometryBuffer::GeometryBuffer(const RenderElementProfile* pProfile, RenderUINT elementCount) {

	init(pProfile, elementCount);
}

GeometryBuffer::GeometryBuffer(const GeometryBuffer& ref, bool copyData) {

	ref.cloneTo(*this, copyData);
}

bool GeometryBuffer::init(const RenderElementProfile* pProfile, RenderUINT elementCount) {

	if (pProfile) {

		mProfile.set(*pProfile);
	} 
	
	mBuffer.setStride(mProfile.elementSizeBytes(), false);
	mBuffer.setElementCount(elementCount);

	return true;
}

bool GeometryBuffer::signalProfileChanged(bool resizeBuffer) {

	if (resizeBuffer) {

		mBuffer.setStride(mProfile.elementSizeBytes(), true);

	} else {

		mBuffer.setStride(mProfile.elementSizeBytes(), false);
	}

	return true;
}

RenderUINT GeometryBuffer::getStride() {

	return mBuffer.getStride();
}

const CoordSys& GeometryBuffer::getCoordSys() {

	return mCoordSys;
}

const RenderElementProfile& GeometryBuffer::getProfile() {

	return mProfile;
}

RenderUINT GeometryBuffer::getBufferSizeBytes() const {

	return mBuffer.size;
}

void* GeometryBuffer::setElementCount(RenderUINT elementCount) {

	if ((elementCount != 0) && (mBuffer.getStride() == 0)) {

		assrt(false);
	}

	mBuffer.setElementCount(elementCount);

	return mBuffer.el;
}

GeometryBuffer::~GeometryBuffer() {
}

void GeometryBuffer::destroy() {

	mBuffer.destroy();
}

RenderUINT GeometryBuffer::getElementCount() const {

	return mBuffer.elementCount();
}

void* GeometryBuffer::bufferPtr() {

	return mBuffer.el;
}

/*
void* GeometryBuffer::bufferPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount) {

	pProfile = &mProfile;
	elementCount = mBuffer.size);

	return mBuffer.el;
}
*/


void GeometryBuffer::cloneTo(GeometryBuffer& buffer, bool copyData) const {

	buffer.init(&mProfile, getElementCount());

	if (copyData) {

		copyDataTo(buffer.mBuffer.el);
	}
}

void GeometryBuffer::copyDataTo(void* pBuff) const {

	memcpy(pBuff, mBuffer.el, mBuffer.size);
}


}