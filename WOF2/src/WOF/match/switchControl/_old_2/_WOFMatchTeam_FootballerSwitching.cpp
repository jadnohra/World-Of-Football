#include "WOFMatchTeam.h"

#include "WOFMatch.h"
#include "WOFMatchObjectIncludes.h"

namespace WOF { namespace match {

Team::SwitchFootballerState_ShotAnalysis::SwitchFootballerState_ShotAnalysis()
	: simulID(-1) {
}

void Team::SwitchFootballerState_ShotAnalysis::onSimulChanged(BallSimulManager::Simul& simul) {



	simulID = simul.getID();
}

Footballer* Team::switchGetNextFootballer_ShotAnalysis(Footballer* pCurrFootballer, bool allowSameFootballer, SwitchFootballerState_ShotAnalysis& state) {

	struct CandidateState {

		float interceptionTime;

		bool init(Footballer* pFootballer, BallSimulManager::Simul& simul) {

			return false;
		}
	};

	struct Candidate : CandidateState {

		SoftPtr<Footballer> footballer;

		void test(Footballer* pFootballer, CandidateState& state, BallSimulManager::Simul& simul) {
		}
	};
	
	SoftPtr<BallSimulManager::Simul> simul = mMatch->getBallSimulManager().getCurrSimul();

	//of course ... split this in many frames and allow isPartial
	//also add heuristic -> dribble shot -> no switch
	if (simul.isValid() && simul->isValid() && !simul->isPartial()) {

		if (simul->getID() != state.simulID) {

			state.onSimulChanged(simul);
		}
		

		float instantTakeInterceptionTimeThreshold = 0.0f;

		CandidateState candidateState;
		SoftPtr<Footballer> candidate;
		
		Candidate bestCandidate;
		
		if (allowSameFootballer && pCurrFootballer) {

			candidate = pCurrFootballer;

			if (candidateState.init(candidate, simul)) {

				if (candidateState.interceptionTime <= instantTakeInterceptionTimeThreshold) {

					return candidate;		
				}

				bestCandidate.test(candidate, candidateState, simul);
			}
		}

		candidate = getFirstFootballer(true);

		while (candidate.isValid()) {

			bool allowCandidate = true;

			if (!allowSameFootballer) {

				allowCandidate = (candidate.ptr() != pCurrFootballer);
			} 

			if (allowCandidate) {

				if (candidateState.init(candidate, simul)) {

					if (candidateState.interceptionTime <= instantTakeInterceptionTimeThreshold) {

						return candidate;		
					}

					bestCandidate.test(candidate, candidateState, simul);
				} 
			}

			candidate = getNextFootballer(candidate->mNumber, true);
		} 

		return bestCandidate.footballer;
	}

	return allowSameFootballer ? pCurrFootballer : NULL;
}

Footballer* Team::switchGetNextFootballer_BallDist(Footballer* pCurrFootballer, bool allowSameFootballer) {

	struct Candidate {

		SoftPtr<Footballer> footballer;
		float relBallDist;

		Candidate() : relBallDist(FLT_MAX) {}

		void test(Footballer* pFootballer, SpatialState::FootballeBallState& state) {

			if (state.getRelBallDist() < relBallDist) {

				relBallDist = state.getRelBallDist();
				footballer = pFootballer;
			}
		}
	};

	SpatialState& spatial = mMatch->getSpatialState();
	SpatialState::FootballeBallState state;

	SoftPtr<Ball> ball = &(mMatch->getBall());
	SoftPtr<Footballer> candidate;
	
	Candidate bestCandidate;
	
	if (allowSameFootballer && pCurrFootballer) {

		candidate = pCurrFootballer;

		if (spatial.getFootballerBallState(candidate, ball, state)) {

			if (mMatch->mFootballerSwitchSetup.enableBuffer
				&& (state.getRelBallDist() <= mMatch->mFootballerSwitchSetup.bufferDistance) ) {

				return pCurrFootballer;		
			}

			bestCandidate.test(candidate, state);
		}
	}

	candidate = getFirstFootballer(true);

	while (candidate.isValid()) {

		bool allowCandidate = true;

		if (!allowSameFootballer) {

			allowCandidate = (candidate.ptr() != pCurrFootballer);
		} 

		if (allowCandidate) {

			if (spatial.getFootballerBallState(candidate, ball, state)) {

				bestCandidate.test(candidate, state);

			} 
		}

		candidate = getNextFootballer(candidate->mNumber, true);
	} 

	return bestCandidate.footballer;
}

Footballer* Team::switchGetNextFootballer_NextNumber(Footballer* pCurrFootballer, bool allowSameFootballer) {

	if (pCurrFootballer == NULL) {
		
		pCurrFootballer = getFirstFootballer(true);

		if (pCurrFootballer != NULL) {

			return pCurrFootballer;
		}
	}

	if (pCurrFootballer != NULL) {

		if (allowSameFootballer) {

			return pCurrFootballer;
		}

		SoftPtr<Footballer> nextFootballer;

		nextFootballer = getNextFootballer(pCurrFootballer->mNumber, true);

		if (nextFootballer.isValid())
			return nextFootballer;

		nextFootballer = getFirstFootballer(true);

		return nextFootballer;
	} 

	return NULL;

}

Footballer* Team::switchGetNextFootballer(FootballerSwitchTechnique technique, Footballer* pCurrFootballer, bool allowSameFootballer) {

	if (technique == FST_Default) {

		technique = mMatch->mFootballerSwitchSetup.technique;
	}

	switch (technique) {

		case FST_ShotAnalysis: {

			return switchGetNextFootballer_ShotAnalysis(pCurrFootballer, allowSameFootballer, mSwitchFootballerState.shotAnalysis);

		} break;

		case FST_BallDist: {

			return switchGetNextFootballer_BallDist(pCurrFootballer, allowSameFootballer);
			
		} break;

		case FST_NextNumber:
		default: {

			return switchGetNextFootballer_NextNumber(pCurrFootballer, allowSameFootballer);
			
		} break;
	}


	return NULL;
}

} }