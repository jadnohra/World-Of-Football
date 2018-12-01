#ifndef _WOFScriptProxy_EntityController_h
#define _WOFScriptProxy_EntityController_h

#include "WE3/WERef.h"
using namespace WE;

#include "../../../../script/WOFScriptContext.h"

namespace WOF {


	/*
	bool Footballer_Body::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	Script_EntityController::bind(match.getScriptContext());

	SquirrelObject testScript;

	testScript = SquirrelVM::CompileBuffer(L" class EntExtend extends Script_EntityController { index = 0;}	");
	SquirrelVM::RunScript(testScript);

	SoftRef<EntityController> native;
	
	WE_MMemNew(native.ptrRef(), EntityController());

	Script_EntityController* pEnt = Script_EntityController::newInstance(match.getScriptContext(), L"entContr", native, L"EntExtend");

	testScript = SquirrelVM::CompileBuffer(L" entContr.start(); ");
	SquirrelVM::RunScript(testScript);

	testScript = SquirrelVM::CompileBuffer(L" entContr.update(0.5); ");
	SquirrelVM::RunScript(testScript);

	testScript = SquirrelVM::CompileBuffer(L" entContr.index = 66; ");
	SquirrelVM::RunScript(testScript);

	testScript = SquirrelVM::CompileBuffer(L" ::print(entContr.index); ");
	SquirrelVM::RunScript(testScript);

	return true;
}
	*/


	class EntityController : public Ref {
	public:

		virtual ~EntityController() {}

		virtual void start() {}
		virtual void update(const float& dt) {}
	};

	class Script_EntityController {
	public:

		static const TCHAR* ScriptClassName;
		
	public:

		static void bind(ScriptContext& context);

		//the lifetime is dependent on the script object's lifetime
		static Script_EntityController* newInstance(ScriptContext& context, const TCHAR* scriptName, EntityController& controller, 
					const TCHAR* scriptClassName = Script_EntityController::ScriptClassName);

		void start();
		void update(float dt);

	public:

		HardRef<EntityController> mNativeEntity;
		HSQOBJECT mScriptEntity; 

	protected:

		struct CreationContext {

			SoftPtr<EntityController> controller;
			SoftPtr<Script_EntityController> object;
		};

		static CreationContext creationContext;

	public:

		//don't use directly
		Script_EntityController();
	};
	
}

#endif