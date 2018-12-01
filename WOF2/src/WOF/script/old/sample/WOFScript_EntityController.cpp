#include "WOFScript_EntityController.h"

namespace WOF {

const TCHAR* Script_EntityController::ScriptClassName = L"Script_EntityController";
Script_EntityController::CreationContext Script_EntityController::creationContext;


void Script_EntityController::bind(ScriptContext& context) {

	SqPlus::SQClassDef<Script_EntityController>(ScriptClassName).
		func(&Script_EntityController::start, L"start").
		func(&Script_EntityController::update, L"update");
}

Script_EntityController* Script_EntityController::newInstance(ScriptContext& context, const TCHAR* name, EntityController& controller, const TCHAR* className) {

	bool ret = false;
	creationContext.controller = &controller;
	HSQUIRRELVM VM = context.nativeContext();

	SQInteger top = sq_gettop(VM);

	sq_pushroottable(VM); 
	sq_pushstring(VM, className, -1);

	if (SQ_SUCCEEDED(sq_get(VM, -2))) { //now class is on stack top

		sq_pushroottable(VM); //call param 1

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

			HSQOBJECT& po = creationContext.object->mScriptEntity;
			sq_resetobject(&po);
			sq_getstackobj(VM, -1, &po);

			//register global variable
			sq_pushroottable(VM); 
			sq_pushstring(VM, name, -1);
			sq_pushobject(VM, po);

			if (SQ_SUCCEEDED(sq_newslot(VM, -3, SQFalse))) {

				ret = true;
			}
		} 
	}

	sq_settop(VM, top);

	return ret ? creationContext.object : NULL;
}

Script_EntityController::Script_EntityController() 
	: mNativeEntity(creationContext.controller) {

	sq_resetobject(&mScriptEntity);
	creationContext.object = this;
}

void Script_EntityController::start() { 

	mNativeEntity->start();
}

void Script_EntityController::update(float dt) { 

	mNativeEntity->update(dt);
}


}