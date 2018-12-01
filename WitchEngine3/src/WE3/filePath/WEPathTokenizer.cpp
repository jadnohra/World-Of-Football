#include "WEPathTokenizer.h"

namespace WE {

bool PathTokenizer::nextToken(const TCHAR* path, int& reserved, StringPartBase& part) {

	if (path == NULL) {

		return false;
	}

	if (reserved < 0) {

		reserved = 0;
	}

	int index = reserved;

	if (path[index] == 0) {

		return false;
	}

	while ((path[index] != L'/') && (path[index] != L'\\') && (path[index] != 0)) {

		++index;
	}
	
	if ((path[index] == L'/') || (path[index] == L'\\')) {

		part.startIndex = reserved;
		part.count = index - reserved;

		reserved = index + 1;

	} else /*path[index] != L'0'*/ {

		part.startIndex = reserved;
		part.count = index - reserved;

		reserved = index;
	}

	return true;
}


bool PathTokenizer::prevToken(const TCHAR* path, int& reserved, StringPartBase& part) {

	if ((path == NULL) || (reserved == -1)) {

		return false;
	}

	if (reserved == -2) {

		reserved = (String::length(path, false) - 1);

		if (reserved < 0) {

			return false;
		}
	}

	int index = reserved;

	while ((index >= 0) && (path[index] != L'/') && (path[index] != L'\\')) {

		index--;
	}

	part.startIndex = index + 1;
	part.count = (reserved - part.startIndex) + 1;
	
	reserved = index <= -1 ? -1 : (index - 1);

		
	return true;
}

void PathTokenizer::extractPathParts(const TCHAR* filePath, StringPartBase& pathPart, StringPartBase& filePart, bool includeLastSlash) {

	pathPart.destroy();
	filePart.destroy();

	int len = String::length(filePath, false);
	int lastSlashIndex = len;
	bool slashFound = false;

	while ((lastSlashIndex >= 0) && (filePath[lastSlashIndex] != L'/') && (filePath[lastSlashIndex] != L'\\')) {

		lastSlashIndex--;
	}

	if ((lastSlashIndex >= 0) && ((filePath[lastSlashIndex] == L'/') || (filePath[lastSlashIndex] == L'\\'))) {

		pathPart.startIndex = 0;
		pathPart.count = lastSlashIndex;

		pathPart.getRemainingPart(filePath, filePart, len);	
		++(filePart.startIndex);

		if (includeLastSlash && pathPart.count) {
			
			++(pathPart.count);
		}
	} 

	
}

}