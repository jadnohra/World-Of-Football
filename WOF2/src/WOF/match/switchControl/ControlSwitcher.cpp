#include "ControlSwitcher.h"

#include "../Match.h"
#include "../entity/footballer/Footballer.h"
#include "../Team.h"
#include "../Player.h"

namespace WOF { namespace match {


void ControlSwitcher::init(Match& match) {

	mProcessedPathInterceptSimulID = (SimulationID) -1;
	mNextUpdateTime = -1.0f;
	mPathInterceptUpdateTime = FLT_MAX;
	mMatch = &match;

	mModeWasPathIntercept = false;
	mPathInterceptLastChangeTime = 0.0f;

	mFrameProcessingSetup.init(match.getClock(), 30.0f, 1, true);

	onActiveFootballersChange();
}

void ControlSwitcher::onActiveFootballersChange() {

	mFrameProcessingState.reset(mFrameProcessingSetup, getFootballerIterator(mMatch).getFootballerCount());
}

void ControlSwitcher::getFootballerDebugText(Match& match, Footballer_Brain& footballer, String& str) {

	/*
	if (mValueValidity.isValidPathIntercept) {

		const SwitchControl::Value_PathIntercept* pPathIntercept = footballer.getSwitchControlState().getValue_PathIntercept(footballer, match.getBallSimulManager().getCurrSimul()); 

		String tempStr;

		if (pPathIntercept && pPathIntercept->shortestTimeIntercept.isValid) {

			tempStr.assignEx(L"T(%.2f / %.2f)", match.getCoordSys().invConvUnit(pPathIntercept->shortestTimeIntercept.interceptDist), pPathIntercept->shortestTimeIntercept.interceptTime);

			if (str.isValid())
				str.append(L" - ");

			str.append(tempStr);
		}

		if (pPathIntercept && pPathIntercept->shortestDistIntercept.isValid) {

			tempStr.assignEx(L"D(%.2f / %.2f)", match.getCoordSys().invConvUnit(pPathIntercept->shortestDistIntercept.interceptDist), pPathIntercept->shortestDistIntercept.interceptTime);

			if (str.isValid())
				str.append(L" - ");

			str.append(tempStr);
		}

		if (pPathIntercept && pPathIntercept->restBallIntercept.isValid) {

			tempStr.assignEx(L"R(%.2f / %.2f)", match.getCoordSys().invConvUnit(pPathIntercept->restBallIntercept.interceptDist), pPathIntercept->restBallIntercept.interceptTime);

			if (str.isValid())
				str.append(L" - ");

			str.append(tempStr);
		}
	}
	*/
}

FootballerIterator& ControlSwitcher::getFootballerIterator(Match& match) {

	return match.getActiveFootballerIterator();
}

void ControlSwitcher::updatePlayerState(Match& match, Player& player) {

	/*
	SwitchControl::PlayerState& state = player.hSwitchState;
	state.footballerScoreInfluence = 1.0f;
	*/

	/*
	if (state.isSignaledShot && mValueValidity.isValidPathIntercept) {

		SimulationID currSimulID = mPathInterceptEvaluator.getCurrSimulID();

		if (currSimulID == state.signaledShotSimulID) {

			state.isSignaledShot = false;
			state.footballerScoreInfluence = (float) state.shotSwitchControlValue;

		} else if (currSimulID > state.signaledShotSimulID) {

			state.isSignaledShot = false;
		}
	}
	*/
}

/*
bool ControlSwitcher::compareIsBetterThan_PathIntercept(Match& match, Footballer& compared, SwitchControl::Value_PathIntercept& pathIntercept, Footballer& ref, const Time& time) {

	SwitchControl::Value_PathIntercept& refPathIntercept = ref.getSwitchControlState().value_pathIntercept;

	float pathInterceptTime = pathIntercept.getInterceptTime(time);
	float refPathInterceptTime = refPathIntercept.getInterceptTime(time);


	float timeDiff = pathInterceptTime - refPathInterceptTime;

	if (fabs(timeDiff) <= match.mSwitchControlSetup.pathInterceptTimeCompareTolerance) {

		if (compared.getSwitchControlState().getPlayerScoreInfluence(compared) != ref.getSwitchControlState().getPlayerScoreInfluence(ref)) {

			return compared.getSwitchControlState().getPlayerScoreInfluence(compared) > ref.getSwitchControlState().getPlayerScoreInfluence(ref);
		}

		float distDiff = pathIntercept.getInterceptDist(time) - refPathIntercept.getInterceptDist(time);

		if (fabs(distDiff) <= match.mSwitchControlSetup.pathInterceptDistCompareTolerance) {

			return timeDiff < 0.0f;

		} else {

			return distDiff < 0.0f;
		}
	} 

	return timeDiff < 0.0f;
}

void ControlSwitcher::insertScoreSorted_PathIntercept(Match& match, Footballer& footballer, SoftFootballerArray& sortedTopArray, BallSimulation& simul, const Time& time) {

	SwitchControl::Value_PathIntercept& pathIntercept = dref(footballer.getSwitchControlState().getValue_PathIntercept(footballer, &simul));

	for (SoftFootballerArray::Index i = 0; i < sortedTopArray.count; ++i) {

		if (compareIsBetterThan_PathIntercept(match, footballer, pathIntercept, dref(sortedTopArray.el[i]), time)) {
		
			sortedTopArray.addOneAtReserved(i, &footballer);
			return;
		}
	}

	sortedTopArray.addOneReserved(&footballer);
}
*/

void ControlSwitcher::updateTopFootballerArray_PathIntercept(Match& match, const Time& time) {

	mTopFootballers[Team_A].count = 0;
	mTopFootballers[Team_A].reserve(match.getTeam(Team_A).getFootballerCount());

	mTopFootballers[Team_B].count = 0;
	mTopFootballers[Team_B].reserve(match.getTeam(Team_B).getFootballerCount());

	SpatialMatchState::BallIntercept& interceptEvaluator = match.getSpatialMatchState().getBallIntercept();

	for (FootballerIndex i = 0; i < interceptEvaluator.getFootballerCount(); ++i) {

		SoftPtr<Footballer> footballer = interceptEvaluator.getFootballer(i);

		mTopFootballers[footballer->mTeam].addOne(footballer);
	}

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);
		
