#include "WEString.h"

#pragma warning( disable : 4995 4996)

#include "../WEMem.h"
#include "../WEMacros.h"
#include "tchar.h"
//#include "strsafe.h"
#include "stdlib.h"
#include "string.h"



namespace WE {


String::String() : /*mIsRef(false), */mSize(0), mBuffer(NULL) {
}

String::String(const TCHAR* string) : /*mIsRef(false), */mSize(0), mBuffer(NULL) {
	
	assign(string);
}

/*
#ifdef  UNICODE
String::String(const char* string) : mIsRef(false), mSize(0), mBuffer(NULL) {
	
	assign(string);
}
#endif
*/

String::String(const String& string) : /*mIsRef(false), */mSize(0), mBuffer(NULL) {
	
	assign(string, string.length());
}


String::~String() {

	destroy();
}

void String::destroy() {

	if (mBuffer) {

		//if (mIsRef == false) {
			
			MMemFree(mBuffer);
		//}
		
		mSize = 0;
	}

	//mIsRef = false;
}

const TCHAR* String::c_tstr() const {

	return mBuffer;
}

int String::length() const {

	return mSize == 0 ? 0 : mSize - 1;
}

bool String::isEmpty() const {

	return mSize <= 1;
}

bool String::isValid() const {

	return mSize > 1;
}

int String::length(const TCHAR* string, bool countNull) {

	if (string == NULL) {

		return 0;
	}

	return safeSizeCast<size_t, int>(_tcslen(string) + (countNull ? 1 : 0));
}

/*
void String::unref() {

	if (mIsRef) {

		if (mSize > 1) {
			
			const TCHAR* ref = mBufferRef;

			mBuffer = (TCHAR*) malloc(mSize * sizeof(TCHAR));
			memcpy(mBuffer, ref, (mSize) * sizeof(TCHAR));

		} else {

			mSize = 0;
			mBuffer = NULL;
		}

		mIsRef = false;
	}
}
*/

void String::assign(const String& string, int count) {
	
	if (count == 0) {

		count = string.mSize - 1;
	}

	assign(string.mBuffer, count);
}

/*
void String::assign(const StringPart& string) {

	if ((string.length() == 0) || (string.pRef == NULL)) {

		destroy();
	}

	assign(string.pRef->mBuffer + string.startIndex, string.count);
}
*/

void String::assign(const TCHAR* string, int count) {
	
	int size;

	if (count == 0) {

		if (string) {

			size = (string == NULL ? 0 : safeSizeCast<size_t, int>(_tcslen(string) + 1));
		} else {

			size = 0;
		}

	} else {

		size = count + 1;
	}

	
	if (size == 0) {

		destroy();

	} else {

		/*
		if (mIsRef) {

			destroy();
		}
		*/

		mBuffer = (TCHAR*) realloc(mBuffer, size * sizeof(TCHAR));
		
		memcpy(mBuffer, string, (size - 1) * sizeof(TCHAR));
		mBuffer[size - 1] = L'\0';
	}

	mSize = size;
}

/*
#ifdef  UNICODE
void String::assign(const char* string, int count) {

	int size;

	if (count == 0) {

		 size = (string == NULL ? 0 : safeSizeCast<size_t, int>(strlen(string) + 1));
	} else {

		size = count + 1;
	}

	if (size == 0) {

		destroy();
		return;
	}
	
	TCHAR* tempBuffer = (TCHAR*) malloc(size * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, 0, string, -1, tempBuffer, size - 1);
	tempBuffer[size - 1] = L'\0';

	assign(tempBuffer, size - 1);

	MMemFree(tempBuffer);
}
#endif
*/

#ifdef _DEBUG
void String::debug(const TCHAR *format, ...) {

	static String debugString;

	va_list ap;
	va_start (ap, format);

	debugString.assignEx(format, ap);

	va_end (ap);

#ifdef _WIN32
	OutputDebugString(debugString.c_tstr());
#endif
}
#endif

TCHAR* String::setSize(int len) {

	destroy();

	mSize = len;

	if (mSize) {

		MMemAlloc(mBuffer, mSize * sizeof(TCHAR), TCHAR*);
		//mBuffer = (TCHAR*) malloc(mSize * sizeof(TCHAR));
	}

	return mBuffer;
}

void String::assignEx(const TCHAR *format, va_list& ap) {

	/*
	if (mIsRef) {

		destroy();
	}
	*/

	if (mBuffer == NULL) {
		
		mSize = safeSizeCast<size_t, int>(_tcslen(format) + 1);

		if (mSize == 1) {

			destroy();
			return;
		}

		MMemAlloc(mBuffer, mSize * sizeof(TCHAR), TCHAR*);
		//mBuffer = (TCHAR*) malloc(mSize * sizeof(TCHAR));
	}

	
	int ret;
	
	while ((ret = _vsntprintf(mBuffer, mSize - 1, format, ap)) < 0) {

		mSize = MMax(1, mSize) * 2;

		mBuffer = (TCHAR*) realloc(mBuffer, mSize * sizeof(TCHAR));
	}
	
	
	if (mSize == ret) {

		mBuffer = (TCHAR*) realloc(mBuffer, (ret + 1) * sizeof(TCHAR));
	}

	mSize = ret + 1;
	mBuffer[mSize - 1] = L'\0';

}

void String::assignEx(const TCHAR *format, ...) {

	/*
	if (mIsRef) {

		destroy();
	}
	*/

	va_list ap;
	
	if (mBuffer == NULL) {
		
		mSize = safeSizeCast<size_t, int>(_tcslen(format) + 1);

		if (mSize == 1) {
			destroy();
			return;
		}

		MMemAlloc(mBuffer, mSize * sizeof(TCHAR), TCHAR*);
		//mBuffer = (TCHAR*) malloc(mSize * sizeof(TCHAR));
	}

	
	int ret;

	
	va_start (ap, format);
	while ((ret = _vsntprintf(mBuffer, mSize - 1, format, ap)) < 0) {

		mSize = ((mSize - 1) * 2) + 1;
		mBuffer = (TCHAR*) realloc(mBuffer, mSize * sizeof(TCHAR));
	}
	va_end (ap);
	
	if (mSize == ret) {
		mBuffer = (TCHAR*) realloc(mBuffer, (ret + 1) * sizeof(TCHAR));
	}

	mSize = ret + 1;
	mBuffer[mSize - 1] = L'\0';
}

void String::append(const String& string, int count) {

	//unref();

	if (count == 0) {

		count = string.mSize == 0 ? 0 : string.mSize - 1;
	}

	append(string.mBuffer, count);
}


/*
void String::append(StringPart& string) {

	//unref();

	if ((string.length() == 0) || (string.pRef == NULL)) {

		destroy();
	}

	append(string.pRef->mBuffer + string.startIndex, string.count);
}
*/

void String::append(const TCHAR* string, int count) {

	//unref();

	if (mSize == 0) {

		assign(string, count);
		return;
	}

	int size;
	if (count == 0) {

		size = (string == NULL ? 0 : safeSizeCast<size_t, int>(_tcslen(string)));
	} else {

		size = count;
	}

	if (size == 0) {

		return;
	}

	//TCHAR* mNewBuffer = (TCHAR*) malloc((mSize + size) * sizeof(TCHAR));
	TCHAR* mNewBuffer = NULL;
	MMemAlloc(mNewBuffer, (mSize + size) * sizeof(TCHAR), TCHAR*);

	memcpy(mNewBuffer, mBuffer, (mSize - 1) * sizeof(TCHAR));
	memcpy(mNewBuffer + (mSize - 1), string, (size) * sizeof(TCHAR));
	mNewBuffer[(mSize + size) - 1] = L'\0';

	mSize += size;
	
	MMemFree(mBuffer);
	mBuffer = mNewBuffer;
}


void String::insert(const TCHAR* string, int count) {

	//unref();

	if (mSize == 0) {

		assign(string, count);
		return;
	}

	int size;
	if (count == 0) {

		size = (string == NULL ? 0 : safeSizeCast<size_t, int>(_tcslen(string)));
	} else {

		size = count;
	}

	if (size == 0) {

		return;
	}

	//TCHAR* mNewBuffer = (TCHAR*) malloc((mSize + size) * sizeof(TCHAR));
	TCHAR* mNewBuffer = NULL;
	MMemAlloc(mNewBuffer, (mSize + size) * sizeof(TCHAR), TCHAR*);

	memcpy(mNewBuffer, string, (size) * sizeof(TCHAR));
	memcpy(mNewBuffer + (size), mBuffer, (mSize - 1) * sizeof(TCHAR));
	mNewBuffer[(mSize + size) - 1] = L'\0';

	mSize += size;
	
	MMemFree(mBuffer);
	mBuffer = mNewBuffer;
}


void String::insert(const String& string, int count) {

	//unref();

	if (count == 0) {

		count = string.mSize == 0 ? 0 : string.mSize - 1;
	}

	insert(string.mBuffer, count);
}

/*
#ifdef  UNICODE
void String::append(const char* string, int count) {

	//unref();

	if (mSize == 0) {

		assign(string, count);
		return;
	}

	int size;
	if (count == 0) {

		size = (string == NULL ? 0 : safeSizeCast<size_t, int>(strlen(string)));
	} else {

		size = count;
	}

	if (size == 0) {

		return;
	}

	TCHAR* tempBuffer = (TCHAR*) malloc(size * sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, 0, string, -1, tempBuffer, size);
	
	append(tempBuffer, size);
	MMemFree(tempBuffer);
}
#endif
*/


void String::replace(TCHAR c, TCHAR replacement) {

	//unref();

	for (int i = 0; i < mSize; i++) {

		if (mBuffer[i] == c) {

			mBuffer[i] = replacement;
		}
	}
}


int String::findNextIgnoreCase(String& key, int start) const {

	int index = start;

	if (key.length() == 0) {
		return -1;
	}

	if (key.mSize > mSize) {
		return -1;
	}

	if ((mSize == 0) || (index >= mSize - 1)) {
		return -1;
	}

	int keyI;
	bool found = false;
	for (; index < (mSize) - (key.mSize - 1); index++) {
		if (_wcsnicmp(mBuffer + index, key.mBuffer + 0, 1) == 0) {
			
			for (keyI = 0; keyI < key.mSize - 1; keyI++) {
				if (_wcsnicmp(mBuffer + index + keyI, key.mBuffer + keyI, 1) != 0) {
					break;
				}
			}
			if (keyI == key.mSize - 1) {
				found = true;
				break;
			}
		}
	}
	if (found) {
		return index;
	}

	return -1;
}


int String::findNext(String& key, int start) const {

	int index = start;

	if (key.length() == 0) {
		return -1;
	}

	if (key.mSize > mSize) {
		return -1;
	}

	if ((mSize == 0) || (index >= mSize - 1)) {
		return -1;
	}

	int keyI;
	bool found = false;
	for (; index < (mSize) - (key.mSize - 1); index++) {
		if (mBuffer[index] == key.mBuffer[0]) {
			
			for (keyI = 0; keyI < key.mSize - 1; keyI++) {
				if (mBuffer[index + keyI] != key.mBuffer[keyI]) {
					break;
				}
			}
			if (keyI == key.mSize - 1) {
				found = true;
				break;
			}
		}
	}
	if (found) {
		return index;
	}

	return -1;
}

int String::findPrevious(TCHAR c, int start, bool& found, int& startIndex, int& count, bool includeChar) const {
	
	if (start == -2) {

		start = mSize - 2;
	}

	int index = start;

	if ((mSize == 0) || (index >= mSize - 1)) {

		return -1;
	}

	for (; index >= 0; index--) {

		if (mBuffer[index] == c) {
			break;
		}
	}

	if ((index > -2) && (index <= start)) {

		found = true;

		if (includeChar) {

			startIndex = index;
		} else {

			startIndex = index + 1;
			index--;
		}
		count = (start - startIndex) + 1;

	} else {

		found = false;
	}
	
	return index;
}


int String::findNext(TCHAR c, int start, String& part, bool includeChar) const {

	bool found;
	int startIndex;
	int count;

	int index = findNext(c, start, found, startIndex, count, includeChar);

	if (found) {

		part.assign(mBuffer + startIndex, count);

	} else {

		part.destroy();
	}

	return index;
}

int String::findNext(TCHAR c, int start, bool& found, int& startIndex, int& count, bool includeChar) const {

	int index = start;

	if ((mSize == 0) || (index >= mSize - 1)) {

		return -1;
	}

	for (; index < mSize - 1; index++) {

		if (mBuffer[index] == c) {

			break;
		}
	}
	if (index >= mSize) {

		index = mSize - 1;
	}

	if ((index >= 0) && (index >= start)) {

		found = true;

		startIndex = start;
		
		if (includeChar) {

			count = (index - start) + 1;
		} else {

			count = (index - start);
		}

	} else {

		found = false;
	}
	
	return index;
}

int String::findNext(const TCHAR* buffer, TCHAR c, int start, bool& found, int& startIndex, int& count, bool includeChar, int& lenIncNull) {

	found = false;

	if (lenIncNull == 0) {

		lenIncNull = length(buffer, true);
	}

	int index = start;

	if ((lenIncNull == 0) || (index >= lenIncNull - 1)) {

		return -1;
	}

	for (; index < lenIncNull - 1; index++) {

		if (buffer[index] == c) {

			break;
		}
	}
	if (index >= lenIncNull) {

		index = lenIncNull - 1;
	}

	if ((index >= 0) && (index >= start)) {

		found = true;

		startIndex = start;
		
		if (includeChar) {

			count = (index - start) + 1;
		} else {

			count = (index - start);
		}

	} else {

		found = false;
	}
	
	return index;
}

int String::findNext(const TCHAR* buffer, TCHAR c, int start, bool& found, int& startIndex, int& lenIncNull) {

	found = false;

	if (lenIncNull == 0) {

		lenIncNull = length(buffer, true);
	}

	int index = start;

	if ((lenIncNull == 0) || (index >= lenIncNull - 1)) {

		return -1;
	}

	for (; index < lenIncNull - 1; index++) {

		if (buffer[index] == c) {

			break;
		}
	}
	if (index >= lenIncNull) {

		index = lenIncNull - 1;
	}

	if ((index >= 0) && (index >= start)) {

		found = true;

		startIndex = start;

	} else {

		found = false;
	}
	
	return index;
}

int String::count(const TCHAR* buffer, TCHAR c) {

	int count = 0;

	if (buffer != NULL) {

		int i = 0;
		while (buffer[i] != 0) {

			if (buffer[i++] == c) {
				count++;
			}
		}
	}

	return count;

}


int String::count(TCHAR c) const {

	int count = 0;

	for(int i = 0; i < mSize; i++) {
		if (mBuffer[i] == c) {
			count++;
		}
	}

	return count;
}

bool String::equals(const String& comp) const {

	if (mSize != comp.mSize) {

		return NULL;
	}

	if (mSize == 0) {

		return true;
	}

	return (wcscmp(mBuffer, comp.mBuffer) == 0);
}

bool String::equalsIgnoreCase(const String& comp) const {

	if (mSize != comp.mSize) {

		return NULL;
	}

	if (mSize == 0) {

		return true;
	}

	return (_wcsicmp(mBuffer, comp.mBuffer) == 0);
}


bool String::equals(const TCHAR* comp) const {

	if (comp == NULL) {

		if (mSize == 0) {

			return true;
		} 

		return false;
	}

	if (mSize == 0)
		return false;

	return (wcscmp(mBuffer, comp) == 0);
}

bool String::equalsIgnoreCase(const TCHAR* comp) const {

	if (comp == NULL) {

		if (mSize == 0) {

			return true;
		} 

		return false;
	}

	if (mSize == 0)
		return false;

	return (_wcsicmp(mBuffer, comp) == 0);
}

bool String::fromString(bool& convert) {

	return fromString(mBuffer, convert);
}

bool String::fromString(const TCHAR* string, bool& convert) {

	if ( string && (

		(_tcsicmp(string, L"true") == 0)
		|| (_tcsicmp(string, L"ok") == 0)
		|| (_tcsicmp(string, L"succes") == 0)
		|| (_tcsicmp(string, L"1") == 0) ) ) {

		convert = true;
	} else {

		convert = false;
	}

	return true;
}

bool String::fromString(const TCHAR* format, void* pConvert) {

	return fromString(mBuffer, format, pConvert);
}

bool String::fromString(const TCHAR* string, const TCHAR* format, void* pConvert) {

	if (string == NULL) {

		return false;
	}

	return (_stscanf(string, format, pConvert) == 1);
}

/*
#ifdef _UNICODE
bool String::fromString(const char* string, bool& convert) {

	if ( string && (

		(stricmp(string, "true") == 0)
		|| (stricmp(string, "ok") == 0)
		|| (stricmp(string, "succes") == 0)
		|| (stricmp(string, "1") == 0) ) ) {

		convert = true;
	} else {

		convert = false;
	}

	return true;
}
#endif
*/

ULONG String::hash() const {

	return String::hash(mBuffer, mSize > 0 ? mSize - 1 : 0);
}

void String::hash(unsigned int& hash) const {

	return String::hash(hash, mBuffer, mSize > 0 ? mSize - 1 : 0);
}


template<class T>
void String_hash(T& hash, const TCHAR* string, int len) {
	
	hash = 0;

	if (string == NULL) {

		return;
	}

	if (len == 0) {

		len = safeSizeCast<size_t, int>(_tcslen(string));
	}

	for (int i = 0; i < len; i++) {

		hash = string[i] + (hash << 6) + (hash << 16) - hash;
	}
}

template<class T>
void String_hash(T& hash, const char* string, int len) {
	
	hash = 0;

	if (string == NULL) {

		return;
	}

	if (len == 0) {

		int i = 0;

		while (string[i] != 0) {

			hash = string[i] + (hash << 6) + (hash << 16) - hash;
			++i;
		}

	} else {

		for (int i = 0; i < len; i++) {

			hash = string[i] + (hash << 6) + (hash << 16) - hash;
		}
	}
}

ULONG String::hash(const TCHAR* string, int len) {

	ULONG hash;

	String_hash<ULONG>(hash, string, len);
	
	return hash;
}


void String::hash(unsigned int& hash, const TCHAR* string, int len) {

	String_hash<unsigned int>(hash, string, len);
}

ULONG String::hash(const char* string, int len) {

	ULONG hash;

	String_hash<ULONG>(hash, string, len);
	
	return hash;
}

void String::hash(unsigned int& hash, const char* string, int len) {

	String_hash<unsigned int>(hash, string, len);
}


/*
Path::Path(const TCHAR* string) {
	assign(string);
}

void Path::assign(const TCHAR* string) {

	completeFilePath.assign(string);
	completeFilePath.replace(L'\\', L'/');

	int lastSlashIndex = -2;
	lastSlashIndex = completeFilePath.findPrevious(L'/', lastSlashIndex, file);
	if (lastSlashIndex >= 0) {

		path.pRef = &completeFilePath;
		path.startIndex = 0;
		path.count = (lastSlashIndex) + 1;
	} else {

		path.destroy();
	}

}

Path::~Path() {
}
*/

/*
StringKey::StringKey() {
	hash = ULONG_MAX;
}

StringKey::StringKey(const StringKey& ref) {
	hash = ref.hash;
}

StringKey::StringKey(const TCHAR* string) {
	hash = String::hash(string);
}

#ifdef UNICODE
StringKey::StringKey(const char* string) {
	hash = String::hash(string);
}
#endif

StringKey::StringKey(const String& ref) {
	hash = ((String&)ref).hash();
}

void StringKey::operator=(const StringKey& ref) {
	hash = ref.hash;
}

bool StringKey::operator<(const StringKey& comp) const {
	return hash < comp.hash;
}
*/



}