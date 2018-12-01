#include "WETLResize.h"

#include "../WEMacros.h"

namespace WETL {

unsigned long ResizeDouble::resize(unsigned long curr, unsigned long minNew) {

	unsigned long temp = 2 * curr;
	return MMax(minNew, temp);
}

unsigned long ResizeExact::resize(unsigned long curr, unsigned long minNew) {

	return minNew;
}

}