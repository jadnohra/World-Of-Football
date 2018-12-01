#include "Ball.h"

#include "WE3/helper/load/WEVarTableHelper.h"
using namespace WE;

#include "controller/BallController.h"
#include "controller/BallControllerType.h"
#include "../../Match.h"

#include "simul/BallSimulation.h"

#include "../camera/SceneCamera.h"
#include "../footballer/Footballer.h"
#include "../../Player.h"


DECLARE_INSTANCE_TYPE_NAME(WOF::match::Ball, CBall);

namespace WOF { namespace match {

const TCHAR* Ball::ScriptClassName = L"CBall";


void Ball::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<Ball>(Ball::ScriptClassName).
		func(&Ball::script_test, L"test").
		func(&Ball::script_getRadius, L"getRadius").
		func(&Ball::script_isResting, L"isResting").
		func(&Ball::script_isOnPitch, L"isOnPitch").
		func(&Ball::isInPlay, L"isInPlay").
		func(&Ball::script_getPos, L"getPos").
		func(&Ball::script_getVel, L"getVel").
		func(&Ball::script_getW, L"getW").
		func(&Ball::script_calcVelocitySyncedW, L"calcVelocitySyncedW").
		func(&Ball::script_hasAfterTouchPosDiff, L"hasAfterTouchPosDiff").
		func(&Ball::script_getAfterTouchPosDiff, L"getAfterTouchPosDiff").
		func(&Ball::script_getOwnerCount, L"getOwnerCount").
		func(&Ball::script_getTeamOwnerCount, L"getTeamOwnerCount").
		func(&Ball::script_getOwner, L"getOwner");
}

Ball::Ball() : mMaxV(0.0f), mIsResting(false), mRestingID(0), mIsVisible(true), mIsOnPitch(false) {

	//mIsSimulable(false);
	mSimulableState = Simulable_None;
	mIsInPlay = false;
	mIsAlienControlled = false;
	mSimulStateTime = 0.0f;

	mTeamOwnerCounts[Team_A] = 0;
	mTeamOwnerCounts[Team_B] = 0;

	mHasAfterTouchPosDiff = false;

	WE_MMemNew(mCommandManager.ptrRef(), BallCommandManager());
}

Ball::~Ball() {

	if (mNodeMatch.isValid()) {

		mNodeMatch->getCollEngine().destroy(mNodeMatch, *this, mCollider);
	}
}

bool Ball::isSimulable() {

	//return mIsSimulable;
	return mSimulableState == Simulable_Phys;
}

float Ball::getRadius(bool meters) {

	if (meters) {

		return mNodeMatch->getCoordSys().invConvUnit(mSphereLocal.radius);
	}

	return mSphereLocal.radius;
}

bool Ball::_init_mesh(Mesh& mesh) {

	if (mMeshInst.init(mesh, mNodeMatch->getLoadContext().render()) == false) {

		return false;
	}

	return true;
}

bool Ball::_initClone_mesh(Ball& clone) {

	mMeshInst.cloneTo(clone.mMeshInst, true);

	return true;
}

bool Ball::_init_volumeLocal() {

	SoftPtr<Mesh> mesh = mMeshInst.getMesh();
	AAB vol(true);

	mesh->getGeometry().extractVolume(vol, true);

	{
		SoftRef<CoordSysConv> conv = mNodeMatch->getLoadContext().convPool().getConverterFor(mesh->getGeometry().getUnifiedCoordSys(), mNodeMatch->getBaseCoordSys(), false);

		if (conv.isValid()) {

			conv->toTargetVolume(vol);
		}
	} 

	mSphereLocal.init(vol, false);
	
	return true;
}

bool Ball::_init_collider(const WorldMaterialID& collMaterial) {

	CollEngine& collEng = mNodeMatch->getCollEngine();

	collEng.init(mNodeMatch, *this, mCollider, collMaterial);

	if (!_init_volumeLocal()) {

		collEng.destroy(mNodeMatch, *this, mCollider);
		return false;
	}	

	return true;
}

bool Ball::_initClone_collider(Ball& clone) {

	CollEngine& collEng = mNodeMatch->getCollEngine();

	return clone._init_collider(mCollider.mCollMaterialID);
}


bool Ball::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;
	chunk.getValue(tempStr);

