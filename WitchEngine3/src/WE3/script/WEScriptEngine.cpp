#include "WEScriptEngine.h"

#include "sqstdio.h"
#include "sqstdaux.h"

#include "../WEAssert.h"
#include "../filePath/WEFilePath.h"
#include "../filePath/WEPathResolver.h"
#include "../filePath/WEFileIterator.h"
#include "../string/WEString.h"
using namespace WE;

#include "WEDeclareClasses.h"

DECLARE_INSTANCE_TYPE_NAME(WE::ScriptEngine, CScriptEngine);

namespace WE {

const TCHAR* ScriptEngine::ScriptClassName = L"CScriptEngine";

void ScriptEngine::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptEngine>(ScriptClassName).
		func(&ScriptEngine::script_runFile, L"runFile").
		func(&ScriptEngine::script_runFileEx, L"runFileEx").
		func(&ScriptEngine::script_runFolder, L"runFolder").
		func(&ScriptEngine::script_runFolderEx, L"runFolderEx").
		func(&ScriptEngine::script_getGame, L"getGame").
		enumInt(ScriptEngine::Out_Current, L"Out_Current").
		enumInt(ScriptEngine::Out_Silent, L"Out_Silent").
		enumInt(ScriptEngine::Out_Log, L"Out_Log").
		enumInt(ScriptEngine::Out_Console, L"Out_Console");

	SqPlus::RegisterGlobal(&ScriptEngine::script_assrt, L"assrt");
	SqPlus::RegisterGlobal(&ScriptEngine::script_log, L"log");
}

bool ScriptEngine::script_runFile(const TCHAR* scriptPath) {

	return runFile(scriptPath);
}

bool ScriptEngine::script_runFileEx(const TCHAR* scriptPath, int errOutput, int output) {

	return runFile(scriptPath, (OutputMode) errOutput, (OutputMode) output);
}

bool ScriptEngine::script_runFolder(const TCHAR* scriptFolderPath, bool recurse) {

	return runFolder(scriptFolderPath, recurse);
}

bool ScriptEngine::script_runFolderEx(const TCHAR* scriptFolderPath, bool recurse, int errOutput, int output) {

	return runFolder(scriptFolderPath, recurse, (OutputMode) errOutput, (OutputMode) output);
}

ScriptEngine::ScriptEngine() 
	: mOutputMode(Out_None) {
}

void ScriptEngine::setScriptPath(const TCHAR* pScriptPath) {

	mScriptPath.assign(pScriptPath);
}

bool ScriptEngine::init(Log* pScriptOutLog, Console* pScriptOutConsole, const TCHAR* pScriptPath) {

	if (mVM.IsInitialized()) {

		//probably more than 1 Scripting Engine created,
		//and SquirrelVM doesnt support that for now ...
		assrt(false);
		return false;
	}

	mLog = pScriptOutLog;
	mConsole = pScriptOutConsole;

	mVM.Init();
	sq_setforeignptr(mVM.GetVMPtr(), this);
	redirectOutput(Out_Log);

	declareInVM(mVM);

	mScriptObject = ScriptObject(this);
	mScriptPath.assign(pScriptPath);

	mIsCalling = false;

	DeclareClasses::declareInVM(mVM);

	return true;
}

ScriptEngine::~ScriptEngine() {

	mVM.Shutdown();
}

void ScriptEngine::setGameObject(ScriptObject* pGameScriptObject) {

	if (pGameScriptObject) {
		
		mGameScriptObject = *pGameScriptObject;

	} else {

		mGameScriptObject.Reset();
	}
}

bool ScriptEngine::runFolder(const TCHAR* scriptFolderPath, bool recurse, OutputMode errOutput, OutputMode output) {

	if (!FilePath::isAbsolutePath(scriptFolderPath) && !mScriptPath.isEmpty()) {

		String path(mScriptPath);
		
		PathResolver::appendPath(path, scriptFolderPath, true, true);

		return runFolder(path.c_tstr(), recurse, errOutput, output);
	}

	int iterCount = recurse == true ? 2 : 1;

	for (int i = 0; i < iterCount; ++i) {

		Platform::FileIterator iter;
		
		if (Platform::startFileIterate(scriptFolderPath, iter)) {

			do {
				
				String fileName;
				bool isFolder;

				if (iter.getCurrFileName(fileName, isFolder)) {

					if (isFolder) {

						if (recurse) {

							String folderPath(scriptFolderPath);
							PathResolver::appendPath(folderPath, fileName.c_tstr(), true, false);

							if (!runFolder(folderPath.c_tstr(), recurse, errOutput, output)) {

								return false;
							}
						}

					} else {

						String filePath(scriptFolderPath);
						PathResolver::appendPath(filePath, fileName.c_tstr(), true, false);

						if (!runFile(filePath.c_tstr(), errOutput, output)) {

							return false;
						}
					}
				}

			} while (Platform::nextFileIterate(iter));
		}
	}

	return true;
}

