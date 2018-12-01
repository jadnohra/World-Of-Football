#include "Tracker.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Input::ScriptTracker, CInputTracker);

namespace WE { namespace Input {

const TCHAR* ScriptTracker::ScriptClassName = L"CInputTracker";

void ScriptTracker::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptTracker>(ScriptClassName).
		staticFunc(&ScriptTracker::construct,L"constructor").
		func(&ScriptTracker::script_onVariableChanged, L"onVariableChanged");
}

ScriptTracker::ScriptTracker(ScriptEngine* pEngine, ScriptObject* pScriptRef)
	: mScriptEngine(pEngine) {

	if (pScriptRef)
		mScriptRef = *pScriptRef;
}

int ScriptTracker::construct(HSQUIRRELVM v) {

	StackHandler sa(v);
	int paramCount = sa.GetParamCount();
	
	if (paramCount == 1) {

		ScriptObject self;
		self.AttachToStackObject(1);

		if (self.IsNull())
			return sq_throwerror(v,_T("Invalid Constructor arguments"));

		SoftPtr<ScriptEngine> eng = ScriptEngine::getScriptEngine(v);

		if (!eng.isValid()) {

			assrt(false);
			return sq_throwerror(v,_T("Failed To Extract ScriptEngine from VM"));
		}

		SoftPtr<ScriptTracker> newObj;
		WE_MMemNew(newObj.ptrRef(), ScriptTracker(eng, &self));


		return SqPlus::PostConstruct<ScriptTracker>(v, newObj, release);
	} 

	return sq_throwerror(v,_T("Invalid Constructor arguments"));
} 

} }