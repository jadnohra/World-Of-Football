#include "WEICollDetector.h"

namespace WE {

ICollDetector::ICollDetector() : mIsEnabledListener(false), mpCollMaskMatrix(NULL) {
}

void ICollDetector::setMaskMatrix(const CollMaskMatrix* pMatrix) {

	mpCollMaskMatrix = pMatrix;
}

void ICollDetector::setListener(ICollDetectListener* pListener) {

	mListener = pListener;
}

void ICollDetector::enableListener(bool enable) {

	mIsEnabledListener = enable;
}

}

