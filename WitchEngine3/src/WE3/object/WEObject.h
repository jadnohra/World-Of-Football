#ifndef _WEObject_h
#define _WEObject_h

namespace WE {

	typedef unsigned short ObjectIndex;
	typedef unsigned short ObjectType;

	struct Object {

		ObjectIndex objectId;
		ObjectType objectType;
	};


}

#endif