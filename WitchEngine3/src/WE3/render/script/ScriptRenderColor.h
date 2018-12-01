#ifndef _ScriptRenderColor_h_WE
#define _ScriptRenderColor_h_WE

#include "../WERenderer.h"

#include "../../script/includeScriptingAPI.h"

namespace WE {

	class ScriptRenderColor {
	public:

		//POD copy allowed

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline RenderColor* scriptArg(RenderColor& val) { return &val; }
	inline RenderColor* scriptArg(RenderColor* val) { return val; }
}

#endif