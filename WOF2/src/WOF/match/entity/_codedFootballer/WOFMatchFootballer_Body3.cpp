#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY3

#include "WE3/helper/load/WEVarTableHelper.h"
using namespace WE;

#include "../../WOFMatch.h"
#include "WOFMatchFootballer.h"
#include "../../WOFMatchTeam.h"


namespace WOF { namespace match {

Footballer_Body::Footballer_Body()	
	: mActiveCamVisible(false), mSavedTransformLocalValid(false), 
		mCollResolutionMode(CollResolve_GhostMode) {
}

Footballer_Body::~Footballer_Body() {

	if (mNodeMatch.isValid()) {

		mNodeMatch->getCollEngine().destroy(mNodeMatch, *this, mCollider);
	}
}

void Footballer_Body::cancel_init_create() {

	mSkelInst.destroy();
}

SkeletonAnimMap* Footballer_Body::_init_animMap(SkeletalMeshPropertyManager& skelMeshProps, Mesh* pMesh) {

	if (pMesh && pMesh->hasSkeleton()) {

		SkeletonAnimMap* pAnimMap;

		if (skelMeshProps.hasProperty(*pMesh) == false) {

			skelMeshProps.createProperty(*pMesh);
			pAnimMap = &(skelMeshProps.getProperty(*pMesh)->mAnimMap);
			pAnimMap->init(AnimEnum_Count);

		} else {

			pAnimMap = &(skelMeshProps.getProperty(*pMesh)->mAnimMap);
		}

		Skeleton& skel = (pMesh->getSkeleton());
		
		if (pAnimMap->mapAnim(skel, L"idle", Anim_Idle) == false) {

			log(L"Footballer Mesh has no idle Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"run", Anim_Running) == false) {

			log(L"Footballer Mesh has no run Animation");
			return NULL;
		}
		
		if (pAnimMap->mapAnim(skel, L"tackle", Anim_Tackling) == false) {

			log(L"Footballer Mesh has no tackle Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"turn180", Anim_Turning180) == false) {

			log(L"Footballer Mesh has no turn180 Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"kick_01", Anim_Shooting) == false) {

			log(L"Footballer Mesh has no kick_01 Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"fall_fwd", Anim_FallingFwd) == false) {

			log(L"Footballer Mesh has no fall_fwd Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"fall_back", Anim_FallingBack) == false) {

			log(L"Footballer Mesh has no fall_back Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"fall_left", Anim_FallingLeft) == false) {

			log(L"Footballer Mesh has no fall_left Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"fall_right", Anim_FallingRight) == false) {

			log(L"Footballer Mesh has no fall_right Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"run", Anim_StrafingFwd) == false) {

			log(L"Footballer Mesh has no run Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"strafe_left", Anim_StrafingLeft) == false) {

			log(L"Footballer Mesh has no run Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"strafe_right", Anim_StrafingRight) == false) {

			log(L"Footballer Mesh has no run Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"walk", Anim_StrafingBack) == false) {

			log(L"Footballer Mesh has no walk Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"k_jump_up", Anim_JumpingUp) == false) {

			log(L"Footballer Mesh has no k_jump_up Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"k_jump_up_left", Anim_JumpingUpLeft) == false) {

			log(L"Footballer Mesh has no k_jump_up_left Animation");
			return NULL;
		}

		if (pAnimMap->mapAnim(skel, L"k_jump_up_right", Anim_JumpingUpRight) == false) {

			log(L"Footballer Mesh has no k_jump_up_right Animation");
			return NULL;
		}


		return pAnimMap;
	} 
	
	return NULL;
}

bool Footballer_Body::_init_mesh(Mesh& mesh, SkeletalMeshPropertyManager& skelMeshProps) {

	if (mesh.hasSkeleton() == false) {

		log(L"Footballer Mesh has no Skeleton");
		return false;
	}

	mAnimMap = _init_animMap(skelMeshProps, &mesh);

	if (mAnimMap.isNull()) {

		return false;
	}

	if (mMeshInst.init(mesh, mNodeMatch->getLoadContext().render()) == false) {

		return false;
	}

	mSkelInst.bind(&mesh.getSkeleton(), 0.7f);
	mSkelInst.useWorldMatrix(&mTransformWorld);
	
	return true;
}

bool Footballer_Body::_init_collider(const WorldMaterialID& collMaterial) {

	SoftPtr<Mesh> mesh = mMeshInst.getMesh();

	if (mesh.isNull()) {

		return false;
	}
	
	CollEngine& collEngine = mNodeMatch->getCollEngine();

	collEngine.init(mNodeMatch, *this, mCollider, collMaterial);

	return true;
}

bool Footballer_Body::initSettings(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return mActionRunning.init(*this, match, tempStr, chunk, pConv);
}

bool Footballer_Body::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;

	chunk.getValue(tempStr);

	initSettings(match, tempStr, chunk, pConv);

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

	{

		SoftRef<DataChunk> actionsChunk;

		if (!match.mFootballerActionsPath.isEmpty()) {

			actionsChunk = match.getLoadContext().dataSourcePool().getChunk(match.mFootballerActionsPath.c_tstr(), true, NULL, true);
		}

		bool allLoaded = true;

		allLoaded = allLoaded && getActionIdle().load(*this, tempStr, actionsChunk, pConv);
		allLoaded = allLoaded && getActionRunning().load(*this, tempStr, actionsChunk, pConv);
		allLoaded = allLoaded && getActionTackling().load(*this, tempStr, actionsChunk, pConv);
		allLoaded = allLoaded && getActionFalling().load(*this, tempStr, actionsChunk, pConv);
		allLoaded = allLoaded && getActionJumping().load(*this, tempStr, actionsChunk, pConv);

		if (!allLoaded) {

			assrt(false);
			return false;
		}
	}

	if (mActionIdle.start(*this, 0.0f, NULL)) {
		
		mAction = &mActionIdle;

	} else {

		initSuccess = false;
	}
	
	//initSuccess = initSuccess && mActions->init(*this, dref(mActions->getActionIdle()));

	return initSuccess;
}

bool Footballer_Body::init_nodeAttached(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return true;
}

void Footballer_Body::init_prepareScene(Match& match, Team& team, Footballer& footballer) {

	mNodeLocalTransformIsDirty = false;
	mNodeWorldTransformChangedFlag = false;

	mSkelInst.signalExternalWorldMatrixChanged();

	CollEngine& collEngine = mNodeMatch->getCollEngine();

	transform(mSkelInst.dirtyGetLocalDynamicAAB(), mTransformWorld, mCollider.volume);
	collEngine.add(mNodeMatch, *this, mCollider, mCollider.volume);
}

void Footballer_Body::onFrameMoved() {

	/*
	Frustum* pFrustum = mNodeMatch->getActiveCamera().getFrustum();
	
	bool wasVis = mActiveCamVisible;

	if (pFrustum) {

		mActiveCamVisible = intersect(mCollider.mVolumeWorld, *pFrustum);
	
	} else {

		mActiveCamVisible = false;
	}
	*/

	mActiveCamVisible = false;
	mBodyCommEvents.count = 0;
}

void Footballer_Body::clean() {

	bool doUpdate = false;

	if (mNodeLocalTransformIsDirty) {

		doUpdate = true;

		assrt(pParent->mNodeLocalTransformIsDirty == false);
		transformUpdateWorld(pParent->mTransformWorld);

	} else if (mNodeWorldTransformChangedFlag) {

		doUpdate = true;
	}

	if (doUpdate || mSkelInst.needsUpdateAnim()) {

		mSkelInst.signalExternalWorldMatrixChanged();
		mSkelInst.update();

		CollEngine& collEngine = mNodeMatch->getCollEngine();

		//transform(mSkelInst.dirtyGetLocalDynamicAAB(), mTransformWorld, mCollider.spatialVolume());
		//collEngine.update(mNodeMatch, *this, mCollider);

		{
			Volume& newVol = collEngine.updateStart(mNodeMatch, *this, mCollider);
			transform(mSkelInst.dirtyGetLocalDynamicAAB(), mTransformWorld, newVol);
			collEngine.updateEnd(mNodeMatch, *this, mCollider, newVol);
		}

		mCollRegistry.update(mNodeMatch->getCollEngine(), mCollider);

		mNodeLocalTransformIsDirty = false;
		mNodeWorldTransformChangedFlag = false;
	}
}

void Footballer_Body::frameMoveTick(Footballer& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

	mAction->frameMoveTick(footballer, time.t_discreet, tickIndex, tickLength);
}

void Footballer_Body::render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass) {

	if (cleanIfNeeded) {
		
		clean();
	}

	mMeshInst.render(renderer, mSkelInst, pass);

	if (flagExtraRenders && (pass == RP_Normal)) {

		mCollRegistry.render(renderer, true, &RenderColor::kGreen, true, &RenderColor::kYellow, false);

		/*
		{
			CollEngine& collEngine = mNodeMatch->getCollEngine();

			collEngine.render(mNodeMatch, *this, mCollider, renderer, 
									mNodeMatch->getFlagRenderBPVolumes(), mNodeMatch->getFlagRenderVolumes(), 
									&RenderColor::kGreen, &RenderColor::kWhite);
		}
		*/

		if (mNodeMatch->getFlagRenderDetailVolumes()) {
			
			mSkelInst.renderBoundingBoxes(renderer, &RenderColor::kBlue, &RenderColor::kRed);
		}
		
	}
}

void Footballer_Body::orientLookAt(const Vector3& lookAt) { orientLookAt(lookAt, mNodeMatch->getCoordAxis(Scene_Up)); }
void Footballer_Body::orientDirection(const Vector3& direction) { orientDirection(direction, mNodeMatch->getCoordAxis(Scene_Up)); }


} }

#endif