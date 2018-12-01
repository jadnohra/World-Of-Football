#ifndef _WESkeletonLoader_v1_h
#define _WESkeletonLoader_v1_h

#include "WESkeleton.h"

#include "../data/WEDataSourcePool.h"
#include "../coordSys/WECoordSysConvPool.h"


namespace WE {

	class SkeletonLoader_v1 {
	public:

		static bool load(Skeleton& skel, DataSourcePool& dataPool, DataSourceChunk skelChunk, const CoordSys* pTargetSemantics, CoordSysConvPool* pConvPool, bool loadCoordSys = true);
	};
}

#endif 