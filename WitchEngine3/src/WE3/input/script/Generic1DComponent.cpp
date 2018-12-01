#include "Generic1DComponent.h"
#include "Tracker.h"
#include "InputManager.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Input::ScriptGeneric1D, CInputGeneric1D);

namespace WE { namespace Input {

const TCHAR* ScriptGeneric1D::ScriptClassName = L"CInputGeneric1D";

void ScriptGeneric1D::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptGeneric1D>(ScriptClassName).
		func(&ScriptGeneric1D::script_isValid, L"isValid").
		func(&ScriptGeneric1D::script_isDiscrete, L"isDiscrete").
		func(&ScriptGeneric1D::script_isContinuous, L"isContinuous").
		func(&ScriptGeneric1D::script_getStepCount, L"getStepCount").
		func(&ScriptGeneric1D::script_getMin, L"getMin").
		func(&ScriptGeneric1D::script_getMax, L"getMax").
		func(&ScriptGeneric1D::script_getValue, L"getValue");
}


} }