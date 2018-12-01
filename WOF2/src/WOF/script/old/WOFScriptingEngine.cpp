#include "WOFScriptingEngine.h"
#include "WOFScriptingEngine_ScriptProxy.h"

#include "sqstdio.h"
#include "sqstdaux.h"

#include "WE3/WEAssert.h"
#include "WE3/filePath/WEFilePath.h"
using namespace WE;

#include "../WOF.h"

#include "WEVector3_ScriptClassDecl.h"
#include "WOFScriptedBodyDynamics_ScriptProxy.h"
#include "WOFConfig_ScriptProxy.h"

namespace WOF {

SoftPtr<ScriptingEngine> ScriptingEngine::gEngine;


ScriptingEngine::ScriptingEngine() 
	: mOutputMode(OM_None) {
}

bool ScriptingEngine::init(Game& game, const TCHAR* globalVarName) {

	if (mVM.IsInitialized()) {

		//probably more than 1 Scripting Engine created,
		//and SquirrelVM doesnt support that for now ...
		assrt(false);
		return false;
	}

	mGame = &game;
	
	mVM.Init();
	redirectOutput(OM_Log);

	mProxy = ScriptingEngine_ScriptProxy::create(mVM, *this, globalVarName);

	if (mProxy.isValid()) {

		Vector3_ScriptClassDecl::declareScriptClass(mVM);
		Config_ScriptProxy::declareScriptClass(mVM);
	}

	return mProxy.isValid();
}

bool ScriptingEngine::start(const TCHAR* startScriptPath) {

	if (!runScript(startScriptPath)) {

		return false;
	}

	{
		bool succeeded = false;

		HSQUIRRELVM VM = mVM.GetVMPtr();
		SQInteger top = sq_gettop(VM);

		//get the function from the root table
		sq_pushroottable(VM);
		sq_pushstring(VM, L"main", -1);
		if (SQ_SUCCEEDED(sq_get(VM, -2))) {

			//set args
			sq_pushroottable(VM); // 'this'
			sq_pushobject(VM, mProxy->getScriptInstance()); // 'engine'

			//call
			if (SQ_SUCCEEDED(sq_call(VM, 2, SQTrue, SQTrue))) {

				if (SQ_SUCCEEDED(sq_getstackobj(VM, -1, mScriptInstance))) {

					mScriptInstance.validate();

					succeeded = mScriptInstance.isValid();
				}
			}
		} 

		sq_settop(VM, top);

		if (!succeeded)
			return false;
	}

	ScriptedBodyDynamics_ScriptProxy test;

	test.create(*this, L"BodyDynamics_DynamicTackle");

	return true;
}

ScriptingEngine::~ScriptingEngine() {

	mVM.Shutdown();
}

bool ScriptingEngine::declareClass(const SQChar* className) {

	return true;
}

bool ScriptingEngine::runScript(const SQChar* scriptPath) {

	bool ret = false;

	HSQUIRRELVM VM = mVM.GetVMPtr();
	String path(mGame->getScriptPath());

	PathResolver::appendPath(path, scriptPath, true, false);

	if (!ret && !FilePath::hasExtension(path)) {

		ret = true;
		path.append(L".nut");

		SQInteger top = sq_gettop(VM);
		sq_pushroottable(VM); 

		if (!SQ_SUCCEEDED(sqstd_dofile(VM, path.c_tstr(), 0, SQTrue))) {

			ret = false;
			log(L"\nError Executing Script [%s]", path.c_tstr());

			const SQChar *err;
			sq_getlasterror(VM);
			
			if (SQ_SUCCEEDED(sq_getstring(VM, -1, &err))) {
			
				log(L"Error Details Start\n");
				log(err);
				log(L"Error Details End\n");
			} 
		}

		sq_settop(VM, top);
	}

	if (!ret) {

		ret = true;

		SQInteger top = sq_gettop(VM);
		sq_pushroottable(VM); 

		if (!SQ_SUCCEEDED(sqstd_dofile(VM, path.c_tstr(), 0, SQTrue))) {

			ret = false;
			log(L"\nError Executing Script [%s]", path.c_tstr());

			const SQChar *err;
			sq_getlasterror(VM);
			
			if (SQ_SUCCEEDED(sq_getstring(VM, -1, &err))) {
			
				log(L"Error Details Start\n");
				log(err);
				log(L"Error Details End\n");
			} 
		}

		sq_settop(VM, top);
	}

	

	if (!ret) {

		//to try to Edit Script and Fix
		DebugBreak();
	}

	return ret;
}

const SQChar* ScriptingEngine::getWOFVer() {

	return mGame->getVersion(); 
}

bool ScriptingEngine::createBodyState(const TCHAR* className, ScriptObject& result) {

	SqPlus::SquirrelFunction<SquirrelObject> fct(mScriptInstance.get(), L"createBodyState");

	result = (fct)(className);

	return result.isValid();
}

bool ScriptingEngine::createBodyDynamics(const TCHAR* className, ScriptObject& result) {

	SqPlus::SquirrelFunction<SquirrelObject> fct(mScriptInstance.get(), L"createBodyDynamics");

	result = (fct)(className);

	return result.isValid();
}

void output_silent(HSQUIRRELVM v, const SQChar *s, ...) { 
} 

void output_log(HSQUIRRELVM v, const SQChar *s, ...) { 

	static BufferString str;

	va_list arglist; 
    va_start(arglist, s); 
    
	str.assignEx(s, arglist);

    va_end(arglist); 

	log(str.c_tstr());
} 

void ScriptingEngine::output_console(HSQUIRRELVM v, const SQChar *s, ...) { 

	if (gEngine.isValid()) {

		static BufferString str;

		va_list arglist; 
		va_start(arglist, s); 
	    
		str.assignEx(s, arglist);

		va_end(arglist); 

		//gEngine->mGame->getConsole().show(true, true);
		gEngine->mGame->getConsole().print(str.c_tstr());
	}
} 

void ScriptingEngine::redirectOutput(OutputMode output) {

	if (output != OM_Current && output != mOutputMode) {

		switch (output) {

			case OM_Silent: {

				sq_setprintfunc(mVM.GetVMPtr(), output_silent);

			} break;
				
			case OM_Log: {

				sq_setprintfunc(mVM.GetVMPtr(), output_log);

			} break; 

			case OM_Console: {

				gEngine = this;
				sq_setprintfunc(mVM.GetVMPtr(), output_console);

			} break;
		}

		mOutputMode = output;
	}
}

void ScriptingEngine::run(const TCHAR* command, OutputMode errOutput, OutputMode output) {

	SquirrelObject compiled = mVM.CompileBuffer(command);

	OutputMode savedMode = mOutputMode;

	try {

		redirectOutput(output);

        mVM.RunScript(compiled); 

    } catch(SquirrelError & e) {

		redirectOutput(errOutput);

		assrt(false);
        sq_getprintfunc(mVM.GetVMPtr()) (mVM.GetVMPtr(),_T("Error: %s\n"), e.desc);
    }

	redirectOutput(savedMode);
}

}
