#include "WEAssert.h"

#include "WEString.h"

#ifdef WIN32
	#include "windows.h"
#endif

namespace WE {


const Log* assrtLog = NULL;
String assrtString;

void setLog(const Log* pLog) {

	assrtLog = pLog;
}

void logFlush() {

	if (assrtLog) {

		assrtLog->flush();
	}
}

void log(const TCHAR* msg, ...) {

	va_list ap;
	va_start (ap, msg);

	assrtString.assignEx(msg, ap);

	if (assrtLog) {

		assrtLog->log(LOG_ERROR, true, true, assrtString.c_tstr());
	}

	va_end (ap);

}

void log(const int& logLevel, const TCHAR* msg, ...) {

	va_list ap;
	va_start (ap, msg);

	assrtString.assignEx(msg, ap);

	if (assrtLog) {

		assrtLog->log((LogType) logLevel, true, true, assrtString.c_tstr());
	}

	va_end (ap);
}


#ifdef _DEBUG

void debugBreak() {

#ifdef WIN32
	DebugBreak();
#endif

}

void assrt(bool condition) {

	if (condition == false) {

		logFlush();
		debugBreak();
	}
}

#else
#endif


}