#ifdef FOOTBALLER_BODY1
#include "WOFMatchFootballer_Body1.h"

#include "../../WOFMatch.h"
#include "WOFMatchFootballer.h"
#include "../../WOFMatchTeam.h"
//#include "../../coll/WOFMatchCollEngine.h"
#include "../ball/ballController/WOFMatchBallControllerPhysImpl.h"

#include "../../WOFMatchObjectIncludes.h"


namespace WOF { namespace match {


Footballer_Body::Footballer_Body() {

	body_mCurrState = BS_None;
	body_mRelBallDist = FLT_MAX;

	body_mBallOwnership.isBallOwner = false;
	body_mBallOwnership.ballControl = 0.0f;
	mCOC.ballCollided = false;

	body_mBallActionDone = false;
	body_mBallActionDoing = false;
	body_mBallInteraction = FBI_None;
	body_mBallFlagPassiveCollided = false;

	body_mMoveBlocked = false;
	body_mIsInGhostMode = false;

	body_mVelConstrainContext.zero();
}

Footballer_Body::~Footballer_Body() {

	if (mNodeMatch.isValid()) {

		/*
		CollEngine& collEng = mNodeMatch->getCollEngine();

		collEng.destroyRegistry(body_mCollRegistry);
		*/
	}
}

const TCHAR* Footballer_Body::toString(const State& state) {

	switch (state) {
		case Footballer_Body::BS_None:
			return L"None";
		case Footballer_Body::BS_Idle:
			return L"Idle";
		case Footballer_Body::BS_Running:
			return L"Running";
		case Footballer_Body::BS_Tackling:
			return L"Tackling";
		case Footballer_Body::BS_Shooting:
			return L"Shooting";
	}

	assrt(false);
	return L"???";
}


void Footballer_Body::cancel_init_create() {
}

bool Footballer_Body::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return true;
}


bool Footballer_Body::init_nodeAttached(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return true;
}

void Footballer_Body::init_prepareScene(Match& match, Team& team, Footballer& footballer) {

	body_setIdle();
}

/*
void Footballer_Body::collBroadPhaseOnStart(Object* pObject, CollRecordPool& collPool) {

	bool alreadyFound;

   	body_mCollRegistry.add(pObject, collPool, alreadyFound);
}

void Footballer_Body::collBroadPhaseOnEnd(Object* pObject, CollRecordPool& collPool) {

	body_mCollRegistry.remove(pObject, collPool);
}
*/

/*
bool Footballer_Body::getOBBContact(const OBB& obb, Vector& normal, float& penetration) {

	switch (scene_mCollider.mVolumeWorld.type) {
		case V_OBB:
		{
			return CollContact::contactOBB(obb, scene_mCollider.mVolumeWorld.toOBB(), normal, penetration, true, Scene_Up);
		}
		break;
	}

	return false;
}
*/

bool Footballer_Body::isCameraIdle() {

	return (BS_Running != body_mCurrState);
}

void Footballer_Body::body_constrainVelocity(const Vector3& posDiff, Vector3& vel, Vector& correction) {

	/*
	OBB obb = scene_mCollider.spatialVolume().toOBB();
	MOBBCenter(obb).add(posDiff);

	scene_mCollRegistry.constrain(obb, body_mVelConstrainContext, Vector3(vel), vel, correction, Scene_Up, false, 0.0f);
	*/

	/*
	OBB& obb = scene_mCollider.mVolumeWorld.toOBB();
	CollRegistry::Records& recs = body_mCollRegistry.mRecords;

	CollEngine::helperConstrainByOBBContacts(obb, recs, &vel, &correction);
	*/
	//XXXXX not needed if above code uncommented
	//correction.zero();
}

bool Footballer_Body::body_testMove(const DiscreeteTime& time, const Vector3& testPos) {

	bool canMove = true;

	/*
	if (body_mCollRegistry.getRecordCount() != 0) {

		canMove = false;

	} else {

		nodeMarkDirty();
		mTransformLocal.setPosition(testPos);

	}
	*/

	return canMove;
}

void Footballer_Body::body_getIdealShotVel(Vector3& vel, Vector3& w) {

	Match& match = mNodeMatch.dref();

	Vector3 vertVel;
	
	switch (m_ShotType) {
	case ST_ShotLow:
	{
		scene_getDirection(Scene_Forward).mul(match.mTweakShotLowVelHoriz, vel);
		match.getCoordAxis(Scene_Up).mul(match.mTweakShotLowVelVert, vertVel);

		w = match.mTweakShotLowRot;

		vel.add(vertVel);
		w.zero();
	}
	break;
	case ST_ShotHigh:
	{
		scene_getDirection(Scene_Forward).mul(match.mTweakShotHighVelHoriz, vel);
		match.getCoordAxis(Scene_Up).mul(match.mTweakShotHighVelVert, vertVel);

		w = match.mTweakShotHighRot;

		vel.add(vertVel);
	}
	break;
	case ST_ShotPass:
	{
		scene_getDirection(Scene_Forward).mul(match.mTweakShotPassVelHoriz, vel);
		match.getCoordAxis(Scene_Up).mul(match.mTweakShotPassVelVert, vertVel);

		w = match.mTweakShotPassRot;

		vel.add(vertVel);
	}
	break;
	case ST_ShotExtra:
	{
		scene_getDirection(Scene_Forward).mul(match.mTweakShotExtraVelHoriz, vel);
		match.getCoordAxis(Scene_Up).mul(match.mTweakShotExtraVelVert, vertVel);

		w = match.mTweakShotExtraRot;

		vel.add(vertVel);
	}
	break;
	}


	//vel.add(body_mRunning_velocity);
	

	//when shooting a first collisionis happening with ground
	//change this
	//match.getCoordAxis(Scene_Up).mul(0.5f, w);
	
}

void Footballer_Body::body_doBallAction(Footballer& footballer) {

	Match& match = mNodeMatch.dref();
	Ball& ball = match.getEntityManager().getBall();
	BallControllerPhysImpl* pBallPhys = ball.getActiveControllerPhys();

	//String::debug(L"dribble\n");

	if (pBallPhys) {

		ball.startFootballerAction(footballer);

		switch (body_mCurrState) {
		case BS_Tackling:
		{
			if (body_mBallActionDone == false) {

				body_mBallActionDoing = false;
				body_mBallActionDone = true;

				Vector3 vertVel;
				Vector3 vel;
				Vector3 w;

				scene_getDirection(Scene_Forward).mul(match.mTweakTackleVelHoriz, vel);
				match.getCoordAxis(Scene_Up).mul(match.mTweakTackleVelVert, vertVel);

				//vel.add(body_mRunning_velocity);
				vel.add(vertVel);
				
				//when shooting a first collisionis happening with ground
				//change this
				//match.getCoordAxis(Scene_Up).mul(0.5f, w);
				w.zero();

				ball.footballerAction_physSetVelocities(vel, w);

				body_mBallInteraction = FBI_ActiveShot;
			}
		}
		break;
		case BS_Shooting:
		{
			
			if (body_mBallActionDone == false) {

				body_mBallActionDoing = false;
				body_mBallActionDone = true;

				Vector3 vel;
				Vector3 w;

				body_getIdealShotVel(vel, w);

				mNodeMatch->getSoundManager().playSound(MS_Kick, scene_getPosition(), vel.mag(), false);


				ball.footballerAction_physSetVelocities(vel, w);

				body_mBallInteraction = FBI_ActiveShot;
			}
		}
		break;
		case BS_Running:
		{
			Vector3 vertVel;
			Vector3 vel;

			body_mRunning_velocity.mul(match.mTweakDribbleVelHorizRatio, vel);

			match.getCoordAxis(Scene_Up).mul(match.mTweakDribbleVelVert, vertVel);

			vel.add(vertVel);

			ball.footballerAction_physSetV(vel, match.getCoordAxis(Scene_Up), 0.3f, false, 0.0f);

			body_mBallInteraction = FBI_PassiveShot;
		}
		break;
		default:
		{
			ball.footballerAction_physSetV(Vector3::kZero, match.getCoordAxis(Scene_Up), 0.0f, true, 0.3f);

			body_mBallInteraction = FBI_Controlled;
		}
		break;
		}

		ball.endFootballerAction(footballer);
	}
}

void Footballer_Body::body_getSweetSpot(Match& match, Vector3& spot) {

	scene_getDirection(Scene_Forward).mul(match.getCoordSys().convUnit(0.4f), spot);
}

void Footballer_Body::body_updateSweetSpot(Match& match) {

	body_getSweetSpot(match, body_mSweetSpot);
}

void Footballer_Body::body_initTackling(const Vector3& startVel) {

	Match& match = mNodeMatch.dref();

	if (startVel.isZero() || (match.mTweakTackleSlideTime == 0.0f) || (match.mTweakTackleSpeedCoeff == 0.0f)) {

		body_mTackling_veolcity.zero();
		body_mTackling_acceleration = 0.0f;

	} else {

		startVel.mul(match.mTweakTackleSpeedCoeff, body_mTackling_veolcity);
		body_mTackling_acceleration = body_mTackling_veolcity.mag() / match.mTweakTackleSlideTime;
	}
}

const Vector3& Footballer_Body::body_updateTacklingVelocity(const float& dt) {

	body_mTackling_veolcity.substractNoReverse(body_mTackling_acceleration * dt);

	return body_mTackling_veolcity;
}

const Vector3& Footballer_Body::body_getTacklingVelocity() {

	return body_mTackling_veolcity;
}

void Footballer_Body::processTacklingCollisions() {

	if (body_mCurrState == BS_Tackling) {

		//we need to signal this only in conservative mode
		if (mNodeMatch->mTweakTackleConservative) {

			CollRegistry::Index recordCount = scene_mCollRegistry.getDynVolRecordCount();

			for (CollRegistry::Index i = 0; i < recordCount; ++i) {

				SoftPtr<CollRecordVol> record =  scene_mCollRegistry.getDynVolRecord(i);

				if (record->getObjectType() == MNT_Footballer) {

					Footballer& target = dref(Footballer::footballerFromObject(record->getOwner()));
					InterFootballerEvent& evt = target.body_addInterFootballerEvent();
					
					evt.source = this;
					evt.type = IFET_Tackled;
				}
			}
		}

	} else {

		if (body_mCurrState != BS_Falling) {

			CollRegistry::Index recordCount = scene_mCollRegistry.getDynVolRecordCount();

			for (CollRegistry::Index i = 0; i < recordCount; ++i) {

				SoftPtr<CollRecordVol> record =  scene_mCollRegistry.getDynVolRecord(i);

				if (record->getObjectType() == MNT_Footballer) {

					Footballer& collider = dref(Footballer::footballerFromObject(record->getOwner()));

					//here we might additionally wait until feet/leg bones touch
					//works only in ghost mode probably

					if (collider.body_getCurrState() == BS_Tackling) {

						fallToTackle(&collider);
					}
				}
			}
		}
	}
}


void Footballer_Body::fallToTackle(Footballer_Body* pSource) {

	body_setFalling();

	if (pSource) {

		InterFootballerEvent& evt = pSource->body_addInterFootballerEvent();

		evt.source = this;
		evt.type = IEFT_FallingToTackle;
	}
}

CollRegistry::Index Footballer_Body::getCollRecordCount(CollRegistry::Index* pFootballerRecordCount) {

	if (pFootballerRecordCount) {

		(*pFootballerRecordCount) = 0;
		CollRegistry::Index dynRecordCount = scene_mCollRegistry.getDynVolRecordCount();

		for (CollRegistry::Index i = 0; i < dynRecordCount; ++i) {

			SoftPtr<CollRecordVol> record =  scene_mCollRegistry.getDynVolRecord(i);

			if (record->getObjectType() == MNT_Footballer) {

				++(*pFootballerRecordCount);
			}
		}
	}

	return scene_mCollRegistry.getTotalRecordCount();;
}

void Footballer_Body::frameMoveTick(Footballer& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

	for (EventIndex i = 0; i < body_mInterFootballerEvents.count; ++i) {

		InterFootballerEvent& evt = body_mInterFootballerEvents[i];

		switch (evt.type) {

			case IFET_Tackled: {

				fallToTackle(evt.source);

			} break;

			case IEFT_FallingToTackle: {

				body_mTackling_veolcity.zero();

			} break;
		}
	}

	bool resolveCollisions = true;
	bool disableLocalTransformSaving = false;
	bool disableLocalTransformSavingIfInGhostMode = true;
	bool penetrateFootballers = false;
	Vector3 framePosDiff(0.0f);

	//updateCollisions();
	scene_addAnimTime(tickLength);
		
	switch (body_mCurrState) {
		
		case BS_Falling: {

			if (scene_animReachedEnd()) {

				body_mCurrState = BS_None;
				
				body_setIdle();

				Event& evt = body_addEvent();
				evt.type = ET_FallEnded;
			}

		} break;

		case BS_Tackling: {

			if (!mNodeMatch->mTweakTackleConservative)
				penetrateFootballers = true;

			body_updateTacklingVelocity(tickLength).mul(tickLength, framePosDiff);

			nodeMarkDirty();
			mTransformLocal.movePosition(framePosDiff);

			if (scene_animReachedEnd()) {

				body_mCurrState = BS_None;
				
				body_setIdle();

				Event& evt = body_addEvent();
				evt.type = ET_TackleEnded;
			}

		} break;

		case BS_Running:
			{
				nodeMarkDirty();

				Vector3 testPos;

				Vector3 vel = body_mRunning_velocity;
				Vector3 correction;

				//vel.mul(tickLength, testPos);
				//body_constrainVelocity(testPos, vel, correction);

				vel.mul(tickLength, framePosDiff);

				vel.mul(tickLength, testPos);
				testPos.add(mTransformLocal.getPosition());
				//testPos.add(correction);

				nodeMarkDirty();
				mTransformLocal.setPosition(testPos);

				/*
				if (body_testMove(time, testPos)) {

				} else {

					body_mLastFeedback = BF_AutoAction;
					body_mAutoAction = BF_Collided;
				}
				*/

			}
			break;
		case BS_Shooting:
			if (scene_animReachedEnd()) {

				body_mCurrState = BS_None;
				
				body_setIdle();

				Event& evt = body_addEvent();
				evt.type = ET_ShootEnded;
			}
			break;
	}

	body_updateSweetSpot(mNodeMatch.dref());

	body_mBallInteraction = FBI_None;

	if (false /*testNew*/) {



		switch(body_mCurrState) {

			case BS_Idle: {

				actBall_Idle(time, tickLength);

			} break;

		}

	} else {

		if (body_mBallOwnership.ballControl == 1.0f) {
			switch (body_mCurrState) {
				case BS_Running:
				{
					float spotDistSq = distanceSq(body_mSweetSpot, body_mRelBallPos);

					if (mCOC.ballCollided) {

						body_doBallAction(footballer);

					} else {

						if (mCOC.lastBallToSpotDistSq != -1.0f) {
						
							if (spotDistSq >= mCOC.lastBallToSpotDistSq) {

								if (mCOC.needChangeDirToActivateDribble == false) {
									
									mNodeMatch->getSoundManager().playSound(MS_Dribble, scene_getPosition(), body_mRunning_velocity.mag(), true);

									body_doBallAction(footballer);
								}
							}
						}
					}

					//static int ct = 0;
					//String::debug(L"%d %f - %f - %s\n", ct++, mCOC.lastBallToSpotDistSq, spotDistSq, mCOC.ballCollided ? L"coll" : L"");

					mCOC.lastBallToSpotDistSq = spotDistSq;
				}
				break;
				case BS_Shooting:
				{
					if (body_mBallActionDone == false) {

						body_doBallAction(footballer);

						body_mBallActionDone = true;
					}
				}
				break;
				case BS_Tackling:
				{
					if (body_mBallActionDone == false) {

						body_doBallAction(footballer);

						body_mBallActionDone = true;
					}
				}
				break;
				case BS_Idle:
					if (mCOC.idleBallHandled == false) {

						mCOC.idleBallHandled = true;

						body_doBallAction(footballer);
					}
				break;
			}
		} else if (body_mBallOwnership.ballControl > 0.0f) {

			switch (body_mCurrState) {
				case BS_Tackling:
				{
					if (body_mBallActionDone == false) {

						body_doBallAction(footballer);

						body_mBallActionDone = true;
					}
				}
				break;
			}
		}

		mCOC.ballCollided = false;
	}

	if (body_mBallFlagPassiveCollided) {

		mNodeMatch->getEventDispatcher().onBallFootballerPassiveCollision(mNodeMatch->getEntityManager().getBall(), &footballer);
		body_mBallFlagPassiveCollided = false;

		if (body_mBallInteraction == FBI_None)
			body_mBallInteraction = FBI_UncontrolledCollision;
	}

	if (resolveCollisions) {

		scene_clean();

		CollRegistry::Index footballerRecordCount;
		CollRegistry::Index recordCount = getCollRecordCount(penetrateFootballers ? &footballerRecordCount : NULL);
		
		if (recordCount) {

			processTacklingCollisions();

			bool trackBack = true;

			if (penetrateFootballers && (footballerRecordCount == recordCount))
				trackBack = false;
			
			if (trackBack) {

				if (scene_loadLocalTransform()) {

					nodeMarkDirty();

					if (!body_mMoveBlocked) {

						body_mMoveBlocked = true;

						Event& evt = body_addEvent();
						evt.type = ET_MoveBlockStarted;
					}
					
					scene_clean();

					recordCount = scene_mCollRegistry.getTotalRecordCount();
				} 

				//could not get resolve collisions
				if (recordCount) {

					//body_mIsInGhostMode = true;
				}

			} else {

				body_mIsInGhostMode = true;
			}

		} else {

			if (disableLocalTransformSaving
				|| (disableLocalTransformSavingIfInGhostMode && body_mIsInGhostMode)) {

				scene_invalidateLocalTransform();

			} else {

				body_mIsInGhostMode = false;
				scene_saveLocalTransform();
							
				if (body_mMoveBlocked) {

					body_mMoveBlocked = false;

					Event& evt = body_addEvent();
					evt.type = ET_MoveBlockEnded;
				}
			} 
		}
	}

	body_mInterFootballerEvents.count = 0;
}


bool Footballer_Body::body_trySwitchTo(const State& state) {


	switch (body_mCurrState) {
		
		case BS_Tackling:
		case BS_Falling: {

			return false;

		} break;
			
	};
	
	body_mCurrState = state;
	return true;
}

void Footballer_Body::body_toVelocity_Running(const FastUnitCoordSys& coordSys, const RunType& type, float& vel) {

	switch (type) {
		case BR_Jog:
			vel = coordSys.convUnit(3.7f);
			break;
		case BR_Normal:
			vel = coordSys.convUnit(6.5f);
			break;
		case BR_Sprint:
			vel = coordSys.convUnit(8.5f);
			break;	
		default:
			vel = 0.0f;
			assrt(false);
			break;
	}
}

float Footballer_Body::body_toVelocity_Running(const RunType& type) {

	float vel;

	body_toVelocity_Running(mNodeMatch->getCoordSys(), type, vel);

	return vel;
}

void Footballer_Body::body_setIdle() {

	if (BS_Idle != body_mCurrState) {

		if (body_trySwitchTo(BS_Idle)) {

			//apply
			float temp;
			body_blindApply_setIdle(mNodeMatch->getCoordSys(), temp);
		}
	}
}


void Footballer_Body::body_setRunning() {

	if (BS_Running != body_mCurrState) {

		if (body_trySwitchTo(BS_Running)) {

			body_mBallActionDoing = false;

			//apply
			const FastUnitCoordSys& coordSys = mNodeMatch->getCoordSys();
			float runVel;
			float temp;

			body_toVelocity_Running(coordSys, body_mRunning_type, runVel);
			body_blindApply_setRunning_velocity(body_mRunning_dir, runVel);
			body_blindApply_setRunning_anim(coordSys, temp, runVel);
		}
	}

}

void Footballer_Body::body_setRunning_dir(const Vector3& runDir, bool forceUpdate) {

	if (BS_Running != body_mCurrState) {

		body_mRunning_dir = runDir;

	} else {

		float dot = body_mRunning_dir.dot(runDir);

		if (forceUpdate || fabs(dot - 1.0f) > 0.0001f) {

			body_mRunning_dir = runDir;

			//apply
			const FastUnitCoordSys& coordSys = mNodeMatch->getCoordSys();
			float runVel;
			
			body_toVelocity_Running(coordSys, body_mRunning_type, runVel);
			body_blindApply_setRunning_velocity(body_mRunning_dir, runVel);
		}
	}

}

void Footballer_Body::body_setRunning_type(const RunType& type) {

	if (BS_Running != body_mCurrState) {

		body_mRunning_type = type;

	} else {

		if (body_mRunning_type != type) {

			body_mRunning_type = type;
			
			//apply
			const FastUnitCoordSys& coordSys = mNodeMatch->getCoordSys();
			float runVel;
			float temp;
			
			body_toVelocity_Running(coordSys, body_mRunning_type, runVel);
			body_blindApply_setRunning_velocity(body_mRunning_dir, runVel);
			body_blindApply_setRunning_anim(coordSys, temp, runVel);
		}
	}

}

void Footballer_Body::body_setFalling() {

	Match& match = mNodeMatch.dref();
	float shotMag;

	if (BS_Falling != body_mCurrState) {

		if (body_trySwitchTo(BS_Falling)) {

			//apply
			float temp;

			shotMag = match.getCoordSys().convUnit(14.0f);
			mNodeMatch->getSoundManager().playSound(MS_Tackle, scene_getPosition(), shotMag, true);
			
			body_blindApply_setFalling(match.getCoordSys(), temp);

			Event& evt = body_addEvent();
			evt.type = ET_FallStarted;
		}
	}
}

void Footballer_Body::body_setTackling(ShotType shotType) {

	Match& match = mNodeMatch.dref();
	float shotMag;

	/*
	if (body_mBallOwnership.ballControl == 0.0f) {

		return;
	}
	*/

	if (body_mBallOwnership.ballControl != 1.0f) {

		if (BS_Tackling != body_mCurrState) {

			bool wasRunning = (BS_Running == body_mCurrState);

			if (body_trySwitchTo(BS_Tackling)) {

				//apply
				float temp;

				shotMag = match.getCoordSys().convUnit(20.0f);
				mNodeMatch->getSoundManager().playSound(MS_Tackle, scene_getPosition(), shotMag, true);
				
				body_blindApply_setTackling(match.getCoordSys(), temp, wasRunning);
			}
		}

	} else {

		if (BS_Shooting != body_mCurrState) {

			if (body_trySwitchTo(BS_Shooting)) {

				m_ShotType = shotType;

				//apply
				float temp;
				
				body_blindApply_setShooting(match.getCoordSys(), temp);
			}
		}
	}
}


void Footballer_Body::setBallPos(const Vector3& pos, const Time& t) {

	Match& match = mNodeMatch.dref();

	pos.subtract(scene_getPosition(), body_mRelBallPos);
	body_mRelBallAngle = angle(scene_getDirection(Scene_Forward), body_mRelBallPos, match.getCoordAxis(Scene_Up), true);
	body_mRelBallPos.mag(body_mRelBallDist);

	if (body_mRelBallDist <= match.mTweakCOCRadius && fabs(body_mRelBallAngle) < kPiOver2) {

		if (body_mBallOwnership.isBallOwner == false) {

			body_mBallOwnership.isBallOwner = true;
			body_mBallOwnership.ballControl = 0.0f;
			body_mBallOwnership.startOwnershipTime = t;

			mCOC.lastBallToSpotDistSq = -1.0f;
			mCOC.needChangeDirToActivateDribble = false;
		}

	} else {

		if (body_mBallOwnership.isBallOwner == true) {

			body_mBallOwnership.isBallOwner = false;
			body_mBallOwnership.ballControl = 0.0f;
			mCOC.needChangeDirToActivateDribble = false;
		}
	}
}


} }

#endif