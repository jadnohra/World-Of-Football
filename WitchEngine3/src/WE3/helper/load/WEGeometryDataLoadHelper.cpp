#include "WEGeometryDataLoadHelper.h"

#include "WEMathDataLoadHelper.h"
#include "../../WEAssert.h"
#include "../../string/WEStringPart.h"


namespace WE {

bool GeometryDataLoadHelper::extract(BufferString& tempStr, DataChunk& chunk, GeometryAnchorType* anchor) {

	if (chunk.getAttribute(L"anchor", tempStr)) {

		StringPartBase part;
		int index;
		bool found;
		short anchorIndex;
		const TCHAR* buff = tempStr.c_tstr();
		int len = tempStr.length();

		anchorIndex = 0;
		index = 0;
		found = true;
		while (found) {
		
			if (anchorIndex >= 3) {

				break;
			}

			index = tempStr.findNext(L',', index, found, part.startIndex, part.count, false);

			if (part.equals(buff, L"min")) {

				anchor[anchorIndex] = GAT_Min;

			} else if (part.equals(buff, L"center")) {

				anchor[anchorIndex] = GAT_Center;

			} else if (part.equals(buff, L"max")) {

				anchor[anchorIndex] = GAT_Max;
			} else {

				anchor[anchorIndex] = GAT_None;
			}

			++index;
			++anchorIndex;
		}

		if (anchorIndex == 1) {

			while (anchorIndex < 3) {

				anchor[++anchorIndex] = anchor[0];
			}

		}

		while (anchorIndex < 3) {

			anchor[++anchorIndex] = GAT_None;
		}

		return reanchorIsAnchoring(anchor);
	}

	return false;
}


bool GeometryDataLoadHelper::extract(BufferString& tempStr, DataChunk& chunk, GeometryVertexTransform& trans, CoordSysConv* pConv) {

	bool identity = true;
	Vector3 value;

	if (chunk.getAttribute(L"scale", tempStr)) {

		if (MathDataLoadHelper::extract(tempStr, chunk, value, true, true, true, L"scale")) {

			identity = false;

			if (pConv) {

				pConv->toTargetVector(value.el);
			}

			trans.setupScale(value);

		} else {

			assrt(false);
		}

	} else {

		trans.identity();
	}

	if (chunk.getAttribute(L"translate", tempStr)) {

		if (MathDataLoadHelper::extract(tempStr, chunk, value, true, true, true, L"translate")) {

			identity = false;

			if (pConv) {

				pConv->toTargetPoint(value.el);
			}

			trans.setTranslation(value);

		} else {

			assrt(false);
		}
	}

	if (chunk.getAttribute(L"axis", tempStr)) {

		if (MathDataLoadHelper::extract(tempStr, chunk, value, true, true, true, L"axis")) {

			identity = false;

			float angle = 0.0f;

			chunk.scanAttribute(tempStr, L"angle", L"%f", &angle);

			angle = degToRad(angle);

			if (pConv) {

				pConv->toTargetRotation(value.el, angle);
			}

			identity = angle == 0.0f;

			if (!identity) {

				trans.setRotate(value, angle);
			}

		} else {

			assrt(false);
		}
	}

	return identity == false;
}

/*
bool GeometryDataLoadHelper::extract(BufferString& tempString, DataChunk& chunk, GeometryGeometryTransform& trans) {

	trans.hasAnchor = GeometryDataLoadHelper::extract(tempString, chunk, trans.anchor);
	trans.hasTransform = GeometryDataLoadHelper::extract(tempString, chunk, trans.transform);

	return (trans.isEmpty() == false);
}
*/

}