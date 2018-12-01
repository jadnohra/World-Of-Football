#include "ChangeQueue.h"

DECLARE_INSTANCE_TYPE_NAME( WE::Input::ScriptChangeQueue, CInputChangeQueue);

namespace WE { namespace Input {

const TCHAR* ScriptChangeQueue::ScriptClassName = L"CInputChangeQueue";

void ScriptChangeQueue::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptChangeQueue>(ScriptClassName).
		func(&ScriptChangeQueue::script_startTracking, L"startTracking").
		func(&ScriptChangeQueue::script_stopTracking, L"stopTracking").
		func(&ScriptChangeQueue::script_getCount, L"getCount").
		func(&ScriptChangeQueue::script_get, L"get").
		func(&ScriptChangeQueue::script_clear, L"clear");
}


} }