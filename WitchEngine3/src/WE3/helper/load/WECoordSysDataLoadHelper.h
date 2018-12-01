#ifndef _WECoordSysDataLoadHelper_h
#define _WECoordSysDataLoadHelper_h

#include "../../string/WEBufferString.h"
#include "../../data/WEDataSource.h"

#include "../../coordSys/WECoordSys.h"

namespace WE {

	bool fromString(BufferString& typeString, CoordSys& coordSys);

	class CoordSysDataLoadHelper {
	public:

		static bool extract(BufferString& tempString, DataChunk* pChunk, CoordSys& coordSys, bool allowUnitsPerMeter, bool allowParentInherit = true);

	protected:

		static bool extract(BufferString& tempString, DataChunk& chunk, CoordSys& coordSys, bool allowUnitsPerMeter, bool* pReserved = NULL);
	};

	
}

#endif