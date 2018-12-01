#ifndef _WEScriptVector3_h
#define _WEScriptVector3_h

#include "../WEVector.h"

#include "../../script/includeScriptingAPI.h"

namespace WE {

	class ScriptVector3 {
	public:

		//POD copy allowed

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline Vector3* scriptArg(Vector3& val) { return &val; }
	inline Vector3* scriptArg(Vector3* val) { return val; }


	class ScriptCtVector3 {
	public:

		//POD copy allowed

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline CtVector3* scriptArg(CtVector3& val) { return &val; }
	inline CtVector3* scriptArg(CtVector3* val) { return val; }
}

#endif