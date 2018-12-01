#include "WEDirect3D.h"

namespace WE {


void Direct3D::getCoordSys(CoordSys& coordSys) {

	coordSys.setValid(true);

	coordSys.el0 = CSD_Right;
	coordSys.el1 = CSD_Up;
	coordSys.el2 = CSD_Forward;
	coordSys.rotationLeftHanded = true;
	coordSys.bitValid = 1;
	coordSys.setFloatUnitsPerMeter(0.0f);
}


}