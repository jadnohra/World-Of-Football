#include "WOFScriptingEngine_ScriptProxy.h"
#include "WOFScriptingEngine.h"

namespace WOF {

const TCHAR* ScriptingEngine_ScriptProxy::ScriptClassName = L"ScriptingEngine";
ScriptingEngine_ScriptProxy::CreationContext ScriptingEngine_ScriptProxy::creationContext;

void ScriptingEngine_ScriptProxy::declareScriptClass(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptingEngine_ScriptProxy>(ScriptClassName).
		func(&ScriptingEngine_ScriptProxy::declareClass, L"declareClass").
		func(&ScriptingEngine_ScriptProxy::runScript, L"runScript").
		func(&ScriptingEngine_ScriptProxy::getWOFVer, L"getWOFVer");
}

ScriptingEngine_ScriptProxy* ScriptingEngine_ScriptProxy::create(SquirrelVM& squirrelVM, ScriptingEngine& engine, const TCHAR* globalVarName) {

	ScriptingEngine_ScriptProxy::declareScriptClass(squirrelVM);

	bool ret = false;
	HSQUIRRELVM VM =  squirrelVM.GetVMPtr();

	SQInteger top = sq_gettop(VM);

	sq_pushroottable(VM); 
	sq_pushstring(VM, ScriptingEngine_ScriptProxy::ScriptClassName, -1);

	if (SQ_SUCCEEDED(sq_get(VM, -2))) { //now class is on stack top

		sq_pushroottable(VM); //call param 1

		//set constructor args
		creationContext.engine = &engine;

		//we created object and called constructor
		if (SQ_SUCCEEDED(sq_call(VM, 1, SQTrue, SQTrue))) {

			#ifdef _DEBUG
			{
				HSQOBJECT po;
				sq_resetobject(&po);
				sq_getstackobj(VM, -1, &po);

				int i = 0;
			}
			#endif

			HSQOBJECT& po = creationContext.object->mScriptInstance;
			sq_getstackobj(VM, -1, &po);
			
			if (globalVarName != NULL) {

				//register as global variable
				sq_pushroottable(VM); 
				sq_pushstring(VM, globalVarName, -1);
				sq_pushobject(VM, po);

				if (!SQ_SUCCEEDED(sq_newslot(VM, -3, SQFalse))) {

					assrt(false);
				}
			} 

			//bind to native object lifetime
			sq_addref(VM, &po);
			creationContext.object->mScriptInstance.setValid(true);

			ret = true;
		} 
	}

	sq_settop(VM, top);

	return ret ? creationContext.object : NULL;
}

ScriptingEngine_ScriptProxy::ScriptingEngine_ScriptProxy() {

	creationContext.object = this;
	mEngine = creationContext.engine;
}

ScriptingEngine_ScriptProxy::~ScriptingEngine_ScriptProxy() {
}

bool ScriptingEngine_ScriptProxy::declareClass(const SQChar* className) {

	return mEngine->declareClass(className);
}

bool ScriptingEngine_ScriptProxy::runScript(const SQChar* scriptPath) {

	return mEngine->runScript(scriptPath);
}
		
const SQChar* ScriptingEngine_ScriptProxy::getWOFVer() {

	return mEngine->getWOFVer();
}

}
