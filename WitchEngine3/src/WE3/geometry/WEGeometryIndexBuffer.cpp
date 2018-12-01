#include "WEGeometryIndexBuffer.h"

#include "../WEMacros.h"

namespace WE {

GeometryIndexBuffer::GeometryIndexBuffer(const RenderElementProfile* pProfile, RenderUINT elementCount) 
	: GeometryBuffer(pProfile, elementCount) {
}

void GeometryIndexBuffer::initMinMaxIndex(UINT32& min, UINT32& max) {

	min = UINT32_MAX;
	max = 0;
}

void GeometryIndexBuffer::updateMinMaxIndex(UINT32* indices, UINT32& min, UINT32& max) {

	min = MMin(indices[0], min);
	min = MMin(indices[1], min);
	min = MMin(indices[2], min);

	max = MMax(indices[0], max);
	max = MMax(indices[1], max);
	max = MMax(indices[2], max);
}

RenderUINT GeometryIndexBuffer::extractMaxIndex() {
	
	RenderUINT maxIndex = 0;
	
	RenderUINT elCount = getElementCount();
	void* pIB = bufferPtr();
	RenderUINT currIndex;

	for (RenderUINT i = 0; i < elCount; ++i) {

		currIndex = mProfile.index_get(pIB, i);
		maxIndex = MMax(currIndex, maxIndex);
	}

	return maxIndex;
}

bool GeometryIndexBuffer::isBit32() {

	return mProfile.index_is32bit();
}

void GeometryIndexBuffer::optimizeIndexData(RenderUINT maxIndex) {

	RenderUINT elementCount = getElementCount();

	if (elementCount == 0) {

		mProfile.index_set(false);
		return;
	}

	RenderUINT newElementSize;
	void* reserved;

	if (mProfile.index_isOptimizable(maxIndex, newElementSize, reserved)) {

		Buffer temp;
		mBuffer.memCloneTo(temp);

		mProfile.index_setOptimized(reserved);
		signalProfileChanged(true);
		
		mProfile.index_optimizeIndexDataType(elementCount, maxIndex, temp.el, bufferPtr(), reserved);
	} 
}


void GeometryIndexBuffer::applyConverter(CoordSysConv* pConv, const CoordSys* pNewSem) {

	if (pConv) {

		mProfile.face_simpleConvertArray(mBuffer.el, mBuffer.el, getElementCount(), pConv);
	}

	if (pNewSem) {

		mCoordSys.set(*pNewSem);
	}
}


}