#include "SpatialMatchState.h"

#include "../entity/footballer/Footballer.h"
#include "../entity/ball/Ball.h"

#include "../Match.h"
#include "../FootballerIteratorImpl.h"
#include "FootballerBallInterceptManager.h"

namespace WOF { namespace match {

SpatialMatchState::BallIntercept::BallIntercept() {

	mIsValid =  false;
	mCurrSimulID = -1;
	mLastChangeTime = 0.0f;
}

void SpatialMatchState::BallIntercept::init(Match& match) {
}

bool SpatialMatchState::BallIntercept::update(Match& match, const Time& time, BallSimulation* pValidSimul, bool& consideredFootballersUpdated) {

	if (mFootballerIteratorIsDirty) {

		FootballerIndex footballerCount = mFootballerIterator->getFootballerCount();
		mFootballerIteratorIsDirty = false;
	}

	if (!mFootballerIterator.isValid()) {

		mIsValid = false;
		return mIsValid;
	}

	SoftPtr<BallSimulation> simul = pValidSimul;
	
	if (!simul.isValid()) {

		mIsValid = false;
		return mIsValid;
	}

	mIsValid = true;
	bool hasUpdates = false;

	Ball& ball = match.getBall();

	if (ball.isResting()) {

		mIsValid = false;
		return mIsValid;
	}

	if (mCurrSimulID != simul->getID()) {

		mLastChangeTime = time; 
		hasUpdates = true;

		mCurrSimulID = simul->getID();

		mDiscardedFootballers.reset();
		mFootballers.reset();

		FootballerIterator& iter = mFootballerIterator.dref();
		SoftPtr<Footballer> footballer;
		FootballerIndex index;
		
		while (iter.iterateFootballers(footballer.ptrRef(), index)) {

			FootballerState& state = footballer->getSpatialState();

			state.updatePath(ball, footballer, time, simul, true, false, false);
			
			switch (state.getPathType()) {

				case BallInterceptEstimate::Estimate_SimulInterceptorWait:
				case BallInterceptEstimate::Estimate_SimulLastBallWait: {

					FootballerIndex index = mFootballers.count;
					mFootballers.addOne(footballer);
					sortInterceptingFootballer(index);

				} break;

				default: {

					state.indexBallIntercept = FootballerIndexInvalid;
					mDiscardedFootballers.addOne(footballer);

				} break;
			}
		}

		consideredFootballersUpdated = true;

	} else if (mFootballers.count > 0) {
		
		bool doFullUpdate = false;
		

		if (simul->isSyncExpired()) {

			if (mFootballers.count) {

				hasUpdates = true;

				for (FootballerIndex i = 0; i < mFootballers.count; ++i) {

					SoftPtr<Footballer> footballer = mTempFootballer[i];
					FootballerState& state = footballer->getSpatialState();

					state.indexBallIntercept = FootballerIndexInvalid;
					mDiscardedFootballers.addOne(footballer);
				}

				mFootballers.reset();
			}

		} else {

			//String::debug(L"considered %u\n", mFootballers.count);

			for (FootballerIndex i = 0; i < mFootballers.count; ++i) {

				SoftPtr<Footballer> footballer = mFootballers[i];

				FootballerState& state = footballer->getSpatialState();

				if (state.isPathValid()) {

					bool changed = state.updatePath(ball, footballer, time, simul, true, false, false) != BallInterceptEstimate::Update_NoChange;
					
					if (changed) {

						doFullUpdate = true;
						break;

					} else {

						switch (state.getPathType()) {

							case BallInterceptEstimate::Estimate_SimulInterceptorWait:
							case BallInterceptEstimate::Estimate_SimulLastBallWait: {
							} break;

							default: {

								doFullUpdate = true;
							}
						}
					}

					if (doFullUpdate) {

						break;
					}

				} else {

					doFullUpdate = true;
					break;
				}
			}
		}

		if (doFullUpdate) {

			hasUpdates = true;
			mLastChangeTime = time;

			//String::debug(L"fullUpdate\n");

			// now this could be split into multiple frames for performance

			consideredFootballersUpdated = true;

			mTempFootballer.reset();

			for (FootballerIndex i = 0; i < mFootballers.count; ++i) {

				mTempFootballer.addOne(mFootballers[i]);
			}

			mFootballers.reset();

			for (FootballerIndex i = 0; i < mTempFootballer.count; ++i) {

				SoftPtr<Footballer> footballer = mTempFootballer[i];
				FootballerState& state = footballer->getSpatialState();

				if (state.isPathValid()) {

					state.updatePath(ball, footballer, time, simul, true, false, false);
					
					switch (state.getPathType()) {

						case BallInterceptEstimate::Estimate_SimulInterceptorWait:
						case BallInterceptEstimate::Estimate_SimulLastBallWait: {

							FootballerIndex index = mFootballers.count;
							mFootballers.addOne(footballer);
							sortInterceptingFootballer(index);

						} break;

						default: {

							state.indexBallIntercept = FootballerIndexInvalid;
							mDiscardedFootballers.addOne(footballer);

						} break;
					}

				} else {

					state.indexBallIntercept = FootballerIndexInvalid;
					mDiscardedFootballers.addOne(footballer);
				}
			}
		}
	}

	if (hasUpdates) {

		mTeamFootballers[Team_A].reset();
		mTeamFootballers[Team_B].reset();

		mTeamFootballers[Team_A].reserve(match.getTeam(Team_A).getFootballerCount());
		mTeamFootballers[Team_B].reserve(match.getTeam(Team_B).getFootballerCount());

		for (FootballerIndex i = 0; i < mFootballers.count; ++i) {

			SoftPtr<Footballer> footballer = mFootballers[i];

			mTeamFootballers[footballer->mTeam].addOne(footballer);
		}
	}
	
	return mIsValid;
}


void SpatialMatchState::BallIntercept::sortInterceptingFootballer(const FootballerIndex& footballerIndex) {

	FootballerIndex count = mFootballers.count;
	Footballer& footballer = dref(mFootballers[footballerIndex]);
	SpatialMatchState::FootballerState& state = footballer.getSpatialState();
	FootballerIndex currFootballerIndex = footballerIndex;

	state.indexBallIntercept = footballerIndex;

	bool moved = false;

	for (FootballerIndex i = footballerIndex + 1; i < count; ++i) {

		SoftPtr<Footballer> compFootballer = mFootballers[i];
		SpatialMatchState::FootballerState& compState = compFootballer->getSpatialState();

		int levelDiff = compState.getPathType() - state.getPathType();
		float scoreDiff;

		if (levelDiff != 0) {

			scoreDiff = (float) levelDiff;

		} else {

			if (compState.getPathType() == BallInterceptEstimate::Estimate_SimulLastBallWait) {

				scoreDiff = ((compState.getPathArrivalTime()) - (state.getPathArrivalTime()));

			} else {

				scoreDiff = ((compState.getPathInterceptTime()) - (state.getPathInterceptTime()));
			}
		}

		if (scoreDiff < 0.0f) {

			moved = true;

			mFootballers[i] = &footballer;
			state.indexBallIntercept = i;
			mFootballers[currFootballerIndex] = compFootballer;
			compState.indexBallIntercept = currFootballerIndex;

			currFootballerIndex = i;

		} else {

			break;
		}
	}

	if (moved)
		return;

	for (FootballerIndex i = footballerIndex - 1, j = 0; j < footballerIndex; --i, ++j) {

		SoftPtr<Footballer> compFootballer = mFootballers[i];
		SpatialMatchState::FootballerState& compState = compFootballer->getSpatialState();
		
		int levelDiff = compState.getPathType() - state.getPathType();
		float scoreDiff;

		if (levelDiff != 0) {

			scoreDiff = (float) levelDiff;

		} else {

			if (compState.getPathType() == BallInterceptEstimate::Estimate_SimulLastBallWait) {

				scoreDiff = ((compState.getPathArrivalTime()) - (state.getPathArrivalTime()));

			} else {

				scoreDiff = ((compState.getPathInterceptTime()) - (state.getPathInterceptTime()));
			}
		}

		if (scoreDiff > 0.0f) {

			moved = true;

			mFootballers[i] = &footballer;
			state.indexBallIntercept = i;
			mFootballers[currFootballerIndex] = compFootballer;
			compState.indexBallIntercept = currFootballerIndex;

			currFootballerIndex = i;

		} else {

			break;
		}
	}
}

} }
