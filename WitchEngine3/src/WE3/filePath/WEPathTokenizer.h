#ifndef _WEPathTokenizer_h
#define _WEPathTokenizer_h

#include "../string/WEStringPart.h"

namespace WE {

	class PathTokenizer {
	public:

		//start with reserved = -1
		static bool nextToken(const TCHAR* path, int& reserved, StringPartBase& part);

		//start with reserved = -2
		static bool prevToken(const TCHAR* path, int& reserved, StringPartBase& part);

		static void extractPathParts(const TCHAR* filePath, StringPartBase& pathPart, StringPartBase& filePart, bool includeLastSlash);
	};

}

#endif