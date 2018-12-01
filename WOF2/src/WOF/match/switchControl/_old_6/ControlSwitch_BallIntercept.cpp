#include "../entity/footballer/Footballer.h"
#include "../entity/ball/Ball.h"
#include "../Player.h"

#include "ControlSwitch_BallIntercept.h"

#include "../Match.h"
#include "../FootballerIteratorImpl.h"
#include "../entity/ball/BallCommand.h"
#include "../spatialQuery/FootballerBallInterceptManager.h"

namespace WOF { namespace match {

ControlSwitch_BallIntercept::ControlSwitch_BallIntercept() {

	mIsValid =  false;
	mCurrSimulID = -1;
}

void ControlSwitch_BallIntercept::init(Match& match) {
}


bool ControlSwitch_BallIntercept::update(Match& match, const Time& time, FootballerBallInterceptManager& manager, bool& consideredFootballersUpdated) {

	if (mFootballerIteratorIsDirty) {

		FootballerIndex footballerCount = mFootballerIterator->getFootballerCount();
		mFootballerIteratorIsDirty = false;
	}

	if (!mFootballerIterator.isValid()) {

		mIsValid = false;
		return mIsValid;
	}

	SoftPtr<BallSimulation> simul = manager.getValidSimul();
	
	if (!simul.isValid()) {

		mIsValid = false;
		return mIsValid;
	}

	mIsValid = true;

	const BallCommand& simulCommand = simul->getCommand();

	SoftPtr<Object> commandOwner;
	float commandOwnerScoreShift = 0.0f;
	
	if (simulCommand.isValid()) {

		if (simulCommand.source.isValid()) {

			commandOwner = simulCommand.source;

			if (simulCommand.type == BallCommand_Vel)
				commandOwnerScoreShift = simulCommand.vel.switchControlInfluence;
			else 
				commandOwnerScoreShift = 0.0f;

			commandOwnerScoreShift *= match.mSwitchControlSetup.switchInfluenceTimeMultiplier;
		} 
	}

	Ball& ball = match.getBall();

	if (mCurrSimulID != simul->getID()) {

		mCurrSimulID = simul->getID();

		mIgnoredFootballers.reset();
		mConsideredFootballers.reset();

		FootballerIterator& iter = match.getActiveFootballerIterator();
		SoftPtr<Footballer> footballer;
		FootballerIndex index;
		
		while (iter.iterateFootballers(footballer.ptrRef(), index)) {

			BallInterceptEstimate& estimate = footballer->getBallInterceptEstimate();

			estimate.updatePath(ball, footballer, time, simul, true, false, false);
			
			bool consider = (estimate.getPathType() == BallInterceptEstimate::Estimate_SimulInterceptorWait);

			if (consider) {

				FootballerIndex index = mConsideredFootballers.count;
				mConsideredFootballers.addOne(footballer);
				sortConsideredFootballer(index, commandOwner, commandOwnerScoreShift);

			} else {

				mIgnoredFootballers.addOne(footballer);
			}
		}

		consideredFootballersUpdated = true;

	} else if (mConsideredFootballers.count > 0) {
		
		bool doFullUpdate = false;
		
		//String::debug(L"considered %u\n", mConsideredFootballers.count);

		for (FootballerIndex i = 0; i < mConsideredFootballers.count; ++i) {

			SoftPtr<Footballer> footballer = mConsideredFootballers[i];
			SoftPtr<Player> player = footballer->getPlayer();

			if (player.isValid()) {

				if (match.mSwitchControlSetup.veryDynamic) {

					{

						BallInterceptEstimate& estimate = footballer->getBallInterceptEstimate();

						if (estimate.isPathValid()) {

							bool changed = estimate.updatePath(ball, footballer, time, simul, true, false, false) != BallInterceptEstimate::Update_NoChange;
							bool consider = (estimate.getPathType() == BallInterceptEstimate::Estimate_SimulInterceptorWait);
							
							if (!consider || (match.mSwitchControlSetup.veryDynamic && changed)) {

								//String::debug(L"unconsider %u\n", footballer->mNumber);

								doFullUpdate = true;
								break;
							}

						} else {

							doFullUpdate = true;
							break;
						}
					}
				}

			} else {

				if (match.mSwitchControlSetup.veryDynamic) {

					BallInterceptEstimate& estimate = footballer->getBallInterceptEstimate();

					if (estimate.isPathValid()) {

						bool changed = estimate.updatePath(ball, footballer, time, simul, true, false, false) != BallInterceptEstimate::Update_NoChange;
						bool consider = (estimate.getPathType() == BallInterceptEstimate::Estimate_SimulInterceptorWait);
						
						if (!consider || changed) {

							//String::debug(L"unconsider %u\n", footballer->mNumber);

							doFullUpdate = true;
							break;
						}

					} else {

						doFullUpdate = true;
						break;
					}
				}
			}
		}

		if (doFullUpdate) {

			//String::debug(L"fullUpdate\n");

			// now this could be split into multiple frames for performance

			consideredFootballersUpdated = true;

			mTempConsideredFootballers.reset();

			for (FootballerIndex i = 0; i < mConsideredFootballers.count; ++i) {

				mTempConsideredFootballers.addOne(mConsideredFootballers[i]);
			}

			mConsideredFootballers.reset();

			for (FootballerIndex i = 0; i < mTempConsideredFootballers.count; ++i) {

				SoftPtr<Footballer> footballer = mTempConsideredFootballers[i];
				BallInterceptEstimate& estimate = footballer->getBallInterceptEstimate();

				bool consider;

				if (estimate.isPathValid()) {

					estimate.updatePath(ball, footballer, time, simul, true, false, false);
					consider = (estimate.getPathType() == BallInterceptEstimate::Estimate_SimulInterceptorWait);

					
				} else {

					consider = false;
				}

				if (consider) {

					FootballerIndex index = mConsideredFootballers.count;
					mConsideredFootballers.addOne(footballer);
					sortConsideredFootballer(index, commandOwner, commandOwnerScoreShift);

				} else {

					mIgnoredFootballers.addOne(footballer);
				}
			}
		}
	}
	
	return mIsValid;
}


void ControlSwitch_BallIntercept::sortConsideredFootballer(const FootballerIndex& footballerIndex, Object* pCommandOwner, float commandOwnerScoreShift) {

	FootballerIndex count = mConsideredFootballers.count;
	Footballer& footballer = dref(mConsideredFootballers[footballerIndex]);
	const BallInterceptEstimate& state = footballer.getBallInterceptEstimate();
	FootballerIndex currFootballerIndex = footballerIndex;

	float footballerScoreShift = (pCommandOwner == &footballer) ? commandOwnerScoreShift : 0.0f;

	bool moved = false;

	for (FootballerIndex i = footballerIndex + 1; i < count; ++i) {

		SoftPtr<Footballer> compFootballer = mConsideredFootballers[i];
		const BallInterceptEstimate& compState = compFootballer->getBallInterceptEstimate();

		float compScoreShift = (pCommandOwner == compFootballer) ? commandOwnerScoreShift : 0.0f;

		float scoreDiff = ((compState.getInterceptTime() - compScoreShift) - (state.getInterceptTime() - footballerScoreShift));

		if (scoreDiff < 0.0f) {

			moved = true;

			mConsideredFootballers[i] = &footballer;
			mConsideredFootballers[currFootballerIndex] = compFootballer;

			currFootballerIndex = i;

		} else {

			break;
		}
	}

	if (moved)
		return;

	for (FootballerIndex i = footballerIndex - 1, j = 0; j < footballerIndex; --i, ++j) {

		SoftPtr<Footballer> compFootballer = mConsideredFootballers[i];
		const BallInterceptEstimate& compState = compFootballer->getBallInterceptEstimate();
		
		float compScoreShift = (pCommandOwner == compFootballer) ? commandOwnerScoreShift : 0.0f;

		float scoreDiff = ((compState.getInterceptTime() - compScoreShift) - (state.getInterceptTime() - footballerScoreShift));

		if (scoreDiff > 0.0f) {

			moved = true;

			mConsideredFootballers[i] = &footballer;
			mConsideredFootballers[currFootballerIndex] = compFootballer;

			currFootballerIndex = i;

		} else {

			break;
		}
	}
}

} }