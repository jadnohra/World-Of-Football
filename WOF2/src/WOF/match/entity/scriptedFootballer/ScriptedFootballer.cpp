#include "../footballer/Footballer.h"

#include "ScriptedFootballer.h"

#include "../../Match.h"
#include "../../Player.h"
#include "../camera/SceneCamera.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::ScriptedFootballer, CFootballer);
DECLARE_INSTANCE_TYPE_NAME(WOF::match::FootballerSavedSpatialState, CFootballerSavedSpatialState);

namespace WOF { namespace match {

const TCHAR* ScriptedFootballer::ScriptClassName = L"CFootballer";
const TCHAR* FootballerSavedSpatialState::ScriptClassName = L"CFootballerSavedSpatialState";

void FootballerSavedSpatialState::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<FootballerSavedSpatialState>(ScriptClassName).
		func(&FootballerSavedSpatialState::init, L"init").
		func(&FootballerSavedSpatialState::getWorldTransformPtr, L"getWorldTransform");
}


void FootballerSavedSpatialState::init(ScriptedFootballer& footballer) {

	//footballer.doClean();
	
	mWorldTransform = footballer.mTransformWorld;
}

void ScriptedFootballer::declareInVM(SquirrelVM& target) {

	FootballerSavedSpatialState::declareInVM(target);

	SqPlus::SQClassDef<ScriptedFootballer>(ScriptClassName).
		MAppendSQClassDefMatchSceneScriptedCharacter_ScriptFuncs(ScriptedFootballer).
		enumInt(BallOwnership_None, L"BallOwn_None").
		enumInt(BallOwnership_Shared, L"BallOwn_Shared").
		enumInt(BallOwnership_Solo, L"BallOwn_Solo").
		enumInt(SimulEst_Wait, L"SimulEst_Wait").
		enumInt(SimulEst_True, L"SimulEst_True").
		enumInt(SimulEst_False, L"SimulEst_False").
		func(&ScriptedFootballer::script_test, L"test").
		func(&ScriptedFootballer::script_getTeamEnum, L"getTeamEnum").
		func(&ScriptedFootballer::script_getTeam, L"getTeam").
		func(&ScriptedFootballer::script_getOppTeamEnum, L"getOppTeamEnum").
		func(&ScriptedFootballer::script_getOppTeam, L"getOppTeam").
		func(&ScriptedFootballer::script_getNumber, L"getNumber").
		func(&ScriptedFootballer::script_isSelf, L"isSelf").
		func(&ScriptedFootballer::script_requestManualSwitch, L"requestManualSwitch").
		func(&ScriptedFootballer::script_isBallOwner, L"isBallOwner").
		func(&ScriptedFootballer::script_isBallSoleOwner, L"isBallSoleOwner").
		func(&ScriptedFootballer::script_isBallSharedOwner, L"isBallSharedOwner").
		func(&ScriptedFootballer::isLastBallOwner, L"isLastBallOwner").
		func(&ScriptedFootballer::setRelativeCOCOffset, L"setRelativeCOCOffset").
		func(&ScriptedFootballer::getRelativeCOCOffsetCtPtr, L"getRelativeCOCOffset").
		func(&ScriptedFootballer::script_getCOCFwdReach, L"getCOCFwdReach").
		func(&ScriptedFootballer::script_isBallAfterTouchOwner, L"isBallAfterTouchOwner").
		func(&ScriptedFootballer::script_giveUpBallAfterTouch, L"giveUpBallAfterTouch").
		func(&ScriptedFootballer::script_requestBallAfterTouch, L"requestBallAfterTouch").
		func(&ScriptedFootballer::script_isBallTransparent, L"isBallTransparent").
		func(&ScriptedFootballer::script_setBallTransparent, L"setBallTransparent").
		func(&ScriptedFootballer::script_getBallInteraction, L"getBallInteraction").
		//func(&ScriptedFootballer::script_needsShootBall, L"needsShootBall").
		func(&ScriptedFootballer::script_shouldTriggerBallDribbleAction, L"shouldTriggerBallDribbleAction").
		func(&ScriptedFootballer::script_controlBall, L"controlBall").
		func(&ScriptedFootballer::script_controlBall2, L"controlBall2").
		func(&ScriptedFootballer::script_addTriggerVolume, L"addTriggerVolume").
		func(&ScriptedFootballer::removeTriggerVolume, L"removeTriggerVolume").
		func(&ScriptedFootballer::clearTriggerVolumes, L"clearTriggerVolumes").
		func(&ScriptedFootballer::executeTriggerVolumesOnBall, L"executeTriggerVolumesOnBall").
		func(&ScriptedFootballer::script_simulEstBallWillTouchPitchInCOC, L"simulEstBallWillTouchPitchInCOC").
		func(&ScriptedFootballer::script_genBallCommandID, L"genBallCommandID").
		func(&ScriptedFootballer::script_cancelBallControl, L"cancelBallControl").
		func(&ScriptedFootballer::script_setBallControlImmediateTeleport, L"setBallControlImmediateTeleport").
		func(&ScriptedFootballer::script_setBallControlSmoothTeleport, L"setBallControlSmoothTeleport").
		func(&ScriptedFootballer::script_setBallControlImmediateVel, L"setBallControlImmediateVel").
		func(&ScriptedFootballer::script_setBallControlImmediateGuard, L"setBallControlImmediateGuard").
		func(&ScriptedFootballer::script_setBallControlGhostedShot, L"setBallControlGhostedShot").
		func(&ScriptedFootballer::script_isShootingBall, L"isShootingBall").
		func(&ScriptedFootballer::script_getFrameBallCommandCount, L"getFrameBallCommandCount").
		func(&ScriptedFootballer::script_calcBallCorrectionDist, L"calcBallCorrectionDist").
		func(&ScriptedFootballer::script_correctBallShotVelocity, L"correctBallShotVelocity").
		func(&ScriptedFootballer::script_simulNextBallSample, L"simulNextBallSample").
		func(&ScriptedFootballer::script_showBar, L"showBar").
		func(&ScriptedFootballer::script_hideBar, L"hideBar").
		func(&ScriptedFootballer::getFormationPos, L"getFormationPos").
		func(&ScriptedFootballer::getTeamForwardDir, L"getTeamFwdDir").
		func(&ScriptedFootballer::isTeamWithForwardDir, L"isTeamWithFwdDir").
		/*
		func(&ScriptedFootballer::tacticalToWorldPos, L"tacticalToWorldPos").
		func(&ScriptedFootballer::tacticalToWorldDir, L"tacticalToWorldDir").
		func(&ScriptedFootballer::worldToTacticalPos, L"worldToTacticalPos").
		func(&ScriptedFootballer::worldToTacticalDir, L"worldToTacticalDir").
		func(&ScriptedFootballer::tacticalToWorldPosEx, L"tacticalToWorldPosEx").
		func(&ScriptedFootballer::tacticalToWorldDirEx, L"tacticalToWorldDirEx").
		func(&ScriptedFootballer::worldToTacticalPosEx, L"worldToTacticalPosEx").
		func(&ScriptedFootballer::worldToTacticalDirEx, L"worldToTacticalDirEx").
		func(&ScriptedFootballer::script_isInRegion, L"isInRegion").*/
		func(&ScriptedFootballer::script_getFormationPositionCode, L"getFormationPositionCode").
		func(&ScriptedFootballer::script_getFormationPositionChunk, L"getFormationPositionChunk").
		func(&ScriptedFootballer::script_getRegion, L"getRegion").
		func(&ScriptedFootballer::script_getRegionPitch, L"getRegionPitch").
		func(&ScriptedFootballer::script_getRegionPenalty, L"getRegionPenalty").
		func(&ScriptedFootballer::script_getRegionOppPenalty, L"getRegionOppPenalty").
		func(&ScriptedFootballer::script_getSpatialStateBallDistIndex, L"getSpatialStateBallDistIndex");
}

ScriptedFootballer::ScriptedFootballer() {

	mTeamFormationCode = 0;
	mTeam = Team_Invalid;
	mNumber = FootballerNumberInvalid;
	mBallTransparent = false;
	mExecutingScriptBallSelfCollision = false;
	mIsAskingForManualSwitch = false;
	mShowBar = false;

	mBallControlMode = BallControl_None;
	mBallControlFrameCommandCount = 0;
	
	mRelativeCOCOffset.zero();
}

bool ScriptedFootballer::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType) {

	SoftRef<DataChunk> gameChunk = chunk.getChild(L"game");
	if (gameChunk.isNull()) {

		return false;
	}

	if (gameChunk->getAttribute(L"team", tempStr) == false) {

		return false;
	}

	if (tempStr.equals(L"0")) {

		mTeam = Team_A;

	} else {

		mTeam = Team_B;
	}

	assrt(sizeof(FootballerNumber) == sizeof(int));
	if (gameChunk->scanAttribute(tempStr, L"number", L"%d", &mNumber) == false) {

		return false;
	}

	if (gameChunk->getAttribute(L"teamFormationCode", tempStr)) {

		mTeamFormationCode = tempStr.hash();
	}

	if (SceneScriptedCharacter::init_create(this, match, tempStr, chunk, pConv, objType)) {

		return true;
	}

	return false;
}

