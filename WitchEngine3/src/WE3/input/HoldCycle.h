#ifndef _HoldCycle_h_WE_Input
#define _HoldCycle_h_WE_Input

#include "../WETime.h"

namespace WE { namespace Input {


	struct HoldCycle {
	
		int cycleIndex;
		float value;

		HoldCycle() : cycleIndex(0), value(0.0f) {}

		float update(Time cycleInterval, Time currTime);
	};

} }

#endif