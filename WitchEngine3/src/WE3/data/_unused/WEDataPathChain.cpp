#include "WEDataPathChain.h"

#include "../WEMacros.h"

namespace WE {

	/*
DataPathChain::DataPathChain(DataPathChain* pPrev, const TCHAR* path)
	: mpPrev(pPrev), mPath(path), mPrevPartCount(0) {

	MSafeAddRef(mpPrev);

	if  (mpPrev) {

		mPrevPartCount = mpPrev->getPartCount();
	}
}

DataPathChain::~DataPathChain() {

	MSafeRelease(mpPrev);
}

DataPath::PartIndex::Type DataPathChain::getPartCount() const {

	return mPath.getPartCount() + mPrevPartCount;
}

String* DataPathChain::getPart(DataPath::PartIndex partIndex) const {

	if (partIndex < mPrevPartCount) {

		return mpPrev->getPart(partIndex);
	}

	return mPath.getPart(partIndex - mPrevPartCount);
}
*/

}