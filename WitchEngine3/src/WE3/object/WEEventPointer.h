#ifndef _WEEventPointer_h
#define _WEEventPointer_h

#include "WEObject.h"
#include "../WEDataTypes.h"
#include "../WETL/WETLArray.h"

namespace WE {

	typedef UINT EventPointerIndex;

	struct EventPointer {

		ObjectIndex evtSource;
		EventPointerIndex srcIndex;
	};

	typedef WETL::CountedArray<EventPointer, false, EventPointerIndex> EventArray;
	typedef EventArray EventRegister;
}

#endif