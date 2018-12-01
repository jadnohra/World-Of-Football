#ifndef _Component_Dir2D_h_WE_Input
#define _Component_Dir2D_h_WE_Input

#include "../WETime.h"
#include "../math/WEResponseCurve.h"
#include "../WELog.h"
#include "../WEMacros.h"

namespace WE { namespace Input {


	class Component_Dir2D {
	public:

		virtual float getValue1() = 0;
		virtual float getValue2() = 0;

		virtual float getRawValue1() = 0;
		virtual float getRawValue2() = 0;

		virtual ResponseCurve& createResponseCurve1() = 0;
		virtual ResponseCurve& createResponseCurve2() = 0;

		virtual void destroyResponseCurve1() = 0;
		virtual void destroyResponseCurve2() = 0;

		virtual ResponseCurve* getResponseCurve1() = 0;
		virtual ResponseCurve* getResponseCurve2() = 0;

		virtual void onResponseCurvesChanged() = 0;

		void setRawValuesAsDeadZones(bool keepMax, bool unite) {

			SoftPtr<ResponseCurve> curve;

			float unitedDeadZone = 0.0f;

			if (unite) {

				unitedDeadZone = tmax(fabs(getRawValue1()), fabs(getRawValue2()));

				if (keepMax) {

					curve = getResponseCurve1();

					if (curve.isValid()) {

						unitedDeadZone = tmax(curve->getDeadZone(), unitedDeadZone);
					}

					curve = getResponseCurve2();

					if (curve.isValid()) {

						unitedDeadZone = tmax(curve->getDeadZone(), unitedDeadZone);
					}
				}
			}

			curve = getResponseCurve1();

			if (curve.isValid()) {

				float deadZone;

				if (unite) {

					deadZone = unitedDeadZone;

				} else {

					float currValue = fabs(getRawValue1());
					deadZone = keepMax ? tmax(curve->getDeadZone(), currValue) : currValue;
				}

				curve->setDeadZone(deadZone);
			}

			curve = getResponseCurve2();

			if (curve.isValid()) {

				float deadZone;

				if (unite) {

					deadZone = unitedDeadZone;

				} else {

					float currValue = fabs(getRawValue2());
					deadZone = keepMax ? tmax(curve->getDeadZone(), currValue) : currValue;
				}

				curve->setDeadZone(deadZone);
			}

			onResponseCurvesChanged();
		}
	};

} }

#endif