bool ScriptedFootballer::loadTransform(BufferString& tempStr, Match& match, DataChunk* pChunk, CoordSysConv* pConv) {

	Team& team = match.getTeam(mTeam);
	const TeamFormation::Element* formationInfo = team.getFormationInfo(mTeamFormationCode, mTeamFormationCode == 0 ? &mNumber : NULL);

	if (formationInfo) {

		Vector3 temp;

		getFormationPos(temp);
		mTransformLocal.setPosition(temp);

		mTransformLocal.setOrientation(getTeamForwardDir(), match.getCoordAxis(Scene_Up));
	}

	return true;
}

void ScriptedFootballer::init_prepareScene() {

	SceneScriptedCharacter::init_prepareScene();

	/*
	if (mCollProcessing.isValid() && mSkelInst.isValid()) {

		const AAB& refVol = mSkelInst->dirtyGetLocalDynamicAAB();

		refVol.center(mBallControlVolume.volume.center);
		mBallControlVolume.volume.center.el[Scene_Up] = refVol.min.el[Scene_Up];

	} else {

		assrt(false);
	}
	*/
}

bool ScriptedFootballer::attachPlayer(Input::Controller* pController, Player* pPlayer, const Time& attachTime) {
	
	if (mPlayer.isValid())
		return false;

	if (activatePlayerMode(pController)) {

		mPlayer = pPlayer;
		mPlayerAttachTime = attachTime;

		return true;
	}

	return false;
}

