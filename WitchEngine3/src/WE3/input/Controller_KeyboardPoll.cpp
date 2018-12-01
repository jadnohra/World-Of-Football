#include "Controller_KeyboardPoll.h"

#include "InputManager.h"
#include "SourceHolder.h"

#ifdef WIN32
#include "../binding/win/WEWinVirtualKeyCodeMap.h"
#endif

namespace WE { namespace Input {

Controller_KeyboardPoll::Controller_KeyboardPoll(const TCHAR* name) 
	: mName(name) {
}

Controller_KeyboardPoll::~Controller_KeyboardPoll() {

	for (Vars::Index i = 0; i < mVars.count; ++i) {

		mVars[i]->setParent(NULL);
	}
}

bool Controller_KeyboardPoll::setName(const TCHAR* name) {

	mName.assign(name);

	return true;
}

void Controller_KeyboardPoll::updateImpl(const Time& t) {

	for (Vars::Index i = 0; i < mVars.count; ++i) {

		mVars[i]->update(t);
	}
}

bool Controller_KeyboardPoll::getKey(const TCHAR* name, Key& key, int& code) {

	if (WinVirtualKeyCodeMap::getKeyCode(name, code)) {

		key = String::hash(name);
		return true;
	}

	return false;
}

Controller::Index Controller_KeyboardPoll::getSourceCount() {

	return WinVirtualKeyCodeMap::getKeyCodeCount();
}

const TCHAR* Controller_KeyboardPoll::getKey(const Index& index, Key& key, int& code) {

	const TCHAR* name = WinVirtualKeyCodeMap::getKeyCodeAt(index, code);

	if (name != NULL) {

		key = String::hash(name);
		return name;
	}

	return false;
}

Controller_KeyboardPoll::SrcImpl* Controller_KeyboardPoll::addSource(const TCHAR* name, const Time& t, const Key& key, const int& code) {

	if (!mVarMap.hasKey(key)) {

		SoftPtr<SrcImpl> var;

		WE_MMemNew(var.ptrRef(), SrcImpl(*this, code, name));

		if (!mVarMap.insert(key, var)) {

			assrt(false);
			WE_MMemDelete(var.ptrRef());
		}

		mVars.addOne(var);
		var->update(t);

		return var;
	}

	return NULL;
}


void Controller_KeyboardPoll::onAddRef(SrcImpl* pVar) {

	if (pVar->getRefCount() == 2) {

		addActiveCount();
	}
}

void Controller_KeyboardPoll::onRelease(SrcImpl* pVar) {

	if (pVar->getRefCount() == 1) {
		
		removeActiveCount();

		if (pVar->hasTrackers()) {

			//forgot to remove tracker
			assrt(false);
		}

		mVarMap.remove(pVar->getKey());

		Vars::Index index;

		if (mVars.searchFor(pVar, 0, mVars.count, index)) {

			mVars.removeSwapWithLast(index, true);
		}
	} 
}

const TCHAR* Controller_KeyboardPoll::getName() {

	return mName.c_tstr();
}

bool Controller_KeyboardPoll::getSourceAt(const Index& index, SourceHolder& source, const Time& t) {

	Key key;
	int code;

	const TCHAR* keyName;

	if ((keyName = getKey(index, key, code)) == NULL)
		return false;

	return getSource(key, code, keyName, source, t);
}

bool Controller_KeyboardPoll::getSource(const TCHAR* name, SourceHolder& source, const Time& t) {

	Key key;
	int code;

	if (!getKey(name, key, code))
		return false;

	return getSource(key, code, name, source, t);
}

bool Controller_KeyboardPoll::getSource(const Key& key, const int& code, const TCHAR* name, SourceHolder& source, const Time& t) {

	SoftPtr<SrcImpl> var;
	
	if (mVarMap.find(key, var.ptrRef())) {

		source.set(var);

	} else {

		source.set(addSource(name, t, key, code));
	}

	return source.isValid();
}

void Controller_KeyboardPoll::SrcImpl::update(const Time& t) {

	float newValue = GetAsyncKeyState(mCode) < 0 ? 1.0f : 0.0f;
	
	if (newValue != mValue) {

		mValue = newValue;
		mTime = t;

		notifyTrackers();
	}
}

} }