#include "HoldCycle.h"

DECLARE_INSTANCE_TYPE_NAME( WE::Input::HoldCycle, CInputHoldCycle);

namespace WE { namespace Input {

const TCHAR* ScriptHoldCycle::ScriptClassName = L"CInputHoldCycle";

void ScriptHoldCycle::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<HoldCycle>(ScriptClassName).
		var(&HoldCycle::cycleIndex, L"cycleIndex").
		var(&HoldCycle::value, L"value").
		func(&HoldCycle::update, L"update");
}


} }