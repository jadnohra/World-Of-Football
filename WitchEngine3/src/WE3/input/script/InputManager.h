#ifndef _WEScriptInputManager_h_Input
#define _WEScriptInputManager_h_Input

#include "../InputManager.h"
#include "Controller.h"

#include "../../script/WEScriptEngine.h"

namespace WE { namespace Input {
	
	
	class ScriptInputManager : public SoftPtr<InputManager> {
	public:

		ScriptInputManager(InputManager* pRef = NULL) : SoftPtr(pRef) {}

	public:

		inline bool script_isValid() { return isValid(); }

		inline InputManager::Index script_getControllerCount() { return ptr()->getControllerCount(); }

		ScriptController script_getController(InputManager::Index index);
		bool script_findController(InputManager::Index index, ScriptController* pController);
		bool script_addController(ScriptController* pController);

		ScriptController script_createVirtualController(const TCHAR* name);

	public:

		ScriptInputManager(const ScriptInputManager& ref) : SoftPtr(ref.pObject) {}
		inline ScriptInputManager& operator=(const ScriptInputManager& ref) { set(ref.pObject); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};


	inline ScriptInputManager scriptArg(InputManager& val) { return ScriptInputManager(&val); }
	inline ScriptInputManager scriptArg(InputManager* val) { return ScriptInputManager(val); }

} }

#endif