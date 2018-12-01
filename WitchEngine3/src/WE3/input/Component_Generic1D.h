#ifndef _Component_Generic1D_h_WE_Input
#define _Component_Generic1D_h_WE_Input

#include "../WETime.h"

namespace WE { namespace Input {


	class Component_Generic1D {
	public:

		virtual bool isDiscrete() { return true; }
		bool isContinuous() { return !isDiscrete(); }

		//discrete
		virtual int getStepCount() { return 0; }
		
		//continuous
		virtual float getMin() { return 0.0f; }
		virtual float getMax() { return 0.0f; }

		virtual float getValue() = 0;
	};

} }

#endif