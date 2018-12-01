#ifndef _WESpatialObjectContainer_h
#define _WESpatialObjectContainer_h

#include "WESpatialObjects.h"
#include "WEISpatialManager.h"
#include "WESpatialManagerBuilder.h"

namespace WE {

	class SpatialObjectContainerObjects {
	public:

		typedef SpatialIndex Index;

		struct TriCont {

			FlexiblePtr<SpatialTriContainer> spatial;
			SoftPtr<ObjectBinding> binding;

			void init(SpatialTriContainer* _pSpatial, ObjectBinding* pBinding, bool destroyWhenDone) {

				spatial.set(_pSpatial, destroyWhenDone ? Ptr_HardRef : Ptr_SoftRef);
				binding = pBinding;
			}
		};

		struct Vol {

			SoftPtr<SpatialVolumeBinding> binding;

			void init(VolumeStruct& _spatial, SpatialVolumeBinding* pBinding) {

				binding = pBinding;
				binding->volume.init(_spatial);
			}

			inline Volume& volume() { return binding->volume; }
		};

		typedef WETL::CountedPtrArrayEx<TriCont, Index>  TriContArray;
		typedef WETL::CountedPtrArrayEx<Vol, Index> VolArray;
		typedef WETL::CountedPtrArrayEx<Vol, Index> DynVolArray;
	};

	class SpatialObjectContainer : public SpatialObjectContainerObjects {
	public:

		~SpatialObjectContainer();

		void add(SpatialTriContainer* pSpatial, ObjectBinding* pBinding, bool destroyWhenDone = false);
		void add(VolumeStruct& spatial, SpatialVolumeBinding* pBinding);
		void addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding);

		bool addObjectsTo(SpatialManager& target, bool clearObjects);
		//bool addObjectsTo(SpatialManagerBuilder& target, bool clearObjects);

	public:

		TriContArray mTriConts;
		VolArray mVols;
		DynVolArray mDynVols;
	};

}

#endif