bool ScriptedFootballer::detachPlayer() {

	if (!mPlayer.isValid())
		return false;

	mPlayer.destroy();

	return activateAIMode();
}

void ScriptedFootballer::frameMove(const Time& time, const Time& dt) {

	//String::debug(L"+footballer %u %f %f\n", timeGetTime(), time, dt);

	mBallTransparent = false;
	mIsAskingForManualSwitch = false;
	
	if (mNodeMatch->getTeam(mTeam).didSwitchFormation()) {

		ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onSwitchedTeamFormation");

		(func)(time);
	}

	SceneScriptedCharacter::frameMove(time, dt);
	ballControlFrameMove(time, dt);

	mBallControlFrameCommandCount = 0;

	//String::debug(L"-footballer %u\n", timeGetTime());
}

void ScriptedFootballer::addSweptBallContacts(
						WorldMaterialManager& matManager, 
						const WorldMaterialID& sphereMat, const float& radius, 
						const Vector3& pos, const Vector3& sweepVector, 
						SweptIntersectionArray& intersections, bool executeMaterials) {

	if (mBallTransparent)
		return;
	
	DynamicVolumeCollider& collider = mVolumeColliders->dynamicColl.dref();

	for (BoneIndex::Type i = 0; i < mSkelInst->binding.boneInstances.size; i++) {

		if (mSkelInst->isEnabledColl(i)) {

			BoneInstance& boneInst = mSkelInst->binding.boneInstances.el[i];
			
			SweptIntersectionTesterOBB tester(boneInst.worldBox, radius, pos, sweepVector);
			
			bool didIntersect;
			
			SweptIntersection* pInter = &(intersections.makeSpaceForOne());

			while (tester.next(didIntersect, pInter->contactPoint, *pInter, *pInter)) {

				if (didIntersect) {
				
					intersections.addOneReserved();

					boneInst.markedColl = true;

					pInter->recheckGeometry = (pInter->sweptFraction == 0.0f);
					pInter->pMaterial = matManager.getCombinedMaterial(sphereMat, collider.getMaterial());
					
					pInter->staticObj = this;

					assrt(boneInst.paletteIndex < mSkelInst->getSkeleton()->getBoneCount());

					pInter->objectPartID = dref(mSkelInst->getSkeleton()).getBone(boneInst.paletteIndex).gameID;
					pInter->contactHasVelocity = false;
				
					pInter = &(intersections.makeSpaceForOne());
				}
			}
		}
	}
}

