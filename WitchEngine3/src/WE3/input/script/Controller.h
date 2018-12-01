#ifndef _WEScriptController_h_Input
#define _WEScriptController_h_Input

#include "../Controller.h"

#include "../../script/WEScriptEngine.h"
#include "Source.h"

namespace WE { namespace Input {
	
	class ScriptInputManager;
	class ScriptInputClient;

	class ScriptController : public HardRef<Controller> {
	public:

		ScriptController(Controller* pRef = NULL) : HardRef(pRef) {}
		
	public:

		inline bool script_isValid() { return isValid(); }
				
		inline const TCHAR* script_getName() { return dref().getName(); }
		const TCHAR* script_getProductName() { return dref().getProductName(); }

		inline bool script_isDevice() { return dref().isDevice(); }
		inline bool script_isVisible() { return dref().isVisible(); }
		inline void script_setVisible(bool visible) { return dref().setVisible(visible); }

		ScriptInputManager script_getManager();

		bool script_setName(const TCHAR* name) { return dref().setName(name); }

		inline Controller::Index script_getSourceCount() { return dref().getSourceCount(); }

		bool script_findSourceAt(Controller::Index, ScriptSource* pVar, Time time);
		bool script_findSource(const TCHAR* name, ScriptSource* pVar, Time time);
		bool script_addSource(ScriptSource* pVar, const TCHAR* name);
		bool script_addConstant(float value, const TCHAR* name);

		ScriptSource script_getSource(const TCHAR* name, Time time);
		ScriptSource script_getSourceAt(Controller::Index, Time time);

		float script_getDeadZone() { return dref().getDeadZone(); }

		void script_setInputClient(ScriptInputClient* pClient);
		ScriptInputClient script_getInputClient();

	public:

		ScriptController(const ScriptController& ref) : HardRef(ref.ptr()) {}
		inline ScriptController& operator=(const ScriptController& ref) { set(ref.ptr()); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptController scriptArg(Controller& val) { return ScriptController(&val); }
	inline ScriptController scriptArg(Controller* val) { return ScriptController(val); }

} }

#endif