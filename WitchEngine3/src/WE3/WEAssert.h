#ifndef _WEAssert_h
#define _WEAssert_h

#include "tchar.h"
#include "assert.h"
#include "WELog.h"

namespace WE {

	void setLog(const Log* pLog);
	void log(const TCHAR* msg, ...);
	void log(const int& logLevel, const TCHAR* msg, ...);
	void logFlush();

#ifdef _DEBUG
	void assrt(bool condition);
	void debugBreak();
#else
	inline void assrt(bool condition) {};
	inline void debugBreak() {};
#endif	

}

#endif