#include "Effect_Negate.h"

#include "InputManager.h"

namespace WE { namespace Input {

bool Effect_Negate::addTracker(InputManager& manager, Tracker* pTracker) { 
	
	if (!Source::addTracker(manager, pTracker))
		return false;

	if (mTrackers.count == 1) {

		if (!mRef->addTracker(manager, this)) {

			Source::removeTracker(manager, pTracker);
			return false;
		}
	}

	return true; 
}

bool Effect_Negate::removeTracker(InputManager& manager, Tracker* pTracker) { 
	
	if (Source::removeTracker(manager, pTracker)) {

		if (mTrackers.count == 0) {

			mRef->removeTracker(manager, this); 
		}

		return true;
	}

	return false;
}

void Effect_Negate::onSourceChanged(Source* pVarImpl) {

	Source::notifyTrackers();
}

} }