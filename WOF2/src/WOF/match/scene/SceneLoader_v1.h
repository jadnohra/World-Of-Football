#ifndef h_WOF_match_SceneLoader_v1
#define h_WOF_match_SceneLoader_v1

#include "SceneNodeFactory.h"

#include "WE3/data/WEDataSourceChunk.h"
#include "WE3/loadContext/WELoadContext.h"
using namespace WE;

namespace WOF { namespace match {

	class SceneLoaderClient {
	public:

		virtual void loadUnknownChunk(BufferString& tempStr, Match& match, DataChunk& chunk, const CoordSys& destCoordSys, CoordSysConv* pConv) = 0;
	};

	class SoundManager;

	class SceneLoader_v1 {	
	public:

		static bool load(Match& match, SceneNode& rootNode, DataChunk& sceneChunk, SceneNodeFactory& nodeFactory, SceneLoaderClient* pClient);

		static bool loadTracks(BufferString& tempStr, SoundManager& soundMan, LoadContext& loadContext, DataChunk& chunk, int &errCount);

		static bool loadEntities(BufferString& tempStr, Match& match, DataChunk& chunk, 
										SceneNodeFactory& nodeFactory, CoordSysConv* pConv, int& errCount);
	};

} }

#endif