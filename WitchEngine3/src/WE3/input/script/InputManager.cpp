#include "InputManager.h"

#include "Controller.h"
#include "../Controller_Virtual.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Input::ScriptInputManager, CInputManager);

namespace WE { namespace Input {

const TCHAR* ScriptInputManager::ScriptClassName = L"CInputManager";

void ScriptInputManager::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptInputManager>(ScriptClassName).
		func(&ScriptInputManager::script_isValid, L"isValid").
		func(&ScriptInputManager::script_getControllerCount, L"getControllerCount").
		func(&ScriptInputManager::script_findController, L"findController").
		func(&ScriptInputManager::script_getController, L"getController").
		func(&ScriptInputManager::script_addController, L"addController").
		func(&ScriptInputManager::script_createVirtualController, L"createVirtualController");
}

ScriptController ScriptInputManager::script_getController(InputManager::Index index) {

	SoftRef<Controller> controller = ptr()->getController(index);

	return ScriptController(controller);
}

bool ScriptInputManager::script_findController(InputManager::Index index, ScriptController* pController) {

	if (pController) {

		SoftRef<Controller> controller = ptr()->getController(index);

		pController->set(controller);

		return controller.isValid();
	} 

	return false;
}

bool ScriptInputManager::script_addController(ScriptController* pController) {

	if (pController) {

		ptr()->addController(pController->ptr());

		return true;
	} 

	assrt(false);
	return false;
}

ScriptController ScriptInputManager::script_createVirtualController(const TCHAR* name) {

	SoftRef<Controller_Virtual> controller;

	WE_MMemNew(controller.ptrRef(), Controller_Virtual(name));
	controller->setManager(this->ptr());

	return (ScriptController(controller));
}

} }