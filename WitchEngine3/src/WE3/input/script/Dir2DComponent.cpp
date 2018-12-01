#include "Dir2DComponent.h"
#include "Tracker.h"
#include "InputManager.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Input::ScriptDir2D, CInputDir2D);

namespace WE { namespace Input {

const TCHAR* ScriptDir2D::ScriptClassName = L"CInputDir2D";

void ScriptDir2D::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptDir2D>(ScriptClassName).
		func(&ScriptDir2D::script_isValid, L"isValid").
		func(&ScriptDir2D::script_getValue1, L"getValue1").
		func(&ScriptDir2D::script_getValue2, L"getValue2").
		func(&ScriptDir2D::script_getRawValue1, L"getRawValue1").
		func(&ScriptDir2D::script_getRawValue2, L"getRawValue2").
		func(&ScriptDir2D::script_setRawValuesAsDeadZones, L"setRawValuesAsDeadZones").
		func(&ScriptDir2D::script_createResponseCurve1, L"createResponseCurve1").
		func(&ScriptDir2D::script_createResponseCurve2, L"createResponseCurve2").
		func(&ScriptDir2D::script_destroyResponseCurve1, L"destroyResponseCurve1").
		func(&ScriptDir2D::script_destroyResponseCurve2, L"destroyResponseCurve2").
		func(&ScriptDir2D::script_getResponseCurve1, L"getResponseCurve1").
		func(&ScriptDir2D::script_getResponseCurve2, L"getResponseCurve2");
}


} }