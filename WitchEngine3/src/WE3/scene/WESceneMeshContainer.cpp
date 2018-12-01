#include "WESceneMeshContainer.h"

namespace WE {

SceneMeshContainer::SceneMeshContainer() {
}

SceneMeshContainer::~SceneMeshContainer() {
}

SceneMeshContainer::MeshKey SceneMeshContainer::meshKey(const TCHAR*  sceneName) {

	return String::hash(sceneName);
}

bool SceneMeshContainer::putMesh(const TCHAR*  sceneName, Mesh* pMesh) {

	return mMeshMap.insert(meshKey(sceneName), pMesh);
}

Mesh* SceneMeshContainer::getMesh(const TCHAR*  sceneName) {
	
	Mesh* pRet = NULL;

	mMeshMap.find(meshKey(sceneName), pRet);
	return pRet;
}

bool SceneMeshContainer::removeMesh(const TCHAR*  sceneName) {

	return mMeshMap.remove(meshKey(sceneName));
}

void SceneMeshContainer::releaseUnused() {

	mMeshMap.removeOneRefObjects();
}

SceneMeshIndex::Type SceneMeshContainer::getObjectCount() {

	return mMeshMap.getCount();
}

}