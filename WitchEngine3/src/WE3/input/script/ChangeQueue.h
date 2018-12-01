#ifndef _WEScriptChangeQueue_h_Input
#define _WEScriptChangeQueue_h_Input

#include "../ChangeQueue.h"
#include "Source.h"
#include "InputManager.h"

#include "../../script/WEScriptEngine.h"
#include "../../WEAssert.h"


namespace WE { namespace Input {
	
	class ScriptChangeQueue : public ChangeQueue {
	public:

		ScriptChangeQueue() {}

	public:

		inline bool script_startTracking(ScriptInputManager* pManager, ScriptSource* pVar) { return (*pVar)->addTracker(pManager->dref(), this); } 
		inline void script_stopTracking(ScriptInputManager* pManager, ScriptSource* pVar) { (*pVar)->removeTracker(pManager->dref(), this); } 

		inline Index script_getCount() { return getCount(); }
		inline ScriptSource script_get(Index index) { return ScriptSource(get(index)); }
		inline void script_clear() { clear(); }
		
	public:

		//no copying
		ScriptChangeQueue(const ScriptChangeQueue& ref) { assrt(false); }
		inline ScriptChangeQueue& operator=(const ScriptChangeQueue& ref) { assrt(false); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptChangeQueue* scriptArg(ScriptChangeQueue& val) { return &val; }
	inline ScriptChangeQueue* scriptArg(ScriptChangeQueue* val) { return val; }


} }

#endif