		if (mTopFootballers[ti].count < team.getFootballerCount()
			&& mTopFootballers[ti].count < team.getPlayerCount()) {

			SoftFootballerArray& topFootballers = mTopFootballers[ti];

			FootballerIndex missingCount = team.getPlayerCount() - mTopFootballers[ti].count;

			if (missingCount > 0) {

				SpatialMatchState::BallDist& ballDistEvaluator = match.getSpatialMatchState().getBallDist();

				for (FootballerIndex i = 0; i < ballDistEvaluator.getFootballerCount((TeamEnum) ti) && missingCount > 0; ++i) {

					SoftPtr<Footballer> candidate = ballDistEvaluator.getFootballer((TeamEnum) ti, i);

					for (FootballerIndex j = 0; j < topFootballers.count; ++j) {

						if (topFootballers[j] == candidate.ptr()) {

							candidate.destroy();
							break;
						}
					}

					if (candidate.isValid()) {

						mTopFootballers[ti].addOne(candidate);
						--missingCount;
					}
				}
			}
		}
	}
}


bool ControlSwitcher::compareIsBetterThan_BallDist(Match& match, Footballer& compared, Footballer& ref, const Time& time) {

	{
		float compValue;
		float refValue;

		compared.getSwitchControlState().getValue_BallOwnership(compared, compValue); 
		ref.getSwitchControlState().getValue_BallOwnership(ref, refValue); 

		if (compValue > refValue) 
			return true;
	}

	{
		float compValue;
		float refValue;

		compared.getSwitchControlState().getValue_CurrBallDist(compared, compValue); 
		ref.getSwitchControlState().getValue_CurrBallDist(ref, refValue); 

		float distDiff = compValue - refValue;

		if (fabs(distDiff) < match.mSwitchControlSetup.ballDistCompareTolerance) {

			if (compared.getSwitchControlState().getPlayerScoreInfluence(compared) != ref.getSwitchControlState().getPlayerScoreInfluence(ref)) {

				return compared.getSwitchControlState().getPlayerScoreInfluence(compared) > ref.getSwitchControlState().getPlayerScoreInfluence(ref);
			}
		
			return distDiff < 0.0f;
		}

		return distDiff < 0.0f;
	}
}

