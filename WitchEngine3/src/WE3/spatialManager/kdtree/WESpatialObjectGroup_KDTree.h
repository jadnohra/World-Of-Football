#ifndef _WESpatialObjectGroup_KDTree_h
#define _WESpatialObjectGroup_KDTree_h

#include "../WESpatialManager.h"
#include "WESpatialManager_KDTreeBase.h"

namespace WE {

	class SpatialObjectGroup_KDTree : public SpatialObjectGroup, public SpatialManager_KDTreeBase {
	public:

		typedef SpatialObjectGroup::Index Index;

		class TriContainer : public SpatialObjectGroup::TriContainer {
		public:

			virtual ObjectBinding* getBinding();
			virtual VolumeStruct* getBoundingVolume(VolumeRef& holder);

		public:

			inline ObjectBinding* binding() { return mBinding; }

			void reset();

			void getTris(FctTriProcess fct, void* context);
			void addTriCont(StaticTriCont* pTriCont, const AAB& boundingVol, bool boundingVolAlreadyAdded);

		public:

			typedef WETL::CountedArray<StaticTriCont*, false, Index> TriConts;

			SoftPtr<ObjectBinding> mBinding;
			TriConts mTriConts;
			AAB mBoundingVol;
		};

	public:

		virtual ~SpatialObjectGroup_KDTree();

		virtual void reset();
		virtual void destroy();

		virtual Index getTriContainerCount();
		virtual SpatialObjectGroup::TriContainer& getTriContainer(const Index& index);
		virtual	void getTris(const SpatialObjectGroup::TriContainer& container, FctTriProcess fct, void* context);

		virtual Index getVolumeCount();
		virtual SpatialVolumeBinding* getVolume(const Index& index, VolumeStruct*& pVolumeOut);

		virtual Index getDynamicVolumeCount();
		virtual SpatialVolumeBinding* getDynamicVolume(const Index& index, VolumeStruct*& pVolumeOut);

	public:

		void uniqueAddTriCont(StaticTriCont* pTriCont, const AAB& boundingVol, bool boundingVolAlreadyAdded); //returns true if unique
		bool uniqueAddVolume(SpatialVolumeBinding* pVolume); //returns true if unique
		bool uniqueAddDynamicVolume(SpatialVolumeBinding* pVolume); //returns true if unique

		//normally used internally
		TriContainer* uniqueAddTriCont(ObjectBinding* pBinding, bool& isUnique);

	protected:

		typedef WETL::CountedPtrArrayEx<TriContainer, Index, WETL::ResizeDouble, true> StaticTriConts;
		typedef WETL::CountedArray<SpatialVolumeBinding*, false, Index> StaticVols;
		typedef WETL::CountedArray<SpatialVolumeBinding*, false, Index> DynVols;

		StaticTriConts mStaticTriConts;
		StaticVols mStaticVols;
		DynVols mDynVols;
	};

}

#endif