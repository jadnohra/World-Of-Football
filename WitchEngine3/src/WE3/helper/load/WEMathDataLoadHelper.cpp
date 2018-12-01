#include "WEMathDataLoadHelper.h"

#include "tchar.h"

namespace WE {

bool MathDataLoadHelper::extract(BufferString& tempString, DataChunk& chunk, Vector2& vector, bool packed, bool allowSingleValue, bool allowNoValue, const TCHAR* attrName, CoordSysConv* pConv, bool convAsVector, bool convToTarget, int* pScannedCount) {

	Vector3 temp;

	if (extract(tempString, chunk, temp, packed, allowSingleValue, allowNoValue, attrName, pConv, convAsVector, convToTarget, pScannedCount)) {

		vector.x = temp.x;
		vector.y = temp.y;

		return true;
	}

	return false;
}


bool MathDataLoadHelper::extractVector3(BufferString& tempString, DataChunk& chunk, float* vector, bool packed, bool allowSingleValue, bool allowNoValue, const TCHAR* attrName, CoordSysConv* pConv, bool convAsVector, bool convToTarget, int* pScannedCount) {

	bool ret;

	int scannedCount = 0;

	if (packed) {

		if (chunk.getAttribute(attrName, tempString) == false) {

			assrt(allowNoValue);
			ret = false;

		} else {

			scannedCount = _stscanf(tempString.c_tstr(), L"%f, %f, %f", &vector[0], &vector[1], &vector[2]);
		}

	} else {


		if (chunk.scanAttribute(tempString, L"x", L"%f", &(vector[0]))) {

			++scannedCount;
		}

		if (chunk.scanAttribute(tempString, L"y", L"%f", &(vector[1]))) {

			++scannedCount;
		}

		if (chunk.scanAttribute(tempString, L"z", L"%f", &(vector[2]))) {

			++scannedCount;
		}
	}

	switch (scannedCount) {
		case 3:
			ret = true;
			break;
		case 2:
			vector[2] = vector[1];
			ret = true;
			break;
		case 1:
			vector[1] = vector[0];
			vector[2] = vector[0];
			ret = true;
			break;
		default:
			ret = false;
			assrt(allowNoValue);
			break;
	}


	if (ret && pConv) {

		if (convToTarget) {

			if (convAsVector) {

				pConv->toTargetVector(vector);

			} else {

				pConv->toTargetPoint(vector);
			}

		} else {

			if (convAsVector) {

				pConv->toSourceVector(vector);

			} else {

				pConv->toSourcePoint(vector);
			}
		}
	}

	if (pScannedCount)
		*pScannedCount = scannedCount;

	return ret;
}

bool MathDataLoadHelper::extract(BufferString& tempString, DataChunk& chunk, Vector3& vector, bool packed, bool allowSingleValue, bool allowNoValue, const TCHAR* attrName, CoordSysConv* pConv, bool convAsVector, bool convToTarget, int* pScannedCount) {

	return extractVector3(tempString, chunk, vector.el, packed, allowSingleValue, allowNoValue, attrName, pConv, convAsVector, convToTarget, pScannedCount);
}

}