void ControlSwitcher::insertScoreSorted_BallDist(Match& match, Footballer& footballer, SoftFootballerArray& sortedTopArray, const Time& time) {

	for (SoftFootballerArray::Index i = 0; i < sortedTopArray.count; ++i) {

		if (compareIsBetterThan_BallDist(match, footballer, dref(sortedTopArray.el[i]), time)) {
		
			sortedTopArray.addOneAtReserved(i, &footballer);
			return;
		}
	}

	sortedTopArray.addOneReserved(&footballer);
}

void ControlSwitcher::updateTopFootballerArray_BallDist(Match& match, const Time& time) {

	/*
	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	FootballerIterator& iterator = getFootballerIterator(match);

	mTopFootballers[Team_A].count = 0;
	mTopFootballers[Team_A].reserve(match.getTeam(Team_A).getFootballerCount());

	mTopFootballers[Team_B].count = 0;
	mTopFootballers[Team_B].reserve(match.getTeam(Team_B).getFootballerCount());
	
	while (iterator.iterateFootballers(footballer.ptrRef(), iter)) {

		insertScoreSorted_BallDist(match, footballer, mTopFootballers[footballer->mTeam], time);
	}
	*/

	mTopFootballers[Team_A].count = 0;
	mTopFootballers[Team_A].reserve(match.getTeam(Team_A).getFootballerCount());

	mTopFootballers[Team_B].count = 0;
	mTopFootballers[Team_B].reserve(match.getTeam(Team_B).getFootballerCount());

	SpatialMatchState::BallDist& ballDistEvaluator = match.getSpatialMatchState().getBallDist();

	for (FootballerIndex i = 0; i < ballDistEvaluator.getFootballerCount(); ++i) {

		SoftPtr<Footballer> footballer = ballDistEvaluator.getFootballer(i);

		mTopFootballers[footballer->mTeam].addOne(footballer);
	}

}

/*
void ControlSwitcher::updateTopFootballerArray(Match& match, const Time& time) {

	//SoftPtr<Footballer> footballer;
	//FootballerIndex iter;
//
//	FootballerIterator& iterator = getFootballerIterator(match);
//
//	while (iterator.iterateFootballers(footballer.ptrRef(), iter)) {
//
//		SoftPtr<Player> player = footballer->getPlayer();
//
//		if (!player.isValid()) {
//
//			footballer->mSwitchState.playerScoreInfluence = 0.0f;
//		}
//	}


	if (mValueValidity.isValidPathIntercept) {

		

		updateTopFootballerArray_PathIntercept(match, time);
		
	} else {

		updateTopFootballerArray_BallDist(match, time);
	}
}
*/

void ControlSwitcher::frameMove(Match& match, const Time& time) {

	switch (match.mSwitchControlSetup.mode) {

		case FSM_Manual: frameMove_Manual(match, time); break;
		case FSM_Automatic: frameMove_Automatic(match, time); break;
	}
}

void ControlSwitcher::frameMove_Manual(Match& match, const Time& time) {

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);

		for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

			Player& player = team.getPlayer(pi);

			if (player.isAskingForManualSwitch()) {

				Team& team = player.getAttachedTeam();
				SoftPtr<Footballer> footballer;
				FootballerIndex iter;

				SoftPtr<Footballer> attached = player.getAttachedFootballer();

				while (team.iterateFootballers(footballer.ptrRef(), iter)) {

					if (attached == NULL) {

						if (footballer->getPlayer() == NULL && footballer->canActivatePlayerMode()) {

							player.attachFootballer(footballer);
							break;
						}

					} else if (footballer.ptr() == attached) {

						player.detachFootballer();
						attached = player.getAttachedFootballer();
					}
				}

				if (player.getAttachedFootballer() == NULL) {

					if (team.iterateFootballers(footballer.ptrRef(), iter, FootballerIterator::F_AI)) {

						player.attachFootballer(footballer);
					}
				}
			}
		}
	}

}

