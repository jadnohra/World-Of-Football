#include "EESceneNodeContainer.h"

namespace EE {

SceneNodeContainer::SceneNodeContainer() {
}

SceneNodeContainer::~SceneNodeContainer() {
}

SceneNodeContainer::NodeKey SceneNodeContainer::nodeKey(const BufferString& sceneName) {

	return sceneName.hash();
}

SceneNodeContainer::NodeKey SceneNodeContainer::nodeKey(const TCHAR* sceneName) {

	return String::hash(sceneName);
}

bool SceneNodeContainer::putNode(SceneNode* pNode) {

	return mNodeMap.insert(nodeKey(pNode->mNodeName), pNode);
}

SceneNode* SceneNodeContainer::getNode(const BufferString& sceneName) {
	
	SceneNode* pRet = NULL;

	mNodeMap.find(nodeKey(sceneName), pRet);
	return pRet;
}

SceneNode* SceneNodeContainer::getNode(const TCHAR* sceneName) {
	
	SceneNode* pRet = NULL;

	mNodeMap.find(nodeKey(sceneName), pRet);
	return pRet;
}

bool SceneNodeContainer::removeNode(const BufferString& sceneName) {

	return mNodeMap.remove(nodeKey(sceneName));
}


//void SceneNodeContainer::releaseUnused() {
//
//	mNodeMap.removeOneRefObjects();
//}


SceneNodeIndex::Type SceneNodeContainer::getObjectCount() {

	return mNodeMap.getCount();
}


}
