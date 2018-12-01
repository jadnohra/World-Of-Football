/*
#include "WESpatialDynamicVolumeImpl.h"

namespace WE {

void SpatialDynamicVolumeImpl::spatialClear() {

	mTriContainerPartials.destroy();
	mVolumes.destroy();
	mDynamicVolumes.destroy();
}

void SpatialDynamicVolumeImpl::spatialAdd(SpatialTriContainer::Partial& object) {

	mTriContainerPartials.addOne(&object);
}
void SpatialDynamicVolumeImpl::spatialAdd(SpatialVolume& object) {

	mVolumes.addOne(&object);
}
void SpatialDynamicVolumeImpl::spatialAdd(SpatialDynamicVolume& object) {

	mDynamicVolumes.addOne(&object);
}

void SpatialDynamicVolumeImpl::spatialRemove(SpatialTriContainer::Partial& object) {

	TriContainerPartials::Index index;

	if (mTriContainerPartials.searchFor(&object, 0, mTriContainerPartials.count, index)) {

		mTriContainerPartials.removeSwapWithLast(index, true);
	}
}
void SpatialDynamicVolumeImpl::spatialRemove(SpatialVolume& object) {

	Volumes::Index index;

	if (mVolumes.searchFor(&object, 0, mVolumes.count, index)) {

		mVolumes.removeSwapWithLast(index, true);
	}
}
void SpatialDynamicVolumeImpl::spatialRemove(SpatialDynamicVolume& object) {

	DynamicVolumes::Index index;

	if (mDynamicVolumes.searchFor(&object, 0, mDynamicVolumes.count, index)) {

		mDynamicVolumes.removeSwapWithLast(index, true);
	}
}

}
*/