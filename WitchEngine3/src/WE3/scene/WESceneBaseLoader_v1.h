#ifndef _WESceneBaseLoader_v1_h
#define _WESceneBaseLoader_v1_h

#include "WESceneBase.h"
#include "../data/WEDataSourceChunk.h"
#include "../audio/WEAudioEngine.h"

namespace WE {


	class SceneBaseLoader_v1 {	
	public:
		
		static void loadDebug(BufferString& tempStr, DataChunk& dbgChunk, int& errCount);

		static bool loadMesh(BufferString& tempStr, SceneMeshContainer& container, LoadContext& loadContext, DataChunk& meshChunk, const CoordSys& loadDestCoordSys, CoordSysConv* pFileToSceneConv, int& errCount);
		static bool loadMeshes(BufferString& tempStr, SceneMeshContainer& container, LoadContext& loadContext, DataChunk& meshesChunk, const CoordSys& loadDestCoordSys, CoordSysConv* pFileToSceneConv, int& errCount);

		static bool loadSounds(BufferString& tempStr, AudioBufferContainer& container, LoadContext& loadContext, DataChunk& soundsChunk, CoordSysConv* pFileToSceneConv, int& errCount);

		static bool loadCoordSys(BufferString& tempStr, LoadContext& loadContext, 
										const CoordSys& loadDestCoordSys, DataChunk& sceneChunk, 
										CoordSys& fileSceneCoordSys, SoftRef<CoordSysConv>& fileToSceneConv, int& errCount);

		//static void loadDebug(BufferString& tempStr, SceneBase& scene, DataChunk& dbgChunk);
		//static bool loadMeshes(BufferString& tempStr, SceneBase& scene, DataChunk& meshesChunk, CoordSys& loadDestCoordSys);
		//static bool loadCoordSys(BufferString& tempStr, SceneBase& scene, DataChunk& sceneChunk, 
		//								CoordSys& fileSceneCoordSys, SoftRef<CoordSysConv>& fileToSceneConv,
		//								CoordSys& loadDestCoordSys);

	};

}

#endif