#ifndef _WEWin32_h
#define _WEWin32_h

#ifdef WIN32

#include "windows.h"
#include "WEString.h"

namespace WE {

	
	class Win32OpenFileDialog {

	public:

		static bool browseOpen(FilePath& path, const TCHAR* filter=L"All Files (*.*)\0*.*\0") {
			
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
			
			
			if (GetOpenFileName(&ofn) && (ofn.lpstrFile != NULL)) {

				path.assign(ofn.lpstrFile);
				
				return true;
			}
			
			return false;
		}
	};
}

#endif

#endif