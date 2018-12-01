#ifndef _WESkeletonAnimMap_h
#define _WESkeletonAnimMap_h

#include "WESkeleton.h"
#include "../WETL/WETLArray.h"
#include "../WETL/WETLHashMap.h"
#include "../WEPtr.h"

namespace WE {

	class SkeletonAnimMap {
	public:

		void init(AnimationIndex::Type animCount);
		bool mapAnim(Skeleton& skeleton, const TCHAR* anim, AnimationIndex::Type mapIndex);

		inline const AnimationIndex::Type& animIndex(const AnimationIndex::Type& mapIndex) {

			return mMap.el[mapIndex];
		}

	protected:

		typedef WETL::StaticArray<AnimationIndex::Type, false, AnimationIndex::Type> Map;
		Map mMap;
	};
}

#endif