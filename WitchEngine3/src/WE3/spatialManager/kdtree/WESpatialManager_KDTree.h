#ifndef WESpatialManager_KDTree_h
#define WESpatialManager_KDTree_h

#include "WESpatialManager_KDTreeBase.h"
#include "../WEISpatialManager.h"

namespace WE {

	/*
		The Current Implementation may report a triangle more than once in
		SpatialObjectGroup, the only efficient way to solve this is with triangle 
		splitting
	*/

	class SpatialManager_KDTree 
		: public SpatialManager
		, public SpatialManager_KDTreeBase
		, protected SpatialObjectContainerObjects {

	public:

		virtual SpatialManagerBuilder* createBuilder();

		virtual void destroy();

		virtual bool addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding);
		virtual Volume* updateStartDynamicVolume(SpatialVolumeBinding* pBinding);
		virtual bool updateEndDynamicVolume(Volume* pVolume, SpatialVolumeBinding* pBinding);
		virtual bool removeDynamicVolume(SpatialVolumeBinding* pBinding);

		virtual void render(Renderer& renderer);

		//SpatialObjectGroup can and better be reused between queries
		virtual SpatialObjectGroup* createSpatialObjectGroup();
		virtual bool queryDynamicVolume(SpatialVolumeBinding* pBinding, VolumeStruct*& pOutVolume, SpatialObjectGroup& result);

	protected:

		friend class SpatialManager_KDTreeBuilder;

		typedef SpatialObjectContainerObjects::Index Index;

		HardPtr<TreeNode> mTreeRoot;
		HardPtr<TriBufferManager> mGlobalTriBuffer;
		HardPtr<StaticVolManager> mGlobalStaticVolumes;
	};

}

#endif