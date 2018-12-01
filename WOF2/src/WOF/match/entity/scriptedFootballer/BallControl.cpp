#include "ScriptedFootballer.h"

#include "../../Match.h"

namespace WOF { namespace match {


bool ScriptedFootballer::script_isShootingBall() {

	return mBallControlMode == BallControl_GhostedShot;
}

void ScriptedFootballer::cancelBallControls(bool notify) {

	if (mBallControlMode != BallControl_None) {

		//String::debug(L"cancelled");

		mBallControlMode = BallControl_None;

		if (notify)
			onBallCommandRejected(mNodeMatch->getBall(), mBallControlCommandID);	
	}
}

void ScriptedFootballer::ballControlFrameMove(const Time& time, const Time& dt) {

	switch (mBallControlMode) {

		case BallControl_SmoothTeleport: ballControlFrameMoveSmoothTeleport(time, dt); break;
		case BallControl_GhostedShot: ballControlFrameMoveGhostedShot(time, dt); break;
	}
}

void ScriptedFootballer::script_cancelBallControl() {

	mBallControlMode = BallControl_None;

	script_setBallTransparent(false);
}

unsigned int ScriptedFootballer::script_getFrameBallCommandCount() {

	return mBallControlFrameCommandCount + (mBallControlMode == BallControl_None) ? 0 : 1;
}

void ScriptedFootballer::script_setBallControlImmediateGuard(unsigned int commandID, float controlStrength) {

	++mBallControlFrameCommandCount;
	mBallControlMode = BallControl_None;

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	BallCommand* pCommand;

	if (mExecutingScriptBallSelfCollision) {

		pCommand = &ball.getImmediateCollResponseCommandRef();

	} else {

		pCommand = &ball.getCommandManager().addCommand();
	}

	pCommand->setGuard(ball, commandID, *this, controlStrength);
}

void ScriptedFootballer::script_setBallControlImmediateVel(unsigned int commandID, float controlStrength, bool ghostBall, CtVector3& vel, CtVector3& w, float switchControlInfluence, bool enableAfterTouch) {

	++mBallControlFrameCommandCount;
	mBallControlMode = BallControl_None;

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	script_setBallTransparent(ghostBall);

	BallCommand* pCommand;

	if (mExecutingScriptBallSelfCollision) {

		pCommand = &ball.getImmediateCollResponseCommandRef();

	} else {

		pCommand = &ball.getCommandManager().addCommand();
	}

	pCommand->setVel(ball, commandID, *this, controlStrength, vel, w, switchControlInfluence, enableAfterTouch);
}


void ScriptedFootballer::script_setBallControlGhostedShot(unsigned int commandID, float controlStrength, 
														  CtVector3& vel, CtVector3& w,
														  float switchControlInfluence, bool enableAfterTouch) {

	mBallControlMode = BallControl_GhostedShot;

	mBallControlCommandID = commandID;

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	script_setBallTransparent(true);

	BallCommand* pCommand;

	if (mExecutingScriptBallSelfCollision) {

		pCommand = &ball.getImmediateCollResponseCommandRef();

	} else {

		pCommand = &ball.getCommandManager().addCommand();
	}

	pCommand->setVel(ball, commandID, *this, controlStrength, vel, w, switchControlInfluence, enableAfterTouch);
}


void ScriptedFootballer::ballControlFrameMoveGhostedShot(const Time& time, const Time& dt) {

	if (isBallOwner()) {

		script_setBallTransparent(true);

	} else {

		mBallControlMode = BallControl_None;
	}
}

void ScriptedFootballer::script_setBallControlImmediateTeleport(unsigned int commandID, float controlStrength, bool ghostBall, CtVector3& targetPos, bool isLocalTargetPos, bool syncRotation) {

	++mBallControlFrameCommandCount;
	mBallControlMode = BallControl_None;

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	script_setBallTransparent(ghostBall);

	Vector3 targetPosHolder;
	CtVector3* pTargetPos;

	if (isLocalTargetPos) {

		mTransformLocal.transformPoint(targetPos, targetPosHolder);
		pTargetPos = &targetPosHolder;

	} else {

		pTargetPos = &targetPos;
	}

	BallCommand* pCommand;

	if (mExecutingScriptBallSelfCollision) {

		pCommand = &ball.getImmediateCollResponseCommandRef();

	} else {

		pCommand = &ball.getCommandManager().addCommand();
	}

	pCommand->setPos(ball, commandID, *this, controlStrength, dref(pTargetPos), syncRotation, &match.getCoordAxis(Scene_Up));
}

void ScriptedFootballer::script_setBallControlSmoothTeleport(unsigned int commandID, float controlStrength, bool ghostBall, float speed, float maxDuration, CtVector3& targetPos, bool isLocalTargetPos, bool syncRotation) {

	mBallControlMode = BallControl_SmoothTeleport;

	if (isLocalTargetPos) {

		mTransformLocal.transformPoint(targetPos, mBallControlTargetPos);

	} else {

		mBallControlTargetPos = targetPos;
	}

	Vector3 dist;

	mBallControlTargetPos.subtract(mNodeMatch->getBall().getPos(), dist);
	float duration = dist.mag() / speed;
	duration = MMin(duration, maxDuration);

	mBallControlCommandID = commandID;
	mBallControlDuration = duration;
	mBallControlTimeLeft = duration;
	mBallControlSyncRotation = syncRotation;
	mBallControlGhostBall = ghostBall;
	mBallControlSmoothTeleportStabilizeEnd = true;
	mBallControlSmoothTeleportIsStabilizingEnd = false;
	mBallControlStrength = controlStrength;

	//ballControlFrameMove(mNodeMatch->getClock().getTime(), 0.0f);
}

void ScriptedFootballer::ballControlFrameMoveSmoothTeleport(const Time& time, const Time& dt) {

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	const PhysRigidBody& physBody = ball.getPhysBody();

	BallCommand* pCommand;

	if (mExecutingScriptBallSelfCollision) {

		pCommand = &ball.getImmediateCollResponseCommandRef();

	} else {

		pCommand = &ball.getCommandManager().addCommand();
	}

	float factor;

	mBallControlTimeLeft -= dt;

	if (mBallControlTimeLeft <= 0.0f) {

		mBallControlTimeLeft = 0.0f;

		if (mBallControlSmoothTeleportIsStabilizingEnd) {

			mBallControlMode = BallControl_None;

			pCommand->setVel(ball, mBallControlCommandID, *this, mBallControlStrength, Vector3::kZero, Vector3::kZero, 0.0f, false);

			return;
		}

		if (mBallControlSmoothTeleportStabilizeEnd) {

			mBallControlSmoothTeleportIsStabilizingEnd = true;

		} else {

			mBallControlMode = BallControl_None;
		}

		factor = 1.0f;

	} else {

		factor = 1.0f - (mBallControlTimeLeft / mBallControlDuration);
	}

	Vector3 targetPoint;

	lerp(physBody.pos, mBallControlTargetPos, factor, targetPoint);

	bool doRotate = (factor == 1.0f || factor == 0.0f) ? false : mBallControlSyncRotation;

	script_setBallTransparent(mBallControlGhostBall);

	pCommand->setPos(ball, mBallControlCommandID, *this, mBallControlStrength, targetPoint, doRotate, &match.getCoordAxis(Scene_Up));
}

bool ScriptedFootballer::script_simulNextBallSample(Ball& ball, BallSimulation::TrajSample& sample) {

	BallSimulation& simul = mNodeMatch->getFootballerBallInterceptManager().getSimul();
	
	return simul.estimateNextBallState(ball, sample);
}

int ScriptedFootballer::script_simulEstBallWillTouchPitchInCOC() {

	SoftPtr<BallSimulation> simul = mNodeMatch->getFootballerBallInterceptManager().getValidSimul();
	
	if (!simul.isValid())
		return SimulEst_Wait;
	
	const Time& time = mNodeMatch->getClock().getTime();
	float ballRadius = mNodeMatch->getBall().getRadius();
	
	for (BallSimulation::Index i = simul->getSyncFlowCollSampleIndex(); i < simul->getCollSampleCount(); ++i) {

		const BallSimulation::CollSample& sample = simul->getCollSample(i);

		if (sample.time >= time) {

			if (sample.collider.isValid() && sample.collider->objectType == Node_Pitch) {

				if (!BallRelativeDist::calcIsInCOC(mNodeMatch, mTransformLocal, mRelativeCOCOffset, (Vector3&) sample.pos, ballRadius))
					return SimulEst_False;
				
				return SimulEst_True;
			}
		}
	}
	
	return SimulEst_False;
	
	/*
		
	if ((simul->getSampleCount() > 0) && simul->getEndReason() == BallSimulation::ER_Resting) {

		const BallSimulation::BallState& endState = simul->getEndState();

		if (endState.time >= time) {

			if (BallRelativeDist::isInCOC(mNodeMatch, endState.pos, ballRadius, dref((Footballer*) this))) {

				if (endState.pos.el[Scene_Up] <= ballRadius + heightTolerance)
					return SimulEst_True;
			}
		}
	}

	for (BallSimulation::Index i = simul->getTimeFlowIndex(); i < simul->getSampleCount(); ++i) {

		const BallSimulation::Sample& sample = dref(simul->getSample(i));

		if (sample.time >= time) {

			if (!BallRelativeDist::isInCOC(mNodeMatch, sample.pos, ballRadius, dref((Footballer*) this))) {

				return SimulEst_False;
			}

			if (sample.pos.el[Scene_Up] <= ballRadius + heightTolerance)
				return SimulEst_True;
		}
	}

	return SimulEst_False;
	*/
}

void ScriptedFootballer::TriggerVolume::init(const unsigned int& _ID, SceneTransform* pTransform, CtVector3& _localPos, const float& _radius) {

	ID = _ID;
	localPos = _localPos;
	radiusSq = _radius * _radius;

	coordFrame = pTransform;
	update();
}


bool ScriptedFootballer::addTriggerVolume(const unsigned int& ID, const TCHAR* boneName, CtVector3& position, const float& radius) {

	SoftPtr<BoneInstance> boneInst = mSkelInst->findBoneInstanceByGameName(boneName);

	if (!boneInst.isValid()) {

		assrt(false);
		return false;
	}

	TriggerVolume& newVol = mTriggerVolumes.addOne();
	
	newVol.init(ID, &(boneInst->worldTransform), position, radius);

	return true;
}

void ScriptedFootballer::clearTriggerVolumes() {

	mTriggerVolumes.count = 0;
}

void ScriptedFootballer::removeTriggerVolume(unsigned int ID) {

	for (TriggerVolumes::Index i = 0; i < mTriggerVolumes.count; ++i) {

		if (mTriggerVolumes[i].ID == ID) {

			mTriggerVolumes.removeSwapWithLast(i, true);
			break;
		}
	}
}

void ScriptedFootballer::executeTriggerVolumesOnBall() {

	if (!isBallOwner())
		return;

	if (true/*doClean()*/) {

		for (TriggerVolumes::Index i = 0; i < mTriggerVolumes.count; ++i) {

			mTriggerVolumes[i].update();
		}
	}

	Ball& ball = mNodeMatch->getBall();
	const Vector3& ballPos = ball.mTransformWorld.getPosition();
	float ballRadiusSq = ball.getRadius();
	ballRadiusSq *= ballRadiusSq;

	for (TriggerVolumes::Index i = 0; i < mTriggerVolumes.count; ++i) {

		if (mTriggerVolumes[i].containsSq(ballPos, ballRadiusSq)) {

			ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"onBallTriggerVolume");

			(func)(scriptArg(ball), mTriggerVolumes[i].ID);
		}
	}
}

bool 
ScriptedFootballer::script_shouldTriggerBallDribbleAction(
	SceneTransform& currTransform, SceneTransform& nextTransform, 
	CtVector3& sweetSpotOffset, CtVector3& wantedBallDir, float ballPosEpsilon, float runDirEpsilon) {

	Match& match = mNodeMatch;
	Ball& ball = match.getBall();

	if (ball.getActiveControllerPhys() == NULL)
		return false;

	if (isBallSharedOwner()) 
		return true;
	
	const PhysRigidBody& physBody = ball.getPhysBody();

	Vector3 ballPosDiff;

	physBody.v.mul(match.getClock().getFrameMoveTime(), ballPosDiff);

	float dirChangeAngleSine = (1.0f - nextTransform.getDirection(Scene_Forward).dot(currTransform.getDirection(Scene_Forward)));
	bool dirChanged = fabs(dirChangeAngleSine) >= runDirEpsilon;
	//dirChanged = !currTransform.getDirection(Scene_Forward).equals(nextTransform.getDirection(Scene_Forward));

	//log(L"%f >= %f ?", fabs(dirChangeAngleSine), runDirEpsilon);

	if (!dirChanged) {
		
		Vector3 ballDir;
		float ballPosDiffMag;

		Vector3 footballerPosDiff;
		Vector3 footballerMoveDir;
		float footballerPosDiffMag;

		nextTransform.getPosition().subtract(currTransform.getPosition(), footballerPosDiff);

		footballerPosDiff.normalize(footballerMoveDir, footballerPosDiffMag);
		ballPosDiff.normalize(ballDir, ballPosDiffMag);

		float dot1 = ballDir.dot(wantedBallDir);
		float dot2 = ballDir.dot(footballerMoveDir);

		if ((fabs(1.0f - dot1) <= 0.2f)
			&& (ballPosDiffMag * fabs(dot2) >= footballerPosDiffMag)) {

			return false;
		}
	}

	//const Vector3& ballPos = physBody.pos;
	Vector3 ballPos = physBody.pos;
	
	float currSweetSpotDistSq;

	{
		Vector3 currSweetSpotPos;

		currTransform.transformPoint(sweetSpotOffset, currSweetSpotPos);
		ballPos.el[Scene_Up] = currSweetSpotPos.el[Scene_Up];

		float epsilonSq = ballPosEpsilon * ballPosEpsilon;

		if ((currSweetSpotDistSq = distanceSq(currSweetSpotPos, ballPos)) <= epsilonSq) {

			//mNodeMatch->getConsole().print(L">1");
			//calcBallCorrectionDist(mTransformLocal.getPosition(), posDiff, ballPos, ballCorrectionDist.value());

			return true;
		}
	}

		
	Vector3 nextSweetSpotPos;
	nextTransform.transformPoint(sweetSpotOffset, nextSweetSpotPos);

	Vector3 nextBallPos;
	ballPos.add(ballPosDiff, nextBallPos);
	nextBallPos.el[Scene_Up] = nextSweetSpotPos.el[Scene_Up];

	float nextBallNextSweetSpotDistSq;
	
	nextBallNextSweetSpotDistSq = distanceSq(nextSweetSpotPos, nextBallPos);

	if (nextBallNextSweetSpotDistSq >= currSweetSpotDistSq) {

		//calcBallCorrectionDist(mTransformLocal.getPosition(), posDiff, ballPos, ballCorrectionDist.value());
		
		//mNodeMatch->getConsole().print(L">2");
		return true;
	}

	return false;
}

/*


void ScriptedFootballer::BallControlVolume::init(const unsigned int& _ID, CtVector3& _position, const float& _radius, VolumeClipType _clipType) {

	ID = _ID;
	volume.center = _position;
	volume.radius = _radius;
	radiusSq = _radius * _radius;
	clipType = _clipType;
}

bool ScriptedFootballer::BallControlVolume::getSegIntersection(const Point& segA, const Point& segB, const float& ptRadius, SphereSegIntersection& intersection) {

	return intersection.init(volume.center, volume.radius + ptRadius, segA, segB, true);
}

void ScriptedFootballer::addBallControlVolume(const unsigned int& ID, const TCHAR* boneName, CtVector3& position, const float& radius, VolumeClipType clipType) {

	SoftPtr<BoneInstance> boneInst = mSkelInst->findBoneInstanceByGameName(boneName);

	if (!boneInst.isValid()) {

		assrt(false);
		return;
	}

	BallControlVolume& newVol = mBallControlVolumes.addOne();
	
	newVol.init(ID, position, radius, clipType);
	newVol.coordFrame = &(boneInst->worldTransform);

	mBallControlVolume.volume.add(newVol.volume);
	mBallControlVolume.radiusSq = mBallControlVolume.volume.radius * mBallControlVolume.volume.radius;
}

The ball in a different coord frame than the volumes!
mBallcontrolVolume shoudl be dynamic!!

void ScriptedFootballer::script_updateBallControlVolumeIntersections() {

	BallControlVolumeIntersections& inters = mBallControlVolumeIntersections;
	BallPathTimeResampler sampler;

	inters.count = 0;
	if (!sampler.reset(mNodeMatch->getFootballerBallInterceptManager().getPossiblyUnanalyzedBaseSimul(), mNodeMatch->getClock().getTime(), mNodeMatch->getClock().getFrameMoveTime())) 
		return;

	const BallPathTimeResampler::Sample* pSample;

	BallPathTimeResampler::Sample segSamples[2];
	bool segSampleIsInVOC[2];
	unsigned int segSampleCount = 0;
	unsigned int segSampleFrom = 0;
	unsigned int segSampleTo = segSampleFrom + 1;

	float ballRadius = mNodeMatch->getBall().getRadius();

	while ((pSample = sampler.getNextSample()) != NULL) {

		segSamples[segSampleTo] = *pSample;
		
		bool inVOC = isInVOC(pSample->pos, ballRadius);
		segSampleIsInVOC[segSampleTo] = inVOC;
		
		//analyze
		if (segSampleCount >= 2) {

			for (BallControlVolumes::Index i = 0; i < mBallControlVolumes.size; ++i) {

				ScriptBallControlVolumeIntersection& intersec = inters.addOne();

				if (mBallControlVolumes[i].getSegIntersection(segSamples[segSampleFrom].pos, segSamples[segSampleTo].pos, ballRadius, intersec)) {

					intersec.volumeID = mBallControlVolumes[i].ID;

					if (inVOC) {

						float distSq[2];

						for (unsigned int i = 0; i < intersec.pointCount; ++i) {

							distSq[0] = distanceSq(intersec.points[i], segSamples[segSampleFrom].pos);
							distSq[1] = distanceSq(intersec.points[i], segSamples[segSampleTo].pos);
							
							if (distSq[0] <= distSq[1]) {

								intersec.refTime[intersec.refTimeCount] = segSamples[segSampleFrom].time;

							} else {

								intersec.refTime[intersec.refTimeCount] = segSamples[segSampleTo].time;
							}

							++intersec.refTimeCount;
						}

					} else {

						intersec.refTimeCount = 1;
						intersec.refTime[0] = segSamples[segSampleFrom].time;
					}

				} else {

					--inters.count;
				}
			}
		}

		++segSampleCount;
		segSampleFrom = (segSampleFrom + 1) % 2;
		segSampleTo = (segSampleTo + 1) % 2;

		if (!inVOC) {

			break;
		}
	}

	if (segSampleCount == 1) {

		//probably only 1 resting sample

		BallPathTimeResampler::Sample& singlSample = segSamples[segSampleFrom];

		for (BallControlVolumes::Index i = 0; i < mBallControlVolumes.size; ++i) {

			ScriptBallControlVolumeIntersection& intersec = inters.addOne();

			if (mBallControlVolumes[i].containsSq(singlSample.pos, ballRadius * ballRadius)) {

				intersec.pointCount = 1;
				intersec.points[0] = singlSample.pos;
				intersec.isInternal[0] = true;

				intersec.volumeID = mBallControlVolumes[i].ID;

				intersec.refTimeCount = 1;
				intersec.refTime[0] = singlSample.time;
			}
		}
	}
}

bool ScriptedFootballer::isInVOC(Ball& ball) { return isInVOC(ball.mTransformWorld.getPosition(), ball.getRadius()); }

bool ScriptedFootballer::isInVOC(const Vector3& pos, const float& radius) {

	float radiusSq = radius * radius;

	if (mBallControlVolume.containsSq(pos, radiusSq)) {

		for (BallControlVolumes::Index i = 0; i < mBallControlVolumes.size; ++i) {

			if (mBallControlVolumes[i].containsSq(pos, radiusSq)) {

				return true;
			}
		}
	}
	
	return false;
}
*/

} }