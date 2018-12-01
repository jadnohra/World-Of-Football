#ifndef _WERefPoolObjectLoadInfo_h
#define _WERefPoolObjectLoadInfo_h

#include "../string/WEString.h"

namespace WE {

	typedef StringHash RefPoolId;

	enum RefPoolObjectPoolState {

		Pool_Auto, Pool_NonePoolable
	};

	struct RefPoolObjectPoolInfo {

		RefPoolObjectPoolState poolState;
		RefPoolId poolId;

		RefPoolObjectPoolInfo(RefPoolObjectPoolState _poolState = Pool_Auto, RefPoolId _poolId = 0);

		bool needsPoolId();
	};

	struct RefPoolObjectLoadInfo : RefPoolObjectPoolInfo {

		RefPoolObjectLoadInfo();

		void injectPoolState(const RefPoolObjectPoolInfo* pPoolInfo = NULL);
	};

}

#endif