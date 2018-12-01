#include "SceneCamera_ControllerPointTrajectory.h"

#include "WE3/helper/load/WEMathDataLoadHelper.h"
#include "WE3/WEMem.h"
using namespace WE;

namespace WOF { namespace match { namespace sceneCamera {


Controller_PointTrajectory::Controller_PointTrajectory() 
 : mEnable(false), mControlTarget(CT_None) {

	 mDistance.zero();

	mStartDelay = 0.0f;
	mStopDelay = 0.0f;
	mChangeDelay = 0.0f;
	mRestartDelay = 0.0f;
}

void Controller_PointTrajectory::getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif) {

	if (!mEnable)
		return;

	switch(mControlTarget) {

		case CT_Eye: {

			eyePosModif.add(mDistance.dist);

		} break;

		case CT_Target: {

			targetPosModif.add(mDistance.dist);

		} break;

		case CT_TargetOffset: {

			targetOffsetModif.add(mDistance.dist);

		} break;

		case CT_Offset: {

			eyePosModif.add(mDistance.dist);
			targetOffsetModif.add(mDistance.dist);

		} break;
	}
}

void Controller_PointTrajectory::init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	if (chunk.scanAttribute(tempStr, L"enable", mEnable) == false) {

		mEnable = false;
	}

	SoftRef<DataChunk> child;

	child = chunk.getChild(L"startProfile");
	mStartSpeedSetup.init(tempStr, child, pConv);

	child = chunk.getChild(L"changeProfile");
	if (child.isValid()) {

		mChangeSpeedSetup.init(tempStr, child, pConv);

	} else {

		mChangeSpeedSetup = mStartSpeedSetup;
	}

	child = chunk.getChild(L"stopProfile");
	if (child.isValid()) {
		
		mStopSpeedSetup.init(tempStr, child, pConv);
	
	} else {

		mStopSpeedSetup = mStartSpeedSetup;
	}


	if (chunk.hasAttribute(L"offset")) {

		Vector3 distance;

		if (MathDataLoadHelper::extract(tempStr, chunk, distance, true, false, false, L"offset")) {

			if (pConv)
				pConv->toTargetPoint(distance.el);

			SoftRef<ControllerProvider_Distance_Impl> provider;

			MMemNew(provider.ptrRef(), ControllerProvider_Distance_Impl());
			provider->setDistance(distance);

			setProvider(provider.ptr());

		} else {

			assrt(false);
		}
	}

	if (chunk.getAttribute(L"controlTarget", tempStr)) {

		if (tempStr.equals(L"eye")) {

			setControlTarget(CT_Eye);

		} else if (tempStr.equals(L"target")) {

			setControlTarget(CT_Target);

		} else if (tempStr.equals(L"targetOffset")) {

			setControlTarget(CT_TargetOffset);

		} else if (tempStr.equals(L"offset")) {

			setControlTarget(CT_Offset);

		} else {

			assrt(false);
		}
	}

