#include "../Ball.h"
#include "../../../Match.h"

#include "BallSimulation.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallSimulation::TrajSample, CBallTrajSample);

namespace WOF { namespace match {

const TCHAR* BallSimulation::TrajSample::ScriptClassName = L"CBallTrajSample";

void BallSimulation::TrajSample::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<TrajSample>(TrajSample::ScriptClassName).
		func(&TrajSample::script_isOnPitch, L"isOnPitch").
		func(&TrajSample::script_getPos, L"getPos").
		func(&TrajSample::script_getVel, L"getVel");
}

void BallSimulation::Sample::set(const Time& _time, const Point& _pos, const Vector3& _vel) {

	time = _time;
	pos = _pos;
	vel = _vel;
}

void BallSimulation::TrajSample::set(const Time& time, const Point& pos, const Vector3& vel, const bool& _isOnPitch) {

	Sample::set(time, pos, vel);
	isOnPitch = _isOnPitch;
}

bool BallSimulation::TrajSample::isBallFootballerReachable(Match& match, Ball& ball) {

	return ball.getPos().el[Scene_Up] - ball.getRadius() <= match.mSwitchControlSetup.footballerReachHeight; 
}

bool BallSimulation::TrajSample::isFootballerReachable(Match& match, const float& ballRadius) const { 
	
	return pos.el[Scene_Up] - ballRadius <= match.mSwitchControlSetup.footballerReachHeight; 
}

void BallSimulation::addSweptBallContacts(Ball& ball, WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials) {

	ball.mCollRegistry.addSweptBallContacts(matManager, sphereMat, radius, pos, sweepVector, intersections, pSimul2, executeMaterials);
}


void BallSimulation::onBallSyncResetSimul(Ball& ball, const BallCommand* pCommand) {

	reset();

	if (pCommand)
		mBallCommand = *pCommand;
	else 
		mBallCommand.invalidate();
}

void BallSimulation::onBallSyncPostCollision(Object& object) {

	if (objectIsDynamic(object))
		reset();
}

bool BallSimulation::estimateNextBallState(Ball& ball, TrajSample& result) {

	const Clock& clock = ball.mNodeMatch->getClock();
	Time nextTime = ball.getSimulStateTime() + clock.getFrameMoveTickLength();

	if ((mSyncBall.ptr() == &ball) && isValidSync() && (mSyncFlowTrajSampleIndex + 1 < mTrajSamples.count)) {

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

void BallSimulation::onBallSyncBallState(Ball& ball, const Time& time) {

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

bool BallSimulation::ignoreCollision(Object* pObject) {

	return objectIsDynamic(dref(pObject));
}

bool BallSimulation::signalPreCollision(Object* pObject, const Time& time, Ball& ball) {

	CollSample& sample = mCollSamples.addOne();

	sample.set(time, ball.getPhysBody().pos, ball.getPhysBody().v);
	sample.collider = pObject;

	return true;
}

bool BallSimulation::signalPostCollision(Object* pObject, const Time& time, Ball& ball) {

	CollSample& sample = mCollSamples[mCollSamples.lastIndex()];
	sample.postVel = ball.getPhysBody().v;

	mCollBetweenSamples = true;

	return true;
}

bool BallSimulation::signalSimulTickEnd(const Time& time, const unsigned int& simulTickIndex, Ball& ball) {

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

		if (mSimulEndAtRest && ball.isResting()) {

			return false;
		}
	}

	return true;
}

void BallSimulation::render(Renderer& renderer, bool useSyncFlow, 
							 const float* pBallRadiusStart, const float* pBallRadiusEnd, 
							 const RenderColor* pLineStart, const RenderColor* pLineEnd, 
							 const RenderColor* pSphereStart, const RenderColor* pSphereEnd,
							 const RenderColor* pSphereColl) {

	if (pBallRadiusStart == NULL && !mSyncBall.isValid()) {

		//assrt(false);
		return;
	}

	if (useSyncFlow && (mState != State_Syncing && mState != State_Synced)) {

		useSyncFlow = false;
	}

	const float& ballRadiusStart = pBallRadiusStart ? *pBallRadiusStart : mSyncBall->getRadius() * 0.15f;
	const float& ballRadiusEnd = pBallRadiusEnd ? *pBallRadiusEnd : mSyncBall->getRadius() * 0.95f;
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

	Index startIndex = useSyncFlow ? mSyncFlowTrajSampleIndex : 0;
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

	startIndex = useSyncFlow ? mSyncFlowCollSampleIndex : 0;
	endIndex = mCollSamples.count;

	for (Index i = startIndex; i < mCollSamples.count && i <= endIndex; ++i) {

		sphere.center = mCollSamples[i].pos;
		sphere.radius = ballRadiusEnd;

		renderer.draw(sphere, &RigidMatrix4x3::kIdentity, &sphereColl);
	}
}

BallSimulation::BallSimulation() : mState(State_None), mID(0) {

	setEnableFootballerReachableAnalysis(true);
	setSimulEndAtRest(true);
	setEnableEarlySimulEnd(true);

	setSimulMaxFrameCount(1);
	setSimulMaxFrameDuration(10.0f);
	setSimulFrameDuration(2.5f);
	setSimulTickSkipCount(1);
	setTolerance(20.0f);
	setEnableExtraTrajPrecision(false);

	/*
	assrt(false);
	//use loadConfig!!

	setSimulMaxFrameCount(3);
	setSimulMaxFrameDuration(10.0f);
	setSimulFrameDuration(2.5f);
	setSimulTickSkipCount(3);
	setTolerance(28.0f);
	setEnableExtraTrajPrecision(true);
	*/
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

void BallSimulation::setSimulMaxFrameCount(unsigned int value) {

	mSimulSimulMaxFrameCount = value;
}

void BallSimulation::setEnableEarlySimulEnd(bool value) {

	mEnableEarlySimulEnd = value;
}

void BallSimulation::setSimulMaxFrameDuration(float value) {

	mSimulMaxFrameDuration = value;
}

void BallSimulation::setSimulFrameDuration(float value) {

	mSimulFrameDuration = value;
}

void BallSimulation::setSimulTickSkipCount(unsigned int value) {

	mSimulTickSkipCount = value;
}


void BallSimulation::setSimulEndAtRest(bool value) {

	mSimulEndAtRest = value;
}

void BallSimulation::setTolerance(float value) {

	mToleranceSq = value * value;
}

void BallSimulation::setEnableExtraTrajPrecision(bool value) {

	mEnableExtraTrajPrecision = value;
}

void BallSimulation::setEnableFootballerReachableAnalysis(bool value) {

	mEnableFootballerReachableAnalysis = value;
}

void BallSimulation::reset() {

	mState = State_None;

	stopSyncBall();
}

BallSimulation::State BallSimulation::continueSimulProcessing(Ball& ball, bool restart, Time simulStartTime) {

	if (restart) {

		mTrajSamples.count = 0;
		mCollSamples.count = 0;
		mFootballerReachableIntervals.clear();
		mNextSimulProcessingFrameIndex = 0;
	}

	SoftPtr<BallControllerPhysImpl> phys = ball.getActiveControllerPhys();

	if (ball.isSimulable() && phys.isValid()) {

		Ball& simulBall = ball.mNodeMatch->getSimulBall();

		if (mNextSimulProcessingFrameIndex == 0) {

			mSyncFlowTrajSampleIndex = 0;
			mSyncFlowCollSampleIndex = 0;
			mSyncFlowFootballerReachableIntervalIndex = 0;

			++mID;
			mNextRecordSimulTick = 0;
			mSimulStartTime = simulStartTime;

			mLastSimulSample.time = mSimulStartTime;
			mLastSimulSample.tick = 0;
			mLastSimulSample.isResting = false;

			phys->setPosition(simulBall, ball.getPhysBody().pos, NULL);
			phys->setVelocities(simulBall, ball.getPhysBody().v, ball.getPhysBody().w, NULL); 

		} else {

			phys->setPosition(simulBall, mLastSimulSample.pos, NULL);
			phys->setVelocities(simulBall, mLastSimulSample.vel, mLastSimulSample.w, NULL); 
		}

		if (mLastSimulSample.isResting) {

			if (mEnableEarlySimulEnd) {

				return State_Simulated;

			} else {

				//do nothing
			}

		} else {

			Time simulDuration = (mNextSimulProcessingFrameIndex + 1 == mSimulSimulMaxFrameCount) ? mSimulMaxFrameDuration : mSimulFrameDuration;

			phys->simulate(ball.mNodeMatch, this, simulBall, mLastSimulSample.time, mLastSimulSample.tick, simulDuration, ball.mNodeMatch->getClock().getFramePhysTickLength(), mLastSimulSample.time, mLastSimulSample.tick);

			mLastSimulSample.pos = simulBall.getPhysBody().pos;
			mLastSimulSample.vel = simulBall.getPhysBody().v;
			mLastSimulSample.w = simulBall.getPhysBody().w;
			mLastSimulSample.isResting = simulBall.isResting();
		}

		++mNextSimulProcessingFrameIndex;

		if (mNextSimulProcessingFrameIndex == mSimulSimulMaxFrameCount) {

			return State_Simulated;

		} else {

			return State_Simulating;
		}

	} else {

		//assrt(false);
		return State_None;
	}
}

BallSimulation::State BallSimulation::updateSimulate(Ball& ball, Time simulStartTime) {

	switch (mState) {

		case State_None: {

			mState = continueSimulProcessing(ball, true, simulStartTime);

		} break;

		case State_Simulating: {

			mState = continueSimulProcessing(ball, false, simulStartTime);

		} break;

		case State_Simulated: {
		} break;

		default: {

			assrt(false);

		} break;
	}

	return mState;
}

void BallSimulation::startSyncBall(Ball& ball) {

	mSyncBall = &ball;
	ball.addListener(*this);
}

void BallSimulation::stopSyncBall() {

	if (mSyncBall.isValid()) {

		mSyncBall->removeListener(*this);
		mSyncBall.destroy();
	}
}

BallSimulation::State BallSimulation::updateSync(Ball& ball) {

	switch (mState) {

		case State_None: {

			switch (continueSimulProcessing(ball, true, ball.getSimulStateTime())) {

				case State_None: {

					mState = State_None;

				} break;

				case State_Simulating: {

					startSyncBall(ball);
					mState = State_Syncing;

				} break;

				case State_Simulated: {

					startSyncBall(ball);
					mState = State_Synced;

				} break;
			}

		} break;

		case State_Syncing: {

			switch (continueSimulProcessing(ball, false)) {

				case State_None: {

					reset();

				} break;

				case State_Simulating: {

					mState = State_Syncing;

				} break;

				case State_Simulated: {

					mState = State_Synced;

				} break;
			}

		} break;

		case State_Synced: {
		} break;

		case State_Expired: {
		} break;

		default: {

			assrt(false);

		} break;
	}

	return mState;
}


void BallSimulation_SimulPitch::addSweptBallContacts(Ball& ball, WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials) {

	ball.mNodeMatch->getSimulPitch().addSweptBallContacts(matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);
}	


} }