void ScriptedFootballer::render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass) {

	SceneScriptedCharacter::render(renderer, cleanIfNeeded, flagExtraRenders, pass);

	if (mShowBar && pass == RP_Normal) {

		float screenX;
		float screenY;

		if (mNodeMatch->getActiveCamera().projectOnScreen(mTransformWorld.getPosition(), screenX, screenY)) {

			String& string = mNodeMatch->getUI().addUITextRequest(NULL, screenX + 0.01f, screenY - 0.027f);

			const int barTickCount = 10;
			const TCHAR* barInitText = L"----------";

			int barTickLocation = tcap((int) (0.5f + mBarValue * (float) (barTickCount - 1)), 0, barTickCount - 1);

			string.assign(barInitText);
			
			for (int i = 0; i <= barTickCount; ++i) {

				if ((float) (i + 1) / (float) barTickCount <= mBarCancelValue) {

					string.mBuffer[i] = L'x';

				} else {

					break;
				}
			}

			string.mBuffer[barTickLocation] = L'0';
		}
	}

	if (flagExtraRenders && pass == RP_Normal) {

		{
			Sphere sphere;
			float ballRadius = mNodeMatch->getBall().getRadius();

			RenderColor color = RenderColor::kYellow;
			float colorRatio = 1.0f / (float) mTriggerVolumes.count;

			for (TriggerVolumes::Index i = 0; i < mTriggerVolumes.count; ++i) {

				TriggerVolume& vol = mTriggerVolumes[i]; 

				vol.update();
				sphere.center = vol.worldPos;
				sphere.radius = sqrtf(vol.radiusSq);

				renderer.draw(sphere, NULL, &color);

				color.el[RC_Red] -= colorRatio;
			}
		}


		/*
		{
			Sphere sphere;
			float ballRadius = mNodeMatch->getBall().getRadius();

			script_getWorldPoint(mBallControlVolume.volume.center, sphere.center);
			sphere.radius = ballRadius + mBallControlVolume.volume.radius;

			renderer.draw(sphere, NULL, &RenderColor::kWhite);


			RenderColor color = RenderColor::kYellow;
			float colorRatio = 1.0f / (float) mBallControlVolumes.size;

			for (BallControlVolumes::Index i = 0; i < mBallControlVolumes.size; ++i) {

				BallControlVolume& vol = mBallControlVolumes[i]; 

				vol.coordFrame->transformPoint(vol.volume.center, sphere.center);
				//script_getWorldPoint(vol.volume.center, sphere.center);
				sphere.radius = ballRadius + vol.volume.radius;

				renderer.draw(sphere, NULL, &color);

				color.el[RC_Red] -= colorRatio;
			}
		}

		{

			Sphere sphere;
			sphere.radius = mNodeMatch->getBall().getRadius() * 0.5f;

			for (BallControlVolumeIntersections::Index i = 0; i < mBallControlVolumeIntersections.count; ++i) {

				ScriptBallControlVolumeIntersection& intersec = mBallControlVolumeIntersections[i];

				for (unsigned int j = 0; j < intersec.pointCount; ++j) { 

					sphere.center = intersec.points[j];
					renderer.draw(sphere, NULL, &RenderColor::kRed);
				}
			}
		}
		*/
	}
}

void ScriptedFootballer::script_showBar(float value, float cancelInterval) {

	mShowBar = true;
	mBarValue = value;
	mBarCancelValue = cancelInterval;
}

void ScriptedFootballer::script_hideBar() {

	mShowBar = false;
}

bool ScriptedFootballer::isBallAfterTouchOwner() {

	return mNodeMatch->getBall().isAfterTouchOwner(this);
}

void ScriptedFootballer::script_giveUpBallAfterTouch() {

	mNodeMatch->getBall().giveUpAfterTouch(this);
}

bool ScriptedFootballer::isLastBallOwner() {

	BallCommand& lastCommand = mNodeMatch->getBall().getCommandManager().getLastCommand();

	if (lastCommand.isValid() && lastCommand.source.ptr() == (Object*) this) {

		return true;
	}

	return false;
}

