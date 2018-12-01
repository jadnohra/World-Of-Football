#ifndef _EESceneLoad_v1_h
#define _EESceneLoad_v1_h

#include "EEScene.h"
#include "EEISceneNodeFactory.h"
#include "node/EESceneNodeCamera.h"

#include "WE3/data/WEDataSourceChunk.h"
using namespace WE;

namespace EE {


	class SceneLoader_v1 {	
	public:

		static bool load(Scene& scene, SceneNode& rootNode, DataChunk& sceneChunk, ISceneNodeFactory& nodeFactory, ISceneGameNodeFactory* pGameFactory = NULL, SceneNodeCamera* pCam = NULL);
	};

}

#endif