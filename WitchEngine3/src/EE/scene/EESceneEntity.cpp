#include "EESceneEntity.h"

#include "EESceneNode.h"
#include "EEScene.h"

#include "WE3/coll/broadPhase/WEICollDetector.h"
using namespace WE;

namespace EE {

SceneEntityBitFlagType SceneEntity::kInitFlag_1_AttachCollider(1);

SceneEntity* SceneEntity::fromObject(Object* pObject) {

	return (SceneEntity*) pObject;
}

SceneEntity* SceneEntity::fromNode(SceneNode* pNode) {

	return (SceneEntity*) pNode;
}

SceneEntity::SceneEntity() {

	mEntInitFlags = 0;
	mEntIsVisible = true;
	mEntIsHighlighted = false;

	mEntCollIsActive = false;
	mEntCollEnable = false;

	CollMaterialManager::invalidateCollMaterial(mEntVolumeMaterial);
}

SceneEntity::~SceneEntity() {

	entDetachCollider();
}

void SceneEntity::entHighlight(bool highlight) {

	mEntIsHighlighted = highlight;
}

bool SceneEntity::entInitCollMaterial(Scene& scene, const String& collMaterial) {

	if (collMaterial.isValid() && scene.mCollMatManager.isValid()) {

		return scene.mCollMatManager->getCollMaterial(collMaterial.hash(), mEntVolumeMaterial);
	}

	CollMaterialManager::invalidateCollMaterial(mEntVolumeMaterial);
	return false;
}

void SceneEntity::entEnableCollider(bool isCollider, const CollMask* pMask) {

	if (mEntCollEnable) {

		if ((isCollider == false) && (entIsAttachedCollider()) ) {
		
			entDetachCollider();
		}

	} else {

		if (isCollider) {
			
			if ((mNodeIsDirty == false) && entIsValidWorldVolume()) {

				entAttachCollider();

			} else {
	
				entCreateColliderData(dref<const CollMask>(pMask));
				_entSetNeedsAttachCollider(true);
			}
		}

	}

	mEntCollEnable = isCollider;
}

bool SceneEntity::entColliderHasCollisions() {

	return mNodeScene->mCollDetector->objectHasColls(*this, mEntCollData);
}

bool SceneEntity::entColliderIsActive() {

	return mEntCollIsActive != 0;
}

bool SceneEntity::entColliderIsAttaching() {

	return (mEntCollEnable && (entNeedsAttachCollider())) ;
}

void SceneEntity::nodeProcessSceneEvt(SceneBaseEvent& sceneBaseEvt, const bool& nodeWasDirty) {

	assrt(false);
}

Volume& SceneEntity::entGetWorldVolume() {

	return mEntVolumeWorld;
}

bool SceneEntity::entIsValidWorldVolume() {

	return mEntVolumeWorld.isValid();
}

void SceneEntity::_entSignalColliderVolumeWorldChanged() {

	ICollDetector& coll = mNodeScene->mCollDetector.dref();
	coll.update(*this, mEntCollData, mEntVolumeWorld);
}

void SceneEntity::_entSignalColliderVolumeWorldChanged_CollAssisted(SceneNodeMoveTester& test) {

	ICollDetector& coll = mNodeScene->mCollDetector.dref();

	if (test.isTestStart) {

		coll.startTest(*this, mEntCollData, mEntVolumeWorld, test.collTestData);

	} else {

		coll.endTest(*this, mEntCollData, mEntVolumeWorld, test.collTestData, test.isCollTestAccepted);
	}

}


void SceneEntity::_entSetNeedsAttachCollider(bool set) {

	if (set) {
		
		mEntInitFlags |= kInitFlag_1_AttachCollider;
	} else {

		mEntInitFlags &= ~kInitFlag_1_AttachCollider;
	}
}

bool SceneEntity::entNeedsAttachCollider() {

	return ((mEntInitFlags & kInitFlag_1_AttachCollider) != 0);
}

bool SceneEntity::entIsAttachedCollider() {

	return (mEntCollEnable && (entNeedsAttachCollider() == false)) ;
}

bool SceneEntity::entNeedsInitCollider() {

	return entNeedsAttachCollider();
}

void SceneEntity::entInitCollider(const bool& nodeWasDirty, SceneEvent& sceneEvt) {

	assrt(mNodeIsDirty == false);

	entAttachCollider();
}

void SceneEntity::entCreateColliderData(const CollMask& mask) {

	ICollDetector& coll = mNodeScene->mCollDetector.dref();
	if (coll.createBinding(*this, mEntCollData, mask) == false) {

		assrt(false);
	}
}

void SceneEntity::entAttachCollider() {

	if (entNeedsAttachCollider()) {

		ICollDetector& coll = mNodeScene->mCollDetector.dref();

		coll.add(*this, mEntCollData, mEntVolumeWorld);

		mEntCollIsActive = true;
		_entSetNeedsAttachCollider(false);
	}
}

void SceneEntity::entDetachCollider() {

	if (entColliderIsActive()) {

		ICollDetector& coll = mNodeScene->mCollDetector.dref();

		coll.remove(*this, mEntCollData);
	}

	mEntCollIsActive = false;
	_entSetNeedsAttachCollider(false);
}


}