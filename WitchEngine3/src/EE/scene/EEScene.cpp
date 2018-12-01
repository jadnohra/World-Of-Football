#include "EEScene.h"


namespace EE {

Scene::Scene() : mNodeIdCounter(0) {

	mFlagExtraRenders = false;
	mFlagRenderGhostNodes = false;
	mFlagRenderLocationNodes = false;
	mFlagRenderNPColliders = false;
	mFlagRenderVolumes = false;
	mFlagRenderDetailVolumes = false;
}

Scene::~Scene() {
}


bool Scene::init(const CoordSys& coordSys, LoadContext& loadContext, ICollDetector* pCollDetector, CollMaterialManager* pCollMatManager) {

	if (SceneBase::baseInit(coordSys, loadContext) == false) {

		return false;
	}

	mFlagRenderVolumes = false;
	mCollDetector = pCollDetector;
	mCollMatManager = pCollMatManager;

	if (true) {

		assrt(mNodes.isNull());
		mNodes.destroy();

		MMemNew(mNodes.ptrRef(), SceneNodeContainer());
	}

	return true;
}

SceneNodeIndex::Type Scene::genNodeId() {

	return mNodeIdCounter++;
}

bool Scene::hasCollDetector() {

	return mCollDetector.isValid();
}

void Scene::enableRenderVolumes(bool enable) {

	mFlagRenderVolumes = enable;
}

bool Scene::isEnabledRenderVolumes() {

	return mFlagRenderVolumes;
}

void Scene::enableRenderDetailVolumes(bool enable) {

	mFlagRenderDetailVolumes = enable;
}

bool Scene::isEnabledRenderDetailVolumes() {

	return mFlagRenderDetailVolumes;
}

void Scene::enableRenderGhostNodes(bool enable, float size) {

	mFlagRenderGhostNodes = enable;
	mFlagRenderGhostNodeSize = size;
}

bool Scene::isEnabledRenderGhostNodes() {

	return mFlagRenderGhostNodes;
}

void Scene::enableRenderLocationNodes(bool enable, float size) {

	mFlagRenderLocationNodes = enable;
	mFlagRenderLocationNodeSize = size;
}

bool Scene::isEnabledRenderLocationNodes() {

	return mFlagRenderLocationNodes;
}

void Scene::enableRenderNPColliders(bool enable) {

	mFlagRenderNPColliders = enable;
}

bool Scene::isEnabledRenderNPColliders() {

	return mFlagRenderNPColliders;
}

void Scene::enableExtraRenders(bool enable) {

	mFlagExtraRenders = enable;
}

bool Scene::isEnabledExtraRenders() {

	return mFlagExtraRenders;
}


}