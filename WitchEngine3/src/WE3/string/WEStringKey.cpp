#include "WEStringKey.h"

namespace WE {


StringKey::StringKey() : mHash(0) {
}

StringKey::StringKey(const TCHAR* key) {

	assign(key);
}

StringKey::StringKey(const String& key) {

	assign(key);
}

StringKey::StringKey(const TCHAR* str, const StringPartBase& key) {

	assign(str, key);
}

StringKey::StringKey(const StringPart& key) {

	assign(key);
}

void StringKey::assign(const TCHAR* key)  {

	mString.assign(key);
	mHash = mString.hash();
}

void StringKey::assign(const String& key) {

	mString.assign(key);
	mHash = mString.hash();
}

void StringKey::assign(const TCHAR* str, const StringPartBase& key) {

	key.assignTo(str, mString);
	mHash = mString.hash();
}

void StringKey::assign(const StringPart& key) {

	key.assignTo(mString);
	mHash = mString.hash();
}

StringKey::~StringKey() {
}

const String& StringKey::getString() {

	return mString;
}

StringKey::operator WETL::Index::Type () const {

	return (WETL::Index::Type) mHash;
}

StringKey::operator WETL::IndexShort::Type () const {

	return (WETL::IndexShort::Type) mHash;
}

bool StringKey::operator==(const StringKey& comp) const {

	return mString.equals(comp.mString);
}

StringKey& StringKey::operator=(const StringKey& ref) {

	mString.assign(ref.mString);
	mHash = ref.mHash;

	return *this;
}

}