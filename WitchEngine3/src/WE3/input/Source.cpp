#include "Source.h"

#include "Tracker.h"
#include "InputManager.h"
#include "Controller.h"

namespace WE { namespace Input {

bool Source::addTracker(InputManager& manager, Tracker* pTracker) {

	mTrackers.addOne(pTracker);

	return true;
}

bool Source::removeTracker(InputManager& manager, Tracker* pTracker) {

	Trackers::Index index;

	if (mTrackers.searchFor(pTracker, 0, mTrackers.count, index)){

		mTrackers.removeSwapWithLast(index, true);
		return true;
	}

	return false;
}

void Source::notifyTrackers() {

	for (Trackers::Index i = 0; i < mTrackers.count; ++i) {

		mTrackers[i]->onSourceChanged(this);
	}
}

bool Source::isMarked() { return getMark() != NULL; }

} }