bool ScriptedFootballer::script_requestBallAfterTouch(unsigned int commandID, CtVector3& acceleration, float sideMag, float upMag, float downMag, float rotSyncRatio, bool transformFwdToUp) {

	Ball& ball = mNodeMatch->getBall();

	if (ball.isAfterTouchOwner(this) && !ball.isResting()) {

		if (transformFwdToUp) {

			Match& match = mNodeMatch;

			Vector3 acc;
			Vector3 temp;
			Vector3 vHorizDir;
			Vector3 compVert;
			Vector3 compHoriz;

			Ball& ball = match.getBall();
			const PhysRigidBody& physBody = ball.getPhysBody();

			physBody.v.cross(match.getCoordAxis(Scene_Up), temp);
			temp.normalize();
			match.getCoordAxis(Scene_Up).cross(temp, vHorizDir);

			if (!vHorizDir.isZero()) {
			
				vHorizDir.normalize();

				decompose(acceleration, vHorizDir, compVert, compHoriz); //acc now contains side-ways component
				
				float coeff = -compVert.dot(vHorizDir); 

				if (coeff >= 0.0f) {

					match.getCoordAxis(Scene_Up).mul(coeff * upMag, compVert);

				} else {

					match.getCoordAxis(Scene_Up).mul(coeff * downMag, compVert);
				}

				compHoriz.mul(sideMag);
				compHoriz.add(compVert, acc);

				ball.getCommandManager().addCommand().setAfterTouch(ball, commandID, dref(this), 0.0f, acc, rotSyncRatio);
			}

		} else {

			ball.getCommandManager().addCommand().setAfterTouch(ball, commandID, dref(this), 0.0f, acceleration, rotSyncRatio);
		}
		
		return true;
	}

	return false;
}


float ScriptedFootballer::script_calcBallCorrectionDist(CtVector3& refPos, CtVector3& refDir, CtVector3& pos) {

	Point segB;
	Point closestPt;
	float u;

	refPos.add(refDir, segB);

	distanceSq(refPos, segB, pos, closestPt, u);	

	Vector3 correctionVect;
	Vector3 rightDir;

	closestPt.subtract(pos, correctionVect);
	refDir.cross(mNodeMatch->getCoordAxis(Scene_Up), rightDir);

	return (correctionVect.dot(rightDir));
}

void ScriptedFootballer::script_correctBallShotVelocity(float correction, Vector3& inOutVelocity) {


}

bool ScriptedFootballer::script_needsShootBall(CtVector3& idealBallActionPosOffset, CtVector3& idealBallDir, 
											   CtVector3& footballerPosDiff, bool footballerDirChanged, float epsilon) {

	/*
		the footballer pos in local transform is the one that will be rendered this frame
		footballerPosDiff is the distance that the footballer moved this frame
		and is already integrated in local transform.

		the order of frameMove's is probably crucial to stay the same for this to work
	*/
	if (isBallSharedOwner()) {

		return true;
	}

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	if (ball.getActiveControllerPhys() == NULL)
		return true;

	const PhysRigidBody& physBody = ball.getPhysBody();

	Vector3 ballPosDiff;

	physBody.v.mul(match.getClock().getFrameMoveTime(), ballPosDiff);

	if (!footballerDirChanged) {

		Vector3 ballDir;
		float ballPosDiffMag;

		Vector3 footballerMoveDir;
		float footballerPosDiffMag;
	

		footballerPosDiff.normalize(footballerMoveDir, footballerPosDiffMag);
		ballPosDiff.normalize(ballDir, ballPosDiffMag);

		float dot1 = ballDir.dot(idealBallDir);
		float dot2 = ballDir.dot(footballerMoveDir);

		if ((fabs(1.0f - dot1) <= 0.2f)
			&& (ballPosDiffMag * fabs(dot2) >= footballerPosDiffMag)) {

			return false;
		}
	}

	const Vector3& ballPos = physBody.pos;
	
	Vector3 newBallPos;
	ballPos.add(ballPosDiff, newBallPos);

	Vector3 newIdealBallPos;
	
	mTransformLocal.getPosition().add(idealBallActionPosOffset, newIdealBallPos);

	Vector3 currIdealBallPos;
	float distCurrBallCurrFootSq;

	newIdealBallPos.subtract(footballerPosDiff, currIdealBallPos);

	float epsilonSq = epsilon * epsilon;

	if ((distCurrBallCurrFootSq = distanceSq(currIdealBallPos, ballPos)) <= epsilonSq) {

		//calcBallCorrectionDist(mTransformLocal.getPosition(), posDiff, ballPos, ballCorrectionDist.value());

		return true;
	}

	float distNewBallBallNewFootSq;

	
	distNewBallBallNewFootSq = distanceSq(newIdealBallPos, newBallPos);

	if (distNewBallBallNewFootSq >= distCurrBallCurrFootSq) {

		//calcBallCorrectionDist(mTransformLocal.getPosition(), posDiff, ballPos, ballCorrectionDist.value());

		return true;
	}

	return false;
}

void ScriptedFootballer::script_controlBall(unsigned int commandID, float controlStrength, CtVector3& targetPoint, float totalTime, float timeLeft, bool rotate) {

	script_controlBall2(commandID, controlStrength, targetPoint, totalTime, timeLeft, rotate, NULL);
}

