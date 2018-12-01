#include "WEGeometryBufferLoader_v1.h"

#include "../helper/load/WECoordSysDataLoadHelper.h"
#include "../WEAssert.h"


namespace WE {

bool GeometryBufferLoader_v1::load(GeometryIndexBuffer& buffer, DataSourceChunk data, bool loadCoordSys) {

	if (data.isNull()) {

		return false;
	}

	if (buffer.getElementCount() != 0) {

		assrt(false);
		return false;
	}

	BufferString tempStr;

	if (loadCoordSys && (CoordSysDataLoadHelper::extract(tempStr, data.pChunk(), buffer.mCoordSys, false) == false)) {

		//no coordSys found
		assrt(false);
		return false;
	}

	RenderUINT faceCount = 0;
		
	if (data->scanAttribute(tempStr, L"faceCount", L"%u", &faceCount) == false) {

		if (data->scanAttribute(tempStr, L"count", L"%u", &faceCount) == false) {

			assrt(false);
			return false;
		}
	} 
	
	if (faceCount == 0) {

		return true;
	}

	RenderElementProfile& profile = buffer.mProfile;

	bool bit32 = true;
	profile.index_set(bit32);

	if (buffer.init(NULL, faceCount * 3) == false) {

		assrt(false);
		return false;
	}

	void* pIB = buffer.bufferPtr();

	DataChunk* pChunk_face = data->getFirstChild();

	UINT32 idx = 0;
	UINT32 buffIndex = 0;
	UINT val[3];
	UINT32 val2[3];

	while(pChunk_face) {

		if (idx >= faceCount) {

			assrt(false);
			break;
		}

		if (pChunk_face->scanAttribute(tempStr, L"v1", L"%u", &val[0]) == false) val[0] = 0;
		if (pChunk_face->scanAttribute(tempStr, L"v2", L"%u", &val[1]) == false) val[1] = 0;
		if (pChunk_face->scanAttribute(tempStr, L"v3", L"%u", &val[2]) == false) val[2] = 0;

		//if (pConverter) pConverter->toSourceFaceIndices(val);

		val2[0] = val[0];
		val2[1] = val[1];
		val2[2] = val[2];

		profile.face_set(pIB, buffIndex, val2);
		
		++idx;
		buffIndex += 3;
		toNextSibling(pChunk_face);
	}

	if (idx != faceCount) {

		assrt(false);
	}

	return true;
}


}