void ControlSwitcher::frameMove_Automatic(Match& match, const Time& time) {

	Ball& ball = match.getBall();

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);

		for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

			Player& player = team.getPlayer(pi);

			player.hSwitchState.isSwitchLocked = player.isSwitchLocked();
		}
	}

	for (FootballerIndex i = 0; i < ball.getOwnerCount(); ++i) {

		SoftPtr<Footballer> footballer = ball.getOwner(i);
		SoftPtr<Player> player = footballer->getPlayer();

		if (!player.isValid() && footballer->canActivatePlayerMode()) {

			Team& team = match.getTeam(footballer->mTeam);

			for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

				Player& testPlayer = team.getPlayer(pi);

				if (!testPlayer.hSwitchState.isSwitchLocked) {

					SoftPtr<Footballer> attachedFootballer = testPlayer.getAttachedFootballer();

					if (attachedFootballer.isValid() && !attachedFootballer->isBallOwner()) {
					
						testPlayer.detachFootballer();
						testPlayer.attachFootballer(footballer);
					}
				}
			}
		}
	}

	bool allAreOwners = true;

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);

		for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

			Player& player = team.getPlayer(pi);
			SoftPtr<Footballer> footballer = player.getAttachedFootballer();

			if (player.hSwitchState.isSwitchLocked || (footballer.isValid() && footballer->isBallOwner())) {
			} else {

				allAreOwners = false;
				break;
			}
		}
	}

	SpatialMatchState::BallIntercept& interceptEvaluator = match.getSpatialMatchState().getBallIntercept();

	if (interceptEvaluator.isValid()) {

		frameMove_PathIntercept(match, time, mModeWasPathIntercept, allAreOwners, interceptEvaluator);
		mModeWasPathIntercept = true;

	} else {

		mModeWasPathIntercept = false;

		if (true/*match.mSwitchControlSetup.fallbackToBallDist*/) {

			frameMove_BallDist(match, time, mModeWasPathIntercept, allAreOwners);
			
		} else {

			return;
		}
	}

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);

		for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

			Player& player = team.getPlayer(pi);
			player.hSwitchState.wasSwitchLocked = player.hSwitchState.isSwitchLocked;
		}
	}
}

void ControlSwitcher::frameMove_PathIntercept(Match& match, const Time& time, const bool& modeWasPathIntercept, const bool& allAreOwners, SpatialMatchState::BallIntercept& interceptEvaluator) {

	if (allAreOwners)
		return;

	if (interceptEvaluator.getLastChangeTime() != mPathInterceptLastChangeTime) {

		mPathInterceptLastChangeTime = interceptEvaluator.getLastChangeTime();

		updatePlayerInfluences(match, time);
		updateTopFootballerArray_PathIntercept(match, time);
		makeSwitches(match, time);
	} 	
}

void ControlSwitcher::frameMove_BallDist(Match& match, const Time& time, const bool& modeWasPathIntercept, const bool& allAreOwners) {

	if (allAreOwners)
		return;

	bool doUpdate = false;

	if (modeWasPathIntercept) {

		doUpdate = true;
		mNextUpdateTime = time + (0.5f * match.mSwitchControlSetup.lazyUpdateInterval);

	} else {

		if (mNextUpdateTime < time) {

			mNextUpdateTime = time + match.mSwitchControlSetup.lazyUpdateInterval;
			doUpdate = true;
		} 
	}

	if (!doUpdate)
		return;

	updatePlayerInfluences(match, time);
	updateTopFootballerArray_BallDist(match, time);
	makeSwitches(match, time);
}

void ControlSwitcher::updatePlayerInfluences(Match& match, const Time& time) {

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);

		for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

			Player& player = team.getPlayer(pi);
			updatePlayerState(match, player);
		}
	}
}

