#ifndef _EEScene_h
#define _EEScene_h

#include "WE3/coll/broadPhase/WEICollDetector.h"
#include "WE3/loadContext/WELoadContext.h"
#include "WE3/coordSys/WECoordSys.h"
#include "WE3/WETL/WETLHashMap.h"
#include "WE3/scene/WESceneBase.h"
#include "WE3/coll/narrowPhase/WECollMaterialManager.h"
using namespace WE;

#include "EESceneNodeContainer.h"


namespace EE {

	
	class Scene : public SceneBase {
	public:

		Scene();
		~Scene();

		bool init(const CoordSys& sceneCoordSys, LoadContext& loadContext, ICollDetector* pCollDetector = NULL, CollMaterialManager* pCollMatManager = NULL);

		bool hasCollDetector();
		
		void enableExtraRenders(bool enable);

		void enableRenderVolumes(bool enable);
		void enableRenderDetailVolumes(bool enable);
		void enableRenderNPColliders(bool enable);

		void enableRenderGhostNodes(bool enable, float sizeUnits);
		void enableRenderLocationNodes(bool enable, float sizeUnits);

		bool isEnabledExtraRenders();
		bool isEnabledRenderVolumes();
		bool isEnabledRenderDetailVolumes();
		bool isEnabledRenderGhostNodes();
		bool isEnabledRenderLocationNodes();
		bool isEnabledRenderNPColliders();

		SceneNodeIndex::Type genNodeId();

	public:

		SceneNodeIndex mNodeIdCounter;

		bool mFlagExtraRenders;

		bool mFlagRenderVolumes;
		bool mFlagRenderDetailVolumes;
		bool mFlagRenderNPColliders;

		bool mFlagRenderGhostNodes;
		float mFlagRenderGhostNodeSize;

		bool mFlagRenderLocationNodes;
		float mFlagRenderLocationNodeSize;

		HardRef<ICollDetector> mCollDetector;
		HardRef<CollMaterialManager> mCollMatManager;
		HardPtr<SceneNodeContainer> mNodes;

	};
}

#endif