	initSuccess = initSuccess &&  nodeInit(match, Node_Ball, match.genNodeId(), tempStr.c_tstr());

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
	initSuccess = initSuccess &&  _init_mesh(srcMesh.dref());
	if (coll) {
		initSuccess = initSuccess &&  _init_collider(collMaterial);
	}
	VarTableHelper::setUnsetParams(tempStr, pVarTable, chunk, attrCount, false);

	initSuccess = initSuccess &&  mCommandManager->init(match, tempStr, chunk, pConv);

	return initSuccess;
}

bool Ball::initClone_create(Ball& clone, const TCHAR* cloneName, ObjectType objType) {

	bool initSuccess = true;

	Match& match = mNodeMatch;

	initSuccess = initSuccess && clone.nodeInit(match, objType, match.genNodeId(), cloneName);
	initSuccess = initSuccess && _initClone_mesh(clone);
	initSuccess = initSuccess && _initClone_collider(clone);

	return initSuccess;
}

bool Ball::init_nodeAttached() {

	mStartPos = mTransformLocal.getPosition();

	mReplayPos = mStartPos;
	mReplayDir.zero();
	mReplayV.zero();
	mReplayW.zero();

	return true;
}

bool Ball::init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return init_nodeAttached();
}

bool Ball::initClone_nodeAttached(Ball& clone) {

	return clone.init_nodeAttached();
}

bool Ball::initClone_nodeAttach(Ball& clone) {

	clone.mTransformLocal = mTransformLocal;
	
	if (initClone_nodeAttached(clone)) {

		if (pParent)
			pParent->addChild(&clone);

		return true;
	}

	return false;
}

void Ball::init_prepareScene() {

	if (true) {

		mNodeLocalTransformIsDirty = false;
		mNodeWorldTransformChangedFlag = false;

		CollEngine& collEngine = mNodeMatch->getCollEngine();

		mCollider.volume.init(V_Sphere);

		transform(mSphereLocal, mTransformWorld, mCollider.volume, true);
		collEngine.add(mNodeMatch, *this, mCollider, mCollider.volume);

		/*
		mCollider.mVolumeWorld.init(V_Sphere);

		transform(mSphereLocal, mTransformWorld, mCollider.mVolumeWorld, true);
		mCollider.addToDetector(*this, mNodeMatch->getCollEngine());
		*/
	} 

	const FastUnitCoordSys& coordSys = mNodeMatch->getCoordSys();

	{
		mPhysBody.init(false);

		mPhysBody.mass = 0.425f;
		mPhysBody.massInv = 1.0f / mPhysBody.mass;

		float radius = mSphereLocal.radius;

		float inertiaCoeff = 1.0f;
		
		mPhysBody.ITensor.type = IT_Simple;

		float radius2 = powf(radius, 2.0f);
		mPhysBody.ITensor.tensorSimple.IBody = (2.0f * mPhysBody.mass * (radius2) / 3.0f) * inertiaCoeff;
		mPhysBody.ITensor.tensorSimple.IBodyInv = 1.0f / mPhysBody.ITensor.tensorSimple.IBody;

		
		mPhysEnv.prepare(mPhysBody.mass, radius, mPhysBody.ITensor.tensorSimple.IBody, 1.0f, coordSys.invConvUnitPow(1.1f, 3), mNodeMatch->mTweakAirDragEffectCoeff);
		
		mTransformWorld.getPosition(mPhysBody.pos);
		mPhysBody.orient.identity();

		mPhysIsDesynced = false;
	}

	Vector3 attachOffset;

	coordSys.setRUF(0.0f, 0.0f, 2.0f * coordSys.invConvUnit(mSphereLocal.radius) + 1.0f, attachOffset.el);

	mControllerFPS.setPosition(mTransformWorld.getPosition());
	mControllerFPS.setVibration(2.5f);
	mControllerFPS.setRotation(2.5f);
	mControllerFPS.setNodeAttachOffset(attachOffset);

	mControllerPhys.init(mNodeMatch.dref());

	mControllerFPS.attach(mNodeMatch.dref(), this);
}

