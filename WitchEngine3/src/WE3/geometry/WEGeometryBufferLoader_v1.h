#ifndef _WEGeometryLoader_v1_h
#define _WEGeometryLoader_v1_h

#include "WEGeometryIndexBuffer.h"
#include "WEGeometryVertexBuffer.h"

#include "../data/WEDataSourceChunk.h"

namespace WE {



	class GeometryBufferLoader_v1 {
	public:

		static bool load(GeometryIndexBuffer& buffer, DataSourceChunk chunk, bool loadCoordSys);
		static bool load(GeometryVertexBuffer& buffer, DataSourceChunk chunk, bool loadCoordSys);
		//static bool load(GeometryBuffer& buffer, DataSourceChunk& chunk, RenderBufferEnum* pCheckType = NULL);

	protected:

		//static bool determinBufferType(DataSourceChunk& chunk, RenderBufferEnum& type);
	};
}

#endif