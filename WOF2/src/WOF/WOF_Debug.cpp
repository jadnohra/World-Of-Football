#include "WOF.h"

#include "match/Match.h"
#include "match/entity/footballer/Footballer.h"
using namespace WOF::match;

namespace WOF {

float Game::debug_team_getTacticalInstantaneousBallOwnership(int team) {

	return mMatch->getTeam((TeamEnum) team).getTacticalInstantaneousBallOwnership();
}

float Game::debug_team_getTacticalBallOwnership(int team) {

	return mMatch->getTeam((TeamEnum) team).getTacticalBallOwnership();
}

unsigned int Game::debug_getBallInterceptFootballerCount() {

	SoftPtr<SpatialMatchState::BallIntercept> intercept = mMatch->getSpatialMatchState().getValidBallIntercept();

	return intercept.isValid() ? intercept->getFootballerCount() : 0;
}

void Game::debug_getBallInterceptFootballer(unsigned int index, FootballerInfo& info, bool& doesInterceptBeforeBallIsResting) {

	SoftPtr<SpatialMatchState::BallIntercept> intercept = mMatch->getSpatialMatchState().getValidBallIntercept();

	if (intercept.isValid()) {

		SoftPtr<Footballer> footballer = intercept->getFootballer(index);

		info.team = footballer->mTeam;
		info.number = footballer->mNumber;

		doesInterceptBeforeBallIsResting = footballer->getSpatialState().getPathType() == BallInterceptEstimate::Estimate_SimulInterceptorWait;
	}
}

unsigned int Game::debug_getBallDistFootballerCount() {

	SpatialMatchState::BallDist& dist = mMatch->getSpatialMatchState().getBallDist();

	return dist.getFootballerCount();
}

void Game::debug_getBallDistFootballer(unsigned int index, FootballerInfo& info) {

	SpatialMatchState::BallDist& dist = mMatch->getSpatialMatchState().getBallDist();

	SoftPtr<Footballer> footballer = dist.getFootballer(index);

	info.team = footballer->mTeam;
	info.number = footballer->mNumber;
}

}