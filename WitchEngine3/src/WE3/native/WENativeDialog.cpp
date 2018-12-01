#include "WENativeDialog.h"

#include "../binding/win/WEWinDialog.h"

namespace WE {

bool NativeDialog::browseOpen(String& path, const TCHAR* filter) {

#ifdef _WIN32

	return WinDialog::browseOpen(path, filter);

#endif

	return false;
}

}