bool Ball::initClone_prepareScene(Ball& clone) {

	clone.init_prepareScene();

	return true;
}

Ball* Ball::clone(const TCHAR* cloneName, ObjectType objType, bool doClonePrepareScene) {

	bool success = true;
	SoftPtr<Ball> ret;
	
	MMemNew(ret.ptrRef(), Ball());
	

	success = success && initClone_create(ret, cloneName, objType);

	if (success)
		success = success && initClone_nodeAttach(ret);

	if (!success && ret->pParent)
		ret->pParent->removeChild(ret);

	if (doClonePrepareScene)
		success = success && initClone_prepareScene(ret);
	
	if (!success)
		MMemDelete(ret.ptrRef());

	return ret.ptr();
}

void Ball::clean() {

	bool doUpdate = false;

	if (mNodeLocalTransformIsDirty) {

		doUpdate = true;

		assrt(pParent->mNodeLocalTransformIsDirty == false);
		transformUpdateWorld(pParent->mTransformWorld);

	} else if (mNodeWorldTransformChangedFlag) {

		doUpdate = true;
	}

	if (doUpdate) {

		{

			CollEngine& collEngine = mNodeMatch->getCollEngine();

			Volume& newVol = collEngine.updateStart(mNodeMatch, *this, mCollider);
			transform(mSphereLocal, mTransformWorld, newVol, true);
			collEngine.updateEnd(mNodeMatch, *this, mCollider, newVol);

			mCollRegistry.update(collEngine, mCollider);
		}

		/*
		transform(mSphereLocal, mTransformWorld, mCollider.mVolumeWorld, true);
		mCollider.updateWorldVolumeInDetector(*this, mNodeMatch->getCollEngine());
		*/

		mNodeLocalTransformIsDirty = false;
		mNodeWorldTransformChangedFlag = false;
	}
}

CtVector3* Ball::script_getPos() { 
	
	return mControllerPhys.isAttached() ? &mPhysBody.pos : &(mTransformWorld.row[3]); 
}

ScriptObject Ball::script_getOwner(unsigned int index) { 
	
	return getOwner(index)->getScriptObject(); 
}

/*
CtVector3* Ball::script_getVel() { 
	
	return mControllerPhys.isAttached() ? &mPhysBody.v : &Vector3::kZero; 
}
*/

void Ball::calcVelocitySyncedW(const CtVector3& vel, const CtVector3& crossAxis, Vector3& w) {

	Vector3 temp;

	crossAxis.cross(vel, temp);
	temp.div(mSphereLocal.radius, w);
}

void Ball::physSetResting(bool resting) { 

	if (resting != mIsResting) {

		if (resting) {

			++mRestingID;
		}

		mIsResting = resting;
	}
}

void Ball::bindController(BallController* pController) {

	mController = pController;

	physSetResting(false);
	physSetIsOnPitch(false);
	
	SimulableState prevState = mSimulableState;

	if (pController) {

		switch (pController->getControllerType()) {

			case BallController_Phys: {

				mSimulableState = Simulable_Phys;

			} break;
			
			default: {

				mSimulableState = Simulable_None;

			} break;
		}

	} else {

		mSimulableState = Simulable_None;
	}

	if (prevState != mSimulableState)
		notifySimulListeners();
}

void Ball::syncCamera() {

	SceneCamera& cam = mNodeMatch->getActiveCamera();

	if (cam.getTrackedNode() == (SceneNode*) this) {

		cam.syncToTracked();
	}
}

BallControllerFPS* Ball::getActiveControllerFPS() {

	if (mController.ptr() == &mControllerFPS) {

		return &mControllerFPS;
	}

	return NULL;
}

