#ifndef _WEScriptVector2_h
#define _WEScriptVector2_h

#include "../WEVector2.h"

#include "../../script/includeScriptingAPI.h"

namespace WE {

	class ScriptVector2 {
	public:

		//POD copy allowed

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline Vector2* scriptArg(Vector2& val) { return &val; }
	inline Vector2* scriptArg(Vector2* val) { return val; }


	class ScriptCtVector2 {
	public:

		//POD copy allowed

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline CtVector2* scriptArg(CtVector2& val) { return &val; }
	inline CtVector2* scriptArg(CtVector2* val) { return val; }
}

#endif