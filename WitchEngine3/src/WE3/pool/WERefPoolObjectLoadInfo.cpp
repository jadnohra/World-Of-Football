#include "WERefPoolObjectLoadInfo.h"

namespace WE {

RefPoolObjectPoolInfo::RefPoolObjectPoolInfo(RefPoolObjectPoolState _poolState, RefPoolId _poolId) {

	poolState = _poolState;
	poolId = _poolId;
}

bool RefPoolObjectPoolInfo::needsPoolId() {

	if ((poolState == Pool_NonePoolable) || (poolId != 0)) {

		return false;
	}

	return true;
}

RefPoolObjectLoadInfo::RefPoolObjectLoadInfo() {

	poolState = Pool_Auto;
	poolId = 0;
}


void RefPoolObjectLoadInfo::injectPoolState(const RefPoolObjectPoolInfo* pPoolInfo) {

	if (pPoolInfo) {

		if (poolState == Pool_Auto) {

			if (pPoolInfo->poolState == Pool_Auto) {

				if (pPoolInfo->poolId != 0) {

					poolId = pPoolInfo->poolId;
				}

			} else {

				poolState = pPoolInfo->poolState;
				poolId = pPoolInfo->poolId;
			}

		} else {

			if (pPoolInfo->poolState == Pool_Auto) {

				if (pPoolInfo->poolId != 0) {

					poolId = pPoolInfo->poolId;
				}

			} else {

				poolState = pPoolInfo->poolState;
				poolId = pPoolInfo->poolId;
			}

		}
	}
}

}