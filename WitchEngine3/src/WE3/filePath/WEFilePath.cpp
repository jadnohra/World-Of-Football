#include "WEFilePath.h"


namespace WE {

FilePath::FilePath(const TCHAR* pPath) {

	assign(pPath);
}

FilePath::~FilePath() {
}

void FilePath::assign(const TCHAR* pPath) {

	mCompletePath.assign(pPath);
	updateParts();	
}

void FilePath::getPath(String& string) {

	mPath.assignTo(mCompletePath.mBuffer, string);
}

void FilePath::getFile(String& string) {

	mFile.assignTo(mCompletePath.mBuffer, string);
}

String& FilePath::getCompletePath() {

	return mCompletePath;
}

void FilePath::updateParts() {

	mCompletePath.replace(L'\\', L'/');

	if (mCompletePath.isValid()) {

		int lastSlashIndex = -2;
		bool slashFound;

		lastSlashIndex = mCompletePath.findPrevious(L'/', lastSlashIndex, slashFound, mFile.startIndex, mFile.count, false);

		if (mFile.startIndex >= 0) {

			mPath.startIndex = 0;
			mPath.count = (lastSlashIndex) + 2;

		} else {

			mPath.destroy();
		}


	} else {

		mPath.destroy();
		mFile.destroy();
	}
}

StringHash FilePath::hash() const {

	return mCompletePath.hash();
}

const TCHAR* FilePath::c_tstr() const {

	return mCompletePath.c_tstr();
}

bool FilePath::isAbsolutePath(const TCHAR* path) {

#ifdef _WIN32

	//put this in Win class

	bool found;
	int index;
	int len;

	len = 0;
	index = 0;
	index = String::findNext(path, L':', 0, found, index, len);

	return found && (index >= 1) && (index <= 3);

	/*
	bool found;
	int index;
	int count;

	index = 0;
	index = path.findNext(L':', 0, found, index, count, true);

	return found && (index >= 1) && (index <= 3);
	*/

#else
	assrt(false);
	return false;
#endif

}

bool FilePath::hasExtension(const String& path) {

	bool found;
	int index;
	int count;

	index = -2;
	index = path.findPrevious(L'.', -2, found, index, count, false);

	return found && (count >= 1) && (count <= 4);
}

}