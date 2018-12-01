#ifndef _WENativeDialog_h
#define _WENativeDialog_h

#include "../WEString.h"

namespace WE {


	class NativeDialog {
	public:

		static bool browseOpen(String& path, const TCHAR* filter=L"All Files (*.*)\0*.*\0");
	};

}

#endif