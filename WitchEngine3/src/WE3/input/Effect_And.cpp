#include "Effect_And.h"

#include "SourceHolder.h"
#include "InputManager.h"
#include "ComponentUtil.h"

namespace WE { namespace Input {

bool Effect_And::create(InputManager& manager, Source& arg1, Source& arg2, const TCHAR* name, SourceHolder& source) {

	SoftRef<Effect_And> ret;

	WE_MMemNew(ret.ptrRef(), Effect_And(manager, arg1, arg2, name));

	if (!ret->init()) {

		ret.destroy();
	}

	source.set(ret);

	return source.isValid();
}

Effect_And::Effect_And(InputManager& manager, Source& arg1, Source& arg2, const TCHAR* name) 
	: mManager(&manager), mArg1(&arg1), mArg2(&arg2), mName(name) {
}


bool Effect_And::init() {

	SoftPtr<Component_Generic1D> compArg1 = comp1D(mArg1);
	SoftPtr<Component_Generic1D> compArg2 = comp1D(mArg2);

	if(compArg1.isValid() && compArg2.isValid()
			&& mArg1->addTracker(mManager, this) 
			&& mArg2->addTracker(mManager, this)) {

		setValue(compArg1->getValue(), compArg2->getValue(), false);

		return true;
	}

	return false;
}

Effect_And::~Effect_And() {

	mArg1->removeTracker(mManager, this);
	mArg2->removeTracker(mManager, this);
}

void Effect_And::onSourceChanged(Source* pVarImpl) {

	SoftPtr<Component_Generic1D> compArg1 = comp1D(mArg1);
	SoftPtr<Component_Generic1D> compArg2 = comp1D(mArg2);

	setValue(compArg1->getValue(), compArg2->getValue(), true);
}

void Effect_And::setValue(const float& v1, const float& v2, const bool& notify) {

	float oldValue = mValue;
	
	mValue = (v1 != 0.0f) && (v2 != 0.0f) ? 1.0f : 0.0f;
	
	if (notify && (oldValue != mValue))
		notifyTrackers();
}

} }