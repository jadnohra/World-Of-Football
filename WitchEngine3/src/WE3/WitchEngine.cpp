#include "WitchEngine.h"

namespace WE {

void WitchEngine::getVersion(Version& version) {

	getWitchEngineVersion(version);
}

void WitchEngine::getCoordSys(CoordSys& coordSys) {

	coordSys.el0 = CSD_Right;
	coordSys.el1 = CSD_Up;
	coordSys.el2 = CSD_Forward;
	coordSys.rotationLeftHanded = true;
	coordSys.bitValid = 1;
	coordSys.setFloatUnitsPerMeter(0.0f);
}

}
