/*
#ifndef _WESpatialDynamicVolumeImpl_h
#define _WESpatialDynamicVolumeImpl_h

#include "WESpatialManagerObjects.h"
#include "../WETL/WETLArray.h"

namespace WE {

	class SpatialDynamicVolumeImpl : public SpatialDynamicVolume {
	public:

		virtual void spatialClear();

		virtual void spatialAdd(SpatialTriContainer::Partial& object);
		virtual void spatialAdd(SpatialVolume& object);
		virtual void spatialAdd(SpatialDynamicVolume& object);

		virtual void spatialRemove(SpatialTriContainer::Partial& object);
		virtual void spatialRemove(SpatialVolume& object);
		virtual void spatialRemove(SpatialDynamicVolume& object);

	protected:

		typedef WETL::CountedArray<SpatialTriContainer::Partial*, false, Index, WETL::ResizeExact> TriContainerPartials;
		typedef WETL::CountedArray<SpatialVolume*, false, Index, WETL::ResizeExact> Volumes;
		typedef WETL::CountedArray<SpatialDynamicVolume*, false, Index, WETL::ResizeExact> DynamicVolumes;

		TriContainerPartials mTriContainerPartials;
		Volumes mVolumes;
		DynamicVolumes mDynamicVolumes;
	};
}

#endif
*/