#include "Controller_Virtual.h"

#include "InputManager.h"
#include "Source.h"

namespace WE { namespace Input {

Controller_Virtual::Controller_Virtual(const TCHAR* name, const TCHAR* selectName, float deviceDeadZone) 
	: mName(name), mSelectName(selectName), mDeviceDeadZone(deviceDeadZone) {
}

Controller_Virtual::~Controller_Virtual() {

	SoftRef<SrcImpl> var;

	VarMap::Iterator i = mVarMap.iterator();
	Index idx = 0;

	while (mVarMap.next(i, var.ptrRef())) {

		if (var->getMark() == this)
			var->mark(NULL);

		var.destroy();
	}
}

float Controller_Virtual::getDeadZone() {

	return mDeviceDeadZone;
}

bool Controller_Virtual::setName(const TCHAR* name) {

	mName.assign(name);

	return true;
}

Controller::Index Controller_Virtual::getSourceCount() {

	return mVarMap.getCount();
}

bool Controller_Virtual::addSource(SourceHolder& source, const TCHAR* name) {

	if (!source.isValid())
		return false;

	StringHash key = String::hash(name);

	if (!mVarMap.hasKey(key)) {

		SoftRef<SrcImpl> var;

		WE_MMemNew(var.ptrRef(), SrcImpl(source, name));

		if (!mVarMap.insert(key, var)) {

			assrt(false);
			WE_MMemDelete(var.ptrRef());
		}

		return var.isValid();
	}

	return false;
}

const TCHAR* Controller_Virtual::getName() {

	return mName.c_tstr();
}

bool Controller_Virtual::getSourceAt(const Index& index, SourceHolder& source, const Time& t) {

	if (index >= mVarMap.getCount())
		return NULL;

	SoftRef<SrcImpl> var;

	VarMap::Iterator i = mVarMap.iterator();
	Index idx = 0;

	while (mVarMap.next(i, var.ptrRef())) {

		if (idx == index) {

			return getSource(var->getKey(), source, t);
		}

		var.destroy();
	}

	return false;
}

bool Controller_Virtual::getSource(const TCHAR* name, SourceHolder& source, const Time& t) {

	StringHash key = String::hash(name);

	return getSource(key, source, t);
}

bool Controller_Virtual::getSource(const Key& key, SourceHolder& source, const Time& t) {

	SoftRef<SrcImpl> var;

	if (mVarMap.find(key, var.ptrRef())) {

		source.set(var);

	} else {

		source.destroy();
	}

	return source.isValid();
}

bool Controller_Virtual::SrcImpl::addTracker(InputManager& manager, Tracker* pTracker) {

	if (!Source::addTracker(manager, pTracker)) {

		return false;
	}

	if (mTrackers.count == 1) {

		if (!mRef->addTracker(manager, this)) {

			Source::removeTracker(manager, pTracker);

			return false;
		}
	}

	return true;
}

bool Controller_Virtual::SrcImpl::removeTracker(InputManager& manager, Tracker* pTracker) {

	if (Source::removeTracker(manager, pTracker)) {

		if (mTrackers.count == 0) {

			mRef->removeTracker(manager, this);
		}

		return true;
	}

	return false;
}

void Controller_Virtual::SrcImpl::onSourceChanged(Source* pVarImpl) {

	notifyTrackers();
}

} }