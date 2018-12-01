#include "Controller.h"

#include "InputManager.h"
#include "InputClient.h"
#include "../Controller_Virtual.h"
#include "../Effect_Constant.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Input::ScriptController, CInputController);

namespace WE { namespace Input {

const TCHAR* ScriptController::ScriptClassName = L"CInputController";

void ScriptController::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptController>(ScriptClassName).
		func(&ScriptController::script_isValid, L"isValid").
		func(&ScriptController::script_isDevice, L"isDevice").
		func(&ScriptController::script_isVisible, L"isVisible").
		func(&ScriptController::script_setVisible, L"setVisible").
		func(&ScriptController::script_getManager, L"getManager").
		func(&ScriptController::script_getName, L"getName").
		func(&ScriptController::script_getDeadZone, L"getDeadZone").
		func(&ScriptController::script_getProductName, L"getProductName").
		func(&ScriptController::script_setName, L"setName").
		func(&ScriptController::script_setInputClient, L"setInputClient").
		func(&ScriptController::script_getInputClient, L"getInputClient").
		func(&ScriptController::script_getSourceCount, L"getSourceCount").
		func(&ScriptController::script_findSourceAt, L"findSourceAt").
		func(&ScriptController::script_findSource, L"findSource").
		func(&ScriptController::script_getSourceAt, L"getSourceAt").
		func(&ScriptController::script_getSource, L"getSource").
		func(&ScriptController::script_addSource, L"addSource").
		func(&ScriptController::script_addConstant, L"addConstant");
}

ScriptInputManager ScriptController::script_getManager() {

	return scriptArg(this->dref().getManager());
}

void ScriptController::script_setInputClient(ScriptInputClient* pClient) { dref().setInputClient(::dref(pClient)); }
ScriptInputClient ScriptController::script_getInputClient() { return scriptArg(dref().getInputClient()); }

ScriptSource ScriptController::script_getSource(const TCHAR* name, Time time) {

	ScriptSource ret;

	ptr()->getSource(name, ret, time);

	return ret;
}

ScriptSource ScriptController::script_getSourceAt(Controller::Index index, Time time) {

	ScriptSource ret;

	ptr()->getSourceAt(index, ret, time);

	return ret;
}

bool ScriptController::script_findSourceAt(Controller::Index index, ScriptSource* pVar, Time time) {

	if (pVar) {

		return ptr()->getSourceAt(index, *pVar, time);
	}

	return false;
}

bool ScriptController::script_findSource(const TCHAR* name, ScriptSource* pVar, Time time) {

	if (pVar) {

		return ptr()->getSource(name, *pVar, time);
	}

	return false;
}

bool ScriptController::script_addSource(ScriptSource* pVar, const TCHAR* name) {

	if (pVar) {

		return ptr()->addSource(*pVar, name);
	}

	assrt(false);
	return false;
}

bool ScriptController::script_addConstant(float value, const TCHAR* name) {

	SourceHolder var;

	Effect_Constant::create(value, name, var);
	
	return ptr()->addSource(var, name);
}

} }