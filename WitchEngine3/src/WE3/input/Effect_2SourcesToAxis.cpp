#include "Effect_2SourcesToAxis.h"

#include "SourceHolder.h"
#include "InputManager.h"
#include "ComponentUtil.h"

namespace WE { namespace Input {

bool Effect_2SourcesToAxis::create(InputManager& manager, Source& neg, Source& pos, const TCHAR* name, SourceHolder& source) {

	SoftRef<Effect_2SourcesToAxis> ret;

	WE_MMemNew(ret.ptrRef(), Effect_2SourcesToAxis(manager, neg, pos, name));

	if (!ret->init()) {

		ret.destroy();
	}

	source.set(ret);

	return source.isValid();
}

Effect_2SourcesToAxis::Effect_2SourcesToAxis(InputManager& manager, Source& neg, Source& pos, const TCHAR* name) 
	: mManager(&manager), mNeg(&neg), mPos(&pos), mName(name) {
}


bool Effect_2SourcesToAxis::init() {

	SoftPtr<Component_Generic1D> compNeg = comp1D(mNeg);
	SoftPtr<Component_Generic1D> compPos = comp1D(mPos);

	if (compNeg.isValid() && compPos.isValid()
			&& mNeg->addTracker(mManager, this) 
			&& mPos->addTracker(mManager, this)) {


		if (compPos->getValue() == compPos->getMax()) {

			mValue = 1.0f;

		} else if (compNeg->getValue() == compNeg->getMax()) {

			mValue = -1.0f;

		} else {

			mValue = 0.0f;
		}

		return true;
	}

	return false;
}

Effect_2SourcesToAxis::~Effect_2SourcesToAxis() {

	mNeg->removeTracker(mManager, this);
	mPos->removeTracker(mManager, this);
}

void Effect_2SourcesToAxis::onSourceChanged(Source* pVarImpl) {

	SoftPtr<Component_Generic1D> compNeg = comp1D(mNeg);
	SoftPtr<Component_Generic1D> compPos = comp1D(mPos);

	float newValue;

	if (compPos->getValue() == compPos->getMax()) {

		if (compNeg->getValue() == compNeg->getMax()) {

			if (mValue != 0.0f)
				newValue = mValue;

		} else {

			newValue = 1.0f;
		}

	} else if (compNeg->getValue() == compNeg->getMax()) {

		newValue = -1.0f;

	} else {

		newValue = 0.0f;
	}

	if (newValue != mValue) {

		mValue = newValue;
		notifyTrackers();
	}

}

} }