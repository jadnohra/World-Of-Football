#ifndef _WEObjectData_h
#define _WEObjectData_h

#include "../WEDataTypes.h"

namespace WE {

	struct ObjectData {

		void* ptr;

		ObjectData() : ptr(NULL) {}
		~ObjectData() {}

		void destroy() { ptr = NULL; }
		bool isValid() { return ptr != NULL; } 
	};
	
}

#endif