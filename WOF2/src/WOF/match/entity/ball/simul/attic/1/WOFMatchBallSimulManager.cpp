#include "WOFMatchBallSimulManager.h"

/*
#include "WOFMatch.h"
#include "WOFMatchObjectIncludes.h"
*/

namespace WOF { namespace match {

/*
void BallSimulManager::init(Match& match) {

	mMatch = &match;
	mEnabled = false;
	mActive = false;

	mLogPerformance = false;

	mFrameCount = 3;
	mFPS = 30;
	mDurationPerFrame = 1.5f;
	mProcessingFrameCount = 3;

	{
		Simul& simul = mCurrSimul;

		simul.setRestingSpeedTurnAroundCount(1, true);
		simul.setupRestingSpeedTurnAround(mMatch->getCoordSys().convUnit(0.2f)); //13 cm/second is minimum

		simul.addIgnoreType(MNT_Footballer);

		simul.enableEvent(Simul::ET_Discontinuity, true);
	}

	mBallSimulDirty = true;
}

void BallSimulManager::load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	//todo add fction to add end pos to trajectory

	{
		chunk.scanAttribute(tempStr, L"logPerformance", mLogPerformance);

		BallSimulation_Filtered_Trajectory& simul = mCurrSimul;

		float temp;

		temp = 1.0f;
		if (pConv)
			pConv->toTargetUnits(temp);

		if (chunk.scanAttribute(tempStr, L"sampleDistance", L"%f", &temp)) {

			if (pConv) {
				pConv->toTargetUnits(temp);
			}
		}

		simul.setSampleDistance(temp, true);
	
		chunk.scanAttribute(tempStr, L"durationPerFrame", L"%f", &mDurationPerFrame);
		chunk.scanAttribute(tempStr, L"frameCount", L"%d", &mFrameCount);
		
		if (!chunk.scanAttribute(tempStr, L"FPS", L"%u", &mFPS)) {

			mFPS = (TickCount) mMatch->getTime().mTime.ticksPerSec;
		}

		assrt(((int) mMatch->getTime().mTime.ticksPerSec) > 0);

		mProcessingFrameCount = mFrameCount * (int) ((float) mFPS / (float) mMatch->getTime().mTime.ticksPerSec);

		assrt(mProcessingFrameCount > 0);

		if (mProcessingFrameCount > (int) mFPS) {

			assrt(false);
			mProcessingFrameCount = mFPS;
		}
	}
}

void BallSimulManager::enable(bool enabled) {

	mEnabled = enabled;
}

BallSimulManager::Simul* BallSimulManager::getCurrSimul() {

	if (mEnabled && mActive) {

		return mCurrSimul.isValid() ? &mCurrSimul : NULL;
	}

	return NULL;
}

void BallSimulManager::onBallSwitchController(Ball& ball) {

	invalidateSimul();
	onBallOwnershipChange(ball);
}

void BallSimulManager::onBallCommand(Ball& ball) {

	mBallSimulDirty = true;

	invalidateSimul();
}

void BallSimulManager::onBallOwnershipChange(Ball& ball) {

	mActive = (mMatch->getBall().getOwnerCount() == 0);

	if (!mActive)
		invalidateSimul();
}

void BallSimulManager::invalidateSimul() {

	mCurrSimul.setValid(false);
}

void BallSimulManager::reSimul(bool newSimul) {

	Ball& ball = mMatch->getBall();
	BallControllerPhysImpl* pPhysImpl = ball.getControllerPhys();

	mCurrSimul.setValid(true);

	if (pPhysImpl) {

		static int ct = 0;
		DWORD time;

		if (mLogPerformance) {

			time = timeGetTime();
		}

		Simul& simul = mCurrSimul;
		const Timing& timing = mMatch->getTime();

		if (newSimul) {

			if (ball.isResting()) {

				setEmptySimul(true);

			} else {

				simul.resetNew(timing.getPhysTime());

				if (mProcessingFrameCount <= 1) {
					
					simul.setDuration(20.0f);

				} else {

					mCurrSimul.mPartID = 0;
					mCurrSimul.mIsPartial = true;
					simul.setDuration(mDurationPerFrame);
				}

				const PhysRigidBody& ballPhys = ball.getPhysBody();

				pPhysImpl->simulate(mMatch, timing, simul, timing.getPhysTime(), ballPhys.pos, ballPhys.v, ballPhys.w);

				++mCurrSimul.mID;
			}

		} else {

			if (mCurrSimul.mIsPartial == true) {

				BallSimulation_Filtered::EndReason endReason = simul.getEndReason();

				simul.resetContinue();

				const BallSimulation_Filtered::BallState& ballState = simul.getEndState();

				switch (endReason) {
					case BallSimulation_Filtered::ER_Distance: 
					case BallSimulation_Filtered::ER_Duration: {

						if ((int) mCurrSimul.mPartID + 1 >= mProcessingFrameCount) {

							simul.setDuration(20.0f);
							
						} else {
							
							simul.setDuration(mDurationPerFrame);
						}

						pPhysImpl->simulate(mMatch, timing, simul, ballState.time, ballState.pos, ballState.v, ballState.w);

					} break;

					default: {
					} break;
				}

				mCurrSimul.mPartID++;
			}

			if (mProcessingFrameCount <= 1) {

				mCurrSimul.mIsPartial = false;

			} else {

				if ((int) mCurrSimul.mPartID >= mProcessingFrameCount) {

					mCurrSimul.mIsPartial = false;

				} else {

					mCurrSimul.mIsPartial = true;
				}
			}

					

			if (mLogPerformance) {

				time = timeGetTime() - time;

				if (mCurrSimul.mIsPartial) {

					log(L"ballsim%d: part%d %d ms. %u samples", ct, mCurrSimul.mPartID, (int) time, mCurrSimul.getSampleCount());

					#ifdef _DEBUG
					String::debug(L"ballsim%d: part%d %d ms. %u samples\n", ct, mCurrSimul.mPartID, (int) time, mCurrSimul.getSampleCount());
					#endif

				} else {

					log(L"ballsim%d: %d ms. %u samples", ct, (int) time, mCurrSimul.getSampleCount());

					#ifdef _DEBUG
					String::debug(L"ballsim%d: %d ms. %u samples\n", ct, (int) time, mCurrSimul.getSampleCount());
					#endif
				}

				ct++;
			}
		}
	}
}

void BallSimulManager::setEmptySimul(bool valid) {

	mCurrSimul.mIsValid = valid;
			
	Simul& simul = mCurrSimul;

	if (simul.getSampleCount() != 0) {

		simul.resetNew(mMatch->getTime().getPhysTime());

		mCurrSimul.mIsPartial = false;
		mCurrSimul.mID++;
	}
}

bool BallSimulManager::simulIsDesynced(Match& match, const DiscreeteTime& time) {

	//make a better estimate comparing ball pos to trajectory

	if (mCurrSimul.mIsValid) {

		BallSimulation_Filtered_Trajectory& simul = mCurrSimul;
		const BallSimulation_Filtered::BallState& ballState = simul.getEndState();
		Ball& ball = mMatch->getBall();

		if (ball.isResting()) {

			return false;
		}

		if (time.t_discreet - ballState.time > 0.1f && (simul.getSampleCount() == 0))
			return true;

		return false;
	} 

	return true;
}

void BallSimulManager::frameMove(Match& match, const DiscreeteTime& time) {

	if (mEnabled && mActive) {

		if (mMatch->getBall().isSimulable()) {

			enum SimulAction {

				SA_None = 0, SA_New, SA_Continue
			};

			SimulAction action = SA_None;

			if (mCurrSimul.mIsValid) {

				if (mBallSimulDirty) {

					action = SA_New;

				} else if (mCurrSimul.mIsPartial) {

					action = SA_Continue;

				} else if (simulIsDesynced(match, time)) {

					action = SA_New;
				}

			} else {

				action = SA_New;
			}

			if (action != SA_None) {

				mBallSimulDirty = false;
			}

			switch (action) {

				case SA_New: {

					reSimul(true);

				} break;

				case SA_Continue: {

					reSimul(false);

				} break;

				default: {
				} break;
			}

		} else {

			setEmptySimul(false);

			mBallSimulDirty = true;
		}
	}
}

void BallSimulManager::render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass) {

	if (flagExtraRenders && mEnabled) {

		if (mCurrSimul.mIsValid) {

			Ball& ball = mMatch->getBall();

			float r1 = ball.getRadius(false) * 0.25f;
			float r2 = r1 * 3.0f;

			mCurrSimul.render(renderer, r1, &r2);
		}
	}
}
*/

} }
