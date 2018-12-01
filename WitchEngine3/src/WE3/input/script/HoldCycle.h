#ifndef _ScriptHoldCycle_h_WE_Input
#define _ScriptHoldCycle_h_WE_Input

#include "../HoldCycle.h"

#include "../../script/WEScriptEngine.h"

namespace WE { namespace Input {


	class ScriptHoldCycle {
	public:

	
	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

} }

#endif