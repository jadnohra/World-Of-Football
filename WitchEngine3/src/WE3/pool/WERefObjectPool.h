#ifndef _WERefObjectPool_h
#define _WERefObjectPool_h

#include "../WERef.h"
#include "../WETL/WETLHashMap.h"
#include "WERefPoolObjectLoadInfo.h"


namespace WE {

	typedef Ref RefPoolObjectBase;
	
	class RefObjectPool {
	public:

		RefObjectPool();
		~RefObjectPool();

		RefPoolObjectBase* baseGet(RefPoolObjectLoadInfo& loadInfo);
		void releaseUnused();

		UINT getObjectCount();

	protected:

		virtual RefPoolObjectBase* create(RefPoolObjectLoadInfo& loadInfo) = 0;

	protected:

		typedef RefPoolObjectBase PoolObject;

		typedef StringHash PoolObjectKey;
		typedef IndexT<UINT> PoolObjectIndex;
		typedef WETL::RefHashMap<PoolObjectKey, PoolObject*, PoolObjectIndex::Type> Map;

		Map mMap;
	};

}

#endif