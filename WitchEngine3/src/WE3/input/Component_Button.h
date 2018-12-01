#ifndef _Component_Button_h_WE_Input
#define _Component_Button_h_WE_Input

#include "../WETime.h"

namespace WE { namespace Input {

	class Component_Button {
	public:

		void setMode(bool detectOn, bool detectHolding, bool detectTapping) {

			setDetectOn(detectOn);
			setDetectTap(detectTapping);
			setDetectHold(detectHolding);
		}

		virtual void setDetectOn(bool detect) = 0;
		virtual void setDetectHold(bool detect) = 0;
		virtual void setDetectTap(bool detect) = 0;

		virtual bool getDetectOn() = 0;
		virtual bool getDetectHold() = 0;
		virtual bool getDetectTap() = 0;

		virtual bool getIsTapping() = 0;
		virtual bool getIsHolding() = 0;
		virtual bool getIsOn() = 0;

		virtual void setTapInterval(float tapInterval) = 0;
		virtual void setTapCountLimit(int limit) = 0; 

		virtual int getTapCount(bool reset) = 0;
		virtual Time getHoldTime(Time time, bool reset) = 0;
	};

} }

#endif