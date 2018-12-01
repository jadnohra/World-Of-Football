#include "EESceneMeshContainer.h"

#include "EEScene.h"

namespace EE {

SceneMeshContainer::SceneMeshContainer(Scene& scene) : mScene(&scene) {
}

SceneMeshContainer::~SceneMeshContainer() {
}

SceneMeshContainer::MeshKey SceneMeshContainer::meshKey(const BufferString& sceneName) {

	return sceneName.hash();
}

bool SceneMeshContainer::putMesh(const BufferString& sceneName, Mesh* pMesh) {

	return mMeshMap.insert(meshKey(sceneName), pMesh);
}

Mesh* SceneMeshContainer::getMesh(const BufferString& sceneName) {
	
	Mesh* pRet = NULL;

	mMeshMap.find(meshKey(sceneName), pRet);
	return pRet;
}

bool SceneMeshContainer::removeMesh(const BufferString& sceneName) {

	Mesh* pRem = NULL;

	return mMeshMap.remove(meshKey(sceneName), pRem);
}

void SceneMeshContainer::releaseUnused() {

	mMeshMap.removeOneRefObjects();
}

SceneMeshIndex::Type SceneMeshContainer::getObjectCount() {

	return mMeshMap.getCount();
}

}