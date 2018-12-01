#ifndef _WEMathUtil_h
#define _WEMathUtil_h

#include "math.h"
#include "rng/rng.h"
#include "../WEAssert.h"

namespace WE {

	const float kPi = 3.14159265358979323846f;
	const float k2Pi = kPi * 2.0f;
	const float kPiOver2 = kPi / 2.0f;
	const float k1OverPi = 1.0f / kPi;
	const float k1Over2Pi = 1.0f / k2Pi;
	const float kPiOver180 = kPi / 180.0f;
	const float k180OverPi = 180.0f / kPi;
	const float kPi2 = pow(kPi, 2.0f);

	const float kPif = 3.14159265f;
	const float k2Pif = kPif * 2.0f;
	const float kPiOver2f = kPif / 2.0f;
	const float k1OverPif = 1.0f / kPif;
	const float k1Over2Pif = 1.0f / k2Pif;
	const float kPiOver180f = kPif / 180.0f;
	const float k180OverPif = 180.0f / kPif;
	const float kPi2f = pow(kPif, 2.0f);

	template<class T>
	T normalDist(T mu, T sigma, int n = 8) {

		if (!sigma)
			return mu;

		assrt( n > 0 );

		T sum = 0.0;
		for( int i = 0; i < n; i++ )
			sum += trand<T>(1.0f);

		// RandomDouble gives us a uniformly distributed number between 0 and 1.
		// This value has mu=0.5 and var=1/12. For the sum, this is mu=n*0.5 and
		// var=n/12.

		sum -= ( (T)n / (T) 2.0 );						// Go to N(0, 1/12n)
		sum *= ( sigma / ( sqrt((T)n / (T) 12.0 )) );	// Go to N(0, var)
		sum += mu;										// Go to N(mu, var)

		return sum;
	}


	inline float safeAcos(float x) {

		if (x <= -1.0f) {
			return kPi;
		}
		if (x >= 1.0f) {
			return 0.0f;
		}

		return acos(x);
	}


	template<class T>
	inline T lerp(T min, T max, float factor) {
		return (T) (min + (T) (((float) (max - min)) * factor));
	}

	inline float lerp(float min, float max, float factor) {
		return lerp<float>(min, max, factor);
	}

	inline int lerp(int min, int max, float factor) {
		return lerp<int>(min, max, factor);
	}

	float wrapPi(float theta);
	float toPositiveAngle(float theta);
	float remove2PiMultiples(float theta);
	float removeMultiples(float theta, float multipleMag);

	inline float degToRad(float deg) { return deg * kPiOver180; }
	inline float radToDeg(float rad) { return rad * k180OverPi; }

	const float k1DegreeInRad = degToRad(1.0f);

	template<class T>
	void sameSignCombine(T& ref, const T& add) {

		if (ref == 0.0f) {

			ref = add;

		} else if (ref > 0.0f) {

			if (add > ref)
				ref = add;

		} else {

			if (add < ref)
				ref = add;
		}
	}

	struct SameSignCombineState {

		bool blocked;

		SameSignCombineState() : blocked(false) {}
	};

	template<class T>
	void sameSignCombine(T& ref, const T& add, SameSignCombineState& state) {

		if (state.blocked)
			return;

		if (ref == 0.0f) {

			ref = add;

		} else if (ref > 0.0f) {

			if (add > ref) {

				ref = add;

			} else if (add < 0.0f) {

				state.blocked = true;
				ref = 0.0f;
			}
				

		} else {

			if (add < ref) {

				ref = add;

			} else if (add > 0.0f) {

				state.blocked = true;
				ref = 0.0f;
			}
		}
	}

	void sinCos(float *returnSin, float *returnCos, float theta, bool leftHanded);
}

#endif