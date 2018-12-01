#include "WESceneDataLoadHelper.h"

#include "WEMathDataLoadHelper.h"
#include "../../WEAssert.h"
#include "../../math/WEMathUtil.h"


namespace WE {

bool SceneDataLoadHelper::extract(BufferString& tempStr, DataChunk& chunk, SceneTransformation& trans) {

	if (MathDataLoadHelper::extract(tempStr, chunk, trans.trans, true, true, true, L"pos") == false) {

		trans.trans.zero();
	}

	if (MathDataLoadHelper::extract(tempStr, chunk, trans.rotaxis, true, true, true, L"axis")) {

		if (chunk.scanAttribute(tempStr, L"angle", L"%f", &trans.rot) == 0) {

			trans.rot = 0.0f;
		}	

	} else {

		trans.rotaxis.zero();
		trans.rot = 0.0f;
	}

	
	
	if (trans.rot != 0.0f) {

		trans.rot = degToRad(trans.rot);

		if (trans.rotaxis.isZero()) {

			return false;
		}
	}

	return true;
}

}