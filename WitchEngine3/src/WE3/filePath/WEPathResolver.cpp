#include "WEPathResolver.h"

#include "WEFilePath.h"
#include "WEPathTokenizer.h"

namespace WE {

void PathResolver::simpleAppendPath(BufferString& currPath, const TCHAR* appendPath) {

	if (currPath.isEmpty() == false) {

		const TCHAR& endChar = currPath.c_tstr()[currPath.length() - 1];

		if ((endChar != L'/') && (endChar != L'\\')) {

			currPath.append(L"/");
		}

		currPath.append(appendPath);

	} else {

		currPath.assign(appendPath);
	}

}

void PathResolver::appendPath(BufferString& currPath, const TCHAR* appendPath, bool process) {

	bool currPathHasFileName;

	if (currPath.isEmpty()) {

			currPathHasFileName = false;

	} else {

		TCHAR lastChar = currPath.c_tstr()[currPath.length() - 1];

		currPathHasFileName = (lastChar == L'/' || lastChar == L'\\');
	}
	
	PathResolver::appendPath(currPath, appendPath, process, currPathHasFileName);
}

void PathResolver::appendPath(BufferString& currPath, const TCHAR* appendPath, bool process, bool currPathHasFileName) {


	if (FilePath::isAbsolutePath(appendPath)) {

		if (currPath.isEmpty())
			currPath.assign(appendPath);

		return;
	}

	if (appendPath == NULL) {

		return;
	}

	if (process) {

		
		int appendStartChar = 0;
		int currBackCount = 0;

		int i = 0;

		while (appendPath[i] != 0) {

			if (appendPath[i] == L'.') {
				++i;

				if (appendPath[i] == L'.') {
					++i;

					if ((appendPath[i] == L'/') || (appendPath[i] == L'\\')) {
						++i;
						appendStartChar = i;
						++currBackCount;


					} else {

						break;
					}

				} else {

					break;
				}

			} else {

				break;
			}
		}

		if (currPathHasFileName) {

			++currBackCount;
		}

		if (currBackCount != 0) {
			

			StringPartBase part;
			const TCHAR* currPathStr = currPath.c_tstr();
			int reserved = -2;

			//++currBackCount; //because of the way prevToken works
			while (currBackCount != 0) {
				
				if (PathTokenizer::prevToken(currPathStr, reserved, part) == false) {

					assrt(false);
					return;
				}

				--currBackCount;
			}
			
			StringPartBase firstPart;
			part.getFirstPart(currPathStr, firstPart);


			if (firstPart.length() != 0) {

				//or better make a 'cut' functionality in String
				BufferString temp(currPath);

				firstPart.assignTo(temp.c_tstr(), currPath);

				//currPath.append(appendPath + appendStartChar);
				simpleAppendPath(currPath, appendPath + appendStartChar);


			} else {

				currPath.assign(appendPath + appendStartChar);

			}
			
		} else {

			//currPath.append(appendPath);
			simpleAppendPath(currPath, appendPath);
		}

	} else {

		//currPath.append(appendPath);
		simpleAppendPath(currPath, appendPath);
	}
}

void resolvePath(PathResolver* pResolver, const TCHAR* path, String& absPath) {

	if (pResolver) {

		pResolver->resolvePath(path, absPath);

	} else {

		if (FilePath::isAbsolutePath(path)) {

			absPath.assign(path);

		} else {

			assrt(false);
			return;
		}
	}
}

}