#ifndef _WEFilePath_h
#define _WEFilePath_h

#include "../string/WEStringPart.h"

namespace WE {


	class FilePath {
	public:

		String mCompletePath;
		StringPartBase mPath;
		StringPartBase mFile;

	public:

		FilePath(const TCHAR* pPath = NULL);
		~FilePath();

		void assign(const TCHAR* pPath);

		void getPath(String& string);
		void getFile(String& string);

		String& getCompletePath();
		void updateParts();

		StringHash hash() const;
		const TCHAR* c_tstr() const;

		static bool isAbsolutePath(const TCHAR* path);
		static bool hasExtension(const String& path);
	};

}


#endif