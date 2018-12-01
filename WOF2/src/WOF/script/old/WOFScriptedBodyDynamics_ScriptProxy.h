#ifndef _WOFScriptedBodyDynamics_ScriptProxy_h
#define _WOFScriptedBodyDynamics_ScriptProxy_h

#include "WE3/WEPtr.h"
using namespace WE;

#include "includeScriptingAPI.h"
#include "WOFScriptingEngine.h"
#include "WOFScriptObject.h"

namespace WOF {

	class ScriptedBodyDynamics_ScriptProxy {
	public:

		bool create(ScriptingEngine& engine, const TCHAR* className);

	public:

	protected:

		ScriptObject mScriptInstance;
	};
}

#endif