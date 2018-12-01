#include "WESpatialObjectGroup_KDTree.h"

namespace WE {

SpatialObjectGroup_KDTree::~SpatialObjectGroup_KDTree() {

	destroy();
}

void SpatialObjectGroup_KDTree::reset() {

	mStaticTriConts.count = 0;
	mStaticVols.count = 0;
	mDynVols.count = 0;
}

void SpatialObjectGroup_KDTree::destroy() {

	mStaticTriConts.count = mStaticTriConts.size;

	mStaticTriConts.destroy();
	mStaticVols.destroy();
	mDynVols.destroy();
}

void SpatialObjectGroup_KDTree::TriContainer::reset() {

	mBinding.destroy();
	mTriConts.count = 0;
	mBoundingVol.empty();
}

void SpatialObjectGroup_KDTree::TriContainer::addTriCont(StaticTriCont* pTriCont, const AAB& boundingVol, bool boundingVolAlreadyAdded) {

	mTriConts.addOne(pTriCont);

	if (!mBinding.isValid())
		mBinding = pTriCont->buffer->binding;

	if (!boundingVolAlreadyAdded) {

		mBoundingVol.add(boundingVol);
	}
}

VolumeStruct* SpatialObjectGroup_KDTree::TriContainer::getBoundingVolume(VolumeRef& holder) {

	holder.init(mBoundingVol);

	return &holder;
}

ObjectBinding* SpatialObjectGroup_KDTree::TriContainer::getBinding() {

	/*
	if (mTriConts.count) {

		return mTriConts[0]->buffer->binding;
	}
	*/

	return mBinding;
}

void SpatialObjectGroup_KDTree::TriContainer::getTris(FctTriProcess fct, void* context) {

	for (Index i = 0; i < mTriConts.count; ++i) {

		StaticTriCont& cont = dref(mTriConts[i]);
		SpatialManager_KDTreeTriBufferManager::TriBuffer& buffer = cont.buffer;
		
		for (Index tii = 0; tii < cont.mTriIndices.size; ++tii) {
			
			fct(buffer.get(cont.mTriIndices[tii]), context);
		}
	}
}

SpatialObjectGroup_KDTree::TriContainer* SpatialObjectGroup_KDTree::uniqueAddTriCont(ObjectBinding* pBinding, bool& isUnique) {

	for (StaticTriConts::Index i = 0; i < mStaticTriConts.count; ++i) {

		if (mStaticTriConts[i]->binding() == pBinding) {

			isUnique = false;
			return mStaticTriConts[i];
		}
	}

	isUnique = true;

	StaticTriConts::Index newIndex = mStaticTriConts.addOneIndex();

	if (mStaticTriConts[newIndex] == NULL) {

		WE_MMemNew(mStaticTriConts[newIndex], TriContainer());

	} else {

		mStaticTriConts[newIndex]->reset();
	}

	return mStaticTriConts[newIndex];
}

void SpatialObjectGroup_KDTree::uniqueAddTriCont(StaticTriCont* pTriCont, const AAB& boundingVol, bool boundingVolAlreadyAdded) {

	bool isUnique;

	SoftPtr<TriContainer> cont = uniqueAddTriCont(pTriCont->buffer->binding, isUnique);

	cont->addTriCont(pTriCont, boundingVol, boundingVolAlreadyAdded);
}

bool SpatialObjectGroup_KDTree::uniqueAddVolume(SpatialVolumeBinding* pVolume) {

	StaticVols::Index index;

	if (mStaticVols.searchFor(pVolume, 0, mStaticVols.count, index)) {

		return false;
	}

	mStaticVols.addOne(pVolume);
	return true;
}

bool SpatialObjectGroup_KDTree::uniqueAddDynamicVolume(SpatialVolumeBinding* pVolume) {

	DynVols::Index index;

	if (mDynVols.searchFor(pVolume, 0, mDynVols.count, index)) {

		return false;
	}

	mDynVols.addOne(pVolume);
	return true;
}

SpatialObjectGroup_KDTree::Index SpatialObjectGroup_KDTree::getTriContainerCount() {

	return mStaticTriConts.count;
}

SpatialObjectGroup::TriContainer& SpatialObjectGroup_KDTree::getTriContainer(const Index& index) {

	return dref(mStaticTriConts[index]);
}

void SpatialObjectGroup_KDTree::getTris(const SpatialObjectGroup::TriContainer& _container, FctTriProcess fct, void* context) {

	TriContainer& container = (TriContainer&) _container;

	container.getTris(fct, context);
}

SpatialObjectGroup_KDTree::Index SpatialObjectGroup_KDTree::getVolumeCount() {

	return mStaticVols.count;
}

SpatialVolumeBinding* SpatialObjectGroup_KDTree::getVolume(const Index& index, VolumeStruct*& pVolumeOut) {

	SoftPtr<SpatialVolumeBinding> binding = mStaticVols[index];

	pVolumeOut = &binding->volume;

	return binding;
}


SpatialObjectGroup_KDTree::Index SpatialObjectGroup_KDTree::getDynamicVolumeCount() {

	return mDynVols.count;
}

SpatialVolumeBinding* SpatialObjectGroup_KDTree::getDynamicVolume(const Index& index, VolumeStruct*& pVolumeOut) {

	SoftPtr<SpatialVolumeBinding> binding = mDynVols[index];

	pVolumeOut = &binding->volume;

	return binding;
}

}