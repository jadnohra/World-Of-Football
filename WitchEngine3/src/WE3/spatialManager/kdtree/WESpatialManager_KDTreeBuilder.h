#ifndef WESpatialManager_KDTreeBuilder_h
#define WESpatialManager_KDTreeBuilder_h

#include "WESpatialManager_KDTreeBase.h"
#include "../WESpatialManagerBuilder.h"
#include "../WESpatialObjectContainer.h"

namespace WE {

	class SpatialManager_KDTreeBuilder 
		: public SpatialManagerBuilder
		, public SpatialManager_KDTreeBase
		, protected SpatialObjectContainerObjects {

	public:

		SpatialManager_KDTreeBuilder(SpatialManager* pCreator);
		
		virtual void* loadConfiguration(BufferString& tempStr, DataChunk* pChunk, CoordSysConv* pConv);
		virtual void destroyConfiguration(void*& pConfig);

		virtual bool process(void* pConfig, bool destroyConfig);
		virtual bool build(SpatialManager& targetManager);

		virtual void clear();

		virtual bool add(SpatialTriContainer* pSpatial, ObjectBinding* pBinding, bool destroyWhenDone);
		virtual bool add(VolumeStruct& spatial, SpatialVolumeBinding* pBinding);
		virtual bool addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding);

		virtual bool isCacheValid() { return false; }
		virtual bool loadFromCache() { return false; }
		virtual bool saveToCache() { return false; }

		virtual void render(Renderer& renderer);

	protected:

		typedef SpatialObjectContainerObjects::Index Index;

		HardPtr<SpatialObjectContainer> mContainer;

		HardPtr<TreeNode> mTreeRoot;
		HardPtr<TriBufferManager> mGlobalTriBuffer;
		HardPtr<StaticVolManager> mGlobalStaticVolumes;
	};

}

#endif