bool Ball::isAttachedTo(SceneNode* pNode) {

	SoftPtr<BallControllerFPS> controller = getActiveControllerFPS();

	return controller.isValid() && controller->isAttachedPositionToNode(pNode);
}

bool Ball::isAfterTouchOwner(const BallCommand* pCommand) {

	if (pCommand 
		&& pCommand->source->objectType == Node_Footballer
		&& mAfterTouchOwner.ptr() == Footballer::footballerFromObject((const Object*) pCommand->source.ptr()))
		return true;

	return false;
}

bool Ball::processWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	bool processed = false;

	switch (uMsg) {
		case WM_KEYDOWN:
		{
			switch (wParam) {
			/*
			case 'P': 
				processed = true;
				{
					if (GetAsyncKeyState(VK_SHIFT) < 0) {

						if (mController == &mControllerBoneAttach) {

							mControllerPhys.attach(mNodeMatch.dref(), this);

						} else {

							Team& team = mNodeMatch->getTeam(Team_A);

							if (team.getFootballerCount()) {

								SoftPtr<Footballer> footballer = team.getFootballerAt(0);
								SoftPtr<BoneInstance> bone = footballer->mSkelInst->findBoneInstanceByGameName(L"r_hand");

								if (bone.isValid()) {
									
									mControllerBoneAttach.attach(mNodeMatch.dref(), this);
									mControllerBoneAttach.attachTo(bone);

									Vector3 offset(0.0f);

									offset.el[Scene_Right] = -getRadius();
									offset.el[Scene_Up] = getRadius();

									mControllerBoneAttach.setAttachOffset(offset);
								}
							}
						}
					}
				}
			break;
			*/
			case 'G':
				processed = true;
				{
					mIsAlienControlled = true;

					{
						//if (mController.isNull() || mController->getControllerType() != BallController_Phys) {

							mControllerPhys.attach(mNodeMatch.dref(), this);
							mControllerPhys.setPosition(mStartPos, NULL);

							mControllerPhys.setVelocities(Vector3::kZero, Vector3::kZero, NULL);

							syncCamera();
						//}
					}
				}
				break;
			case 'H':
				if (mNodeMatch->mTweakEnableDebugKeys) {
				processed = true;
				{
					//if (mController.isNull() || mController->getControllerType() != BallController_FPS) {

						mIsAlienControlled = true;

						mControllerFPS.attachPositionToNode(&(mNodeMatch->getActiveCamera()));
						mControllerFPS.attach(mNodeMatch.dref(), this);

						syncCamera();
					//}
				}
				}
				break;
			/*
			case 'J':
				processed = true;
				{
					//if (mController.isNull() || mController->getControllerType() != BallController_Phys) {

						mControllerPhys.attach(mNodeMatch.dref(), this);

						mReplayPos = mPhysBody.pos;

						mReplayV.zero();
						mReplayW.zero();

						mControllerPhys.setVelocities(mReplayV, mReplayW, NULL);

						syncCamera();
					//}
				}
				break;
			case 'K':
				processed = true;
				{
					//if (mController.isNull() || mController->getControllerType() != BallController_Phys) {

						mControllerPhys.attach(mNodeMatch.dref(), this);

						mReplayPos = mPhysBody.pos;

						mReplayV.zero();
						mReplayW.zero();
						mReplayW.set(0.0f, degToRad(360.0f * 2.0f), 0.0f);

						mControllerPhys.setVelocities(mReplayV, mReplayW, NULL);

						syncCamera();
					//}
				}
				break;		
			*/
			case 'L':
				if (mNodeMatch->mTweakEnableDebugKeys) {
				processed = true;
				{
					//if (mController.isNull() || mController->getControllerType() != BallController_Phys) {

						mIsAlienControlled = true;

						mControllerPhys.attach(mNodeMatch.dref(), this);
						mControllerPhys.setPosition(mReplayPos, NULL);

						mControllerPhys.setVelocities(mReplayV, mReplayW, NULL);

						syncCamera();
					//}
				}
				}
				break;
			case 'N': //Numpad Enter
				if (mNodeMatch->mTweakEnableDebugKeys) {
				processed = true;
				{
					mIsAlienControlled = true;

					mControllerFPS.attachPositionToNode(&(mNodeMatch->getActiveCamera()));
					mControllerFPS.attach(mNodeMatch.dref(), this);
					mControllerFPS.startLoadFiring(mNodeMatch->getClock());
				}
				}
				break;
			}
		}
		break;

		case WM_KEYUP:
		{
			switch (wParam) {
			if (mNodeMatch->mTweakEnableDebugKeys) {
			case 'L':
				processed = true;
				mIsAlienControlled = false;
			break;	
			case 'H':
				processed = true;
				mIsAlienControlled = false;
			break;	
			case 'G':
				processed = true;
				mIsAlienControlled = false;
			break;	
			case 'N':
				processed = true;
				{
					mIsAlienControlled = false;

					if (mController.isValid() && mController->getControllerType() == BallController_FPS) {

						mControllerFPS.stopLoadFiring(mReplayV);

						float scale;
						mNodeMatch->getCoordSys().convUnit(6.0f, scale);
						mReplayV.mul(scale);

						mControllerPhys.attach(mNodeMatch.dref(), this);

						mReplayPos = mPhysBody.pos;
						mReplayW.zero();

						mControllerPhys.setVelocities(mReplayV, mReplayW, NULL);

						syncCamera();
					}
				}
				}
				break;
			}
		}
		break;
	}

	return processed;
}