bool ScriptEngine::runFile(const TCHAR* scriptPath, OutputMode errOutput, OutputMode output) {

	if (!FilePath::isAbsolutePath(scriptPath) && !mScriptPath.isEmpty()) {

		String path(mScriptPath);
		
		PathResolver::appendPath(path, scriptPath, true, true);

		return runFile(path.c_tstr(), errOutput, output);
	}

	if (!FilePath::hasExtension(scriptPath)) {

		String path(scriptPath);
		path.append(L".nut");

		return runFile(path.c_tstr(), errOutput, output);
	}

	bool ret = false;

	OutputMode savedMode = mOutputMode;
	
	bool retry = true;

	while (!ret && retry) {

		retry = false;
		
		#ifdef _DEBUG
			retry = true;
		#endif


		redirectOutput(output);

		HSQUIRRELVM VM = mVM.GetVMPtr();
		SQInteger top = sq_gettop(VM);
		sq_pushroottable(VM); 

		if (!SQ_SUCCEEDED(sqstd_dofile(VM, scriptPath, 0, SQTrue))) {

			redirectOutput(errOutput);
			
			const SQChar *err;
			sq_getlasterror(VM);
			
			if (SQ_SUCCEEDED(sq_getstring(VM, -1, &err))) {
			
				sq_getprintfunc(mVM.GetVMPtr()) (mVM.GetVMPtr(),_T("error: %s"), scriptPath);
				sq_getprintfunc(mVM.GetVMPtr()) (mVM.GetVMPtr(),_T("%s\n"), err);
			} 

			//if this assrt is reached u can edit the file fix the error
			//and set the local variable retry to true
			assrt(false);
			ret = false;

		} else {

			ret = true;
		}

		sq_settop(VM, top);
	}

	redirectOutput(savedMode);

	return ret;
}

bool ScriptEngine::createInstance(const TCHAR* className, ScriptObject& instance, const TCHAR* classDeclFile, bool tryUseClassNameAsDeclFile) {

	if (!instance.IsNull()) {

		assrt(false);
		return false;
	}

	if (classDeclFile != NULL) {

		if (!runFile(classDeclFile)) {

			return false;
		}
	}

	HSQUIRRELVM VM = mVM.GetVMPtr();

	if (SqPlus::CreateConstructNativeClassInstance(VM, className)) {

		instance.AttachToStackObject(-1);
		sq_poptop(VM);

	} else {

		if (tryUseClassNameAsDeclFile && runFile(className)) {

			return createInstance(className, instance, NULL, false);
		}
	}

	return !instance.IsNull();
}

void ScriptEngine::pushArg(const ScriptObject &o) {

	mVM.PushParam(o);
}

void ScriptEngine::pushArg(const SQChar *s) {

	mVM.PushParam(s);
}

void ScriptEngine::pushArg(SQInteger n) {

	mVM.PushParam(n);
}

void ScriptEngine::pushArg(SQFloat f) {

	mVM.PushParam(f);
}

void ScriptEngine::pushArg(SQUserPointer up){

	mVM.PushParam(up);
}

void ScriptEngine::pushArg(const ScriptObject* pO) {

	if (pO) {

		mVM.PushParam(*pO);

	} else {

		mVM.PushParamNull();
	}
}

bool ScriptEngine::call(const TCHAR* fctName, ScriptObject* pInstance, ScriptObject* pRet, const TCHAR* funcDeclFile, OutputMode errOutput) {

	return beginCall(fctName, pInstance, funcDeclFile) && endCall(pRet, errOutput);
}

bool ScriptEngine::beginCall(const TCHAR* fctName, ScriptObject* pInstance, const TCHAR* funcDeclFile) {

	if (mIsCalling) {

		assrt(false);
		return false;
	}

	mIsCalling = true;

	if (funcDeclFile != NULL) {

		if (!runFile(funcDeclFile)) {

			mIsCalling = false;
			return false;
		}
	}

	ScriptObject root = SquirrelVM::GetRootTable();
	ScriptObject scriptFct;
	
	if (pInstance) {

		scriptFct = pInstance->GetValue(fctName);

	} else {

		scriptFct = root.GetValue(fctName);
	}

	if (!scriptFct.IsNull()) {

		bool success = false;

		if (pInstance) {

			success = mVM.BeginCall(scriptFct, *pInstance) == TRUE;

		} else {

			success = mVM.BeginCall(scriptFct) == TRUE;
		}

		return success;
	}

	mIsCalling = false;
	return false;
}

bool ScriptEngine::endCall(ScriptObject* pRet, OutputMode errOutput) {

	if (!mIsCalling) {

		assrt(false);
		return false;
	}

	try {

		if (pRet) {

			*pRet = mVM.EndCall();

		} else {

			mVM.EndCall();
		}

		mIsCalling = false;

		return true;

	} catch(SquirrelError& e) {

		assrt(false);

		OutputMode savedMode = mOutputMode;
		redirectOutput(errOutput);

		sq_getprintfunc(mVM.GetVMPtr()) (mVM.GetVMPtr(),_T("%s\n"), e.desc);

		redirectOutput(savedMode);
	}

	mIsCalling = false;

	return false;
}

