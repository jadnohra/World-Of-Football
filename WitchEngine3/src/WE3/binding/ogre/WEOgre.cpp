#include "WEOgre.h"

namespace WE {


void Ogre::getCoordSys(CoordSys& coordSys) {

	coordSys.el0 = CSD_Left;
	coordSys.el1 = CSD_Up;
	coordSys.el2 = CSD_Forward;
	coordSys.rotationLeftHanded = false;
	coordSys.bitValid = 1;
	coordSys.setFloatUnitsPerMeter(0.0f);
}


}