void Ball::setAfterTouchOwner(BallOwner* pObject) { 
	
	if (pObject == mAfterTouchOwner)
		return;

	if (mAfterTouchOwner.isValid())
		mAfterTouchOwner->onBallLostAfterTouch(*this);

	mAfterTouchOwner = pObject; 

	if (mAfterTouchOwner.isValid())
		mAfterTouchOwner->onBallAcquiredAfterTouch(*this);

	mHasAfterTouchPosDiff = false;
	mAfterTouchVel.zero();
	mAfterTouchPosDiff.zero();
}

void Ball::giveUpAfterTouch(BallOwner* pCurrOwner) { 
	
	if (mAfterTouchOwner.ptr() == pCurrOwner) 
		setAfterTouchOwner(NULL); 

	mHasAfterTouchPosDiff = false;
	mAfterTouchVel.zero();
	mAfterTouchPosDiff.zero();
}

void Ball::postExecuteCommand(BallCommand* pCommand) {

	if (pCommand && pCommand->isValid()) {

		BallCommand& command = dref(pCommand);

		SoftPtr<BallControllerPhysImpl> controllerPhys = getActiveControllerPhys();

		if (controllerPhys.isValid()) {

			if (command.type == BallCommand_Pos) {

				controllerPhys->setV(Vector3::kZero, NULL);
			}
		}
	}
}

bool Ball::executeCommand(BallCommand* pCommand) {

	bool executed = false;
	//bool wasSimulable = mIsSimulable;
	SimulableState lastSimulableState = mSimulableState;

	if (pCommand && pCommand->isValid()) {

		BallCommand& command = dref(pCommand);

		SoftPtr<BallControllerPhysImpl> controllerPhys = getActiveControllerPhys();

		if (controllerPhys.isValid()) {

			if (controllerPhys->executeCommand(command)) {

				if (command.type == BallCommand_Vel && command.vel.enableAfterTouch && command.source->objectType == Node_Footballer)
					setAfterTouchOwner(Footballer::footballerFromObject(command.source));

				notifySimulListeners(&command);

				if (controllerPhys.isValid()) {

					if (command.type == BallCommand_Vel || command.type == BallCommand_Guard) {

						mSimulableState = Simulable_Phys;

					} else if (command.type == BallCommand_AfterTouch) {

						mSimulableState = Simulable_PhysAfterTouch;

					} else {

						mSimulableState = Simulable_None;
					}

				} else {

					mSimulableState = Simulable_None;
				}

				/*
				mIsSimulable = controllerPhys.isValid() 
								&& (
									command.type == BallCommand_Vel
									|| command.type == BallCommand_Guard
									);
				*/

				notifyCommand(command);

				executed = true;

			} else {

				command.notifyRejected();
				executed = false;
			}

		} else {

			command.notifyRejected();
			executed = false;
		}
	}

	if (!executed) {

		SoftPtr<BallControllerPhysImpl> controllerPhys = getActiveControllerPhys();

		if (controllerPhys.isValid()) {

			mSimulableState = Simulable_Phys;

		} else {

			mSimulableState = Simulable_None;
		}
	}

	if (mSimulableState != lastSimulableState) {

		notifySimulListeners();
	}

	return executed;
}

