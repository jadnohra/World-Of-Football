#ifndef _includeWindows_h
#define _includeWindows_h
#ifdef WIN32

#define NOMINMAX
//#define WIN32_LEAN_AND_MEAN
//needed form WEWinDialog.h

#include "windows.h"

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B // (not always defined)
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C // (not always defined)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A // (not always defined)
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120 // (not always defined)
#endif

namespace WE {

	class String;

	class Win {
	public:

		struct FileIterator {

			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;

			FileIterator();
			~FileIterator();

			void destroy();

			bool getCurrFileName(String& out, bool& isFolder, bool allowVirtualFolders = false);
		};

	public:

		static bool isValidFile(const TCHAR* path, bool& isFolder);
		static bool executeProcess(const TCHAR* process, const TCHAR* workingDir, const TCHAR* arg, int& exitCode);

		static bool startFileIterate(const TCHAR* path, FileIterator& iter);
		static bool nextFileIterate(FileIterator& iter);
		static void breakFileIterate(FileIterator& iter);
	};

}

#endif
#endif