	mState = S_Stopped;
	mPreparingState = S_None;
}

bool Controller_PointTrajectory::setProvider(ControllerProvider_Distance* pProvider) {

	mProvider = pProvider;

	return true;
}

void Controller_PointTrajectory::setControlTarget(const ControlTarget& target) {

	mControlTarget = target;
}

void Controller_PointTrajectory::forceStarted(CamState& state) {

	if (!mEnable)
		return;

	mPreparingState = S_None;

	mState = S_DelayingStart;
	mStateStartTime = state.t;
	
	mDistance.zero();
}

void Controller_PointTrajectory::start(CamState& state) {

	if (!mEnable)
		return;

	switch(mState) {

		case S_None:
		case S_Stopped: {

			stateSwitch(state.t, S_DelayingStart, NULL);

		} break;

		case S_DelayingRestart: 
		case S_Starting: 
		case S_Started: 
		case S_DelayingChange: 
		case S_Changing: {
		

		} break;

		case S_DelayingStop: {

			stateSwitch(state.t, S_DelayingStart, NULL);

		} break;

		case S_Stopping: {

			stateSwitch(state.t, S_DelayingStart, NULL);

		} break;

		default : {

			assrt(false);

		} break;
	}
}

void Controller_PointTrajectory::stop(CamState& state) {

	if (!mEnable)
		return;


	switch(mState) {

		case S_None:
		case S_Stopped: 
		case S_DelayingStop: 
		case S_Stopping: {

		} break;

		case S_DelayingRestart:
		case S_DelayingStart: 
		case S_DelayingChange: {

			stateSwitch(state.t, S_DelayingStop, NULL);

		} break;

		case S_Starting: 
		case S_Started: 
		case S_Changing: {

			stateSwitch(state.t, S_DelayingStop, NULL);

		} break;
	}
}

void Controller_PointTrajectory::stateHelperDelay(const Time& t, const Time& delay, const State& nextState, bool& recurse) {

	State newState = S_None;

	if (delay != 0.0f) {

		if (t - mStateStartTime >= delay)
			newState = nextState;

	} else {

		newState = nextState;
	}

	if (newState != mState) {

		stateSwitch(t, newState, &recurse);
	}
}

void Controller_PointTrajectory::stateHelperMoveTo(CamState& state, const Time& t, const State& nextState, 
															const bool& isNewState, const SpeedProfile::Setup& speedProfileSetup, 
															const Vector3& wantedDistance, const bool& wantedDistanceChanged, 
															bool& recurse, CoordSysConv* pConv) {

	if (isNewState || wantedDistanceChanged) {

		Vector3 diff;
		
		mDistance.dist.subtract(wantedDistance, diff);

		float mag = diff.mag();

		if (pConv)
			pConv->fromTargetUnits(mag);

		mSpeedProfile.setup(speedProfileSetup, pConv, mag, speedProfileSetup.time);

		mSpeed = 0.0f;
	}

	mSpeed = mSpeedProfile.calcSpeed(mStateStartTime, t, state.updateDt, mSpeed);
	
	if (mDistance.moveTo(wantedDistance, wantedDistance.mag(), mSpeed * state.updateDt)) {

		if (nextState != S_None) {
		
			stateSwitch(t, nextState, &recurse);
		}
	}
}

void Controller_PointTrajectory::stateSwitch(const Time& t, const State& newState, bool* pRecurse) {

	mState = newState;
	mStateStartTime = t;

	if (pRecurse)
		*pRecurse = true;
}

bool Controller_PointTrajectory::statePrepare() {

	bool ret = (mState != mPreparingState);

	mPreparingState = mState;

	return ret;
}

void Controller_PointTrajectory::update(CamState& state) {

	if (!mEnable)
		return;

	bool recurse = true;
	
	while (recurse && mProvider.isValid()) {

		recurse = false;

		bool isNewState = statePrepare();

		switch (mState) {

			//S_Stopped
			default: {

			} break;

			case S_DelayingRestart: {

				stateHelperDelay(state.t, mRestartDelay, S_Starting, recurse);

			} break;
			
			case S_DelayingStart: {

				stateHelperDelay(state.t, mStartDelay, S_Starting, recurse);

			} break;

			case S_Starting: {

				Vector3 wantedDistance;
				bool wantedDistanceChanged;

				wantedDistanceChanged = mProvider->provideDistance(state, wantedDistance);

				stateHelperMoveTo(state, state.t, S_Started, isNewState, mStartSpeedSetup, 
									wantedDistance, wantedDistanceChanged, 
									recurse, state.coordSysConv);

			} break;

			case S_Started: {

				Vector3 wantedDistance;
				bool wantedDistanceChanged;

				wantedDistanceChanged = mProvider->provideDistance(state, wantedDistance);

				if (wantedDistanceChanged) {

					stateSwitch(state.t, S_DelayingChange, &recurse);

				} else {

					//no change
				}
				
			} break;

			case S_DelayingChange: {

				stateHelperDelay(state.t, mChangeDelay, S_Changing, recurse);

			} break;

			case S_Changing: {

				Vector3 wantedDistance;
				bool wantedDistanceChanged;

				wantedDistanceChanged = mProvider->provideDistance(state, wantedDistance);

				stateHelperMoveTo(state, state.t, S_Started, isNewState, mChangeSpeedSetup, 
									wantedDistance, wantedDistanceChanged, 
									recurse, state.coordSysConv);

			} break;

			case S_DelayingStop: {

				stateHelperDelay(state.t, mStopDelay, S_Stopping, recurse);

			} break;

			case S_Stopping: {

				stateHelperMoveTo(state, state.t, S_Stopped, isNewState, mStopSpeedSetup, 
									Vector3::kZero, false, 
									recurse, state.coordSysConv);

			} break;

		}
	}
}


} } }