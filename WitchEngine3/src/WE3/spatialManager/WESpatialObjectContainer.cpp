#include "WESpatialObjectContainer.h"

namespace WE {

SpatialObjectContainer::~SpatialObjectContainer() {
}

void SpatialObjectContainer::add(SpatialTriContainer* pSpatial, ObjectBinding* pBinding, bool destroyWhenDone) {

	mTriConts.addNewOne()->init(pSpatial, pBinding, destroyWhenDone);
}

void SpatialObjectContainer::add(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) {

	mVols.addNewOne()->init(spatial, pBinding);
}

void SpatialObjectContainer::addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) {

	mDynVols.addNewOne()->init(spatial, pBinding);
}

bool SpatialObjectContainer::addObjectsTo(SpatialManager& target, bool clearObjects) {

	for (Index i = 0; i < mTriConts.count; ++i) {

		if (!target.add(mTriConts[i]->spatial, mTriConts[i]->binding))
			return false;
	}

	for (Index i = 0; i < mVols.count; ++i) {

		if (!target.add(mVols[i]->binding->volume, mVols[i]->binding))
			return false;
	}

	for (Index i = 0; i < mDynVols.count; ++i) {

		if (!target.add(mDynVols[i]->binding->volume, mDynVols[i]->binding))
			return false;
	}

	if (clearObjects) {

		mTriConts.destroy();
		mVols.destroy();
		mDynVols.destroy();
	}

	return true;
}

}