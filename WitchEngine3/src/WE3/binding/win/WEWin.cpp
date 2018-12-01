#include "includeWindows.h"
#ifdef WIN32

#include "../../string/WEBufferString.h"

namespace WE {

bool Win::isValidFile(const TCHAR* path, bool& isFolder) {

	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(path, &findData);

	if (hFind == INVALID_HANDLE_VALUE) {

		int l = String::length(path, false);

		if (l > 0) {

			if (path[l - 1] == L'/' || path[l - 1] == L'\\') {

				String temp;

				temp.assign(path, l - 1);

				return isValidFile(temp.c_tstr(), isFolder);
			}
		}

		return false;
	} 


	isFolder = ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
	FindClose(hFind);
	

	return true;
}

bool Win::executeProcess(const TCHAR* _processPath, const TCHAR* workingDir, const TCHAR* arg, int& exitCode) {

	BufferString processPath(_processPath);

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	processPath.append(L" ");
	processPath.append(arg);

	if (!CreateProcessW(NULL, processPath.mBuffer, NULL, NULL, false, CREATE_NO_WINDOW, NULL, 
							workingDir, &si, &pi)) {

		return false;
	}

	WaitForSingleObject( pi.hProcess, INFINITE );

	DWORD _exitCode = -1;

	GetExitCodeProcess(pi.hProcess, &_exitCode);

	exitCode = _exitCode;

	// Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

	return true;
}

Win::FileIterator::FileIterator() {

	hFind = INVALID_HANDLE_VALUE;
}


Win::FileIterator::~FileIterator() {

	destroy();
}


void Win::FileIterator::destroy() {

	if (hFind != INVALID_HANDLE_VALUE) {

		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}
}

bool Win::FileIterator::getCurrFileName(String& out, bool& isFolder, bool allowVirtualFolders) {

	if (hFind != INVALID_HANDLE_VALUE) {

		isFolder = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		out.assign(FindFileData.cFileName);

		if (!allowVirtualFolders) {

			if (out.equals(L".") || out.equals(L".."))
				return false;
		}

		return true;
	} 

	return false;
}

void Win::breakFileIterate(FileIterator& iter) {

	iter.destroy();
}

bool Win::startFileIterate(const TCHAR* path, FileIterator& iter) {

	iter.destroy();

	BufferString crit(path);

	crit.append(L"*");

	iter.hFind = FindFirstFile(crit.c_tstr(), &iter.FindFileData);

	return iter.hFind != INVALID_HANDLE_VALUE;
}

bool Win::nextFileIterate(FileIterator& iter) {

	if (iter.hFind != INVALID_HANDLE_VALUE) {

		if (FindNextFile(iter.hFind, &iter.FindFileData)) {

			return true;
		}

		iter.destroy();
	}

	return false;
}

}

#endif