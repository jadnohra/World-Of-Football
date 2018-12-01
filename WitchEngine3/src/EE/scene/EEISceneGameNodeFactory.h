#ifndef _EEISceneGameNodeFactory_h
#define _EEISceneGameNodeFactory_h

#include "WE3/string/WEBufferString.h"
#include "WE3/data/WEDataSource.h"
#include "WE3/coll/WECollDataTypes.h"
using namespace WE;

#include "EESceneNode.h"

namespace EE {

	class Scene;

	class ISceneGameNodeFactory {
	public:

		virtual ~ISceneGameNodeFactory() {};

		//collMask can be set even for none-game objects, it will be used on the none-game entity
		virtual bool getGameType(BufferString& tempStr, Scene& scene, DataChunk& chunk, ObjectType& baseType, ObjectType& gameType, CollMask& collMask) = 0;
		virtual SceneNode* create(BufferString& tempStr, Scene& scene, DataChunk& chunk, const ObjectType& baseType, const ObjectType& gameType) = 0;
		virtual bool init(BufferString& tempStr, Scene& scene, DataChunk& chunk, SceneNode* pNode) = 0;
		virtual void destroy(BufferString& tempStr, Scene& scene, DataChunk& chunk, SceneNode* pNode) = 0;
	};

}

#endif