void ScriptedFootballer::script_controlBall2(unsigned int commandID, float controlStrength, CtVector3& targetPoint, float totalTime, float timeLeft, bool rotate, Vector3* pOutNextBallPos) {

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	const PhysRigidBody& physBody = ball.getPhysBody();

	Vector3 temp;
	Vector3& outNextBallPos = pOutNextBallPos ? *pOutNextBallPos : temp;

	float factor;
	bool doRotate;

	if (totalTime != 0.0f) {
	
		factor = 1.0f - (timeLeft / totalTime);

		lerp(physBody.pos, targetPoint, factor, outNextBallPos);

		doRotate = (factor == 1.0f || factor == 0.0f) ? false : rotate;

	} else {

		factor = 1.0f;

		outNextBallPos = targetPoint;

		doRotate = rotate;
	}

	ball.getCommandManager().addCommand().setPos(ball, commandID, *this, controlStrength, outNextBallPos, doRotate, &match.getCoordAxis(Scene_Up));
}


float ScriptedFootballer::script_getCOCFwdReach() {

	return mRelativeCOCOffset.el[Scene_Forward] + mNodeMatch->mCOCSetup.radius;
}

unsigned int ScriptedFootballer::script_genBallCommandID() { 
	
	return mNodeMatch->genBallCommandID(); 
}




/*
void ScriptedFootballer::tacticalToWorldPos(CtVector2& inPos, Vector3& outPos) {

	mNodeMatch->getTeam(mTeam).tacticalToWorldPos((inPos), (outPos), 0.0f);
}

void ScriptedFootballer::tacticalToWorldDir(CtVector2& inDir, Vector3& outDir) {

	mNodeMatch->getTeam(mTeam).tacticalToWorldDir((inDir), (outDir));
}

void ScriptedFootballer::worldToTacticalPos(CtVector3& inPos, Vector2& outPos) {

	mNodeMatch->getTeam(mTeam).worldToTacticalPos((inPos), (outPos));
}

void ScriptedFootballer::worldToTacticalDir(CtVector3& inDir, Vector2& outDir) {

	mNodeMatch->getTeam(mTeam).worldToTacticalDir((inDir), (outDir));
}

void ScriptedFootballer::tacticalToWorldPosEx(CtVector2& inPos, Vector3& outPos, int refTeam) {

	mNodeMatch->getTeam(mTeam).tacticalToWorldPos((inPos), (outPos), 0.0f, refTeam != mTeam);
}

void ScriptedFootballer::tacticalToWorldDirEx(CtVector2& inDir, Vector3& outDir, int refTeam) {

	mNodeMatch->getTeam(mTeam).tacticalToWorldDir((inDir), (outDir), refTeam != mTeam);
}

void ScriptedFootballer::worldToTacticalPosEx(CtVector3& inPos, Vector2& outPos, int refTeam) {

	mNodeMatch->getTeam(mTeam).worldToTacticalPos((inPos), (outPos), refTeam != mTeam);
}

void ScriptedFootballer::worldToTacticalDirEx(CtVector3& inDir, Vector2& outDir, int refTeam) {

	mNodeMatch->getTeam(mTeam).worldToTacticalDir((inDir), (outDir), refTeam != mTeam);
}
*/

/*
bool ScriptedFootballer::isInRegion(PitchRegionShape& region, TeamEnum regionTeam) {

	Vector2 pos2D;

	mNodeMatch->toScene2D(localPos(), pos2D);
	mNodeMatch->tacticalTeamRelInPlace(pos2D, mTeam, regionTeam);

	return region.contains(pos2D);
}
*/

bool ScriptedFootballer::getFormationPos(Vector3& outPos) {

	Team& team = mNodeMatch->getTeam(mTeam);
	const TeamFormation::Element* formationInfo = team.getFormationInfo(mTeamFormationCode, mTeamFormationCode == 0 ? &mNumber : NULL);

	if (formationInfo) {

		team.formationToWorldPos(formationInfo->getPos(), outPos, getLocalPos().el[Scene_Up]);

		return true;
	}

	return false;
}

CtVector3& ScriptedFootballer::getTeamForwardDir() { 
	
	return mNodeMatch->getTeam(mTeam).getForwardDir(); 
}

bool ScriptedFootballer::isTeamWithForwardDir() { 
	
	return mNodeMatch->getTeam(mTeam).isTeamWithFwdDir(); 
}

