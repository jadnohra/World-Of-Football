#include "Effect_Button.h"

#include "SourceHolder.h"
#include "InputManager.h"
#include "ComponentIDs.h"
#include "ComponentUtil.h"

namespace WE { namespace Input {

	
bool Effect_Button::create(InputManager& manager, Source& ref, const TCHAR* name, SourceHolder& source) {

	SoftRef<Effect_Button> ret;

	WE_MMemNew(ret.ptrRef(), Effect_Button(manager, ref, name));

	if (!ret->init()) {

		ret.destroy();
	}

	source.set(ret);

	return source.isValid();
}

Effect_Button::Effect_Button(InputManager& manager, Source& ref, const TCHAR* name) 
			: mManager(&manager),
				mRef(&ref), mName(name), 
				mDetectTap(false), mDetectHold(false), mDetectOn(false),
				mTapInterval(0.0f), mTapCount(0), mTapEventTime(0.0f), mTapCountLimit(0), mTapLimitReached(false), 
				mHoldEventTime(0.0f),
				mIsTapping(false), mIsHolding(false), mIsOn(false) {
}

void* Effect_Button::getComponent(int compID) {

	if (compID == ComponentID_Button)
		return (Component_Button*) this;

	return this;
}

bool Effect_Button::init() {

	return comp1D(mRef) && mRef->addTracker(mManager, this);
}

Effect_Button::~Effect_Button() {

	if (mTrackers.count)
		mManager->removeUpdatable(this);

	mRef->removeTracker(mManager, this);
}

bool Effect_Button::addTracker(InputManager& manager, Tracker* pTracker) {

	if (mTrackers.count == 0 && pTracker) {

		manager.addUpdatable(this);
	}

	mTrackers.addOne(pTracker);

	return true;
}

bool Effect_Button::removeTracker(InputManager& manager, Tracker* pTracker) {

	Trackers::Index index;

	if (mTrackers.searchFor(pTracker, 0, mTrackers.count, index)){

		mTrackers.removeSwapWithLast(index, true);

		if (mTrackers.count == 0) {

			manager.removeUpdatable(this);
		}

		return true;
	}

	return false;
}

void Effect_Button::notifyTrackers() {

	for (Trackers::Index i = 0; i < mTrackers.count; ++i) {

		mTrackers[i]->onSourceChanged(this);
	}
}

void Effect_Button::onSourceChanged(Source* pVarImpl) {

	bool stateChanged = false;

	SoftPtr<Component_Generic1D> val = comp1D(dref(pVarImpl));

	if (mDetectTap && mTapInterval) {

		bool isTap = mIsTapping;
		
		Time time = pVarImpl->getTime();

		if (mTapCount == 0) {

			mTapEventTime = time;
			mTapLimitReached = false;
		}

		bool tapLimitReached = mTapLimitReached;

		float interval = time - mTapEventTime;

		if (val->getValue() != 0.0f) {

			mIsTapping = true;
			++mTapCount;
		}

		if (interval > mTapInterval) {

			mIsTapping = false;
		}

		if (mIsTapping && mTapCountLimit > 0 && mTapCount > mTapCountLimit) {

			mTapLimitReached = true;
		}

		mTapEventTime = time;

		if (isTap != mIsTapping || tapLimitReached != mTapLimitReached) {

			stateChanged = true;
		} 
	} 

	if (mDetectHold) {

		bool isHold = mIsHolding;

		if (val->getValue() != 0.0f) {

			mIsHolding = true;
			mHoldEventTime = pVarImpl->getTime();
			
		} else {

			mHoldTime = pVarImpl->getTime() - mHoldEventTime;

			mIsHolding = false;
		}

		if (isHold != mIsHolding)
			stateChanged = true;
	}

	if (mDetectOn) {

		bool isOn = mIsOn;

		mIsOn = val->getValue() != 0.0f;

		if (isOn != mIsOn)
			stateChanged = true;
	}
	
	if (stateChanged)
		notifyTrackers();
}

void Effect_Button::setTapInterval(float tapInterval) {

	mTapInterval = tapInterval;
}

void Effect_Button::inputUpdate(const Time& time) {

	if (mDetectTap && mTapInterval) {

		if (mIsTapping) {

			float interval = time - mTapEventTime;

			if (interval > mTapInterval) {

				mIsTapping = false;
				notifyTrackers();
			}
		}
	}
}

bool Effect_Button::getIsTapping() {

	if (mIsTapping) {

		return !mTapLimitReached;
	}

	return false;
}

bool Effect_Button::getIsHolding() {

	return mIsHolding;
}

bool Effect_Button::getIsOn() {

	return mIsOn;
}

int Effect_Button::getTapCount(bool reset) {

	int ret = mTapCount;

	if (reset)
		mTapCount = 0;

	if (mTapCountLimit > 0 && ret > mTapCountLimit)
		ret = mTapCountLimit;

	return ret;
}

float Effect_Button::getHoldTime(Time time, bool reset) {

	if (mIsHolding) {

		if (reset)
			mHoldEventTime = time;

		return time - mHoldEventTime;
	} 

	float ret = mHoldTime;

	if (reset)
		mHoldTime = 0.0f;

	return ret;
}

} }