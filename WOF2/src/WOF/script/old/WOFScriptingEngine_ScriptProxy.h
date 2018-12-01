#ifndef _WOFScriptingEngine_ScriptProxy_h
#define _WOFScriptingEngine_ScriptProxy_h

#include "WE3/WEPtr.h"
using namespace WE;

#include "includeScriptingAPI.h"
#include "WOFScriptObject.h"

namespace WOF {

	class ScriptingEngine;

	class ScriptingEngine_ScriptProxy {
	public:

		static ScriptingEngine_ScriptProxy* create(SquirrelVM& squirrelVM, ScriptingEngine& engine, const TCHAR* globalVarName = NULL);

		ScriptingEngine_ScriptProxy();
		~ScriptingEngine_ScriptProxy();

		inline ScriptingEngine& getEngine() { return mEngine; }
		inline ScriptObject& getScriptInstance() { return mScriptInstance; }

	public:

		bool declareClass(const SQChar* className);
		bool runScript(const SQChar* scriptPath);
		const SQChar* getWOFVer();
		
	public:

		static const TCHAR* ScriptClassName;

		static void declareScriptClass(SquirrelVM& target);

	protected:

		struct CreationContext {

			SoftPtr<ScriptingEngine> engine;
			SoftPtr<ScriptingEngine_ScriptProxy> object;
		};

		static CreationContext creationContext;

	protected:

		SoftPtr<ScriptingEngine> mEngine;
		ScriptObject mScriptInstance;
	};
}

#endif