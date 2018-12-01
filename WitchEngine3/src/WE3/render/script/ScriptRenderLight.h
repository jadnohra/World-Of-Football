#ifndef _ScriptRenderLight_h_WE
#define _ScriptRenderLight_h_WE

#include "../WERenderer.h"

#include "../../script/includeScriptingAPI.h"

namespace WE {

	class ScriptRenderLight {
	public:

		//POD copy allowed

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline RenderLight* scriptArg(RenderLight& val) { return &val; }
	inline RenderLight* scriptArg(RenderLight* val) { return val; }
}

#endif