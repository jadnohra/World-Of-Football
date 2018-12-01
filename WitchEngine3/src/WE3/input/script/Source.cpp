#include "Source.h"
#include "Tracker.h"

#include "../Effect_Constant.h"
#include "../Effect_Negate.h"
#include "../Effect_Button.h"
#include "../Effect_2SourcesToAxis.h"
#include "../Effect_2AxisToDir2D.h"
#include "../Effect_And.h"

#include "InputManager.h"
#include "Controller.h"
#include "../ComponentIDs.h"
#include "ButtonComponent.h"
#include "Generic1DComponent.h"
#include "Dir2DComponent.h"

DECLARE_INSTANCE_TYPE_NAME(WE::Input::ScriptSource, CInputSource);

namespace WE { namespace Input {

const TCHAR* ScriptSource::ScriptClassName = L"CInputSource";

void ScriptSource::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptSource>(ScriptClassName).
		enumInt(ComponentID_None, L"ComponentID_None").
		enumInt(ComponentID_Button, L"ComponentID_Button").
		enumInt(ComponentID_Generic1D, L"ComponentID_Generic1D").
		enumInt(ComponentID_Dir2D, L"ComponentID_Dir2D").
		func(&ScriptSource::script_isValid, L"isValid").
		func(&ScriptSource::script_getTime, L"getTime").
		func(&ScriptSource::script_getName, L"getName").
		func(&ScriptSource::script_addTracker, L"addTracker").
		func(&ScriptSource::script_removeTracker, L"removeTracker").
		func(&ScriptSource::script_getBaseSource, L"getBaseSource").
		func(&ScriptSource::script_createNegative, L"createNegative").
		func(&ScriptSource::script_createButton, L"createButton").
		staticFunc(&ScriptSource::script_createAnd, L"createAnd").
		func(&ScriptSource::script_hasComponent, L"hasComponent").
		func(&ScriptSource::script_isButton, L"isButton").
		func(&ScriptSource::script_isGeneric1D, L"isGeneric1D").
		func(&ScriptSource::script_isDir2D, L"isDir2D").
		func(&ScriptSource::script_getButton, L"getButton").
		func(&ScriptSource::script_getGeneric1D, L"getGeneric1D").
		func(&ScriptSource::script_getDir2D, L"getDir2D").
		staticFunc(&ScriptSource::script_createAxisFrom2Sources, L"createAxisFrom2Sources").
		staticFunc(&ScriptSource::script_createDir2DFrom2Axis, L"createDir2DFrom2Axis").
		staticFunc(&ScriptSource::script_createConstant, L"createConstant").
		staticFunc(&ScriptSource::script_createAnd, L"createAnd");
}

bool ScriptSource::script_addTracker(ScriptInputManager* pManager, ScriptTracker* pTracker) { return dref().addTracker(pManager->dref(), pTracker); }
bool ScriptSource::script_removeTracker(ScriptInputManager* pManager, ScriptTracker* pTracker) { return dref().removeTracker(pManager->dref(), pTracker); }

ScriptSource ScriptSource::script_getBaseSource() {

	return scriptArg(SoftRef<Source>(this->dref().getBaseSource()).ptr());
}

ScriptButton ScriptSource::script_getButton() {

	return scriptArg((Component_Button*) this->dref().getComponent(ComponentID_Button));
}

ScriptGeneric1D ScriptSource::script_getGeneric1D() {

	return scriptArg((Component_Generic1D*) this->dref().getComponent(ComponentID_Generic1D));
}

ScriptDir2D ScriptSource::script_getDir2D() {

	return scriptArg((Component_Dir2D*) this->dref().getComponent(ComponentID_Dir2D));
}

bool ScriptSource::script_hasComponent(int compID) {

	return (this->dref().getComponent(compID)) != NULL;
}

bool ScriptSource::script_isButton() {

	return (this->dref().getComponent(ComponentID_Button)) != NULL;
}

bool ScriptSource::script_isGeneric1D() {

	return (this->dref().getComponent(ComponentID_Generic1D)) != NULL;
}

bool ScriptSource::script_isDir2D() {

	return (this->dref().getComponent(ComponentID_Dir2D)) != NULL;
}

ScriptSource ScriptSource::script_createButton(ScriptInputManager* pManager, const TCHAR* name) {

	ScriptSource ret;

	if (isValid()) {

		Effect_Button::create(::dref(pManager).dref(), *this, name, ret);
	}

	return ret;
}

ScriptSource ScriptSource::script_createNegative(ScriptInputManager* pManager, const TCHAR* name) {

	ScriptSource ret;

	if (isValid()) {

		Effect_Negate::create(*this, name, ret);
	}

	return ret;
}

ScriptSource ScriptSource::script_createAnd(ScriptInputManager* pManager, ScriptSource* pArg1, ScriptSource* pArg2, const TCHAR* name) {

	ScriptSource ret;

	if (pArg1 && pArg2 && pArg1->isValid() && pArg2->isValid()) {

		Effect_And::create(::dref(pManager).dref(), *pArg1, *pArg2, name, ret);
	}

	return ret;
}

ScriptSource ScriptSource::script_createDir2DFrom2Axis(ScriptInputManager* pManager, ScriptSource* pAxis1, ScriptSource* pAxis2, const TCHAR* name) {

	ScriptSource ret;

	if (pAxis1 && pAxis2 && pAxis1->isValid() && pAxis2->isValid()) {

		Effect_2AxisToDir2D::create(::dref(pManager).dref(), *pAxis1, *pAxis2, name, ret);
	}

	return ret;
}

ScriptSource ScriptSource::script_createAxisFrom2Sources(ScriptInputManager* pManager, ScriptSource* pNeg, ScriptSource* pPos, const TCHAR* name) {

	ScriptSource ret;

	if (pNeg && pPos && pNeg->isValid() && pPos->isValid()) {

		Effect_2SourcesToAxis::create(::dref(pManager).dref(), *pNeg, *pPos, name, ret);
	}

	return ret;
}

ScriptSource ScriptSource::script_createConstant(ScriptInputManager* pManager, const TCHAR* name, float value) {

	ScriptSource ret;

	Effect_Constant::create(value, name, ret);
	
	return ret;
}

} }