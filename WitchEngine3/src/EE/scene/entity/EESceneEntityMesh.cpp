#include "EESceneEntityMesh.h"
#include "EESceneEntityType.h"

#include "../EESceneNode.h"
#include "../EEScene.h"

namespace EE {

SceneEntityBitFlagType SceneEntityMesh::kInitFlag_2_ReTransfNPCollider(2);

SceneEntityMesh* SceneEntityMesh::fromObject(Object* pObject) {

	return (SceneEntityMesh*) pObject;
}

SceneEntityMesh* SceneEntityMesh::fromNode(SceneNode* pNode) {

	return (SceneEntityMesh*) pNode;
}

SceneEntityMesh::SceneEntityMesh() {

	objectType = SET_Mesh;
}

SceneEntityMesh::~SceneEntityMesh() {
}

MeshInstance* SceneEntityMesh::entGetMeshInstance(bool markDirty) {

	if (markDirty) {

		nodeMarkDirty();
	}

	return mEntMeshInstance.ptr();
}


bool SceneEntityMesh::entInit(Scene& scene, Mesh& mesh, bool enableCollider, CollMask* pCollMask, bool enableNPCollider, const String* pCollMaterial, bool overrideMeshMaterials) {

	if (mEntMeshInstance.isValid()) {

		assrt(false);
		return false;
	}

	MMemNew(mEntMeshInstance.ptrRef(), MeshInstance());
	if (mEntMeshInstance->init(mesh, scene.mLoadContext->render()) == false) {

		mEntMeshInstance.destroy();
		return false;
	}

	mEntMeshInstance->useWorldMatrix(&mTransformWorld);


	if (pCollMaterial) {

		entInitCollMaterial(scene, *pCollMaterial);
	}

	entInitLocalVolume(scene);
	if (enableCollider) entEnableCollider(enableCollider, pCollMask);
	if (enableNPCollider) entCreateNPCollider(scene, overrideMeshMaterials);

	/*
	SkeletonInstance* pSkel = mEntMeshInstance->getSkeletonInstance();
	if (pSkel) pSkel->setAnimation(1);
	*/
	
	
	return true;
}

void SceneEntityMesh::entInitLocalVolume(Scene& scene) {

	Mesh& mesh = mEntMeshInstance->mMesh.dref();

	AAB vol(true);
	
	mesh.getGeometry().extractVolume(vol, true);

	if (mesh.getGeometry().hasUnifiedCoordSys()) {

		SoftRef<CoordSysConv> conv = scene.mLoadContext->convPool().getConverterFor(mesh.getGeometry().getUnifiedCoordSys(), scene.mCoordSys, false);

		if (conv.isValid()) {

			conv->toTargetVolume(vol);
		}
	} else {

		assrt(false);
	}

	mEntVolumeLocal.init(&vol);
}

NPGeometryCollider* SceneEntityMesh::entGetNPCollider() {

	return mEntNPCollider.ptr();
}

void SceneEntityMesh::entCreateNPCollider(Scene& scene, bool overrideMeshMaterials) {

	if (mEntMeshInstance.isValid()) {
		
		if (mEntNPCollider.isNull()) {

			Mesh& mesh = mEntMeshInstance->mMesh.dref();

			if (mesh.getGeometry().hasUnifiedCoordSys() == false) {

				assrt(false);
				return;
			}

			if (mesh.getGeometry().getUnifiedCoordSys().isCompatibleWith(scene.mCoordSys) == false) {

				assrt(false);
				return;
			}
			
			MMemNew(mEntNPCollider.ptrRef(), NPGeometryCollider());

			if (overrideMeshMaterials) {

				assrt(CollMaterialManager::isValidCollMaterial(mEntVolumeMaterial));

				mEntNPCollider->create(mesh, mNodeScene->mCollMatManager.ptr(), &mEntVolumeMaterial);
			} else {

				mEntNPCollider->create(mesh, mNodeScene->mCollMatManager.ptr(), NULL);
			}
			_entSetNeedsReTransfNPCollider(true);
			

		} else {

			assrt(false);
		}
	}
}

bool SceneEntityMesh::entNeedsReTransfNPCollider() {

	return (mEntInitFlags & kInitFlag_2_ReTransfNPCollider) != 0;
}

void SceneEntityMesh::_entSetNeedsReTransfNPCollider(bool set) {

	if (set) {
	
		mEntInitFlags |= kInitFlag_2_ReTransfNPCollider;
	} else {

		mEntInitFlags &= ~kInitFlag_2_ReTransfNPCollider;
	}
}

void SceneEntityMesh::entReTransformNPCollider() {

	if (mNodeIsDirty) {

		assrt(false);
		return;
	}

	if (entNeedsReTransfNPCollider()) {

		_entBlindReTransformNPCollider();
	}

}

void SceneEntityMesh::_entBlindReTransformNPCollider() {

	Mesh& mesh = mEntMeshInstance->mMesh.dref();

	mEntNPCollider->init(mesh, &mTransformWorld);
	_entSetNeedsReTransfNPCollider(false);
}

/*
void SceneEntityMesh::entAddTime(const bool& nodeWasDirty, SceneEvent& sceneEvt) {

	SkeletonInstance* pSkel = mEntMeshInstance->getSkeletonInstance();
	if (pSkel) {

		pSkel->addTime(sceneEvt.dt);
	} 
}
*/

bool SceneEntityMesh::entClean(const bool& nodeWasDirty, SceneEvent& sceneEvt) {

	if (mNodeIsDirty) {

		assrt(false);
		return false;
	}


	bool updateVolumeWorld = false;

	SkeletonInstance* pSkel = mEntMeshInstance->getSkeletonInstance();
	if (pSkel) {

		if (pSkel->update()) {

			mEntVolumeLocal.init(&(pSkel->getLocalDynamicAAB()));
			updateVolumeWorld = true;
		}
	} 

	if (nodeWasDirty || updateVolumeWorld) {

		mEntMeshInstance->setWorldMatrix(NULL, true);

		entUpdateVolumeWorld(mTransformWorld);
		entSignalColliderVolumeWorldChanged();


		if (nodeWasDirty && entNeedsReTransfNPCollider()) {

			_entBlindReTransformNPCollider();
		}
	}
	
	return true;
}

bool SceneEntityMesh::entHandleMoveTest(SceneNodeMoveTester& test) {

	bool doUpdates;
	
	if (test.isTestStart) {

		doUpdates = true;
	} else {

		doUpdates = (test.isCollTestAccepted == false);
	}

	if (doUpdates) {

		mEntMeshInstance->setWorldMatrix(NULL, true);

		entUpdateVolumeWorld(mTransformWorld);

		//do we really wanna do this?
		if (entNeedsReTransfNPCollider()) {

			_entBlindReTransformNPCollider();
		}
	}

	entSignalColliderVolumeWorldChanged(test);

	
	return true;
}

void SceneEntityMesh::entInit(const bool& nodeWasDirty, SceneEvent& sceneEvt) {

	if (mNodeIsDirty) {

		assrt(false);
		return;
	}

	if (nodeWasDirty) {
				
		mEntMeshInstance->setWorldMatrix(NULL, true);
		entUpdateVolumeWorld(mTransformWorld);
	}

	if (mEntInitFlags == 0) {

		entSignalColliderVolumeWorldChanged();	

	} else {


		bool doSignalVolChanged = true;

		if (entNeedsInitCollider()) {

			doSignalVolChanged = false;
			entInitCollider(nodeWasDirty, sceneEvt);
		} 
		
		if (nodeWasDirty) {

			if (doSignalVolChanged) {
			
				entSignalColliderVolumeWorldChanged();
			}

			if (mEntNPCollider.isValid()) {

				entReTransformNPCollider();
			}
		}
	}
	
}

void SceneEntityMesh::entRender(SceneEvent& sceneEvt, const bool& nodeWasDirty) {

	assrt(mEntInitFlags == 0);
	if (entClean(nodeWasDirty, sceneEvt) == false) {

		return;
	}

	if (mEntIsVisible) {

		Renderer& renderer = dref<Renderer>(sceneEvt.pRenderer);

		mEntMeshInstance->render(renderer);

		if (mNodeScene->mFlagExtraRenders) {

			if (mNodeScene->mFlagRenderVolumes) {

				const RenderColor* pCol = NULL;

				if (mEntIsHighlighted) {

					pCol = &RenderColor::kRed;

				} else if (mEntCollEnable) {

					if (entIsAttachedCollider()) {

						//pCol =  entColliderHasCollisions() ? &RenderColor::kRed : &RenderColor::kGreen;
						pCol = &RenderColor::kGreen;
					} else {

						pCol =  &RenderColor::kMagenta;
					}
				}

				renderer.drawVol(mEntVolumeWorld, &Matrix43::kIdentity, pCol);
			}

			if (mNodeScene->mFlagRenderDetailVolumes) {

				SkeletonInstance* pSkel = mEntMeshInstance->getSkeletonInstance();
				if (pSkel) {

					pSkel->renderBoundingBoxes(renderer, &RenderColor::kBlue);
				}
			}

			if (mNodeScene->mFlagRenderNPColliders) {

				if (mEntNPCollider.isValid()) {

					mEntNPCollider->render(renderer, &RenderColor::kYellow);
				}
			}
		}

	}

}

void SceneEntityMesh::nodeProcessSceneEvt(SceneBaseEvent& sceneBaseEvt, const bool& nodeWasDirty) {

	if (sceneBaseEvt.isGameEvt == false) {

		SceneEvent& sceneEvt = (SceneEvent&) sceneBaseEvt;

		if (mEntMeshInstance.isValid()) {

			if (sceneEvt.type == SE_Render) {

				entRender(sceneEvt, nodeWasDirty);

			} else {

				entProcessBaseEvent(sceneEvt, nodeWasDirty);
			}
		} 	
	}
}

void SceneEntityMesh::entProcessBaseEvent(SceneEvent& sceneEvt, const bool& nodeWasDirty) {

	if (mEntMeshInstance.isValid()) {

		switch (sceneEvt.type) {
			case SE_Init:
				entInit(nodeWasDirty, sceneEvt);
				break;
			case SE_MoveTest:
				entHandleMoveTest((SceneNodeMoveTester&) sceneEvt);
				break;
		}
	} 	
}

}