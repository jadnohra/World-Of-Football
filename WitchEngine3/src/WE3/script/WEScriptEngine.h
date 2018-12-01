#ifndef _WEScriptEngine_h
#define _WEScriptEngine_h

#include "../WEPtr.h"
#include "../WELog.h"
#include "../string/WEString.h"
using namespace WE;

#include "includeScriptingAPI.h"
#include "../WEConsole.h"

namespace WE {

	typedef SquirrelObject ScriptObject;

	class ScriptEngine {
	public:

		enum OutputMode {

			Out_None = -1, Out_Current = 0, Out_Silent, Out_Log, Out_Console, Out_Both
		};

	public:

		ScriptEngine();
		~ScriptEngine();

		bool init(Log* pScriptOutLog, Console* pScriptOutConsole, const TCHAR* pScriptPath);
		void setScriptPath(const TCHAR* pScriptPath);

		void setGameObject(ScriptObject* pGameScriptObject);
		inline ScriptObject& getGameObject() { return mGameScriptObject; }

		void redirectOutput(OutputMode output);
		bool run(const TCHAR* script, OutputMode errOutput = Out_Current, OutputMode output = Out_Current, String* pResultString = NULL);
		bool runFile(const TCHAR* scriptPath, OutputMode errOutput = Out_Current, OutputMode output = Out_Current);

		bool runFolder(const TCHAR* scriptFolderPath, bool recurse, OutputMode errOutput = Out_Current, OutputMode output = Out_Current);

		inline SquirrelVM& getVM() { return mVM; }
		inline HSQUIRRELVM getNativeVM() { return mVM.GetVMPtr(); }
		inline ScriptObject& getScriptObject() { return mScriptObject; }

		inline const ScriptObject& toScriptArg() { return mScriptObject; }

		static inline ScriptEngine* getScriptEngine(HSQUIRRELVM nativeVM) { return (ScriptEngine*) sq_getforeignptr(nativeVM); }
	
	public:

		bool createInstance(const TCHAR* className, ScriptObject& instance, const TCHAR* classDeclFile = NULL, bool tryUseClassNameAsDeclFile = true);
		bool beginCall(const TCHAR* fctName, ScriptObject* pInstance = NULL, const TCHAR* funcDeclFile = NULL);
		void pushArg(const SquirrelObject &o);
		void pushArg(const SQChar *s);
		void pushArg(SQInteger n);
		void pushArg(SQFloat f);
		void pushArg(SQUserPointer up);
		void pushArg(const SquirrelObject* pO = NULL);
		bool endCall(ScriptObject* pRet = NULL, OutputMode errOutput = Out_Current);

		bool call(const TCHAR* fctName, ScriptObject* pInstance = NULL, ScriptObject* pRet = NULL, const TCHAR* funcDeclFile = NULL, OutputMode errOutput = Out_Current);

	protected:

		static void output_silent(HSQUIRRELVM v, const SQChar *s, ...);
		static void output_console(HSQUIRRELVM v, const SQChar *s, ...);
		static void output_both(HSQUIRRELVM v, const SQChar *s, ...);
		static void output_log(HSQUIRRELVM v, const SQChar *s, ...);

		static void script_assrt(const TCHAR* msg);
		static void script_log(const TCHAR* msg);

	protected:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);

	public:

		bool script_runFile(const TCHAR* scriptPath);
		bool script_runFileEx(const TCHAR* scriptPath, int errOutput, int output);
		bool script_runFolder(const TCHAR* scriptFolderPath, bool recurse);
		bool script_runFolderEx(const TCHAR* scriptFolderPath, bool recurse, int errOutput, int output);
		inline ScriptObject script_getGame() { return mGameScriptObject; }

	protected:

		SquirrelVM mVM;
		OutputMode mOutputMode;
		bool mIsCalling;

		SoftPtr<Log> mLog;
		SoftPtr<Console> mConsole;
		SoftPtr<String> mResultString;
		
		String mScriptPath;

		ScriptObject mScriptObject;
		ScriptObject mGameScriptObject;
	};

	template<typename RT>
	struct ScriptFunctionCall : SqPlus::SquirrelFunction<RT> {

		/*
		ScriptFunctionCall(HSQUIRRELVM _v, const SquirrelObject & _object, const SquirrelObject & _func) : SquirrelFunction(_v, _object,_func) {}
		ScriptFunctionCall(const SquirrelObject & _object, const SquirrelObject & _func) : SquirrelFunction(_object, _func) {}
		ScriptFunctionCall(const SquirrelObject & _object, const SQChar * name) : SquirrelFunction(_object, name) {}
		ScriptFunctionCall(const SQChar * name) : SquirrelFunction(name) {}
		*/
		ScriptFunctionCall(ScriptEngine& engine, const TCHAR* funcName, const TCHAR* funcDeclFile) {

			if (funcDeclFile != NULL) {

				if (!engine.runFile(funcDeclFile)) {

					assrt(!func.IsNull());
					return;
				}
			}

			v = engine.getNativeVM();
			object = engine.getVM().GetRootTable();
			func = object.GetValue(funcName);

			assrt(!func.IsNull());
		}

		ScriptFunctionCall(ScriptEngine& engine, const TCHAR* funcName) {

			v = engine.getNativeVM();
			object = engine.getVM().GetRootTable();
			func = object.GetValue(funcName);
		}

		ScriptFunctionCall(ScriptEngine& engine, const ScriptObject& instance, const TCHAR* funcName, const TCHAR* funcDeclFile) {

			if (funcDeclFile != NULL) {

				if (!engine.runFile(funcDeclFile)) {

					assrt(!func.IsNull());
					return;
				}
			}

			v = engine.getNativeVM();
			object = instance;
			func = object.GetValue(funcName);

			assrt(!func.IsNull());
		}

		ScriptFunctionCall(ScriptEngine& engine, const ScriptObject& instance, const TCHAR* funcName) {

			v = engine.getNativeVM();
			object = instance;
			func = object.GetValue(funcName);

			assrt(!func.IsNull());
		}
	};

}

#endif