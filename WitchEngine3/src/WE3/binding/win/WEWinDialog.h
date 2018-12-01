#ifndef _WEWinDialog_h
#define _WEWinDialog_h
#ifdef WIN32

#include "includeWindows.h"
#include "../../WEString.h"

namespace WE {

	
	class WinDialog {

	public:

		static bool browseOpen(WE::String& path, const TCHAR* filter=L"All Files (*.*)\0*.*\0") {
			
			OPENFILENAME ofn;
			
			TCHAR szFileName[MAX_PATH] = L"";
			
			ZeroMemory(&ofn, sizeof(ofn));
			
			ofn.lStructSize = sizeof(ofn); 
			ofn.hwndOwner = NULL;
			ofn.lpstrFilter = filter;
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = L"";
			
			
			if (GetOpenFileName(&ofn)) {

				path.assign(ofn.lpstrFile);

				return true;
			}
			
			return false;
		}
	};
}

#endif
#endif