void Ball::frameMove(const Clock& time) {

	{
		SoftPtr<Footballer> footballer = mNodeMatch->getMagicRequestingBallFootballer();

		if (footballer.isValid()) {

			Vector3 pos = footballer->mTransformWorld.getPosition();
			
			footballer->mTransformWorld.getDirection(Scene_Forward).mulAndAdd(mNodeMatch->mCOCSetup.radius * 0.5f, pos);
			pos.el[Scene_Up] = getRadius();

			mControllerPhys.setPosition(pos, NULL);
			mControllerPhys.setVelocities(Vector3::kZero, Vector3::kZero, NULL);

			syncCamera();
		}

		mNodeMatch->setMagicRequestingBallFootballer(NULL);
	}


	SoftPtr<BallCommand> frameCommand;

	if (mCommandManager.isValid()) {

		mCommandManager->process(mNodeMatch, *this);
		frameCommand = mCommandManager->getValidFinalCommand();
	} 
	
	bool comExecuted = executeCommand(frameCommand); //should be called even if command is NULL/invalid to update simulable
	
	if (mController.isValid()) {

		mController->frameMove(mNodeMatch.dref(), time, mSimulStateTime);
		
		if (comExecuted)
			postExecuteCommand(frameCommand);

		for (SimulSyncListeners::Index i = 0; i < mSimulSyncListeners.count; ++i) {

			mSimulSyncListeners[i]->onBallSyncBallState(*this, mSimulStateTime);
		}

	} else {

		if (comExecuted)
			postExecuteCommand(frameCommand);
	}

	{
		bool hadAfterTouchPosDiff = mHasAfterTouchPosDiff;

		mHasAfterTouchPosDiff = !mAfterTouchPosDiff.isZero();

		if (mHasAfterTouchPosDiff && !hadAfterTouchPosDiff) {

			mAfterTouchPosDiffStartTime = time.getTime();
		}
	}

}

Ball::ImmediateCollisionResponseType Ball::onPostPhysCollision(Object& object, const unsigned int& objectPartID) {

	mImmediateCollResponseCommand.invalidate();

	/*
	if (objectIsDynamic(object)) {

		mSimulableState = Simulable_None;
		notifySimulListeners();

		mSimulableState = Simulable_Phys;
		notifySimulListeners();
	}
	*/
	for (SimulSyncListeners::Index i = 0; i < mSimulSyncListeners.count; ++i) {

		mSimulSyncListeners[i]->onBallSyncPostCollision(*this, object);
	}

	if (mOwners.count || mAfterTouchOwner.isValid()) {

		notifyPhysCollisionImpl(object, objectPartID); 

	} else {

		if (object.objectType == Node_Footballer) {

			notifyFootballerBallColision(object, objectPartID);
		}
	}

	if (mImmediateCollResponseCommand.isValid()) {

		executeCommand(&mImmediateCollResponseCommand);
	}

	return ImmCollResp_Continue;
}

void Ball::notifySimulListeners(const BallCommand* pCommand) {

	for (SimulSyncListeners::Index i = 0; i < mSimulSyncListeners.count; ++i) {

		mSimulSyncListeners[i]->onBallSyncCommand(*this, pCommand);
	}
}

void Ball::notifySimulListeners() {

	for (SimulSyncListeners::Index i = 0; i < mSimulSyncListeners.count; ++i) {

		mSimulSyncListeners[i]->onBallSyncSimulableStateChange(*this);
	}
}

