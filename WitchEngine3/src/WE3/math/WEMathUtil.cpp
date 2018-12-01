#include "WEMathUtil.h"

namespace WE {

float removeMultiples(float theta, float multipleMag) {

	float multiples = (float) ((int) theta / multipleMag);
	return theta - multiples * multipleMag;
}

float remove2PiMultiples(float theta) {

	float multiples = (float) ((int) theta / k2Pi);
	return theta - multiples * k2Pi;
}

float wrapPi(float theta) {

	theta += kPi;
	theta -= floor(theta * k1Over2Pi) * k2Pi;
	theta -= kPi;
	return theta;
}

float toPositiveAngle(float theta) {

	if (theta < 0.0f) {

		theta -= (float) (((int) (theta / k2Pi))) * k2Pi;

		if (theta < 0.0f) {

			theta += k2Pi;
		}
	}

	return theta;
}

void sinCos(float *returnSin, float *returnCos, float theta, bool leftHanded) {

	// For simplicity, we'll just use the normal trig functions.
	// Note that on some platforms we may be able to do better

	if (leftHanded) {
		*returnSin = sin(theta);
		*returnCos = cos(theta);
	} else {
		*returnSin = sin(-theta);
		*returnCos = cos(-theta);
	}
}

}
