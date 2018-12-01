#ifndef _WEScriptSceneTransform_h
#define _WEScriptSceneTransform_h

#include "../WESceneTransform.h"

#include "../../script/includeScriptingAPI.h"

namespace WE {

	class ScriptSceneTransform {
	public:

		//POD copy allowed

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline SceneTransform* scriptArg(SceneTransform& val) { return &val; }
	inline SceneTransform* scriptArg(SceneTransform* val) { return val; }
}

#endif