void Ball::render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass) {

	if (cleanIfNeeded) {
		
		clean();
	}

	if (mIsVisible) {

		/*

		use quaternion and slerp on direction

		if (GetAsyncKeyState(VK_SHIFT) < 0) {

			if (pass == RP_Normal) {

				SceneTransform tempTransform;

				int sampleCount = 3;

				for (int i = 0 ; i < sampleCount; ++i) {
					
					for (int r = 0 ; r < 4; ++r) {

						lerp(mLastTransformWorld.row[r], mTransformWorld.row[r], 
								(float) (i + 1) / (float) (sampleCount + 1), tempTransform.row[r]);

						if (r < 3)
							tempTransform.row[r].normalize();
					}

					mMeshInst.render(renderer, tempTransform, pass);
				}
			}
		}
		*/

		mLastTransformWorld = mTransformWorld;
		mMeshInst.render(renderer, mTransformWorld, pass);

		if (pass == RP_Normal && objectType == Node_Ball) {

			if (mNodeMatch->getFlagRenderEntityStates()) {

				const FastUnitCoordSys& coordSys = mNodeMatch->getCoordSys();

				int dbgX = 380;
				int dbgY = 175;

				mMaxV = MMax(mMaxV, mPhysBody.v.mag());

				//terminal vel. of soccer ball should be 44 m/s

				String str;

				str.assignEx(L"V-[%g, %g, %g] W-[%g, %g, %g] VMax-[%g]", 
						coordSys.invConvUnit(mPhysBody.v.el[0]),
						coordSys.invConvUnit(mPhysBody.v.el[1]),
						coordSys.invConvUnit(mPhysBody.v.el[2]),
						coordSys.invConvUnit(mPhysBody.w.el[0]),
						coordSys.invConvUnit(mPhysBody.w.el[1]),
						coordSys.invConvUnit(mPhysBody.w.el[2]),
						coordSys.invConvUnit(mMaxV)
						);

				renderer.drawText(str.c_tstr(), dbgX, dbgY);
			}

			if (flagExtraRenders) {

				/*
				mCollider.render(*this, mNodeMatch->getCollEngine(), renderer, 
									mNodeMatch->getFlagRenderBPVolumes(), mNodeMatch->getFlagRenderVolumes(), 
									&RenderColor::kGreen, &RenderColor::kWhite);
									*/
			}
		}
	}
}

