#include "SceneCamera_ControllerBufferZone.h"

namespace WOF { namespace match { namespace sceneCamera {


Controller_BufferZone::Controller_BufferZone() 
 : mEnable(false) {

	 mOffset.zero();
}

void Controller_BufferZone::getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif) {

	if (!mEnable)
		return;

	targetPosModif.add(mOffset.dist);
}

float Controller_BufferZone::getRadius() {

	return mRadiusIsDirty ? 0.0f : mRadius;
}

void Controller_BufferZone::init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	if (chunk.scanAttribute(tempStr, L"enable", mEnable) == false) {

		mEnable = false;
	}

	if (chunk.scanAttribute(tempStr, L"radius", L"%f", &mRefRadius) == false) {

		mRefRadius = 10.0f;
	}

	if (chunk.scanAttribute(tempStr, L"refScreenHeight", L"%f", &mRefScreenWidth) == false) {

		mRefScreenWidth = 800.0f;
	}

	if (pConv)
		pConv->toTargetUnits(mRefRadius);

	mRadiusIsDirty = true;

	mState = S_Stopped;
	mPreparingState = S_None;
}

void Controller_BufferZone::forceStarted(CamState& state) {

	if (!mEnable)
		return;

	mPreparingState = S_None;

	mState = S_Started;
	mStateStartTime = state.t;
	
	mOffset.zero();
}

void Controller_BufferZone::start(CamState& state) {

	if (!mEnable)
		return;

	if (mState == S_None || mState == S_Stopped) {

		stateSwitch(state.t, S_Started, NULL);
	}
}

void Controller_BufferZone::stop(CamState& state) {

	if (!mEnable)
		return;

	if (mState != S_Stopped) {

		stateSwitch(state.t, S_Stopped, NULL);
	}
}

void Controller_BufferZone::stateHelperMoveTo(CamState& state, const Time& t, const State& nextState, 
															const bool& isNewState, const SpeedProfile::Setup& speedProfileSetup, 
															const Vector3& wantedDistance, const bool& wantedDistanceChanged, 
															bool& recurse, CoordSysConv* pConv) {

	/*
	if (isNewState || wantedDistanceChanged) {

		mSpeedProfile.setup(speedProfileSetup, pConv, wantedDistance.mag(), speedProfileSetup.time);

		mSpeed = 0.0f;
	}

	mSpeed = mSpeedProfile.calcSpeed(mStateStartTime, t, state.updateDt, mSpeed);
	
	if (mDistance.moveTo(wantedDistance, wantedDistance.mag(), mSpeed * state.updateDt)) {

		if (nextState != S_None) {
		
			stateSwitch(t, nextState, &recurse);
		}
	}
	*/
}

void Controller_BufferZone::stateSwitch(const Time& t, const State& newState, bool* pRecurse) {

	mState = newState;
	mStateStartTime = t;

	if (pRecurse)
		*pRecurse = true;
}

bool Controller_BufferZone::statePrepare() {

	bool ret = (mState != mPreparingState);

	mPreparingState = mState;

	return ret;
}

void Controller_BufferZone::update(CamState& state) {

	if (!mEnable)
		return;

	if (state.resolutionChangedFlag || mRadiusIsDirty) {

		float scale = (float) state.resWidth / mRefScreenWidth;

		mRadius = mRefRadius * scale;
		//mSpeedSetup.speed *= scale;

		mRadiusIsDirty = false;
	}

	bool recurse = true;
	
	while (recurse) {

		recurse = false;

		bool isNewState = statePrepare();

		switch (mState) {

			//S_Stopped
			default: {

				if (isNewState)
					mOffset.zero();

			} break;

			case S_Started: {

				if (isNewState) {

					mOffset.zero();
				} 

				Distance trackedOffset;

				state.track.pos.currPos.subtract(state.track.lookAtBasePos.currPos, trackedOffset.dist);
				trackedOffset.update();

				if (trackedOffset.mag > mRadius) {

				} else {

					trackedOffset.dist.negate(mOffset.dist);
					mOffset.update();
				}

			} break;
		}
	}
}


} } }