#ifndef _EEISceneNodeFactory_h
#define _EEISceneNodeFactory_h

#include "WE3/string/WEBufferString.h"
#include "WE3/data/WEDataSource.h"
using namespace WE;

#include "EEISceneGameNodeFactory.h"
#include "EESceneNode.h"

namespace EE {

	class Scene;

	class ISceneNodeFactory {
	public:

		virtual ~ISceneNodeFactory() {};

		virtual SceneNode* create(BufferString& tempStr, Scene& scene, DataChunk& chunk, ISceneGameNodeFactory* pGameFactory) = 0;
	};

}

#endif

