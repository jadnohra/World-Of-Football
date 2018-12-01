#ifndef _WEISpatialManager_h
#define _WEISpatialManager_h

#include "WESpatialObjects.h"
#include "../render/WERenderer.h"

namespace WE {

	class SpatialManagerBuilder;

	class SpatialListener_DynamicVolumeEvents {
	public:

		/*
		virtual void spatialStartUpdate(SpatialDynamicVolume& dynObject, void*& pOutContext) = 0;

		virtual void spatialClearCells(SpatialDynamicVolume& dynObject, void* pContext) = 0;
		virtual void spatialEnteredCell(SpatialDynamicVolume& dynObject, SpatialManagerCell& cell, void* pContext) = 0;
		virtual void spatialLeftCell(SpatialDynamicVolume& dynObject, SpatialManagerCell& cell, void* pContext) = 0;

		virtual void spatialEndUpdate(SpatialDynamicVolume& dynObject, void* pContext) = 0;
		*/
	};

	/*
		The TrianglEx1 objects and volume struct objects returned from queryDynamicVolume
		are guaranteed to remain valid so pointers to them can be used
	*/

	class SpatialManager {
	public:

		virtual ~SpatialManager() {};

		virtual SpatialManagerBuilder* createBuilder() = 0;

		virtual bool canBuildDynamically() { return false; }
		virtual bool providesDynamicVolumeEvents() { return false; }
		virtual bool canRemoveTris() { return false; }
		virtual bool canRemoveVolumes() { return false; }

		virtual void destroy() = 0;

		virtual bool setListener(SpatialListener_DynamicVolumeEvents* pListener) { return false; }

		virtual bool add(SpatialTriContainer* pSpatial, ObjectBinding* pBinding) { return false; }
		virtual bool add(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) { return false; }
		virtual bool removeTris(ObjectBinding* pBinding) { return false; }
		virtual bool removeVolume(SpatialVolumeBinding* pBinding) { return false; }

		virtual bool addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) = 0;
		virtual Volume* updateStartDynamicVolume(SpatialVolumeBinding* pBinding) = 0;
		virtual bool updateEndDynamicVolume(Volume* pVolume, SpatialVolumeBinding* pBinding) = 0;
		virtual bool removeDynamicVolume(SpatialVolumeBinding* pBinding) = 0;

		//SpatialObjectGroup can and better be reused between queries
		virtual SpatialObjectGroup* createSpatialObjectGroup() = 0;
		virtual bool queryDynamicVolume(SpatialVolumeBinding* pBinding, VolumeStruct*& pOutVolume, SpatialObjectGroup& result) = 0;

		virtual void render(Renderer& renderer) {}
	};

	
}

#endif