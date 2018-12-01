#include "HoldCycle.h"

#include "math.h"

namespace WE { namespace Input {


float HoldCycle::update(Time cycleInterval, Time currTime) {

	unsigned int holdCycleIndex = (unsigned int) (currTime / cycleInterval);
	bool isUpCycle = holdCycleIndex % 2 == 0;
	unsigned int cycleBaseIndex = isUpCycle ? holdCycleIndex : holdCycleIndex + 1;
	Time cycleBaseTime = (Time) ((unsigned int) (((float) cycleBaseIndex) * cycleInterval));
	Time holdResidual = fabs(currTime - cycleBaseTime);

	this->cycleIndex = (int) holdCycleIndex;
	this->value = holdResidual / cycleInterval;

	return this->value;
}

} }

