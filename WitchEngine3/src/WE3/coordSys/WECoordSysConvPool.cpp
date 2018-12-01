#include "WECoordSysConvPool.h"
#include "WECoordSysConv_Swizzle.h"

namespace WE {


CoordSysConvPool::CoordSysConvPool() {
}

CoordSysConvPool::~CoordSysConvPool() {
}

void CoordSysConvPool::getHashFor(const CoordSys& src, const CoordSys& dest, ConvHashType& hash) {

	hash = 0;

	if (src.equals(dest)) {

		return;
	}

	hash += src._bit_14;
	hash += (dest._bit_14) << 16;
}

CoordSysConv* CoordSysConvPool::createConverterFor(const CoordSys& src, const CoordSys& dest, bool createIfNotNeeded) {

	if ((src.isValid() == false) || (dest.isValid() == false)) {

		assrt(false);
		return NULL;
	}

	CoordSysConv* pConv = NULL;

	if (src.isCompatibleWith(dest)) {

		if (createIfNotNeeded == true) {

			MMemNew(pConv, CoordSysConv());
		}

	} else {

		MMemNew(pConv, CoordSysConv_Swizzle);
		((CoordSysConv_Swizzle*) pConv)->init(src, dest);
	}

	return pConv;
}

void CoordSysConvPool::createConverterFor(const CoordSys& src, const CoordSys& dest, const ConvHashType& hash, CoordSysConv*& pConv, bool createIfNotNeeded) {

	pConv = createConverterFor(src, dest, createIfNotNeeded);

	if (pConv) {

		mConvMap.insert(hash, pConv);
	}
}

CoordSysConv* CoordSysConvPool::getConverterFor(const CoordSys& src, const CoordSys& dest, bool createIfNotNeeded) {

	ConvHashType hash;

	getHashFor(src, dest, hash);

	CoordSysConv* pRet;

	if (mConvMap.find(hash, pRet)) {

		return pRet;
	}

	createConverterFor(src, dest, hash, pRet, createIfNotNeeded);

	return pRet;
}

}