#ifndef _WESpatialManagerBuildPhase_h
#define _WESpatialManagerBuildPhase_h

#include "WESpatialManagerBuilder.h"
#include "WESpatialObjectContainer.h"

namespace WE {

	class SpatialManagerBuildPhase {
	public:

		//~SpatialManagerBuildPhase() { finalize(); }
		SpatialManagerBuildPhase();
		~SpatialManagerBuildPhase();

		bool init(SpatialManager& manager, bool tryLoadFromCache);
		bool finalize(void* pConfig = NULL); //uses loaded config if NULL passed

		void* loadConfiguration(BufferString& tempStr, DataChunk* pChunk, CoordSysConv* pConv, bool saveConfiguration = true);
		void destroyConfiguration(void*& pConfig);


		//pSpatial has to remain valid until finalize is called, it will be destroyed automatically
		//if destroyOnFinalize is true
		bool add(SpatialTriContainer* pSpatial, ObjectBinding* pBinding, bool destroyOnFinalize);
		bool add(VolumeStruct& spatial, SpatialVolumeBinding* pBinding);
		bool addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding);

	protected:

		SoftPtr<SpatialManager> mManager;
		HardPtr<SpatialManagerBuilder> mBuilder;

		void* mpConfig;

		bool isCacheValid;
	};

}

#endif