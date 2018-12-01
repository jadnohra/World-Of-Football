#ifndef _WEGeometryDataLoadHelper_h
#define _WEGeometryDataLoadHelper_h

#include "../../string/WEBufferString.h"
#include "../../data/WEDataSource.h"
#include "../../coordSys/WECoordSysConv.h"

#include "../../geometry/WEGeometryTransform.h"
#include "../../geometry/WEGeometryAnchor.h"
//#include "../../mesh/WEGeometryGeometryTransform.h"

namespace WE {

	class GeometryDataLoadHelper {
	public:

		static bool extract(BufferString& tempString, DataChunk& chunk, GeometryAnchorType* anchor);
		static bool extract(BufferString& tempStr, DataChunk& chunk, GeometryVertexTransform& trans, CoordSysConv* pConv);
		//static bool extract(BufferString& tempString, DataChunk& chunk, GeometryGeometryTransform& trans);
	};
}

#endif