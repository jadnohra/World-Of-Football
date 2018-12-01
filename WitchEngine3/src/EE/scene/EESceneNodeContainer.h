#ifndef _EESceneNodeContainer_h
#define _EESceneNodeContainer_h

#include "WE3/string/WEBufferString.h"
#include "WE3/WETL/WETLHashMap.h"
using namespace WE;

#include "EESceneNode.h"
#include "EESceneDataTypes.h"

namespace EE {

	class SceneNodeContainer {
	public:

		SceneNodeContainer();
		~SceneNodeContainer();

		bool putNode(SceneNode* pNode);

		SceneNode* getNode(const BufferString& sceneName);
		SceneNode* getNode(const TCHAR* sceneName);
		bool removeNode(const BufferString& sceneName);

		//void releaseUnused();
		SceneNodeIndex::Type getObjectCount();

	protected:

		typedef StringHash NodeKey;
		typedef WETL::PtrHashMap<NodeKey, SceneNode*, SceneNodeIndex::Type, WETL::ResizeDouble, 16> NodeMap;

		NodeMap mNodeMap;

		inline NodeKey nodeKey(const BufferString& sceneName);
		inline NodeKey nodeKey(const TCHAR* sceneName);
	};
}


#endif