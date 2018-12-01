#ifndef _includeDebug_h
#define _includeDebug_h

#if defined(DEBUG) | defined(_DEBUG)
#include "crtdbg.h"

struct DebugInfo {

	DebugInfo() {
		// Enable run-time memory check for debug builds.
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	}

};

static const DebugInfo debugInfo;

#endif

#endif