#ifndef _WOFScriptingEngine_h
#define _WOFScriptingEngine_h

#include "WE3/WEPtr.h"
using namespace WE;

#include "includeScriptingAPI.h"
#include "WOFScriptObject.h"

namespace WOF {

	class Game;
	class ScriptingEngine_ScriptProxy;

	typedef SquirrelVM ScriptingClassDeclTarget;

	class ScriptingEngine {
	public:

		enum OutputMode {

			OM_None = -1, OM_Current = 0, OM_Silent, OM_Log, OM_Console
		};

	public:

		ScriptingEngine();
		~ScriptingEngine();

		bool init(Game& game, const TCHAR* globalVarName = NULL);
		bool start(const TCHAR* startScriptPath);

		inline const ScriptObject& getScriptObject() { return mScriptInstance; }

	public:

		void redirectOutput(OutputMode output);
		void run(const TCHAR* script, OutputMode errOutput = OM_Current, OutputMode output = OM_Current);

	public:

		bool createBodyState(const TCHAR* className, ScriptObject& result);
		bool createBodyDynamics(const TCHAR* className, ScriptObject& result);

	public:

		bool declareClass(const SQChar* className); 
		bool runScript(const SQChar* scriptPath); 
		
		const SQChar* getWOFVer();

	protected:

		static void output_console(HSQUIRRELVM v, const SQChar *s, ...);
		static void output_error(const SQChar *s, ...);

	protected:

		static SoftPtr<ScriptingEngine> gEngine;

	protected:

		SquirrelVM mVM;

		SoftPtr<Game> mGame;
		SoftPtr<ScriptingEngine_ScriptProxy> mProxy;
		ScriptObject mScriptInstance;

		OutputMode mOutputMode;
	};
}

#endif