Team* ScriptedFootballer::script_getTeam() {

	return &(mNodeMatch->getTeam(mTeam));
}

Team* ScriptedFootballer::script_getOppTeam() {

	return &(mNodeMatch->getTeam(opponentTeam(mTeam)));
}

const TCHAR* ScriptedFootballer::script_getFormationPositionCode() {

	Team& team = mNodeMatch->getTeam(mTeam);
	const TeamFormation::Element* formationInfo = team.getFormationInfo(mTeamFormationCode, mTeamFormationCode == 0 ? &mNumber : NULL);

	if (formationInfo) {

		return formationInfo->getCode().c_tstr();
	}

	return L"";
}

ScriptDataChunk ScriptedFootballer::script_getFormationPositionChunk() {

	Team& team = mNodeMatch->getTeam(mTeam);
	TeamFormation::Element* formationInfo = team.getFormationInfoRef(mTeamFormationCode, mTeamFormationCode == 0 ? &mNumber : NULL);

	if (formationInfo) {

		return ScriptDataChunk(formationInfo->getChunk());
	}

	return ScriptDataChunk();
}

PitchRegionShape* ScriptedFootballer::script_getRegion(const TCHAR* name) {

	SoftPtr<TeamFormation::Mode> mode = mNodeMatch->getTeam(mTeam).getCurrFormationMode();

	if (mode.isValid()) {

		return mode->findRegion(String::hash(name));
	}

	return NULL;
}

PitchRegionShape* ScriptedFootballer::script_getRegionPitch() {

	return &(mNodeMatch->getSpatialPitchModel().getRegionPitch());
}

PitchRegionShape* ScriptedFootballer::script_getRegionPenalty() {

	return &(mNodeMatch->getSpatialPitchModel().getRegionPenalty(mNodeMatch->getTeam(mTeam)));
}

PitchRegionShape* ScriptedFootballer::script_getRegionOppPenalty() {

	return &(mNodeMatch->getSpatialPitchModel().getRegionOppPenalty(mNodeMatch->getTeam(mTeam)));
}

/*
bool ScriptedFootballer::isValidBallIntercept(BallInterceptEstimate& intercept, float time) {

	Match& match = mNodeMatch;

	if (intercept.isSimulated) {

		if (time > intercept.simulExpiryTime)
			return false;

		FootballerBallInterceptManager& manager = match.getFootballerBallInterceptManager();

		if (!manager.isStateValid() || ((manager.getSimulID() != intercept.simulID)))
			return false;

		return true;

	} else {

		if (intercept.isResting) {

			Ball& ball = match.getBall();

			if (!ball.isResting() || (ball.getRestingID() != intercept.restingID)) 
				return false;

			return true;
		}
	}

	return false;
}

int
ScriptedFootballer::script_updateBallIntercept(float time, float maxAnalysisAge, float toleranceMeters, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos, FootballerBallIntercept& result) {

	if (result.isValid && isValidBallIntercept(result, time))
		return Est_NoChange;

	result.isValid = false;
	result.isSimulated = false;
	result.isResting = false;

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	FootballerBallInterceptManager& manager = match.getFootballerBallInterceptManager();

	if (manager.isStateValid()) {

		
		PathIntercept& intercept = mBallPathIntercept;
		bool needsUpdate = false;

		if ((intercept.simulID == manager.getSimulID())
			&& (time - intercept.analysisTime <= maxAnalysisAge)) {

			//valid, no need to request
			if (intercept.expiryTime < time) {

				needsUpdate = true;
			}

		} else {

			 needsUpdate = true;
		}

		bool requested = false;

		if (needsUpdate) {
			
			if (!manager.requestUpdate(match, (Footballer&) *this, time)) {

				requested = true;
			}
		}

		if (!requested) {

			if (intercept.type == PI_InterceptorWait) {

				result.isValid = true;

				result.position = intercept.interceptPos;
				result.isSimulated = true;
				result.simulID = intercept.simulID;
				result.simulExpiryTime = intercept.expiryTime;
				result.velocity = manager.getBaseSimul()->getSample(intercept.sampleIndex).vel;

			} else {

				if (fallbackLastSimulSample) {

					SoftPtr<BallSimulation> simul = manager.getBaseSimul();

					if (simul->getSampleCount()) {

						const BallSimulation::Sample& sample = simul->getSample(simul->getSampleCount() - 1);

						result.isValid = true;

						result.position = sample.pos;
						result.velocity = sample.vel;
						result.isSimulated = true;
						result.simulID = intercept.simulID;
						result.simulExpiryTime = sample.time;
					}
				}
			}
		}

	} else if (manager.isStateProcessing()) {

		manager.requestUpdate(match, (Footballer&) *this, time);
	}
		
	if (!result.isValid) {

		bool consider = false;

		if (fallbackRestingPos) {

			if (ball.isResting()) {

				consider = true;
			}
		} 

		if (!consider && fallbackCurrPos) {
			
			consider = true;
		}

		if (consider) {

			result.isValid = true;

			result.position = ball.mTransformWorld.getPosition();
			result.isSimulated = false;
			result.isResting = ball.isResting();
			result.restingID = ball.getRestingID();
		}
	}

	return result.isValid ? Est_Changed : Est_Invalid;
}
*/