/*
void ControlSwitcher::frameMove(Match& match, const Time& time) {

	if (match.mSwitchControlSetup.mode != FSM_Automatic)
		return;

	//DWORD t = timeGetTime();

	bool doUpdate = false;
	bool modeWasPathIntercept = (mPathInterceptEvaluator.getState() == PathInterceptEvaluator::State_Valid);
	bool modeIsPathIntercept;
	Ball& ball = match.getBall();

	PathInterceptEvaluator::State pathInterceptEvalState = mPathInterceptEvaluator.update(match, time);

	if (pathInterceptEvalState == PathInterceptEvaluator::State_Valid) {

		modeIsPathIntercept = true;

		if (modeWasPathIntercept) {

			if (time > mPathInterceptUpdateTime) {

				doUpdate = true;
			}

		} else {

			doUpdate = true;
			mPathInterceptUpdateTime = FLT_MAX;
			mProcessedPathInterceptSimulID = mPathInterceptEvaluator.getCurrSimulID();
		}

	} else {

		modeIsPathIntercept = false;

		if (modeWasPathIntercept) {

			doUpdate = true;
			mNextUpdateTime = time + (0.5f * match.mSwitchControlSetup.lazyUpdateInterval);

		} else {

			if (mNextUpdateTime < time) {

				bool allAreOwners = true;

				for (int ti = 0; ti < 2; ++ti) {

					Team& team = match.getTeam((TeamEnum) ti);

					for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

						Player& player = team.getPlayer(pi);
						SoftPtr<Footballer> footballer = player.getAttachedFootballer();

						if (footballer.isValid() && footballer->isBallOwner()) {
						} else {

							allAreOwners = false;
							break;
						}
					}
				}

				if (!allAreOwners) {

					doUpdate = true;
				}

				mNextUpdateTime = time + match.mSwitchControlSetup.lazyUpdateInterval;
				//we can maybe make this frame-based depending on footballers moving if we want to ??
				//but will it be really useful in a game where ebdy is moving?
			}
		}
	}

	if (!doUpdate)
		return;

	/*
	//static int ct = 0;
	//static String dbgStr;

	//dbgStr.assignEx(L"%d switchUpdate %f", ct++, time);

	//match.getConsole().show(true, true);
	//match.getConsole().print(dbgStr.c_tstr());
	//log(dbgStr.c_tstr());
	

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);

		for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

			Player& player = team.getPlayer(pi);
			updatePlayerState(match, player);
		}
	}
	
	if (modeIsPathIntercept) {

		updateTopFootballerArray_PathIntercept(match, time);

	} else {

		updateTopFootballerArray_BallDist(match, time);
	}

	bool switchedAny = false;
	bool pathInterceptUpdateTimeUpdated = false;

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);
		SoftFootballerArray& topFootballers = mTopFootballers[ti];

		PlayerIndex playerCount = team.getPlayerCount();
		bool detachedAny = false;

		//detach players with footballer not in top 'playerCount' footballers
		for (PlayerIndex pi = 0; pi < playerCount; ++pi) {

			Player& player = team.getPlayer(pi);

			bool foundPlayer = false;

			for (FootballerIndex fi = 0; fi < topFootballers.count && fi < playerCount; ++fi) {

				SoftPtr<Footballer> footballer = topFootballers[fi];

				if (footballer->getPlayer() == &player) {

					foundPlayer = true;
					break;
				}
			}

			if (!foundPlayer) {

				player.detachFootballer();
				detachedAny = true;
			}
		}

		if (detachedAny) {

			//attach detached players 
			for (PlayerIndex pi = 0; pi < playerCount; ++pi) {

				Player& player = team.getPlayer(pi);

				for (FootballerIndex fi = 0; fi < topFootballers.count && fi < playerCount; ++fi) {

					SoftPtr<Footballer> footballer = topFootballers[fi];

					if (footballer->getPlayer() == NULL) {

						switchedAny = true;
						player.attachFootballer(footballer);

						if (modeIsPathIntercept) {

							SwitchControl::Value_PathIntercept& refPathIntercept = footballer->mSwitchState.value_pathIntercept;
	
							if (refPathIntercept.shortestTimeIntercept.isValid) {

								pathInterceptUpdateTimeUpdated = true;
								mPathInterceptUpdateTime = MMin(mPathInterceptUpdateTime, refPathIntercept.shortestTimeIntercept.interceptTime);
							} 
						}
					}
				}
			}
		}
	}

	if (modeIsPathIntercept && (!pathInterceptUpdateTimeUpdated || !switchedAny)) {

		mPathInterceptUpdateTime = FLT_MAX;
	}

	//DWORD dt = timeGetTime() - t;
	//if (dt) log(L"%d", (int) dt);
}
*/

