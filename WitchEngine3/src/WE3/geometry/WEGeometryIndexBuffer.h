#ifndef _WEGeometryIndexBuffer_h
#define _WEGeometryIndexBuffer_h

#include "WEGeometryBuffer.h"

namespace WE {

	class GeometryIndexBuffer : public GeometryBuffer {
	public:

		GeometryIndexBuffer(const RenderElementProfile* pProfile = NULL, RenderUINT elementCount = 0);

		bool isBit32();

		RenderUINT extractMaxIndex();
		void optimizeIndexData(RenderUINT maxIndex);

		void applyConverter(CoordSysConv* pConv, const CoordSys* pNewSem);

		static void initMinMaxIndex(UINT32& min, UINT32& max);
		static void updateMinMaxIndex(UINT32* indices, UINT32& min, UINT32& max);
	};

}

#endif