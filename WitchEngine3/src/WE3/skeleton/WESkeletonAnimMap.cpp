#include "WESkeletonAnimMap.h"

#include "../string/WETCHAR.h"

namespace WE {

void SkeletonAnimMap::init(AnimationIndex::Type animCount) {

	mMap.setSize(animCount);
}

bool SkeletonAnimMap::mapAnim(Skeleton& skeleton, const TCHAR* anim, AnimationIndex::Type mapIndex) {

	AnimationIndex::Type count = skeleton.getAnimationCount();
	
	for (AnimationIndex::Type i = 0; i < count; i++) {

		if (tcharCompare(anim, skeleton.getAnimationName(i)) == 0) {

			mMap[mapIndex] = i;
			return true;
		}
	}

	return false;
}


}