bool ControlSwitcher::makeSwitches(Match& match, const Time& time) {

	bool switchedAny = false;

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);
		SoftFootballerArray& topFootballers = mTopFootballers[ti];

		const PlayerIndex& playerCount = team.getPlayerCount();
		bool detachedAny = false;

		//detach players with footballer not in top 'playerCount' footballers
		for (PlayerIndex pi = 0; pi < playerCount; ++pi) {

			Player& player = team.getPlayer(pi);
			bool foundPlayer = false;

			if (player.hSwitchState.isSwitchLocked) {

				foundPlayer = true;

			} else {

				PlayerIndex testPlayerCount = playerCount;

				for (FootballerIndex fi = 0; fi < topFootballers.count && fi < testPlayerCount; ++fi) {

					SoftPtr<Footballer> footballer = topFootballers[fi];

					if (footballer->getPlayer() == &player) {

						foundPlayer = true;
						break;
					}

					//account for locked footballers
					if (!footballer->canActivatePlayerMode())
						++testPlayerCount;
				}
			}

			if (!foundPlayer) {

				player.detachFootballer();
				detachedAny = true;
			}
		}

		if (detachedAny) {

			//attach detached players 
			for (PlayerIndex pi = 0; pi < playerCount; ++pi) {

				Player& player = team.getPlayer(pi);

				if (!player.hSwitchState.isSwitchLocked && (player.getAttachedFootballer() == NULL)) {

					for (FootballerIndex fi = 0; fi < topFootballers.count; ++fi) {

						SoftPtr<Footballer> footballer = topFootballers[fi];

						if (footballer->getPlayer() == NULL && footballer->canActivatePlayerMode()) {

							switchedAny = true;
							player.attachFootballer(footballer);
							break;
						}
					}
				}
			}
		}
	}

	return switchedAny;
}

/*
void ControlSwitcher::frameMove(Match& match, const Time& time) {

	if (match.mSwitchControlSetup.mode != FSM_Automatic)
		return;

	//DWORD t = timeGetTime();

	PlayerManager::TempPlayers players;

	bool needUpdateTopArray = false;

	match.getPlayerManager().getAllPlayers(players);
	PlayerManager::sortBySwitchPriority(players, false);

	for (PlayerManager::TempPlayers::Index i = 0; i < players.count; ++i) {

		Player& player = dref(players.el[i]);
		SoftPtr<Footballer> attachedFootballer = player.getAttachedFootballer(); 

		player.hSwitchState.doSwitch = false;

		if (attachedFootballer.isValid()) {

			Time attachedInterval = time - attachedFootballer->getPlayerAttachTime();

			if (attachedInterval >= match.mSwitchControlSetup.minSwitchInterval) {

				updatePlayerState(match, player, &player.hSwitchState.doSwitch);

			} else {

				updatePlayerState(match, player, NULL);
			}

		} else {

			player.hSwitchState.doSwitch = true;
		}

		if (player.hSwitchState.doSwitch) {

			needUpdateTopArray = true;
		}
	}
	
	if (!needUpdateTopArray) {

		return;
	}

	updateTopFootballerArray(match, time);

	for (PlayerManager::TempPlayers::Index i = 0; i < players.count; ++i) {

		Player& player = dref(players.el[i]);

		if (player.hSwitchState.doSwitch) {

			player.detachFootballer();
		} 
	}

	for (PlayerManager::TempPlayers::Index i = 0; i < players.count; ++i) {

		Player& player = dref(players.el[i]);
		TeamEnum playerTeam = player.getAttachedTeamEnum();
		SoftFootballerArray& topFootballers = mTopFootballers[playerTeam];

		if (player.hSwitchState.doSwitch) {

			for (FootballerIndex fi = 0; fi < topFootballers.count; ++fi) {

				if (topFootballers.el[fi]->mTeam == playerTeam && topFootballers.el[fi]->isInAIMode()) {

					player.attachFootballer(dref(topFootballers.el[fi]));

					break;
				}
			}
		} 
	}
	

	//DWORD dt = timeGetTime() - t;
	//
	//if (dt)
	//	log(L"%d", (int) dt);
}
*/

Footballer* ControlSwitcher::switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch) {

	if (technique == FST_Default) {

		technique = mMatch->mSwitchControlSetup.technique;
	}

	return NULL;
}

void ControlSwitcher::render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass) {
}

} }