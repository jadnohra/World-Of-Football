#ifndef _WESpatialManagerBuilder_h
#define _WESpatialManagerBuilder_h

#include "WEISpatialManager.h"
#include "../data/WEDataSource.h"
#include "../coordSys/WECoordSysConv.h"

namespace WE {

	class SpatialManagerBuilder {
	public:

		SpatialManagerBuilder(SpatialManager* pCreator = NULL) { mCreator = pCreator; }
		virtual ~SpatialManagerBuilder() {}

		virtual void* loadConfiguration(BufferString& tempStr, DataChunk* pChunk, CoordSysConv* pConv) { return NULL; }
		virtual void destroyConfiguration(void*& pConfig) {}

		virtual bool process(void* pConfig, bool destroyConfig) = 0;
		virtual bool build(SpatialManager& targetManager) = 0;

		virtual void clear() = 0;

		//pSpatial has to remain valid until build is called
		//it will be destoyed automatically after build if destroyOnBuild is true
		virtual bool add(SpatialTriContainer* pSpatial, ObjectBinding* pBinding, bool destroyOnBuild) = 0;
		virtual bool add(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) = 0;
		virtual bool addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) = 0;

		virtual bool isCacheValid() = 0;
		virtual bool loadFromCache() = 0;
		virtual bool saveToCache() = 0;

		virtual void render(Renderer& renderer) {}

	protected:

		SoftPtr<SpatialManager> mCreator;
	};
}

#endif