#ifndef _WEScriptTracker_h_Input
#define _WEScriptTracker_h_Input

#include "../Tracker.h"
#include "Source.h"

#include "../../script/WEScriptEngine.h"

namespace WE { namespace Input {
	
	class ScriptTracker : public Tracker {
	public:

		ScriptTracker(ScriptEngine* pEngine = NULL, ScriptObject* pObject = NULL);

	public:

		virtual void onSourceChanged(Source* pImpl) {

			ScriptFunctionCall<void> func(mScriptEngine, mScriptRef, L"onVariableChanged");
			
			(func)(scriptArg(pImpl));
		}

	public:

		void script_onVariableChanged() {}

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);

	public:

		static int construct(HSQUIRRELVM v);
		static int release(SQUserPointer up, SQInteger size) { SQ_DELETE_CLASS(ScriptTracker); } 

	protected:

		SoftPtr<ScriptEngine> mScriptEngine;
		ScriptObject mScriptRef;
	};

} }

#endif