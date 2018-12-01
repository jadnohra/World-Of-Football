#ifndef _WEConsole_h
#define _WEConsole_h

#include "string/WEString.h"

namespace WE {

	class Console {
	public:

		virtual void show(bool show, bool readOnly) = 0;
		virtual void print(const TCHAR* msg, bool printLineNumber = false) = 0;
		virtual const TCHAR* getLastLine() = 0;
	};

}

#endif