#include "ButtonComponent.h"
#include "Tracker.h"
#include "InputManager.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Input::ScriptButton, CInputButton);

namespace WE { namespace Input {

const TCHAR* ScriptButton::ScriptClassName = L"CInputButton";

void ScriptButton::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptButton>(ScriptClassName).
		func(&ScriptButton::script_isValid, L"isValid").
		func(&ScriptButton::script_setMode, L"setMode").
		func(&ScriptButton::script_setDetectOn, L"setDetectOn").
		func(&ScriptButton::script_setDetectTap, L"setDetectTap").
		func(&ScriptButton::script_setDetectHold, L"setDetectHold").
		func(&ScriptButton::script_getDetectOn, L"getDetectOn").
		func(&ScriptButton::script_getDetectTap, L"getDetectTap").
		func(&ScriptButton::script_getDetectHold, L"getDetectHold").
		func(&ScriptButton::script_getIsTapping, L"getIsTapping").
		func(&ScriptButton::script_getIsHolding, L"getIsHolding").
		func(&ScriptButton::script_getIsOn, L"getIsOn").
		func(&ScriptButton::script_setTapInterval, L"setTapInterval").
		func(&ScriptButton::script_setTapCountLimit, L"setTapCountLimit").
		func(&ScriptButton::script_getTapCount, L"getTapCount").
		func(&ScriptButton::script_getHoldTime, L"getHoldTime");
}


} }