#ifndef _WESpatialManager_h
#define _WESpatialManager_h

#include "WESpatialManagerObjects.h"
#include "../WEPtr.h"
#include "../WETL/WETLArray.h"
#include "../WEDataTypes.h"
#include "../render/WERenderer.h"

namespace WE {

	class SpatialManagerBuilder;

	class SpatialManager : public SpatialManagerTypeBase {
	public:

		virtual ~SpatialManager() {};

		virtual SpatialManagerBuilder* createBuilder() = 0;
		virtual bool canBuildDynamically() { return false; }

		virtual void destroy() = 0;

		virtual void setListener(SpatialEventListener* pListener) { mListener = pListener; }

		virtual bool add(SpatialTriContainer& object) { return false; }
		virtual bool add(SpatialVolume& object) { return false; }
		//virtual bool modify(SpatialTriContainer& object, SpatialTriContainer::Modification& modif) = 0;
		virtual bool remove(SpatialTriContainer& object) { return false; }
		virtual bool remove(SpatialVolume& object) { return false; }

		virtual bool add(SpatialDynamicVolume& object) = 0;
		virtual bool update(SpatialDynamicVolume& object) = 0;
		virtual bool remove(SpatialDynamicVolume& object) = 0;

		virtual void render(Renderer& renderer, 
								bool renderMain = true, const RenderColor* pMainColor = NULL, 
								bool renderContainers = true, const RenderColor* pContainersColor = NULL, 
								bool renderDetails = false, const RenderColor* pDetailColor = NULL) {}

		virtual Index getCellCount(SpatialDynamicVolume& object) = 0;
		virtual SpatialManagerCell& getCell(SpatialDynamicVolume& object, const Index& cellIndex) = 0;

	protected:

		SoftPtr<SpatialEventListener> mListener;
	};

	class SpatialManagerBuilder : public SpatialManagerTypeDefs {
	public:

		struct SubdivisionSettings {

			Vector3 minCellSize;
			UINT32 maxDepth;
			Index maxCellElementCount;
			Index splitPreferenceFactor; //the bigger this is the more balanced splits will be made, the small, the more 'free space' splits will be made
			float freeSpaceLoad;
			float largeVolumeCoeff;

			SubdivisionSettings(const Vector3* pMinCellSize = NULL, const UINT32* pMaxDepth = NULL, 
								const Index* pMaxCellElementCount = NULL, const Index* pSplitPreferenceFactor = NULL,
								const float* pFreeSpaceLoad = NULL, const float* pLargeVolumeCoeff = NULL); 
		};

	public:

		SpatialManagerBuilder(SpatialManager* pCreator = NULL) { mCreator = pCreator; }

		virtual bool process(const SubdivisionSettings& settings) = 0;
		virtual bool build(SpatialManager& targetManager) = 0;

		virtual void clear() = 0;

		virtual bool add(SpatialTriContainer& object) = 0;
		virtual bool add(SpatialVolume& object) = 0;

		virtual bool isCacheValid() = 0;
		virtual bool loadFromCache() = 0;
		virtual bool saveToCache() = 0;

		virtual void render(Renderer& renderer, 
								bool renderMain = true, const RenderColor* pMainColor = NULL, 
								bool renderContainers = true, const RenderColor* pContainersColor = NULL, 
								bool renderDetails = false, const RenderColor* pDetailColor = NULL) {}

	protected:

		SoftPtr<SpatialManager> mCreator;
	};

	class SpatialManagerBuildPhase {
	public:

		//~SpatialManagerBuildPhase() { finalize(); }

		bool init(SpatialManager& manager, bool tryLoadFromCache);
		bool finalize(const SpatialManagerBuilder::SubdivisionSettings& settings);

		bool add(SpatialTriContainer& object, bool keep = true);
		bool add(SpatialVolume& object, bool keep = true);
		bool add(SpatialDynamicVolume& object);

	protected:

		void keep(SpatialTriContainer& object);
		void keep(SpatialVolume& object);
		void keep(SpatialDynamicVolume& object);

		bool addKept(bool clearKept);

	protected:

		typedef UINT32 Index;

		SoftPtr<SpatialManager> mManager;
		HardPtr<SpatialManagerBuilder> mBuilder;

		bool isCacheValid;

		typedef WETL::CountedArray<SpatialTriContainer*, false, Index, WETL::ResizeDouble>  SpatialTriObjectArray;
		typedef WETL::CountedArray<SpatialVolume*, false, Index, WETL::ResizeDouble>  SpatialVolumeArray;
		typedef WETL::CountedArray<SpatialDynamicVolume*, false, Index, WETL::ResizeDouble>  SpatialDynamicVolumeArray;

		SpatialTriObjectArray mKeptTriObjects;
		SpatialVolumeArray mKeptVolumes;
		SpatialDynamicVolumeArray mKeptDynamicVolumes;
	};

}

#endif