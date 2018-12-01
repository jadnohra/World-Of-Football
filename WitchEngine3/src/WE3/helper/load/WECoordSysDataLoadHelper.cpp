#include "WECoordSysDataLoadHelper.h"

#include "../../WitchEngine.h"
#include "../../binding/ogre/WEOgre.h"
#include "../../binding/directx/WEDirect3D.h"


namespace WE {


bool fromString(String& typeString, CoordSys& coordSys) {

	coordSys.setValid(true);

	if (typeString.equalsIgnoreCase(L"witchengine")) {

		WitchEngine::getCoordSys(coordSys);
		return true;

	} else if (typeString.equalsIgnoreCase(L"direct3d")) {

		Direct3D::getCoordSys(coordSys);
		return true;

	} else if (typeString.equalsIgnoreCase(L"ogre")) {

		Ogre::getCoordSys(coordSys);
		return true;

	} else if (typeString.equalsIgnoreCase(L"we")) {

		WitchEngine::getCoordSys(coordSys);
		return true;

	} else if (typeString.equalsIgnoreCase(L"d3d")) {

		Direct3D::getCoordSys(coordSys);
		return true;
	} 

	log(L"Invalid coordSys [%s]", typeString.c_tstr());
	coordSys.setValid(false);

	return false;
}

bool CoordSysDataLoadHelper::extract(BufferString& tempString, DataChunk& chunk, CoordSys& coordSys, bool allowUnitsPerMeter, bool* pAbort) {

	bool ret = false;

	if (pAbort != NULL) *pAbort = false;
	

	if (chunk.getAttribute(L"coordSys", tempString)) {

		if (fromString(tempString, coordSys) == false) {

			if (pAbort != NULL) *pAbort = true;
			ret = false;

		} else {

			ret = true;
		}
	} 

	if (allowUnitsPerMeter && coordSys.hasUnitsPerMeter() == false) {

		float unitsPerMeter;
		
		if (chunk.scanAttribute(tempString, L"unitsPerMeter", L"%f", &unitsPerMeter)) {

			if (coordSys.setFloatUnitsPerMeter(unitsPerMeter) == false) {

				if (pAbort != NULL) *pAbort = true;
				
				return false;
			}
		}

	}
	
	return ret;
}

bool CoordSysDataLoadHelper::extract(BufferString& tempString, DataChunk* pChunk, CoordSys& coordSys, bool allowUnitsPerMeter, bool allowParentInherit) {

	if (pChunk == NULL) {

		return false;
	}

	bool abort;
	coordSys.unitsPerMeter = CSU_None;

	RefWrap<DataChunk> walkUp;
	
	walkUp.set(pChunk, true);

	while (walkUp.isValid()) {

		if (extract(tempString, walkUp.dref(), coordSys, allowUnitsPerMeter, &abort)) {

			return true;

		} else {

			if (abort) {

				return false;
			}
		}

		if ((allowParentInherit == false) && (walkUp.ptr() == pChunk)) {

			return false;
		}

		toParent(walkUp);
	}

	return false;
}


}