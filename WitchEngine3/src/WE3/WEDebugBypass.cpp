#include "WEDebugBypass.h"

#include "stdlib.h"

namespace WE {

DebugBypass* pBypass = NULL;
DebugBypass kStaticBypass;

void setDebugBypass(DebugBypass* _pBypass) {

	pBypass = _pBypass;
}

DebugBypass* getDebugBypass() {

	return pBypass;
}

DebugBypass& getStaticDebugBypass() {

	return kStaticBypass;
}

}