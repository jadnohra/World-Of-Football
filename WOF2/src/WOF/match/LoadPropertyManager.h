#ifndef h_WOF_LoadPropertyManager
#define h_WOF_LoadPropertyManager

#include "WE3/helper/WEPropertyManagerT.h"
using namespace WE;

namespace WOF {

	typedef void* LoadObject;

	class LoadProperty {
	public:

		virtual ~LoadProperty();
	};
	
	class LoadPropertyManager : public PropertyManagerT<LoadObject, LoadProperty> {
	};

} 

#endif