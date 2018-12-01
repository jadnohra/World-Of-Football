#ifndef _WEDeclareClasses_h_Input
#define _WEDeclareClasses_h_Input

#include "../../script/WEScriptEngine.h"

#include "Source.h"
#include "Controller.h"
#include "InputManager.h"
#include "Tracker.h"
#include "ChangeQueue.h"
#include "InputClient.h"
#include "Generic1DComponent.h"
#include "ButtonComponent.h"
#include "Dir2DComponent.h"

#include "HoldCycle.h"


namespace WE { namespace Input {

	class DeclareClasses {
	public:

		static void declareInVM(SquirrelVM& target) {

			ScriptSource::declareInVM(target);
			ScriptTracker::declareInVM(target);
			ScriptController::declareInVM(target);
			ScriptInputManager::declareInVM(target);
			ScriptChangeQueue::declareInVM(target);
			ScriptInputClient::declareInVM(target);
			ScriptGeneric1D::declareInVM(target);
			ScriptButton::declareInVM(target);
			ScriptDir2D::declareInVM(target);

			ScriptHoldCycle::declareInVM(target);
		}
	};

} }

#endif