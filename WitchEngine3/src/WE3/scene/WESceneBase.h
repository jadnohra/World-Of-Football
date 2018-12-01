#ifndef _WESceneBase_h
#define _WESceneBase_h

#include "../coll/broadPhase/WEICollDetector.h"
#include "../loadContext/WELoadContext.h"
#include "../coordSys/WECoordSys.h"
#include "../WETL/WETLHashMap.h"

#include "WESceneMeshContainer.h"
#include "WESceneDirection.h"

namespace WE {

	
	class SceneBase {
	public:

		SceneBase();
		~SceneBase();

		bool baseInit(const CoordSys& sceneCoordSys, LoadContext& loadContext);

		Renderer& renderer();


	public:

		CoordSys mCoordSys;

		Vector3 mCoordSysAxis[3];
		bool mCoorSysRotLeftHanded;
		
		SoftPtr<LoadContext> mLoadContext;
		HardPtr<SceneMeshContainer> mMeshes;
	
	protected:
		
		bool mIsInited;
	};
}

#endif