/*
bool ScriptedFootballer::script_isBallAtRestNearTargetPoint(CtVector3& targetPoint, float epsilon) {

	Ball& ball = mNodeMatch->getBall();
	const PhysRigidBody& physBody = ball.getPhysBody();

	float distSq = distanceSq(targetPoint,  physBody.pos);

	if (epsilon * epsilon >= distSq) {

		//we could check vel/rot here, but if the ball rolls away well get it next time anyway
		return true;
	}

	return false;
}
*/

void ScriptedFootballer::onBallCollision(Ball& ball, Object& object, const unsigned int& objectPartID) {
 
	if (isBallAfterTouchOwner())
		mNodeMatch->getBall().giveUpAfterTouch(this);

	switch (object.objectType) {

		case Node_Pitch: {

			ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallPitchCollision");

			(func)(scriptArg(ball));

		} break;

		case Node_Footballer: {

			if (&object == this) {

				if (!mBallTransparent) {

					bool isOwner;
					bool isInFront = false;

					if (isBallOwner()) {

						isOwner = true;

					} else {

						isOwner = BallRelativeDist::calcIsInCOC2(mNodeMatch, mTransformWorld, mRelativeCOCOffset, dref(ball.script_getPos()), ball.getRadius(), isInFront);
					}

					if (isOwner) {

						mExecutingScriptBallSelfCollision = true;

						ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallSelfCollision");

						(func)(scriptArg(ball), objectPartID);

						mExecutingScriptBallSelfCollision = false;

					} else {

						ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallPassiveSelfCollision");

						(func)(scriptArg(ball), objectPartID, isInFront);
					}

				} else {

				}

			} else {

				cancelBallControls(true);

				ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallFootballerCollision");

				(func)(scriptArg(ball), footballerScriptedFromObject(&object)->getScriptObject(), objectPartID);
			}

		} break;

		default: {

			cancelBallControls(true);

			ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallObstacleCollision");

			(func)(scriptArg(ball), scriptArg(object));

		} break;
	}
}

void ScriptedFootballer::onBallCommand(BallCommand& command) { 

	if (this == command.source) {

		ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallCommandAccepted");

		(func)(scriptArg(command));

	} else {

		cancelBallControls(true);

		ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallCommand");

		(func)(scriptArg(command));
	}
}

void ScriptedFootballer::onBallCommandRejected(Ball& ball, const unsigned int& commandID) {

	cancelBallControls(false);

	ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallCommandRejected");

	(func)(scriptArg(ball), commandID);
}

void ScriptedFootballer::onBallAcquiredAfterTouch(Ball& ball) {

	ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallAcquiredAfterTouch");

	(func)(scriptArg(ball));
}

void ScriptedFootballer::onBallLostAfterTouch(Ball& ball) {

	ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallLostAfterTouch");

	(func)(scriptArg(ball));
}

float ScriptedFootballer::getDefaultRunSpeed() {

	ScriptFunctionCall<float> func(mNodeMatch->getScriptEngine(), mScriptObject, L"getDefaultRunSpeed");

	return (func)();
}

bool ScriptedFootballer::isSwitchLocked() {

	if (isBallAfterTouchOwner())
		return true;

	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"isSwitchLocked");

	return (func)();
}


bool ScriptedFootballer::isAskingForManualSwitch() {

	if (isBallAfterTouchOwner())
		return false;

	return mIsAskingForManualSwitch;

	/*
	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"isAskingForManualSwitch");

	return (func)();
	*/
}

void ScriptedFootballer::scriptedOnLostBall(Ball& ball) {

	ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onLostBall");

	(func)(scriptArg(ball));
}

void ScriptedFootballer::scriptedOnAcquiredBall(Ball& ball, bool isSolo) {

	ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onAcquiredBall");

	(func)(scriptArg(ball), isSolo);
}

} }