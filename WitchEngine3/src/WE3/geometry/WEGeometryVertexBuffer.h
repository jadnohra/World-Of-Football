#ifndef _WEGeometryVertexBuffer_h
#define _WEGeometryVertexBuffer_h

#include "WEGeometryBuffer.h"
#include "WEGeometryTransform.h"

namespace WE {

	class GeometryVertexBuffer : public GeometryBuffer {
	public:

		GeometryVertexBuffer(const RenderElementProfile* pProfile = NULL, RenderUINT elementCount = 0);

		void applyConverter(CoordSysConv* pConv, const CoordSys* pNewSem);
		void applyTransform(GeometryVertexTransform* pTrans);
		void reanchor(float* anchoringOffset);
	};

}

#endif