void Ball::updateFootballerOwnerships(Match& match, const Clock& time) {

	EntityManager& entManager = match.getEntityManager();
	CtVector3& ballPos = mTransformWorld.getPositionCt();
	const Vector3& sceneUpAxis = match.getCoordAxis(Scene_Up);
	Vector3 COCPos;

	bool isOwnable = (getControllerPhys() != NULL);

	mOwners.count = 0;
	mTeamOwnerCounts[Team_A] = 0;
	mTeamOwnerCounts[Team_B] = 0;

	SoftPtr<Footballer> firstOwner;
	bool wasSetFirstOwner = false;

	for (int i = 0; i < 2; ++i) {

		Team& team = entManager.getTeam((TeamEnum) i);
		SoftPtr<Footballer> footballer;
		FootballerIndex iter;

		while (team.iterateFootballers(footballer.ptrRef(), iter)) {

			BallInteraction& interaction = footballer->getBallInteraction();
			
			interaction.relativeDist.update(match, footballer->mTransformWorld, footballer->getRelativeCOCOffsetCt(), ballPos, getRadius());
			
			{
				bool wasOwner = interaction.ownership.type != BallOwnership_None;

				if (isOwnable && interaction.relativeDist.isInCOC(match)) {

					if (!wasSetFirstOwner) {

						wasSetFirstOwner = true;
						firstOwner = footballer;

					} else {

						if (firstOwner.isValid()) {

							BallInteraction& interaction = firstOwner->getBallInteraction();
							bool wasOwner = interaction.ownership.type != BallOwnership_None;

							if (wasOwner) {

								if (interaction.ownership.type == BallOwnership_Solo) {

									interaction.ownership.type = BallOwnership_Shared;
									footballer->onAcquiredBall(*this, false);
								}

							} else {

								interaction.ownership.ownershipStartTime = time.getTime();

								interaction.ownership.type = BallOwnership_Shared;
								footballer->onAcquiredBall(*this, false);
							}

							firstOwner.destroy();
						}

						if (wasOwner) {

							if (interaction.ownership.type == BallOwnership_Solo) {

								interaction.ownership.type = BallOwnership_Shared;
								footballer->onAcquiredBall(*this, false);
							}

						} else {

							interaction.ownership.ownershipStartTime = time.getTime();

							interaction.ownership.type = BallOwnership_Shared;
							footballer->onAcquiredBall(*this, false);
						}
					}

					mOwners.addOne(footballer);
					++mTeamOwnerCounts[footballer->mTeam];

				} else {

					if (wasOwner) {

						footballer->onLostBall(*this);
						interaction.ownership.type = BallOwnership_None;
					}
				}
			}
		}
	}

	if (firstOwner.isValid()) {

		BallInteraction& interaction = firstOwner->getBallInteraction();
		bool wasOwner = interaction.ownership.type != BallOwnership_None;

		if (wasOwner) {

			if (interaction.ownership.type == BallOwnership_Shared) {

				interaction.ownership.type = BallOwnership_Solo;
				firstOwner->onAcquiredBall(*this, true);
			}

		} else {

			interaction.ownership.ownershipStartTime = time.getTime();

			interaction.ownership.type = BallOwnership_Solo;
			firstOwner->onAcquiredBall(*this, true);

			//String::debug(L"Owner: %u\n", firstOwner->mNumber);
		}

		firstOwner.destroy();
	}
}

void Ball::notifyFootballerBallColision(Object& object, const unsigned int& objectPartID) {

	Footballer::footballerFromObject(&object)->onBallCollision(*this, object, objectPartID);
}

void Ball::notifyPhysCollisionImpl(Object& object, const unsigned int& objectPartID) {

	bool notifyAfterTouch = true;
	bool notifyFootballerObject = true;

	SoftPtr<Footballer> footballerObject;

	if (object.objectType == Node_Footballer) {

		footballerObject = Footballer::footballerFromObject(&object);
	}

	for (Owners::Index i = 0; i < mOwners.count; ++i) {

		//Listeners::Index currCount = mListeners.count;
		BallOwner* pOwner = mOwners[i];

		if (pOwner == mAfterTouchOwner)
			notifyAfterTouch = false;
		else if (pOwner == footballerObject.ptr())
			notifyFootballerObject = false;

		pOwner->onBallCollision(*this, object, objectPartID);

		/*
		//repeat if coll listeners modified...
		if (currCount != mListeners.count) {

			i = 0;
		}
		*/
	}

	if (notifyAfterTouch && mAfterTouchOwner.isValid())
		mAfterTouchOwner->onBallCollision(*this, object, objectPartID);

	if (notifyFootballerObject && footballerObject.isValid())
		footballerObject->onBallCollision(*this, object, objectPartID);
}

void Ball::notifyCommand(BallCommand& command) {

	bool notifyAfterTouch = true;

	for (Owners::Index i = 0; i < mOwners.count; ++i) {

		//Listeners::Index currCount = mListeners.count;
		BallOwner* pOwner = mOwners[i];

		if (pOwner == mAfterTouchOwner)
			notifyAfterTouch = false;

		mOwners[i]->onBallCommand(command);

		/*
		//repeat if coll listeners modified...
		if (currCount != mListeners.count) {

			i = 0;
		}
		*/
	}

	if (notifyAfterTouch && mAfterTouchOwner.isValid())
		mAfterTouchOwner->onBallCommand(command);
}

void Ball::removeListener(BallSimulSyncListener& listener) {

	SimulSyncListeners::Index i;

	if (mSimulSyncListeners.searchFor(&listener, 0, mSimulSyncListeners.count, i)) {

		mSimulSyncListeners.removeSwapWithLast(i, true);

	}
}

} }