#include "SceneCamera_ControllerSmoother.h"

#include "../../scene/SceneNode.h"
#include "../../Match.h"
#include "SceneCamera.h"

namespace WOF { namespace match { namespace sceneCamera {


Controller_Smoother::Controller_Smoother() 
 : mEnable(false) {

	 mOffset.zero();

	mTimeTolerance = 0.15f;
	mDynamicFixPoint = false;
	mOffsetCancelSpeed = 1.0f;
	mDistTolerance = 0.0f;
}

void Controller_Smoother::getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif) {

	if (!mEnable)
		return;

	targetPosModif.add(mOffset);
}

void Controller_Smoother::getPostModifs(Vector3& postModif) {

	if (!mEnable)
		return;
}


void Controller_Smoother::init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	if (chunk.scanAttribute(tempStr, L"enable", mEnable) == false) {

		mEnable = false;
	}

	mTimeTolerance = 0.15f;

	chunk.scanAttribute(tempStr, L"timeTolerance", L"%f", &mTimeTolerance);

	mDynamicFixPoint = false;

	chunk.scanAttribute(tempStr, L"dynamicFixPoint", mDynamicFixPoint);

	mDistTolerance = 0.0f;

	if (chunk.scanAttribute(tempStr, L"distanceTolerance", L"%f", &mDistTolerance)) {

		if (pConv) {

			pConv->toTargetUnits(mDistTolerance);
		}
	}

	mOffsetCancelSpeed = 1.0f;

	if (chunk.scanAttribute(tempStr, L"moveSpeed", L"%f", &mOffsetCancelSpeed)) {

		if (pConv) {

			pConv->toTargetUnits(mOffsetCancelSpeed);
		}
	}


	SoftRef<DataChunk> childChunk = chunk.getFirstChild();

	while(childChunk.isValid()) {

		toNextSibling(childChunk);
	}

	mState = S_Stopped;
	mPreparingState = S_None;
}

void Controller_Smoother::forceStarted(CamState& state) {

	if (!mEnable)
		return;

	mPreparingState = S_None;

	mState = S_Started;
	mStateStartTime = state.t;
	
	mOffset.zero();
}

void Controller_Smoother::start(CamState& state) {

	if (!mEnable)
		return;

	if (mState == S_None || mState == S_Stopped) {

		stateSwitch(state.t, S_Started, NULL);
	}
}

void Controller_Smoother::stop(CamState& state) {

	if (!mEnable)
		return;

	if (mState != S_Stopped) {

		stateSwitch(state.t, S_Stopped, NULL);
	}
}


void Controller_Smoother::stateSwitch(const Time& t, const State& newState, bool* pRecurse) {

	mState = newState;
	mStateStartTime = t;

	if (pRecurse)
		*pRecurse = true;
}

bool Controller_Smoother::statePrepare() {

	bool ret = (mState != mPreparingState);

	mPreparingState = mState;

	return ret;
}

void Controller_Smoother::update(CamState& state) {

	if (!mEnable)
		return;

	bool recurse = true;
	
	while (recurse) {

		recurse = false;

		bool isNewState = statePrepare();

		switch (mState) {

			//S_Stopped
			default: {

				if (isNewState) {

					mOffset.zero();
					mLastSpeedDir.zero();
					mHasFixedPoint = false;
				}

			} break;

			case S_Started: {

				if (isNewState) {

					mOffset.zero();
					mLastSpeedDir = state.camera.target.speedDir;

					mHasFixedPoint = false;
		
				} else {

					/*
						There is no ideal way to handle this without having more information
						because a teleport is only perceived as such after the teleport ended (new position reached, and then a stop ...).
						if it was a shot in the same dir as teleport and contined with current speed
						it would be ok, but the fact the the speed is broken is the problem, but it is 
						broken only after the teleport happens .. so unless we know this is a teleport
						we have no way of differentiating it from normal shot
					*/

					CamState::CameraPoint& camPoint = state.track.pos;

					const Vector3& newSpeedDir = camPoint.speedDir;
					float dot = newSpeedDir.dot(mLastSpeedDir);

					bool wasOffset = false;
					bool needsFixedPoint = !mLastSpeedDir.isZero() && !newSpeedDir.isZero() && dot <= 0.80f;
					

					if (mDistTolerance > 0.0f) {

						needsFixedPoint == needsFixedPoint && (distanceSq(camPoint.lastPos, camPoint.currPos) >= mDistTolerance * mDistTolerance);
					}

					if (needsFixedPoint || (mHasFixedPoint && (state.t - mFixedPointTime < mTimeTolerance))) {

						wasOffset = true;
						bool includeResidual = false;

						if (!mHasFixedPoint) {

							mHasFixedPoint = true;
							mFixedPointTime = state.t;
							camPoint.lastPos.add(mOffset, mFixedPoint);
						}

						if (needsFixedPoint) {

							mFixedPointTime = state.t;
						} 

						Vector3 residual;

						mFixedPoint.subtract(camPoint.currPos, mOffset);
					}

					mLastSpeedDir = camPoint.speedDir;

					if (!wasOffset) {

						mHasFixedPoint = false;
					}

					if (!mHasFixedPoint || mDynamicFixPoint) {

						mOffset.substractNoReverse(state.updateDt * mOffsetCancelSpeed);
					}
				}

			} break;
		}
	}
}


} } }