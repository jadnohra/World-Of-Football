#ifndef _WEVector3_ScriptClassDecl_h
#define _WEVector3_ScriptClassDecl_h

#include "WE3/WEPtr.h"
#include "WE3/math/WEVector.h"
using namespace WE;

#include "includeScriptingAPI.h"
#include "WOFScriptObject.h"

namespace WOF {
	

	class Vector3_ScriptClassDecl {
	public:

		static const TCHAR* ScriptClassName;

		static void declareScriptClass(SquirrelVM& target);
	};
}

#endif