#ifndef _WEMathDataLoadHelper_h
#define _WEMathDataLoadHelper_h

#include "../../string/WEBufferString.h"
#include "../../data/WEDataSource.h"
#include "../../coordSys/WECoordSysConv.h"

#include "../../math/WEVector.h"
#include "../../math/WEVector2.h"

namespace WE {

	class MathDataLoadHelper {
	public:

		//packed format: value="x,y,z"
		//unpacked format: x="" y="" z=""
		static bool extractVector3(BufferString& tempString, DataChunk& chunk, float* vector, bool packed = true, bool allowSingleValue = true, bool allowNoValue = false, const TCHAR* attrName = L"value", CoordSysConv* pConv = NULL, bool convAsVector = true, bool convToTarget = true, int* pScannedCount = NULL);
		static bool extract(BufferString& tempString, DataChunk& chunk, Vector3& vector, bool packed = true, bool allowSingleValue = true, bool allowNoValue = false, const TCHAR* attrName = L"value", CoordSysConv* pConv = NULL, bool convAsVector = true, bool convToTarget = true, int* pScannedCount = NULL);

		static bool extract(BufferString& tempString, DataChunk& chunk, Vector2& vector, bool packed = true, bool allowSingleValue = true, bool allowNoValue = false, const TCHAR* attrName = L"value", CoordSysConv* pConv = NULL, bool convAsVector = true, bool convToTarget = true, int* pScannedCount = NULL);

	};
}

#endif