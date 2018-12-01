#ifndef _WEAppPath_h
#define _WEAppPath_h

#include "WEString.h"

namespace WE {

	class AppPath {
	protected:

		static bool mIsProcessed;
		static String mPath;

		static bool process();

	public:

		static const TCHAR* getPath(); 
	};

}

#endif