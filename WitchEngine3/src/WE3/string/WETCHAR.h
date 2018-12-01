#ifndef _WETCHAR_h
#define _WETCHAR_h

#include "../binding/win/includeWindows.h"
#include "tchar.h"
#include "../WEPtr.h"
#include "../WETL/WETLSizeAlloc.h"

namespace WE {

	int tcharCompare(const TCHAR* str1, const TCHAR* str2);

	void tcharToChar(const TCHAR* unicodeBuff, char* charBuff, int len = 0);
	void tcharToChar(const TCHAR* unicodeBuff, FlexiblePtr<char>& charBuff, int len = 0);

	void charToTChar(const char* charBuff, TCHAR* unicodeBuff, int len = 0);
	void charToTChar(const char* charBuff, FlexiblePtr<TCHAR>& unicodeBuff, int len = 0);
	
	class String;
	void charToString(const char* charBuff, String& string, int len = 0);
	void stringToChar(const String& string, FlexiblePtr<char>& charBuff);


	typedef WETL::SizeAllocT<char, int> BuffChars;
	typedef WETL::SizeAllocT<TCHAR, int> BuffTChars;
	const char* tcharToChar(const TCHAR* source, BuffChars& buff, int len = 0);
	const TCHAR* charToTChar(const char* source, BuffTChars& buff, int len = 0);
}

#endif