bool ScriptEngine::run(const TCHAR* script, OutputMode errOutput, OutputMode output, String* pResultString) {

	bool ret = true;
	mResultString = pResultString;

	ScriptObject compiled = mVM.CompileBuffer(script);

	OutputMode savedMode = mOutputMode;

	try {

		redirectOutput(output);

        mVM.RunScript(compiled); 

    } catch(SquirrelError& e) {

		assrt(false);
		ret = false;

		redirectOutput(errOutput);
        sq_getprintfunc(mVM.GetVMPtr()) (mVM.GetVMPtr(),_T("%s\n"), e.desc);
    }

	redirectOutput(savedMode);
	mResultString.destroy();

	return ret;
}

void ScriptEngine::script_assrt(const TCHAR* msg) {

	log(msg);
	assrt(false);
}

void ScriptEngine::script_log(const TCHAR* msg) {

	log(msg);
}

void ScriptEngine::output_silent(HSQUIRRELVM v, const SQChar *s, ...) { 

	bool genString = false;

	SoftPtr<ScriptEngine> eng = getScriptEngine(v);

	if (eng.isValid() && eng->mResultString.isValid()) {
		
		genString = true;
	}

#ifdef _DEBUG

	genString = true;

#endif

	static String str;

	if (genString) {

		va_list arglist; 
		va_start(arglist, s); 
	    
		str.assignEx(s, arglist);

		va_end(arglist); 
	}

#ifdef _DEBUG
	OutputDebugString(str.c_tstr());
	OutputDebugString(L"\n");
#endif

	if (eng.isValid() && eng->mResultString.isValid()) {
		
		eng->mResultString->append(str);
	}

} 

void ScriptEngine::output_log(HSQUIRRELVM v, const SQChar *s, ...) { 

	static String str;

	va_list arglist; 
    va_start(arglist, s); 
    
	str.assignEx(s, arglist);

    va_end(arglist); 

	SoftPtr<ScriptEngine> eng = getScriptEngine(v);

	if (eng.isValid() && eng->mLog.isValid()) {

		eng->mLog->log(str.c_tstr());
	}

	if (eng.isValid() && eng->mResultString.isValid()) {
		
		eng->mResultString->append(str);
	}

	if (eng.isValid() && eng->mResultString.isValid()) {
		
		eng->mResultString->append(str);
	}

#ifdef _DEBUG
	OutputDebugString(str.c_tstr());
	OutputDebugString(L"\n");
#endif
} 

void ScriptEngine::output_console(HSQUIRRELVM v, const SQChar *s, ...) { 

	static String str;

	va_list arglist; 
	va_start(arglist, s); 
	    
	str.assignEx(s, arglist);

	va_end(arglist); 

	SoftPtr<ScriptEngine> eng = getScriptEngine(v);

	if (eng.isValid() && eng->mConsole.isValid()) {

		eng->mConsole->show(true, true);
		eng->mConsole->print(str.c_tstr(), true);
	}

	if (eng.isValid() && eng->mResultString.isValid()) {
		
		eng->mResultString->append(str);
	}

#ifdef _DEBUG
	OutputDebugString(str.c_tstr());
	OutputDebugString(L"\n");
#endif

} 

void ScriptEngine::output_both(HSQUIRRELVM v, const SQChar *s, ...) { 

	static String str;

	va_list arglist; 
    va_start(arglist, s); 
    
	str.assignEx(s, arglist);

    va_end(arglist); 

	SoftPtr<ScriptEngine> eng = getScriptEngine(v);
	
	if (eng.isValid() && eng->mLog.isValid()) {

		eng->mLog->log(str.c_tstr());
	}

	if (eng.isValid() && eng->mConsole.isValid()) {

		eng->mConsole->show(true, true);
		eng->mConsole->print(str.c_tstr(), true);
	}

	if (eng.isValid() && eng->mResultString.isValid()) {
		
		eng->mResultString->append(str);
	}

#ifdef _DEBUG
	OutputDebugString(str.c_tstr());
	OutputDebugString(L"\n");
#endif

}

void ScriptEngine::redirectOutput(OutputMode output) {

	if (output != Out_Current && output != mOutputMode) {

		switch (output) {

			case Out_Silent: {

				sq_setprintfunc(mVM.GetVMPtr(), output_silent);

			} break;
				
			case Out_Log: {

				sq_setprintfunc(mVM.GetVMPtr(), output_log);


			} break; 

			case Out_Console: {

				sq_setprintfunc(mVM.GetVMPtr(), output_console);

			} break;

			case Out_Both: {

				sq_setprintfunc(mVM.GetVMPtr(), output_both);


			} break;
		}

		mOutputMode = output;
	}
}



}
