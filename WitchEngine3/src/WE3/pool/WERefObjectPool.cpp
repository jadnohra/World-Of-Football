#include "WERefObjectPool.h"

namespace WE {

RefObjectPool::RefObjectPool() {
}

RefObjectPool::~RefObjectPool() {
}

UINT RefObjectPool::getObjectCount() {

	return mMap.getCount();
}

RefPoolObjectBase* RefObjectPool::baseGet(RefPoolObjectLoadInfo& loadInfo) {

	PoolObject* pPoolObject;
	
	if ((loadInfo.poolState == Pool_NonePoolable) || (loadInfo.poolId == 0)) {

		if (loadInfo.poolState == Pool_NonePoolable) {

			//log(LOG_INFO, L"Pool: Creating none-poolable object");

		} else {

			log(LOG_WARN, L"Pool: Poolable object has no poolId, creating as unpooled");
		}

		return create(loadInfo);
	}

	if (mMap.find(loadInfo.poolId, pPoolObject) == false) {

		pPoolObject = create(loadInfo);
		
		if (pPoolObject) {

			mMap.insert(loadInfo.poolId, pPoolObject);
		}
	}
	
	return pPoolObject;
}

void RefObjectPool::releaseUnused() {

	mMap.removeOneRefObjects();
}

}