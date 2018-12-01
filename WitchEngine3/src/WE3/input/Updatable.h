#ifndef _Updatable_h_Input
#define _Updatable_h_Input

#include "../WETime.h"

namespace WE { namespace Input {

	class Updatable {
	public:

		virtual void inputUpdate(const Time& t) {}
	};

} }

#endif