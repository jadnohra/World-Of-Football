#ifndef _WitchEngine_h
#define _WitchEngine_h

#include "WEVersion.h"
#include "coordSys/WECoordSys.h"


namespace WE {


	class WitchEngine {
	public:

		static void getVersion(Version& version);
		static void getCoordSys(CoordSys& coordSys);
	};

}

#endif