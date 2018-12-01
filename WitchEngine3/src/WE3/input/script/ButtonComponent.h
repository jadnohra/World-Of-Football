#ifndef _WEButtonComponent_h_Input
#define _WEButtonComponent_h_Input

#include "../Component_Button.h"

#include "../../WEPtr.h"
#include "../../script/WEScriptEngine.h"

namespace WE { namespace Input {
	
	class ScriptTracker;
	class ScriptInputManager;

	class ScriptButton : public SoftPtr<Component_Button> {
	public:

		ScriptButton(Component_Button* pRef = NULL) : SoftPtr<Component_Button>(pRef) {}

	public:

		inline bool script_isValid() { return isValid(); }
		
		inline void script_setMode(bool detOn, bool detHold, bool detTap) { return dref().setMode(detOn, detHold, detTap); }

		inline void script_setDetectOn(bool val) { dref().setDetectOn(val); }
		inline void script_setDetectHold(bool val) { dref().setDetectHold(val); }
		inline void script_setDetectTap(bool val) { dref().setDetectTap(val); }
		
		inline bool script_getDetectOn() { return dref().getDetectOn(); }
		inline bool script_getDetectHold() { return dref().getDetectHold(); }
		inline bool script_getDetectTap() { return dref().getDetectTap(); }

		inline bool script_getIsTapping() { return dref().getIsTapping(); }
		inline bool script_getIsHolding() { return dref().getIsHolding(); }
		inline bool script_getIsOn() { return dref().getIsOn(); }

		inline void script_setTapInterval(float tapInterval) { return dref().setTapInterval(tapInterval); }
		inline void script_setTapCountLimit(int limit) { return dref().setTapCountLimit(limit); }

		inline int script_getTapCount(bool reset) { return dref().getTapCount(reset); }
		inline float script_getHoldTime(float time, bool reset)  { return dref().getHoldTime(time, reset); }

	public:

		ScriptButton(const ScriptButton& ref) : SoftPtr<Component_Button>(ref.ptr()) {}
		inline ScriptButton& operator=(const ScriptButton& ref) { set(ref.ptr()); return *this; }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptButton scriptArg(Component_Button& val) { return ScriptButton(&val); }
	inline ScriptButton scriptArg(Component_Button* val) { return ScriptButton(val); }


} }

#endif