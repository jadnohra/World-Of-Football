#include "WEGeometryVertexBuffer.h"

#include "../WEMacros.h"

namespace WE {

GeometryVertexBuffer::GeometryVertexBuffer(const RenderElementProfile* pProfile, RenderUINT elementCount) 
	: GeometryBuffer(pProfile, elementCount) {
}

void GeometryVertexBuffer::applyConverter(CoordSysConv* pConv, const CoordSys* pNewSem) {

	if (pConv) {

		mProfile.vertex_simpleConvertArray(mBuffer.el, mBuffer.el, getElementCount(), pConv);
	}

	if (pNewSem) {

		mCoordSys.set(*pNewSem);
	}
}

void GeometryVertexBuffer::applyTransform(GeometryVertexTransform* pTrans) {

	if (pTrans) {

		mProfile.vertex_simpleConvertArray(mBuffer.el, mBuffer.el, getElementCount(), pTrans);
	}
}

void GeometryVertexBuffer::reanchor(float* anchoringOffset) {

	mProfile.vertex_reanchorArray(mBuffer.el, mBuffer.el, getElementCount(), anchoringOffset);
}

}