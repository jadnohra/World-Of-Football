#ifndef _DestructorNotifiable_h_Input
#define _DestructorNotifiable_h_Input

#include "../WETime.h"

namespace WE { namespace Input {

	class DestructorNotifiable {
	public:

		virtual void inputDestructNotify(void* pObject) {}
	};

} }

#endif