#ifndef _WENativeAppInfo_h
#define _WENativeAppInfo_h

#ifdef WIN32

	#include "../binding/win/includeWindows.h"

	namespace WE {

		struct NativeAppInfo {

			HINSTANCE mAppInstance;
			HWND mWnd;
		};
	}

#endif

#endif