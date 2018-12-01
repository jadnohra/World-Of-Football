#include "InputClient.h"

namespace WE { namespace Input {

InputClient::InputClient(InputManager* pManager) {

	mIsIteratingEvents = false;
	setManager(pManager);
}

InputClient::~InputClient() {

	destroy();

	setManager(NULL);
}

void InputClient::setManager(InputManager* pManager) {

	if (mManager) {

		mManager->removeDestructorNotifiable(this);
	}

	mManager = pManager;

	if (mManager) {

		mManager->addDestructorNotifiable(this);
	}
}

void InputClient::inputDestructNotify(void* pObject) {

	destroy();

	mManager.destroy();
}

void InputClient::destroy() {

	for (Index i = 0; i < mSources.count;  ++i) {

		stopTracking(mManager, mSources[i]);
	}

	mSources.destroy();
	mIDByVarMap.destroy();
	mVarByIDMap.destroy();

	mPendingEvents.count = 0;
	mEvents.count = 0;
}

bool InputClient::addSource(Source* pSource, bool mapIDByVar, bool mapSourceByID, IDType ID) {

	if (startTracking(mManager, pSource)) {

		if (mapSourceByID) {

			if (mapIDByVar) {

				mVarByIDMap.insert(ID, pSource);

			} else {

				assrt(false);

				stopTracking(mManager, pSource);
				return false;
			}
		}

		pSource->AddRef();
		mSources.addOne(pSource);

		if (mapIDByVar) {

			mIDByVarMap.insert(pSource, ID);
		}
		

		return true;
	}
	
	assrt(false);
	return false;
}

bool InputClient::removeSource(Source* pSource) {

	stopTracking(mManager, pSource);

	Index i;

	if (mSources.searchFor(pSource, 0, mSources.count, i)) {

		mSources.removeSwapWithLast(i, true);
	}

	IDType ID;

	if (mIDByVarMap.find(pSource, ID)) {

		mIDByVarMap.remove(pSource);
		mVarByIDMap.remove(ID);
	}
	
	return true;
}

bool InputClient::hasSourceID(Source* pSource) {

	return mIDByVarMap.hasKey(pSource);
}

bool InputClient::getSourceID(Source* pSource, IDType& ID) {

	return mIDByVarMap.find(pSource, ID);
}

Source* InputClient::getSourceByID(IDType ID) {

	SoftPtr<Source> ret;

	mVarByIDMap.find(ID, ret.ptrRef());

	return ret;
}

bool InputClient::startTracking(InputManager& manager, Source* pSource) { 
	
	return pSource->addTracker(manager, this); 
}

bool InputClient::stopTracking(InputManager& manager, Source* pSource) { 
	
	return pSource->removeTracker(manager, this); 
}

void InputClient::onSourceChanged(Source* pSource) { 

	if (mIsIteratingEvents) {
		
		mPendingEvents.addOne(pSource); 

	} else {

		mEvents.addOne(pSource); 
	}
}

Source* InputClient::startIterateEvents() {

	if (mIsIteratingEvents) {

		assrt(false);
		return NULL;
	}

	if (mEvents.count) {

		mIsIteratingEvents = true;
		mIterateIndex = 0;

		return nextEvent();
	} 

	return NULL;
}

Source* InputClient::nextEvent() {

	if (mIterateIndex == mEvents.count) {

		breakIterateEvents();
		return NULL;
	}

	SoftPtr<Source> ret = mEvents[mIterateIndex];

	++mIterateIndex;

	return ret;
}

void InputClient::breakIterateEvents() {

	if (mIsIteratingEvents) {

		/*
			This is not entirely safe, events might ultimately get lost
		*/

		mEvents.count = 0;

		for (Index i = 0; i < mPendingEvents.count; ++i) {

			mEvents.addOne(mPendingEvents[i]);
		}

		mIsIteratingEvents = false;
		mPendingEvents.count = 0;
	}
}

void InputClient::clearEvents() {

	mPendingEvents.count = 0;
	mEvents.count = 0;
}

} }

