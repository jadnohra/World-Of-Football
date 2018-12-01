#include "InputManager.h"
#include "Controller.h"
#include "Effect_Button.h"
#include "SourceHolder.h"
#include "InputClient.h"

namespace WE { namespace Input {

Controller::Controller() : mLastUpdateTime(-1.0f), mActiveCount(0), mIsVisible(true) {}
Controller::~Controller() {}

void Controller::setManager(InputManager* pManager) { 

	mManager = pManager; 
}

InputManager* Controller::getManager() { 
	
	return mManager; 
}

bool Controller::createButton(const TCHAR* variableName, const TCHAR* buttonName, const Time& t, SourceHolder& outVar, bool addButton, const TCHAR* addButtonName) {

	Input::SourceHolder source;

	if (getSource(variableName, source, t)) {

		if (Effect_Button::create(mManager, source, buttonName, outVar)) {

			if (addButton) {

				if (addSource(outVar, addButtonName ? addButtonName : buttonName)) {

					return true;

				} else {

					outVar.destroy();

					return false;
				}

			} else {

				return true;
			}
		}

		return false;
	}

	return false;
}

void Controller::setInputClient(InputClient* pClient) {

	mInputClient = pClient;
}

InputClient* Controller::getInputClient() {

	return mInputClient;
}

} }