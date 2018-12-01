#include "../Ball.h"
#include "../../../Match.h"

#include "BallSimulation.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallSimulation::TrajSample, CBallTrajSample);

namespace WOF { namespace match {

const TCHAR* BallSimulation::TrajSample::ScriptClassName = L"CBallTrajSample";

void BallSimulation::TrajSample::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<TrajSample>(TrajSample::ScriptClassName).
		func(&TrajSample::script_set, L"set").
		func(&TrajSample::script_isOnPitch, L"isOnPitch").
		func(&TrajSample::script_getTime, L"getTime").
		func(&TrajSample::script_getPos, L"getPos").
		func(&TrajSample::script_getVel, L"getVel").
		func(&TrajSample::script_pos, L"pos");
}

void BallSimulation::Sample::set(const Time& _time, const Point& _pos, const Vector3& _vel) {

	time = _time;
	pos = _pos;
	vel = _vel;
}

void BallSimulation::TrajSample::set(const Time& time, const Point& pos, const Vector3& vel, const bool& _isOnPitch, bool isResting) {

	Sample::set(time, pos, vel);
	isOnPitch = _isOnPitch;

	if (isResting)
		this->vel.zero();
}

void BallSimulation::Sample::interpolateByFactor(const Sample& nextSample, float factor, Sample& result) {

	result.time = lerp(time, nextSample.time, factor);
	lerp(pos, nextSample.pos, factor, result.pos);
	lerp(vel, nextSample.vel, factor, result.vel);
}

void BallSimulation::Sample::interpolateByTime(const Sample& nextSample, Time t, Sample& result) {

	interpolateByFactor(nextSample, (t - time) / (nextSample.time - time), result);
}

void BallSimulation::Sample::interpolateBySpeedSq(const Sample& nextSample, float speedSq, Sample& result) {

	float thisSpeedSq = vel.magSquared();
	float nextSpeedSq = nextSample.vel.magSquared();

	interpolateByFactor(nextSample, (speedSq - thisSpeedSq) / (nextSpeedSq - thisSpeedSq), result);
}

void BallSimulation::Sample::interpolateByPosDim(const Sample& nextSample, int posDim, float posValue, Sample& result) {

	interpolateByFactor(nextSample, (posValue - pos.el[posDim]) / (nextSample.pos.el[posDim] - pos.el[posDim]), result);
}

void BallSimulation::Sample::interpolateByPlane(const Sample& nextSample, const AAPlane& plane, Sample& result) {

	float classif[2];

	plane.classify(pos, classif[0]); 
	plane.classify(nextSample.pos, classif[1]); 

	interpolateByFactor(nextSample, (0.0f - classif[0]) / (classif[1] - classif[0]), result);
}

void BallSimulation::Sample::interpolateByPlane(const Sample& nextSample, const Plane& plane, Sample& result) {

	float classif[2];

	plane.classify(pos, classif[0]); 
	plane.classify(nextSample.pos, classif[1]); 

	interpolateByFactor(nextSample, (0.0f - classif[0]) / (classif[1] - classif[0]), result);
}

void BallSimulation::TrajSample::interpolateByFactor(const TrajSample& nextSample, float factor, TrajSample& result) {

	Sample::interpolateByFactor(nextSample, factor, result);

	result.isOnPitch = isOnPitch && nextSample.isOnPitch;
}

void BallSimulation::TrajSample::interpolateByTime(const TrajSample& nextSample, Time t, TrajSample& result) {

	Sample::interpolateByTime(nextSample, t, result);

	result.isOnPitch = isOnPitch && nextSample.isOnPitch;
}

void BallSimulation::TrajSample::interpolateByPosDim(const TrajSample& nextSample, int posDim, float posValue, TrajSample& result) {

	Sample::interpolateByPosDim(nextSample, posDim, posValue, result);

	result.isOnPitch = isOnPitch && nextSample.isOnPitch;
}

void BallSimulation::TrajSample::interpolateBySpeedSq(const TrajSample& nextSample, float speedSq, TrajSample& result) {

	Sample::interpolateBySpeedSq(nextSample, speedSq, result);

	result.isOnPitch = isOnPitch && nextSample.isOnPitch;
}

bool BallSimulation::Sample::isBallFootballerReachable(Match& match, Ball& ball) {

	return ball.getPos().el[Scene_Up] - ball.getRadius() <= match.mSwitchControlSetup.footballerReachHeight; 
}

bool BallSimulation::Sample::isFootballerReachable(Match& match, const float& ballRadius) const { 
	
	return pos.el[Scene_Up] - ballRadius <= match.mSwitchControlSetup.footballerReachHeight; 
}

void BallSimulation::addSweptBallContacts(Ball& ball, WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials) {

	ball.mCollRegistry.addSweptBallContacts(matManager, sphereMat, radius, pos, sweepVector, intersections, pSimul2, executeMaterials);
}

void BallSimulation::onBallSyncCommand(Ball& ball, const BallCommand* pCommand) {

	if (pCommand) {

		switch (pCommand->type) {

			case BallCommand_Guard:
			case BallCommand_AfterTouch: {
			} break;

			default: {

				resetSimul();
				stopSyncBall();

				mBallCommand = *pCommand;

			} break;
		}

	} else {

		resetSimul();
		stopSyncBall();

		mBallCommand.invalidate();
	}
}

void BallSimulation::onBallSyncSimulableStateChange(Ball& ball) {

	switch (ball.getSimulableState()) {

		case Ball::Simulable_None:
		case Ball::Simulable_Phys: {

			resetSimul();
			stopSyncBall();
		
		} break;

		case Ball::Simulable_PhysAfterTouch: {

			if (mSimulData.isValid()) {

				mSimulData->mApproximateType = Approximate_AfterTouch;
			}

		} break;
	}
}

/*
void BallSimulation::onBallSyncResetSimul(Ball& ball, const BallCommand* pCommand) {

	

	
	
	if (pCommand && pCommand->type == BallCommand_AfterTouch) {

		if (mSimulData.isValid()) {
	
			mSimulData->mApproximateType = Approximate_AfterTouch;
		}

	} else {

		resetSimul();
		stopSyncBall();

		if (pCommand)
			mBallCommand = *pCommand;
	}

	if (!pCommand)
		mBallCommand.invalidate();
	
}
*/

void BallSimulation::onBallSyncPostCollision(Ball& ball, Object& object) {

	if (objectIsDynamic(object)) {

		resetSimul();
		stopSyncBall();
	}
}

bool BallSimulation::estimateStateAtTime(Time t, bool useSyncFlow, TrajSample& result, bool* pIsLastSamplFallback, Time epsilon, bool allowLastSampleFallback) {

	if (useSyncFlow && !mSyncData.isValid())
		return false;

	if (pIsLastSamplFallback)
		*pIsLastSamplFallback = false;

	Index startIndex = useSyncFlow ? mSyncData->flowTrajSampleIndex : 0;
	Index endIndex = mTrajSamples.count;

	SoftPtr<TrajSample> prevSample;

	if (startIndex < mTrajSamples.count) {

		const TrajSample* lastSample = getLastSample();

		if (lastSample->time < t) {

			startIndex = mTrajSamples.count;
		}
	}

	for (Index i = startIndex; i < mTrajSamples.count && i <= endIndex; ++i) {

		TrajSample& sample = mTrajSamples[i];
		float compDiff = t - sample.time;

		if (fabs(compDiff) <= epsilon) {

			result = sample;
			return true;

		} else {

			if (compDiff > 0.0f) {

				prevSample = &sample;

			} else {

				if (prevSample.isValid()) {

					prevSample->interpolateByTime(sample, t, result);
					return true;

				} else {

					result = sample;
					return true;
				}
			}
		}
	}

	if (allowLastSampleFallback && mTrajSamples.count) {

		TrajSample& lastSample = mTrajSamples.el[mTrajSamples.count - 1];

		if (lastSample.vel.isZero()) {

			result = lastSample;

			if (pIsLastSamplFallback)
				*pIsLastSamplFallback = true;

			return true;
		}

		return false;
	}

	return false;
}

bool BallSimulation::estimateStateAtPosDim(int posDim, float posValue, bool useSyncFlow, TrajSample& result, bool* pIsLastSamplFallback, float epsilon, bool allowLastSampleFallback) {

	//only works in 'local coord system of shot' 
	assrt(posValue > 0.0f);
	
	if (useSyncFlow && !mSyncData.isValid())
		return false;

	if (pIsLastSamplFallback)
		*pIsLastSamplFallback = false;

	Index startIndex = useSyncFlow ? mSyncData->flowTrajSampleIndex : 0;
	Index endIndex = mTrajSamples.count;

	SoftPtr<TrajSample> prevSample;

	if (startIndex < mTrajSamples.count) {

		const TrajSample* lastSample = getLastSample();

		if (lastSample->pos.el[posDim] < posValue) {

			startIndex = mTrajSamples.count;
		}
	}

	for (Index i = startIndex; i < mTrajSamples.count && i <= endIndex; ++i) {

		TrajSample& sample = mTrajSamples[i];
		float compDiff = posValue - sample.pos.el[posDim];

		if (fabs(compDiff) <= epsilon) {

			result = sample;
			return true;

		} else {

			if (compDiff > 0.0f) {

				prevSample = &sample;

			} else {

				if (prevSample.isValid()) {

					prevSample->interpolateByPosDim(sample, posDim, posValue, result);
					return true;

				} else {

					result = sample;
					return true;
				}
			}
		}
	}

	if (allowLastSampleFallback && mTrajSamples.count) {

		TrajSample& lastSample = mTrajSamples.el[mTrajSamples.count - 1];

		if (lastSample.vel.isZero()) {

			result = lastSample;

			if (pIsLastSamplFallback)
				*pIsLastSamplFallback = true;

			return true;
		}

		return false;
	}

	return false;
}

bool BallSimulation::estimateStateAtPlane(const AAPlane& plane, bool useSyncFlow, TrajSample& result, bool isNoObstacleSimul, bool* pIsLastSamplFallback, bool allowLastSampleFallback) {

	if (useSyncFlow && !mSyncData.isValid())
		return false;

	if (pIsLastSamplFallback)
		*pIsLastSamplFallback = false;

	Index startIndex = useSyncFlow ? mSyncData->flowTrajSampleIndex : 0;
	Index endIndex = mTrajSamples.count;

	SoftPtr<TrajSample> prevSample;

	if (isNoObstacleSimul) {

		if (startIndex < mTrajSamples.count) {

			const TrajSample* lastSample = getLastSample();

			if (plane.classify(lastSample->pos) == PP_Front) {

				startIndex = mTrajSamples.count;
			}
		}
	}

	for (Index i = startIndex; i < mTrajSamples.count && i <= endIndex; ++i) {

		TrajSample& sample = mTrajSamples[i];
		PlanePartitionResult compDiff = plane.classify(sample.pos);

		if (compDiff == PP_Front) {

			prevSample = &sample;

		} else {

			if (prevSample.isValid()) {

				prevSample->interpolateByPlane(sample, plane, result);
				return true;

			} else {

				result = sample;
				return true;
			}
		}
	}

	if (allowLastSampleFallback && mTrajSamples.count) {

		TrajSample& lastSample = mTrajSamples.el[mTrajSamples.count - 1];

		result = lastSample;

		if (pIsLastSamplFallback)
			*pIsLastSamplFallback = true;

		return true;
	}

	return false;
}

bool BallSimulation::estimateStateAtPlane(const Plane& plane, bool useSyncFlow, TrajSample& result, bool isNoObstacleSimul, bool* pIsLastSamplFallback, bool allowLastSampleFallback) {

	if (useSyncFlow && !mSyncData.isValid())
		return false;

	if (pIsLastSamplFallback)
		*pIsLastSamplFallback = false;

	Index startIndex = useSyncFlow ? mSyncData->flowTrajSampleIndex : 0;
	Index endIndex = mTrajSamples.count;

	SoftPtr<TrajSample> prevSample;

	if (isNoObstacleSimul) {

		if (startIndex < mTrajSamples.count) {

			const TrajSample* lastSample = getLastSample();

			if (plane.classify(lastSample->pos) == PP_Front) {

				startIndex = mTrajSamples.count;
			}
		}
	}

	for (Index i = startIndex; i < mTrajSamples.count && i <= endIndex; ++i) {

		TrajSample& sample = mTrajSamples[i];
		PlanePartitionResult compDiff = plane.classify(sample.pos);

		if (compDiff == PP_Front) {

			prevSample = &sample;

		} else {

			if (prevSample.isValid()) {

				prevSample->interpolateByPlane(sample, plane, result);
				return true;

			} else {

				result = sample;
				return true;
			}
		}
	}

	if (allowLastSampleFallback && mTrajSamples.count) {

		TrajSample& lastSample = mTrajSamples.el[mTrajSamples.count - 1];

		result = lastSample;

		if (pIsLastSamplFallback)
			*pIsLastSamplFallback = true;

		return true;
	}

	return false;
}

bool BallSimulation::estimateStateAtSpeedSq(float speedSq, bool useSyncFlow, TrajSample& result, bool* pIsLastSamplFallback, float epsilonSq, bool allowLastSampleFallback) {

	if (useSyncFlow && !mSyncData.isValid())
		return false;

	if (pIsLastSamplFallback)
		*pIsLastSamplFallback = false;

	Index startIndex = useSyncFlow ? mSyncData->flowTrajSampleIndex : 0;
	Index endIndex = mTrajSamples.count;

	SoftPtr<TrajSample> prevSample;

	for (Index i = startIndex; i < mTrajSamples.count && i <= endIndex; ++i) {

		TrajSample& sample = mTrajSamples[i];
		float compDiff = speedSq - sample.vel.magSquared();

		if (fabs(compDiff) <= epsilonSq) {

			result = sample;
			return true;

		} else {

			if (compDiff > 0.0f) {

				prevSample = &sample;

			} else {

				if (prevSample.isValid()) {

					prevSample->interpolateBySpeedSq(sample, speedSq, result);
					return true;

				} else {

					result = sample;
					return true;
				}
			}
		}
	}

	if (allowLastSampleFallback && mTrajSamples.count) {

		TrajSample& lastSample = mTrajSamples.el[mTrajSamples.count - 1];

		if (lastSample.vel.isZero()) {

			result = lastSample;

			if (pIsLastSamplFallback)
				*pIsLastSamplFallback = true;

			return true;
		}

		return false;
	}

	return false;
}

bool BallSimulation::estimateNextBallState(Ball& ball, TrajSample& result) {

	const Clock& clock = ball.mNodeMatch->getClock();
	Time nextTime = ball.getSimulStateTime() + clock.getFrameMoveTickLength();

	if (mSyncData.isValid()) {

		SyncData& syncData = mSyncData;

		if ((syncData.ball.ptr() == &ball) && isValidSync() && (syncData.flowTrajSampleIndex + 1 < mTrajSamples.count)) {

			if (syncData.flowTrajSampleIndex + 1 < mTrajSamples.count) {

				const TrajSample& sample = mTrajSamples[syncData.flowTrajSampleIndex];
				const TrajSample& nextSample = mTrajSamples[syncData.flowTrajSampleIndex + 1];

				if (fabs(nextTime - nextSample.time) < 0.5f * clock.getFrameMoveTickLength()) {

					result.time = nextTime;
					result.pos = nextSample.pos;
					result.vel = nextSample.vel;
					result.isOnPitch = nextSample.isOnPitch;

				} else {

					result.time = nextTime;
					sample.pos.addMultiplication(sample.vel, nextTime - sample.time, result.pos);
					sample.vel.mul(0.9f, result.vel);
					result.isOnPitch = sample.isOnPitch;
				}

			} else {

				const TrajSample& sample = mTrajSamples[mTrajSamples.lastIndex()];

				result.time = nextTime;
				result.pos = sample.pos;
				result.vel.zero();
				result.isOnPitch = sample.isOnPitch;
			}

			return true;
		}
	}

	if (ball.isSimulable()) {

		if (ball.isResting()) {

			result.time = nextTime;
			result.pos = ball.getPos();
			result.vel.zero();
			result.isOnPitch = ball.isOnPitch();

		} else {

			result.time = nextTime;
			result.pos = ball.getPos();
			ball.getPos().addMultiplication(dref(ball.getVel()), nextTime - ball.getSimulStateTime(), result.pos);
			dref(ball.getVel()).mul(0.9f, result.vel);
			result.isOnPitch = ball.isOnPitch();
		}

		return true;
	}

	return false;
}

bool BallSimulation::tuneApproximatedEstimate(Ball& ball, TrajSample& sample) {

	if (mSimulData.isValid()) {

		switch (mSimulData->mApproximateType) {

			case Approximate_AfterTouch: {

				if (ball.hasAfterTouchPosDiff()) {

					const Time& currTime = ball.mNodeMatch->getClock().getTime();
					const Time& sampleTime = sample.time;

					float timeDiff = sampleTime - currTime;
					float afterTouchTotalTime = currTime - ball.getAfterTouchPosDiffStartTime();

					if (timeDiff > 0.0f && afterTouchTotalTime > 0.0f) {

						ball.getAfterTouchPosDiff().mulAndAdd((timeDiff + afterTouchTotalTime) / afterTouchTotalTime, sample.pos);

						return true;
					}
				}

			} break;

			case Approximate_None: {
			} break;
			
			default: {

				assrt(false);

			} break;
		}
	}

	return false;
}

void BallSimulation::onBallSyncBallState(Ball& ball, const Time& time) {

	if (!mSyncData.isValid() || !mSimulData.isValid()) 
		return;

	SyncData& syncData = mSyncData;

	bool found;

	found = false;

	for (; syncData.flowTrajSampleIndex < mTrajSamples.count; ++syncData.flowTrajSampleIndex) {

		if (mTrajSamples[syncData.flowTrajSampleIndex].time >= time) {

			found = true;
			break;
		}
	}

	if (found) {

		if (syncData.flowTrajSampleIndex > 0)
			--syncData.flowTrajSampleIndex;

	} else {

		if (syncData.state == Sync_Syncing) {

			//could only happen if ball is resting, the sync call caught up with processing simul
			if (mSimulData->mSimulState == Simul_Simulating) {

				mSimulData->mSimulState = continueSimulProcessing(ball, false);

				if (mSimulData->mSimulState != Simul_Simulated) {

					assrt(false);
					mSimulData->mSimulState = Simul_Simulated;
				}

			} else {

				assrt(false);
			}
		}

		syncData.state = Sync_Expired;
		return;
	}

	if (mSimulData->mEnableFootballerReachableAnalysis) {

		for (; syncData.flowFootballerReachableIntervalIndex < mFootballerReachableIntervals.getCount(); ++syncData.flowFootballerReachableIntervalIndex) {

			if (mFootballerReachableIntervals.get(syncData.flowFootballerReachableIntervalIndex).max >= syncData.flowTrajSampleIndex) {

				found = true;
				break;
			}
		}

		if (found) {

			if (syncData.flowFootballerReachableIntervalIndex > 0)
				--syncData.flowFootballerReachableIntervalIndex;
		}
	}


	found = false;

	for (; syncData.flowCollSampleIndex < mCollSamples.count; ++syncData.flowCollSampleIndex) {

		if (mCollSamples[syncData.flowCollSampleIndex].time >= time) {

			found = true;
			break;
		}
	}

	if (found) { 

		if (syncData.flowCollSampleIndex > 0)
			--syncData.flowCollSampleIndex;
	}
}

bool BallSimulation::ignoreCollision(Object* pObject) {

	return objectIsDynamic(dref(pObject));
}

bool BallSimulation::signalPreCollision(Object* pObject, const Time& time, Ball& ball) {

	if (mSimulData->mRecordSamples || (mSimulData->mEndConditionType == EndCondition_Coll)) {

		CollSample& sample = mCollSamples.addOne();

		sample.set(time, ball.getPhysBody().pos, ball.getPhysBody().v);
		sample.collider = pObject;
	}

	return true;
}

bool BallSimulation::signalPostCollision(Object* pObject, const Time& time, Ball& ball) {

	if (mSimulData->mRecordSamples || (mSimulData->mEndConditionType == EndCondition_Coll)) {

		CollSample& sample = mCollSamples[mCollSamples.lastIndex()];
		sample.postVel = ball.getPhysBody().v;

		mSimulData->mCollBetweenSamples = true;
	}

	return true;
}

bool BallSimulation::signalPreTrigger(WorldMaterialCombo* pMaterial, const Time& time, Ball& ball, const SimulationID& ballMoveTickID) {

	if (mSimulData->mRecordSamples) {

		const TriggerSample* pLastSample = getTriggerLastSample();

		//ignore continuous triggers
		if (pLastSample && pLastSample->ballMoveTickID + 1 == ballMoveTickID)
			return true;

		TriggerSample& sample = mTriggerSamples.addOne();

		sample.set(time, ball.getPhysBody().pos, ball.getPhysBody().v);

		if (pMaterial) {

			if (pMaterial->mats[0]->trigger.isValid())
				sample.trigger = pMaterial->mats[0]->trigger;
			else
				sample.trigger = pMaterial->mats[1]->trigger;

		} else {

			sample.trigger = NULL;
		}

		assrt(sample.trigger.isValid());

		sample.ballMoveTickID = ballMoveTickID;
	}

	return true;
}

bool BallSimulation::signalPostTrigger(WorldMaterialCombo* pMaterial, const Time& time, Ball& ball) {

	return true;
}

bool BallSimulation::signalSimulTickEnd(const Time& time, const unsigned int& simulTickIndex, Ball& ball) {

	SimulData& simulData = mSimulData;

	if (simulData.mRecordSamples) {

		if (simulData.mNextRecordSimulTick == simulTickIndex) {

			bool addSample = false;
			bool addLastSample = false;
			
			if (mTrajSamples.count > 0) {

				Vector3 extrapolPos;

				mTrajSamples[mTrajSamples.lastIndex()].interpolatePos(time, extrapolPos);
				
				if (distanceSq(extrapolPos, ball.getPhysBody().pos) >= simulData.mToleranceSq) {

					if (simulData.mEnableExtraTrajPrecision && simulData.mCollBetweenSamples) {

						addLastSample = true;
					}

					addSample = true;
				}

			} else {

				addSample = true;
			}

			bool isFootballerReachable = false;
				
			if (simulData.mEnableFootballerReachableAnalysis)
				isFootballerReachable = TrajSample::isBallFootballerReachable(ball.mNodeMatch, ball);

			if (!addSample) {
				
				if (ball.isResting()) {

					addSample = true;

				} else if (simulData.mEnableFootballerReachableAnalysis && isFootballerReachable && !simulData.mLastInspectedTickEndSampleReachable) {

					addSample = true;
				}
			}

			if (!addLastSample) {

				if (simulData.mEnableFootballerReachableAnalysis && !isFootballerReachable && simulData.mLastInspectedTickEndSampleReachable) {

					addLastSample = true;
				}
			}

			simulData.mLastInspectedTickEndSampleReachable = isFootballerReachable;

			if (addLastSample && !simulData.mLastInspectedTickEndSampleAdded)
				mTrajSamples.addOne().set(time, simulData.mLastInspectedTickEndSample.pos, simulData.mLastInspectedTickEndSample.vel, simulData.mLastInspectedTickEndSample.isOnPitch);

			if (addSample) {

				simulData.mCollBetweenSamples = false;
				simulData.mLastInspectedTickEndSampleAdded = true;

				TrajSample& sample = mTrajSamples.addOne();
				sample.set(time, ball.getPhysBody().pos, ball.getPhysBody().v, ball.isOnPitch(), ball.isResting());
				
				if (simulData.mEnableFootballerReachableAnalysis && isFootballerReachable) {
				
					mFootballerReachableIntervals.add(mTrajSamples.count - 1);
				}

			} else {

				simulData.mLastInspectedTickEndSampleAdded = false;
				simulData.mLastInspectedTickEndSample.set(time, ball.getPhysBody().pos, ball.getPhysBody().v, ball.isOnPitch(), ball.isResting());
			}
			
			simulData.mNextRecordSimulTick += simulData.mSimulTickSkipCount;
		}
	}

	{
		float apogeeValue = ball.getPhysBody().pos.el[Scene_Up];

		switch (simulData.mEndConditionType) {

			case EndCondition_Apogee: {

				if (simulTickIndex != 0) {

					if (apogeeValue <= simulData.mApogeeHeight) {

						return false;
					}
				}

			} break;

			case EndCondition_Coll: {

				if (mCollSamples.count != 0)
					return false;

			} break;
		}

		if ((apogeeValue > simulData.mApogeeHeight) || (simulTickIndex == 0)) {

			simulData.mApogeeHeight = apogeeValue;
			simulData.mApogeeTime = time;
		} 
	}

	if (simulData.mSimulEndAtRest && ball.isResting()) {

		return false;
	}

	return true;
}

void BallSimulation::render(Renderer& renderer, bool useSyncFlow, 
							 const float* pBallRadiusStart, const float* pBallRadiusEnd, 
							 const RenderColor* pLineStart, const RenderColor* pLineEnd, 
							 const RenderColor* pSphereStart, const RenderColor* pSphereEnd,
							 const RenderColor* pSphereColl) {

	if (pBallRadiusStart == NULL && (!mSyncData.isValid() || !mSyncData->ball.isValid())) {

		//assrt(false);
		return;
	}

	if (useSyncFlow && (!mSyncData.isValid() || mSyncData->state != Sync_Syncing && mSyncData->state != Sync_Synced)) {

		useSyncFlow = false;
	}

	const float& ballRadiusStart = pBallRadiusStart ? *pBallRadiusStart : mSyncData->ball->getRadius() * 0.15f;
	const float& ballRadiusEnd = pBallRadiusEnd ? *pBallRadiusEnd : mSyncData->ball->getRadius() * 0.95f;
	const RenderColor& lineStart = pLineStart ? *pLineStart : RenderColor::kWhite;
	const RenderColor& lineEnd = pLineEnd ? *pLineEnd : lineStart;
	RenderColor sphereStart = pSphereStart ? *pSphereStart : RenderColor::kWhite;
	RenderColor sphereEnd = pSphereEnd ? *pSphereEnd : sphereStart;
	const RenderColor& sphereColl = pSphereColl ? *pSphereColl : RenderColor::kBlack;

	if (mSimulData.isValid()) {

		if (mSimulData->mApproximateType == Approximate_AfterTouch) {

			sphereStart = RenderColor::kLightPink;
			sphereEnd = RenderColor::kDeepPink;
		}
	}

	
	Sphere sphere;
	
	float factor;
	float lfactor1;
	//float lfactor2;

	float colRange = (float) (mTrajSamples.count + 1);

	RenderColor col1;
	col1.alpha = 1.0f;
	//RenderColor col2;

	Index startIndex = useSyncFlow ? mSyncData->flowTrajSampleIndex : 0;
	Index endIndex = mTrajSamples.count;

	for (Index i = 0; i < mTrajSamples.count && i <= endIndex; ++i) {

		factor = (float) i / (float) mTrajSamples.count;
		lfactor1 = (float) i / colRange;
		//lfactor2 = (float) i / colRange;

		sphere.center = mTrajSamples[i].pos;
		sphere.radius = lerp(ballRadiusStart, ballRadiusEnd, factor);

		lerp((Vector&) sphereStart, (Vector&) sphereEnd, factor, (Vector&) col1);

		if (i >= startIndex)
			renderer.draw(sphere, &RigidMatrix4x3::kIdentity, &col1);

		lerp((Vector&) lineStart, (Vector&) lineEnd, lfactor1, (Vector&) col1);
		//lerp((Vector&) lineStart, (Vector&) lineEnd, lfactor2, (Vector&) col2);

		if (i + 1 < mTrajSamples.count) {

			if (i + 1 >= startIndex)
				renderer.draw(mTrajSamples[i].pos, mTrajSamples[i + 1].pos, &RigidMatrix4x3::kIdentity, &col1);
		}
	}

	startIndex = useSyncFlow ? mSyncData->flowCollSampleIndex : 0;
	endIndex = mCollSamples.count;

	for (Index i = startIndex; i < mCollSamples.count && i <= endIndex; ++i) {

		sphere.center = mCollSamples[i].pos;
		sphere.radius = ballRadiusEnd;

		renderer.draw(sphere, &RigidMatrix4x3::kIdentity, &sphereColl);
	}
}

BallSimulation::BallSimulation() : mID(0) {
}

BallSimulation::SimulData::SimulData() {

	mSimulState = Simul_None;
	mSimulMaxFrameDuration = 10.0f;
	mSimulFrameDuration = 2.5f;
	mSimulSimulMaxFrameCount = 1;
	mSimulTickSkipCount = 1;
	mToleranceSq = 20.0f * 20.0f;
	mSimulEndAtRest = true;
	mEnableEarlySimulEnd = true;
	mEnableExtraTrajPrecision = false;
	mEnableFootballerReachableAnalysis = true;
	mApproximateType = Approximate_None;

	mRecordSamples = true;
	mEndConditionType = EndCondition_None;
}

void BallSimulation::setupUsingMatch(Match& match) {

	setSimulMaxFrameCount(3);
	setSimulMaxFrameDuration(10.0f);
	setSimulFrameDuration(2.5f);
	setSimulTickSkipCount(match.getClock().getPhysFPSMultiplier());
	setTolerance(match.getCoordSys().convUnit(0.28f));
	setEnableExtraTrajPrecision(true);
}

void BallSimulation::loadConfig(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	{
		//chunk.scanAttribute(tempStr, L"logPerformance", logPerformance);

		float tempf;
		bool tempb;
		unsigned int tempu;

		if (chunk.scanAttribute(tempStr, L"simulFrameDuration", L"%f", &tempf))
			setSimulFrameDuration(tempf);

		if (chunk.scanAttribute(tempStr, L"simulMaxFrameDuration", L"%f", &tempf))
			setSimulMaxFrameDuration(tempf);

		if (chunk.scanAttribute(tempStr, L"simulMaxFrameCount", L"%u", &tempu))
			setSimulMaxFrameCount(tempu);

		if (chunk.scanAttribute(tempStr, L"simulTolerance", L"%f", &tempf)) {
		
			if (pConv) {
			
				pConv->toTargetUnits(tempf);
				
			}

			setTolerance(tempf);
		}

		setSimulTickSkipCount(match.getClock().getPhysFPSMultiplier());
		
		if (chunk.scanAttribute(tempStr, L"simulExtraTrajPrecision", tempb)) {

			setEnableExtraTrajPrecision(tempb);
		}
	}
}



BallSimulation::SyncData::SyncData() {

	state = Sync_None;

	flowTrajSampleIndex = 0;
	flowCollSampleIndex = 0;
	flowFootballerReachableIntervalIndex = 0;
}

void BallSimulation::enableSyncMode(bool enable) {

	if (enable) {

		if (!mSyncData.isValid()) {

			WE_MMemNew(mSyncData.ptrRef(), SyncData());
		}

	} else {

		stopSyncBall();
		mSyncData.destroy();
	}
}

void BallSimulation::enableSimul(bool enable) {

	mSimulData.destroy();

	if (enable) {

		if (!mSimulData.isValid()) {

			WE_MMemNew(mSimulData.ptrRef(), SimulData());
			mSimulData.setType(Ptr_HardRef, false);
		}
	} 
}

void BallSimulation::setSimulData(SimulData* pSimulData) {

	mSimulData.destroy();

	if (pSimulData) {

		mSimulData.setType(Ptr_SoftRef, false);
		mSimulData = pSimulData;
	} 
}

BallSimulation::SimulData* BallSimulation::cloneSimulData() {

	if (mSimulData.isValid()) {

		SoftPtr<SimulData> simulData;

		WE_MMemNew(simulData.ptrRef(), SimulData());
		*simulData = mSimulData.dref();

		return simulData;
	}

	return NULL;
}

void BallSimulation::setSimulMaxFrameCount(unsigned int value) {

	mSimulData->mSimulSimulMaxFrameCount = value;
}

void BallSimulation::setEnableEarlySimulEnd(bool value) {

	mSimulData->mEnableEarlySimulEnd = value;
}

void BallSimulation::setSimulMaxFrameDuration(float value) {

	mSimulData->mSimulMaxFrameDuration = value;
}

void BallSimulation::setSimulFrameDuration(float value) {

	mSimulData->mSimulFrameDuration = value;
}

void BallSimulation::setSimulTickSkipCount(unsigned int value) {

	mSimulData->mSimulTickSkipCount = value;
}


void BallSimulation::setSimulEndAtRest(bool value) {

	mSimulData->mSimulEndAtRest = value;
}

void BallSimulation::setTolerance(float value) {

	mSimulData->mToleranceSq = value * value;
}

void BallSimulation::setEnableExtraTrajPrecision(bool value) {

	mSimulData->mEnableExtraTrajPrecision = value;
}

void BallSimulation::setEnableFootballerReachableAnalysis(bool value) {

	mSimulData->mEnableFootballerReachableAnalysis = value;
}

void BallSimulation::resetSimul() {

	if (mSimulData.isValid()) {
	
		mSimulData->mSimulState = Simul_None;
		mSimulData->mApproximateType = Approximate_None;
	}
}

void BallSimulation::rewindSync() {

	if (mSyncData.isValid()) {

		mSyncData->flowTrajSampleIndex = 0;
		mSyncData->flowCollSampleIndex = 0;
		mSyncData->flowFootballerReachableIntervalIndex = 0;
	}
}

float BallSimulation::getMaxRemainingSimulTime(Ball& ball) {

	if (mSimulData->mSimulState == Simul_Simulating) {

		unsigned int remainingFrameCount = mSimulData->mSimulSimulMaxFrameCount - mSimulData->mNextSimulProcessingFrameIndex;

		return remainingFrameCount * ball.mNodeMatch->getClock().getFrameMoveTickLength();
	}

	return mSimulData->mSimulState == Simul_Simulated ? 0.0f : -1.0f;
}

BallSimulation::SimulState BallSimulation::continueSimulProcessing(Ball& ball, bool restart, Time simulStartTime) {

	SimulData& simulData = mSimulData;

	if (restart) {

		mTrajSamples.count = 0;
		mCollSamples.count = 0;
		mTriggerSamples.count = 0;
		mFootballerReachableIntervals.clear();
		simulData.mNextSimulProcessingFrameIndex = 0;
		simulData.mApproximateType = Approximate_None;
	}

	SoftPtr<BallControllerPhysImpl> phys = ball.getActiveControllerPhys();

	if ((ball.getSimulableState() != Ball::Simulable_None) && phys.isValid()) {

		Ball& simulBall = ball.mNodeMatch->getSimulBall();

		if (simulData.mNextSimulProcessingFrameIndex == 0) {

			rewindSync();

			++mID;
			simulData.mNextRecordSimulTick = 0;
			mSimulStartTime = simulStartTime;

			simulData.mLastSimulSample.time = mSimulStartTime;
			simulData.mLastSimulSample.tick = 0;
			simulData.mLastSimulSample.isResting = false;

			if (&simulBall != &ball) {

				phys->setPosition(simulBall, ball.getPhysBody().pos, NULL);
				phys->setVelocities(simulBall, ball.getPhysBody().v, ball.getPhysBody().w, NULL); 
			}

		} else {

			phys->setPosition(simulBall, simulData.mLastSimulSample.pos, NULL);
			phys->setVelocities(simulBall, simulData.mLastSimulSample.vel, simulData.mLastSimulSample.w, NULL); 
		}

		if (simulData.mLastSimulSample.isResting) {

			if (simulData.mEnableEarlySimulEnd) {

				return Simul_Simulated;

			} else {

				//do nothing
			}

		} else {

			Time simulDuration = (simulData.mNextSimulProcessingFrameIndex + 1 == simulData.mSimulSimulMaxFrameCount) ? simulData.mSimulMaxFrameDuration : simulData.mSimulFrameDuration;

			phys->simulate(ball.mNodeMatch, this, simulBall, simulData.mLastSimulSample.time, simulData.mLastSimulSample.tick, simulDuration, ball.mNodeMatch->getClock().getFramePhysTickLength(), simulData.mLastSimulSample.time, simulData.mLastSimulSample.tick);

			simulData.mLastSimulSample.pos = simulBall.getPhysBody().pos;
			simulData.mLastSimulSample.vel = simulBall.getPhysBody().v;
			simulData.mLastSimulSample.w = simulBall.getPhysBody().w;
			simulData.mLastSimulSample.isResting = simulBall.isResting();
		}

		++simulData.mNextSimulProcessingFrameIndex;

		if (simulData.mNextSimulProcessingFrameIndex == simulData.mSimulSimulMaxFrameCount) {

			return Simul_Simulated;

		} else {

			return Simul_Simulating;
		}

	} else {

		//assrt(false);
		return Simul_None;
	}
}

BallSimulation::SimulState BallSimulation::updateSimulate(Ball& ball, Time simulStartTime) {

	SimulData& simulData = mSimulData;

	switch (simulData.mSimulState) {

		case Simul_None: {

			simulData.mSimulState = continueSimulProcessing(ball, true, simulStartTime);

		} break;

		case Simul_Simulating: {

			simulData.mSimulState = continueSimulProcessing(ball, false, simulStartTime);

		} break;

		case Simul_Simulated: {
		} break;

		default: {

			assrt(false);

		} break;
	}

	return simulData.mSimulState;
}

void BallSimulation::startSyncBall(Ball& ball) {

	if (mSyncData.isValid()) {

		mSyncData->state = Sync_None;
		mSyncData->ball = &ball;
		ball.addListener(*this);
	}
}

void BallSimulation::stopSyncBall() {

	if (mSyncData.isValid()) {

		mSyncData->state = Sync_None;

		if (mSyncData->ball.isValid()) {

			mSyncData->ball->removeListener(*this);
			mSyncData->ball.destroy();
		}
	}
}

BallSimulation::SyncState BallSimulation::updateSync(Ball& ball) {

	if (!mSyncData.isValid()) {

		assrt(false);
		return Sync_None;
	}

	SyncData& syncData = mSyncData;
	SimulData& simulData = mSimulData;

	switch (syncData.state) {

		case Sync_None: {

			switch (simulData.mSimulState = continueSimulProcessing(ball, true, ball.getSimulStateTime())) {

				case Simul_None: {

					syncData.state = Sync_None;

				} break;

				case Simul_Simulating: {

					startSyncBall(ball);
					syncData.state = Sync_Syncing;

				} break;

				case Simul_Simulated: {

					startSyncBall(ball);
					syncData.state = Sync_Synced;

				} break;
			}

		} break;

		case Sync_Syncing: {

			switch (simulData.mSimulState = continueSimulProcessing(ball, false)) {

				case Simul_None: {

					resetSimul();
					stopSyncBall();

				} break;

				case Simul_Simulating: {

					syncData.state = Sync_Syncing;

				} break;

				case Simul_Simulated: {

					syncData.state = Sync_Synced;

				} break;
			}

		} break;

		case Sync_Synced: {
		} break;

		case Sync_Expired: {
		} break;

		default: {

			assrt(false);

		} break;
	}

	return syncData.state;
}


void BallSimulation_SimulPitch::addSweptBallContacts(Ball& ball, WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials) {

	ball.mNodeMatch->getSimulPitch().addSweptBallContacts(matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);
}	



} }