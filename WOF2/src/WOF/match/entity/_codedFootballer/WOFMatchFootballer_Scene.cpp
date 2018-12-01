#include "WOFMatchFootballer_Scene.h"

#include "WE3/helper/load/WEVarTableHelper.h"
#include "WE3/math/WEFrustum.h"
#include "WE3/WEPtr.h"
using namespace WE;

#include "../../WOFMatch.h"
//#include "../../coll/WOFMatchCollEngine.h"
#include "WOFMatchFootballer.h"
#include "../../WOFMatchTeam.h"


namespace WOF { namespace match {

Footballer_Scene::Footballer_Scene() : scene_mActiveCamVisible(false), scene_mSavedTransformLocalValid(false) {
}

Footballer_Scene::~Footballer_Scene() {

	if (mNodeMatch.isValid()) {

		CollEngine& collEng2 = mNodeMatch->getCollEngine();

		mNodeMatch->getCollEngine().destroy(mNodeMatch, *this, scene_mCollider);
	}
}

void Footballer_Scene::cancel_init_create() {

	scene_mSkelInst.destroy();
}

SkeletonAnimMap* Footballer_Scene::_init_animMap(SkeletalMeshPropertyManager& skelMeshProps, Mesh* pMesh) {

	if (pMesh && pMesh->hasSkeleton()) {

		SkeletonAnimMap* pAnimMap;

		if (skelMeshProps.hasProperty(*pMesh) == false) {

			skelMeshProps.createProperty(*pMesh);
			pAnimMap = &(skelMeshProps.getProperty(*pMesh)->mAnimMap);
			pAnimMap->init(FA_ReservedSize);

		} else {

			pAnimMap = &(skelMeshProps.getProperty(*pMesh)->mAnimMap);
		}

		Skeleton& skel = (pMesh->getSkeleton());
		
		if (pAnimMap->mapAnim(skel, L"idle", FA_Idle) == false) {

			log(L"Footballer Mesh has no idle Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"run", FA_Run) == false) {

			log(L"Footballer Mesh has no run Animation");
			return NULL;
		}
		
		if (pAnimMap->mapAnim(skel, L"tackle", FA_Tackle) == false) {

			log(L"Footballer Mesh has no tackle Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"turn180", FA_Turn180) == false) {

			log(L"Footballer Mesh has no turn180 Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"kick_01", FA_Shoot) == false) {

			log(L"Footballer Mesh has no kick_01 Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"fall_back", FA_Fall) == false) {

			log(L"Footballer Mesh has no fall_back Animation");
			return NULL;
		}


		return pAnimMap;
	} 
	
	return NULL;
}

bool Footballer_Scene::_init_mesh(Mesh& mesh, SkeletalMeshPropertyManager& skelMeshProps) {

	if (mesh.hasSkeleton() == false) {

		log(L"Footballer Mesh has no Skeleton");
		return false;
	}

	scene_mAnimMap = _init_animMap(skelMeshProps, &mesh);

	if (scene_mAnimMap.isNull()) {

		return false;
	}

	if (scene_mMeshInst.init(mesh, mNodeMatch->getLoadContext().render()) == false) {

		return false;
	}

	scene_mSkelInst.bind(&mesh.getSkeleton(), 0.7f);
	scene_mSkelInst.useWorldMatrix(&mTransformWorld);
	
	return true;
}

bool Footballer_Scene::_init_collider(const WorldMaterialID& collMaterial) {

	SoftPtr<Mesh> mesh = scene_mMeshInst.getMesh();

	if (mesh.isNull()) {

		return false;
	}
	
	CollEngine& collEngine = mNodeMatch->getCollEngine();

	collEngine.init(mNodeMatch, *this, scene_mCollider, collMaterial);

	return true;
}

bool Footballer_Scene::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;

	chunk.getValue(tempStr);

	initSuccess = initSuccess &&  nodeInit(match, MNT_Footballer, match.genNodeId(), tempStr.c_tstr());

	initSuccess = initSuccess &&  chunk.getAttribute(L"source", tempStr);

	SoftRef<Mesh> srcMesh = match.getMeshes().getMesh(tempStr.c_tstr());

	initSuccess = initSuccess &&  srcMesh.isValid();

	if (initSuccess == false) {

		return false;
	}
	
	int attrCount;
	ScriptVarTable* pVarTable = match.getLoadContext().varTablePtr();

	bool coll = false;
	chunk.scanAttribute(tempStr, L"coll", coll);

	WorldMaterialID collMaterial;
	WorldMaterialManager::invalidateMaterialID(collMaterial);
	if (chunk.getAttribute(L"collMaterial", tempStr)) {

		match.getWorldMaterialManager().getMaterialID(tempStr.hash(), collMaterial);
	} 
	

	
	VarTableHelper::setUnsetParams(tempStr, pVarTable, chunk, attrCount, true);
	initSuccess = initSuccess && _init_mesh(srcMesh.dref(), match.getSkelMeshPropManager());
	if (coll) {
		initSuccess = initSuccess && _init_collider(collMaterial);
	}
	VarTableHelper::setUnsetParams(tempStr, pVarTable, chunk, attrCount, false);

	return initSuccess;
}

bool Footballer_Scene::init_nodeAttached(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return true;
}

void Footballer_Scene::init_prepareScene(Match& match, Team& team, Footballer& footballer) {

	mNodeLocalTransformIsDirty = false;
	mNodeWorldTransformChangedFlag = false;

	scene_mSkelInst.signalExternalWorldMatrixChanged();

	CollEngine& collEngine = mNodeMatch->getCollEngine();

	//transform(scene_mSkelInst.dirtyGetLocalDynamicAAB(), mTransformWorld, scene_mCollider.spatialVolume());
	//collEngine.add(mNodeMatch, *this, scene_mCollider, scene_mCollider.spatialVolume());

	transform(scene_mSkelInst.dirtyGetLocalDynamicAAB(), mTransformWorld, scene_mCollider.volume);
	collEngine.add(mNodeMatch, *this, scene_mCollider, scene_mCollider.volume);

	//scene_setAnim(FA_Idle, 1.0f);

	//setAnim(FA_Run, 2.0f);
	//move(getDirection(Scene_Forward), match.getCoordSys().convUnit(2.0f));
}

void Footballer_Scene::scene_clean() {

	bool doUpdate = false;

	if (mNodeLocalTransformIsDirty) {

		doUpdate = true;

		assrt(pParent->mNodeLocalTransformIsDirty == false);
		transformUpdateWorld(pParent->mTransformWorld);

	} else if (mNodeWorldTransformChangedFlag) {

		doUpdate = true;
	}

	if (doUpdate || scene_mSkelInst.needsUpdateAnim()) {

		scene_mSkelInst.signalExternalWorldMatrixChanged();
		scene_mSkelInst.update();

		CollEngine& collEngine = mNodeMatch->getCollEngine();

		//transform(scene_mSkelInst.dirtyGetLocalDynamicAAB(), mTransformWorld, scene_mCollider.spatialVolume());
		//collEngine.update(mNodeMatch, *this, scene_mCollider);

		{
			Volume& newVol = collEngine.updateStart(mNodeMatch, *this, scene_mCollider);
			transform(scene_mSkelInst.dirtyGetLocalDynamicAAB(), mTransformWorld, newVol);
			collEngine.updateEnd(mNodeMatch, *this, scene_mCollider, newVol);
		}

		scene_mCollRegistry.update(mNodeMatch->getCollEngine(), scene_mCollider);

		mNodeLocalTransformIsDirty = false;
		mNodeWorldTransformChangedFlag = false;
	}
}

void Footballer_Scene::updateCollisions() {

	
}


void Footballer_Scene::onFrameMoved() {

	Frustum* pFrustum = mNodeMatch->getActiveCamera().getFrustum();
	

	bool wasVis = scene_mActiveCamVisible;

	if (pFrustum) {

		/*
		scene_mActiveCamVisible = intersect(scene_mCollider.mVolumeWorld, *pFrustum);
		*/

	} else {

		scene_mActiveCamVisible = false;
	}
}

void Footballer_Scene::render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass) {

	if (cleanIfNeeded) {
		
		scene_clean();
	}

	//mSkelInst->update();
	scene_mMeshInst.render(renderer, scene_mSkelInst, pass);

	if (flagExtraRenders && (pass == RP_Normal)) {

		scene_mCollRegistry.render(renderer, true, &RenderColor::kGreen, true, &RenderColor::kYellow, false);

		{
			CollEngine& collEngine = mNodeMatch->getCollEngine();

			/*
			collEngine.render(mNodeMatch, *this, scene_mCollider, renderer, 
									mNodeMatch->getFlagRenderBPVolumes(), mNodeMatch->getFlagRenderVolumes(), 
									&RenderColor::kGreen, &RenderColor::kWhite);
									*/
		}

		if (mNodeMatch->getFlagRenderDetailVolumes()) {
			
			scene_mSkelInst.renderBoundingBoxes(renderer, &RenderColor::kBlue, &RenderColor::kRed);
		}
		
	}
}




/*
void Footballer_Scene::collBroadPhaseOnStart(Object* pObject, CollRecordPool& collPool) {

	bool alreadyFound;

   	scene_mCollRegistry.add(pObject, collPool, alreadyFound);
}

void Footballer_Scene::collBroadPhaseOnEnd(Object* pObject, CollRecordPool& collPool) {

	scene_mCollRegistry.remove(pObject, collPool);
}
*/

/*
void Footballer_Scene::nodeProcessSceneEvt(SceneBaseEvent& sceneBaseEvt, const bool& nodeWasDirty) {

	if (sceneBaseEvt.isGameEvt) {
	} else {

		SceneEvent& sceneEvt = (SceneEvent&) sceneBaseEvt;
			
		switch (sceneEvt.type) {
			case SE_Render:
				entRender(sceneEvt, nodeWasDirty);
				break;
			case SE_AddTime:
				{
					scene_mSkel->addTime(sceneEvt.dt);

					//we could 'check' if we are really moving
					//bu for now we assume, well be moving most of the time anyway
					nodeMarkDirty();
					Vector3 move;
					scene_mVelocity.mul(sceneEvt.dt, move);
					//mTransformLocal.movePosition(move);


					move.add(mTransformLocal.getPosition());
					if (mMatch->getCollEngine().tryMoveTo(*this, move) == false) {

						scene_mVelocity.zero();
					}
					
				}
				break;
			case SE_Init:
				{
					entInit(nodeWasDirty, sceneEvt);

					scene_startSession(); {

						//scene_setAnim(FA_Idle, 1.0f);

						scene_setAnim(FA_Run, 2.0f);
						scene_move(scene_direction(Scene_Forward), 200.0f);
						
					} scene_endSession();
				}
				break;
			default:
				entProcessBaseEvent(sceneEvt, nodeWasDirty);
				break;
		}

	}
}
*/

void Footballer_Scene::scene_orientLookAt(const Vector3& lookAt) {

	scene_orientLookAt(lookAt, mNodeMatch->getCoordAxis(Scene_Up));
}

void Footballer_Scene::scene_orientDirection(const Vector3& direction) {

	scene_orientDirection(direction, mNodeMatch->getCoordAxis(Scene_Up));
}

} }