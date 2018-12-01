#include "WETCHAR.h"

#pragma warning( disable : 4995 4996)

#include "../WEMem.h"
#include "../WEMacros.h"
#include "tchar.h"
//#include "strsafe.h"
#include "stdlib.h"
#include "string.h"

#include "WEString.h"

namespace WE {

int tcharCompare(const TCHAR* str1, const TCHAR* str2) {

	return _tcscmp(str1, str2);
}

#ifdef UNICODE

void tcharToChar(const TCHAR* tcharBuff, char* charBuff, int len) {

	if (len == 0) {

		len = safeSizeCast<size_t, int>(_tcslen(tcharBuff));
	}

	WideCharToMultiByte(CP_ACP, 0, tcharBuff, -1, charBuff, len, NULL, NULL);
}

void charToTChar(const char* charBuff, TCHAR* tcharBuff, int len) {

	if (len == 0) {

		len = safeSizeCast<size_t, int>(strlen(charBuff));
	}

	MultiByteToWideChar(CP_ACP, 0, charBuff, -1, tcharBuff, len);
}


#else

void tcharToChar(const TCHAR* unicodeBuff, char* charBuff, int len) {

	if (len == 0) {

		len = safeSizeCast<size_t, int>(strlen(unicodeBuff));
	}

	memcpy(charBuff, tcharBuff, len * sizeof(char));
}

void charToTChar(const char* charBuff, TCHAR* unicodeBuff, int len) {

	if (len == 0) {

		len = safeSizeCast<size_t, int>(strlen(charBuff));
	}

	memcpy(tcharBuff, charBuff, len * sizeof(char));
}

#endif

void tcharToChar(const TCHAR* tcharBuff, FlexiblePtr<char>& charBuff, int len) {

	if (len == 0) {

		len = safeSizeCast<size_t, int>(String::length(tcharBuff, false));
	}

	len++;

	charBuff.setType(Ptr_HardRefArray);
	MMemNew(charBuff.pObject, char[len]);
	

	tcharToChar(tcharBuff, charBuff.ptr(), len);
}

void charToTChar(const char* charBuff, FlexiblePtr<TCHAR>& tcharBuff, int len) {

	if (len == 0) {

		len = safeSizeCast<size_t, int>(strlen(charBuff));
	}

	len++;

	tcharBuff.setType(Ptr_HardRefArray);
	MMemNew(tcharBuff.pObject, TCHAR[len]);
	

	charToTChar(charBuff, tcharBuff.ptr(), len);
}

void charToString(const char* charBuff, String& string, int len) {

	if (len == 0) {

		len = safeSizeCast<size_t, int>(strlen(charBuff));
	}

	string.destroy();
	
	if (len == 0) {

		return;
	}

	len++;

	TCHAR* tcharBuff = string.setSize(len);
	
	charToTChar(charBuff, string.setSize(len), len);
}

void stringToChar(const String& string, FlexiblePtr<char>& charBuff) {

	int len = string.length();

	if (len == 0) {

		charBuff.destroy();

		return;
	}

	len++;

	charBuff.setType(Ptr_HardRefArray);
	MMemNew(charBuff.pObject, char[len]);
	

	tcharToChar(string.c_tstr(), charBuff.ptr(), len);
}

#ifdef UNICODE
const char* tcharToChar(const TCHAR* source, BuffChars& buff, int len) {

	if (len == 0 && source != NULL) {

		len = String::length(source, false);
	}

	if (len == 0) {

		//for handycaped strlen
		buff.resize(1);
		buff.el[0] = 0;

		return buff.el;
	}

	++len;

	buff.resize(len);

	tcharToChar(source, buff.el, len);

	return buff.el;
}

const TCHAR* charToTChar(const char* source, BuffTChars& buff, int len) {

	if (len == 0 && source != NULL) {

		len = safeSizeCast<size_t, int>(strlen(source));
	}

	if (len == 0) {

		//for handycaped strlen
		buff.resize(1);
		buff.el[0] = 0;

		return buff.el;
	}

	++len;

	buff.resize(len);

	charToTChar(source, buff.el, len);

	return buff.el;
}
#else

const char* tcharToChar(const TCHAR* source, BuffChars& buff, int len) {

	return source;
}

const TCHAR* charToTChar(const char* source, BuffTChars& buff, int len) {

	return source;
}

#endif


/*
#ifdef UNICODE
void String::toCharPtr(FlexiblePtr<char>& ptr) const {

	return toCharPtr(ptr, mBuffer, mSize - 1);
}


void String::toCharPtr(FlexiblePtr<char>& ptr, const TCHAR* buff, int len) {

	if (len == 0) {

		len = safeSizeCast<size_t, int>(_tcslen(buff));
	}

	len++;

	if (len <= 0) {

		ptr.set("", false);

	} else {

		char* pCharBuff = (char*) malloc(len * sizeof(char));
		WideCharToMultiByte(CP_ACP, 0, buff, -1, pCharBuff, len, NULL, NULL);
	
		ptr.set(pCharBuff, true);
	}
}

#endif
*/

}

