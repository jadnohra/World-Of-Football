#include "WEStringPart.h"

#include "../WEMacros.h"
#include "tchar.h"

namespace WE {

StringPartBase::StringPartBase() : startIndex(-1), count(0) {
}

void StringPartBase::destroy() {

	startIndex = -1;
	count = 0;
}

int StringPartBase::length() const {

	return count;
}

int StringPartBase::lastIndex() const {

	return (startIndex + count) - 1;
}

void StringPartBase::getRemainingPart(const TCHAR* buff, StringPartBase& remain, int len) const {

	remain.destroy();

	if (len == 0) {

		len = String::length(buff, false);
	}

	if ((buff == NULL) || (len == 0)) {

		return;
	}

	remain.startIndex = lastIndex() + 1;
	remain.count = len - remain.startIndex;
}

void StringPartBase::getFirstPart(const TCHAR* buff, StringPartBase& first) const {

	first.destroy();
	
	if (buff == NULL) {

		return;
	}

	first.startIndex = 0;
	first.count = startIndex;
}

ULONG StringPartBase::hash(const TCHAR* buff) const {

	if ((count <= 0) || (buff == NULL)) {

		return 0;
	}

	return String::hash(buff + startIndex, count);
}

void StringPartBase::assignTo(const TCHAR* buff, String& string) const {

	if (count == 0) {

		string.destroy();

	} else {

		string.assign(buff + startIndex, count);
	}
}

void StringPartBase::appendTo(const TCHAR* buff, String& string) const {

	string.append(buff + startIndex, count);
}

bool StringPartBase::equals(const TCHAR* buff, const TCHAR* comp, int size) const {

	if (size == 0) {

		size = safeSizeCast<size_t, int>(_tcslen(comp));
	}

	if (count != size) {

		return false;
	}

	if (count == 0) {

		return true;
	}

	int endIndex = startIndex + count;
	int compIndex = 0;

	for (int i = startIndex; i < endIndex; i++) {

		if (buff[i] != comp[compIndex++]) {

			return false;
		}
	}

	return true;
}



StringPart::StringPart(const String* _pRef) : pRef(_pRef) {
}

void StringPart::destroy() {

	StringPartBase::destroy();
	pRef = NULL;
}


ULONG StringPart::hash() const {

	return StringPartBase::hash(pRef ? pRef->mBuffer : NULL);
}

bool StringPart::equals(const TCHAR* comp, int size) const {

	return StringPartBase::equals(pRef ? pRef->mBuffer : NULL, comp, size);
}

void StringPart::assignTo(String& string) const {

	if (pRef) {
		
		StringPartBase::assignTo(pRef->mBuffer, string);
	} else {

		string.destroy();
	}
}


void StringPart::appendTo(String& string) const {

	if (pRef) {
		
		StringPartBase::appendTo(pRef->mBuffer, string);
	} 
}

/*
bool StringPart::equals(const String& comp) {

	if (count + 1 != comp.mSize) {
		return false;
	}

	if (count == 0) {
		return true;
	}

	int endIndex = startIndex + count;
	for (int i = startIndex; i < endIndex; i++) {
		if (pRef->mBuffer[i] != comp.mBuffer[i]) {
			return false;
		}
	}

	return true;
}
*/

}