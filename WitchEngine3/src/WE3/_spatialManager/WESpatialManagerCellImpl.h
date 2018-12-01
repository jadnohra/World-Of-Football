#ifndef _SpatialManagerCellImpl_h
#define _SpatialManagerCellImpl_h

#include "WESpatialManagerObjects.h"
#include "../WETL/WETLArray.h"

namespace WE {

	class SpatialManagerCellImpl : public SpatialManagerCell {
	public:

		virtual Index getTriContainerCount() { return mTriObjects.size; }
		virtual SpatialTriContainer::Partial& getTriContainer(const Index& index) { return dref(mTriObjects.el[index]); }

		virtual Index getVolumeCount() { return mVolumes.count; }
		virtual SpatialVolume& getVolume(const Index& index) { return dref(mVolumes.el[index]); }

		virtual Index getDynamicVolumeCount() { return mDynamicVolumes.count; }
		virtual SpatialDynamicVolume& getDynamicVolume(const Index& index) { return dref(mDynamicVolumes.el[index]); }

	public:

		typedef SpatialTriContainer::Partial::PtrArray SpatialTriObjects;
		typedef WETL::CountedArray<SpatialVolume*, false, Index> SpatialVolumes;
		typedef WETL::CountedArray<SpatialDynamicVolume*, false, Index> SpatialDynamicVolumes;

		SpatialTriObjects mTriObjects;
		SpatialVolumes mVolumes;
		SpatialDynamicVolumes mDynamicVolumes;
	};
}

#endif