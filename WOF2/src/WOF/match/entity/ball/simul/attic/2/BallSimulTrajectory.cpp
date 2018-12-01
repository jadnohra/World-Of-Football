#include "../Ball.h"
#include "../../../Match.h"

#include "BallSimulTrajectory.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallSimulTrajectory::TrajSample, CBallTrajSample);

namespace WOF { namespace match {

const TCHAR* BallSimulTrajectory::TrajSample::ScriptClassName = L"CBallTrajSample";

void BallSimulTrajectory::TrajSample::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<TrajSample>(TrajSample::ScriptClassName).
		func(&TrajSample::script_isOnPitch, L"isOnPitch").
		func(&TrajSample::script_getPos, L"getPos").
		func(&TrajSample::script_getVel, L"getVel");
}

void BallSimulTrajectory::Sample::set(const Time& _time, const Point& _pos, const Vector3& _vel) {

	time = _time;
	pos = _pos;
	vel = _vel;
}

void BallSimulTrajectory::TrajSample::set(const Time& time, const Point& pos, const Vector3& vel, const bool& _isOnPitch) {

	Sample::set(time, pos, vel);
	isOnPitch = _isOnPitch;
}

bool BallSimulTrajectory::TrajSample::isBallFootballerReachable(Match& match, Ball& ball) {

	return ball.getPos().el[Scene_Up] - ball.getRadius() <= match.mSwitchControlSetup.footballerReachHeight; 
}

bool BallSimulTrajectory::TrajSample::isFootballerReachable(Match& match, const float& ballRadius) const { 
	
	return pos.el[Scene_Up] - ballRadius <= match.mSwitchControlSetup.footballerReachHeight; 
}

bool BallSimulTrajectory::ignoreCollision(Object* pObject) {

	return objectIsDynamic(dref(pObject));
}

bool BallSimulTrajectory::signalPreCollision(Object* pObject, const Time& time, Ball& ball) {

	CollSample& sample = mCollSamples.addOne();

	sample.set(time, ball.getPhysBody().pos, ball.getPhysBody().v);
	sample.collider = pObject;

	return true;
}

bool BallSimulTrajectory::signalPostCollision(Object* pObject, const Time& time, Ball& ball) {

	CollSample& sample = mCollSamples[mCollSamples.lastIndex()];
	sample.postVel = ball.getPhysBody().v;

	mCollBetweenSamples = true;

	return true;
}

bool BallSimulTrajectory::signalSimulTickEnd(const Time& time, const unsigned int& simulTickIndex, Ball& ball) {

	if (mNextRecordSimulTick == simulTickIndex) {

		bool addSample = false;
		bool addLastSample = false;
		
		if (mTrajSamples.count > 0) {

			Vector3 extrapolPos;

			mTrajSamples[mTrajSamples.lastIndex()].extrapolatePos(time, extrapolPos);
			
			if (distanceSq(extrapolPos, ball.getPhysBody().pos) >= mToleranceSq) {

				if (mEnableExtraTrajPrecision && mCollBetweenSamples) {

					addLastSample = true;
				}

				addSample = true;
			}

		} else {

			addSample = true;
		}

		bool isFootballerReachable = false;
			
		if (mEnableFootballerReachableAnalysis)
			isFootballerReachable = TrajSample::isBallFootballerReachable(ball.mNodeMatch, ball);

		if (!addSample) {
			
			if (ball.isResting()) {

				addSample = true;

			} else if (mEnableFootballerReachableAnalysis && isFootballerReachable && !mLastInspectedTickEndSampleReachable) {

				addSample = true;
			}
		}

		if (!addLastSample) {

			if (mEnableFootballerReachableAnalysis && !isFootballerReachable && mLastInspectedTickEndSampleReachable) {

				addLastSample = true;
			}
		}

		mLastInspectedTickEndSampleReachable = isFootballerReachable;

		if (addLastSample && !mLastInspectedTickEndSampleAdded)
			mTrajSamples.addOne().set(time, mLastInspectedTickEndSample.pos, mLastInspectedTickEndSample.vel, mLastInspectedTickEndSample.isOnPitch);

		if (addSample) {

			mCollBetweenSamples = false;
			mLastInspectedTickEndSampleAdded = true;

			TrajSample& sample = mTrajSamples.addOne();
			sample.set(time, ball.getPhysBody().pos, ball.getPhysBody().v, ball.isOnPitch());
			
			if (mEnableFootballerReachableAnalysis && isFootballerReachable) {
			
				mFootballerReachableIntervals.add(mTrajSamples.count - 1);
			}

		} else {

			mLastInspectedTickEndSampleAdded = false;
			mLastInspectedTickEndSample.set(time, ball.getPhysBody().pos, ball.getPhysBody().v, ball.isOnPitch());
		}
		
		mNextRecordSimulTick += mSimulTickSkipCount;
	}

	return true;
}

void BallSimulTrajectory::render(Renderer& renderer, Ball* pBall, BallSimulTrajectorySync* pSync, 
							 const float* pBallRadiusStart, const float* pBallRadiusEnd, 
							 const RenderColor* pLineStart, const RenderColor* pLineEnd, 
							 const RenderColor* pSphereStart, const RenderColor* pSphereEnd,
							 const RenderColor* pSphereColl) {

	if (pBallRadiusStart == NULL && !pBall) {

		//assrt(false);
		return;
	}

	if (pSync) {
		
		BallSimulTrajectorySync::SyncState syncState = pSync->getState();

		if (syncState != BallSimulSync_Syncing && syncState != BallSimulSync_Synced) {

			pSync = NULL;
		}
	}

	const float& ballRadiusStart = pBallRadiusStart ? *pBallRadiusStart : pBall->getRadius() * 0.15f;
	const float& ballRadiusEnd = pBallRadiusEnd ? *pBallRadiusEnd : pBall->getRadius() * 0.95f;
	const RenderColor& lineStart = pLineStart ? *pLineStart : RenderColor::kWhite;
	const RenderColor& lineEnd = pLineEnd ? *pLineEnd : lineStart;
	const RenderColor& sphereStart = pSphereStart ? *pSphereStart : RenderColor::kWhite;
	const RenderColor& sphereEnd = pSphereEnd ? *pSphereEnd : sphereStart;
	const RenderColor& sphereColl = pSphereColl ? *pSphereColl : RenderColor::kBlack;

	
	Sphere sphere;
	
	float factor;
	float lfactor1;
	//float lfactor2;

	float colRange = (float) (mTrajSamples.count + 1);

	RenderColor col1;
	col1.alpha = 1.0f;
	//RenderColor col2;

	Index startIndex = pSync ? pSync->getSyncFlowTrajSampleIndex() : 0;
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

	startIndex = pSync ? pSync->getSyncFlowCollSampleIndex() : 0;
	endIndex = mCollSamples.count;

	for (Index i = startIndex; i < mCollSamples.count && i <= endIndex; ++i) {

		sphere.center = mCollSamples[i].pos;
		sphere.radius = ballRadiusEnd;

		renderer.draw(sphere, &RigidMatrix4x3::kIdentity, &sphereColl);
	}
}

BallSimulTrajectory::BallSimulTrajectory() : mID(0) {

	setEnableFootballerReachableAnalysis(true);
	setTolerance(20.0f);
	setEnableExtraTrajPrecision(false);
	setSimulTickSkipCount(1);
}

void BallSimulTrajectory::loadConfig(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	{
		//chunk.scanAttribute(tempStr, L"logPerformance", logPerformance);

		float tempf;
		bool tempb;

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

void BallSimulTrajectory::setTolerance(float value) {

	mToleranceSq = value * value;
}

void BallSimulTrajectory::setEnableExtraTrajPrecision(bool value) {

	mEnableExtraTrajPrecision = value;
}

void BallSimulTrajectory::setEnableFootballerReachableAnalysis(bool value) {

	mEnableFootballerReachableAnalysis = value;
}

void BallSimulTrajectory::setSimulTickSkipCount(unsigned int value) {

	mSimulTickSkipCount = value;
}


bool BallSimulTrajectorySync::estimateNextBallState(Ball& ball, BallSimulTrajectory::TrajSample& result) {

	const Clock& clock = ball.mNodeMatch->getClock();
	Time nextTime = ball.getSimulStateTime() + clock.getFrameMoveTickLength();

	if ((mSyncBall.ptr() == &ball) && isValid() && (mSyncFlowTrajSampleIndex + 1 < mTrajSamples.count)) {

		if (mSyncFlowTrajSampleIndex + 1 < mTrajSamples.count) {

			const TrajSample& sample = mTrajSamples[mSyncFlowTrajSampleIndex];
			const TrajSample& nextSample = mTrajSamples[mSyncFlowTrajSampleIndex + 1];

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

void BallSimulTrajectorySync::onBallSyncBallState(Ball& ball, const Time& time) {

	bool found;

	found = false;

	for (; mSyncFlowTrajSampleIndex < mTrajSamples.count; ++mSyncFlowTrajSampleIndex) {

		if (mTrajSamples[mSyncFlowTrajSampleIndex].time >= time) {

			found = true;
			break;
		}
	}

	if (found) {

		if (mSyncFlowTrajSampleIndex > 0)
			--mSyncFlowTrajSampleIndex;

	} else {

		mState = State_Expired;
		return;
	}

	if (mEnableFootballerReachableAnalysis) {

		for (; mSyncFlowFootballerReachableIntervalIndex < mFootballerReachableIntervals.getCount(); ++mSyncFlowFootballerReachableIntervalIndex) {

			if (mFootballerReachableIntervals.get(mSyncFlowFootballerReachableIntervalIndex).max >= mSyncFlowTrajSampleIndex) {

				found = true;
				break;
			}
		}

		if (found) {

			if (mSyncFlowFootballerReachableIntervalIndex > 0)
				--mSyncFlowFootballerReachableIntervalIndex;
		}
	}


	found = false;

	for (; mSyncFlowCollSampleIndex < mCollSamples.count; ++mSyncFlowCollSampleIndex) {

		if (mCollSamples[mSyncFlowCollSampleIndex].time >= time) {

			found = true;
			break;
		}
	}

	if (found) { 

		if (mSyncFlowCollSampleIndex > 0)
			--mSyncFlowCollSampleIndex;
	}
}

} }