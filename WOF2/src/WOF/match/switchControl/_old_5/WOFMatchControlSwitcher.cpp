#include "WOFMatchControlSwitcher.h"

#include "../WOFMatch.h"
#include "../WOFMatchObjectIncludes.h"

namespace WOF { namespace match {


void ControlSwitcher::init(Match& match) {

	mProcessedPathInterceptSimulID = (SimulationID) -1;
	mNextUpdateTime = -1.0f;
	mPathInterceptUpdateTime = FLT_MAX;
	mMatch = &match;

	mFrameProcessingSetup.init(match.getTime(), 30.0f, 1, true);

	mPathInterceptEvaluator.destroy();
	WE_MMemNew(mPathInterceptEvaluator.ptrRef(), PathInterceptEvaluator());
	mPathInterceptEvaluator->init(match);

	onActiveFootballersChange();
}

void ControlSwitcher::onActiveFootballersChange() {

	mPathInterceptEvaluator->resetFootballerIterator(&mMatch->getActiveFootballerIterator());

	mFrameProcessingState.reset(mFrameProcessingSetup, getFootballerIterator(mMatch).getFootballerCount());
}

void ControlSwitcher::getFootballerDebugText(Match& match, Footballer_Brain& footballer, String& str) {

	/*
	if (mValueValidity.isValidPathIntercept) {

		const SwitchControl::Value_PathIntercept* pPathIntercept = footballer.mSwitchState.getValue_PathIntercept(footballer, match.getBallSimulManager().getCurrSimul()); 

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

	SwitchControl::PlayerState& state = player.hSwitchState;
	state.footballerScoreInfluence = 1.0f;

	/*
	if (state.isSignaledShot && mValueValidity.isValidPathIntercept) {

		SimulationID currSimulID = mPathInterceptEvaluator->getCurrSimulID();

		if (currSimulID == state.signaledShotSimulID) {

			state.isSignaledShot = false;
			state.footballerScoreInfluence = (float) state.shotSwitchControlValue;

		} else if (currSimulID > state.signaledShotSimulID) {

			state.isSignaledShot = false;
		}
	}
	*/
}

bool ControlSwitcher::compareIsBetterThan_ModePathIntercept(Match& match, Footballer& compared, SwitchControl::Value_PathIntercept& pathIntercept, Footballer& ref, const Time& time) {

	SwitchControl::Value_PathIntercept& refPathIntercept = ref.mSwitchState.value_pathIntercept;

	float pathInterceptTime = pathIntercept.getInterceptTime(time);
	float refPathInterceptTime = refPathIntercept.getInterceptTime(time);


	float timeDiff = pathInterceptTime - refPathInterceptTime;

	if (fabs(timeDiff) <= match.mSwitchControlSetup.pathInterceptTimeCompareTolerance) {

		if (compared.mSwitchState.getPlayerScoreInfluence(compared) != ref.mSwitchState.getPlayerScoreInfluence(ref)) {

			return compared.mSwitchState.getPlayerScoreInfluence(compared) > ref.mSwitchState.getPlayerScoreInfluence(ref);
		}

		float distDiff = pathIntercept.getInterceptDist(time) - refPathIntercept.getInterceptDist(time);

		if (fabs(distDiff) <= match.mSwitchControlSetup.pathInterceptDistCompareTolerance) {

			return timeDiff < 0.0f;

		} else {

			return distDiff < 0.0f;
		}
	} 

	return timeDiff < 0.0f;

	/*


	if (pathIntercept.shortestTimeIntercept.checkValidity(time)) {

		if (refPathIntercept.shortestTimeIntercept.isValid) {

			float timeDiff = pathIntercept.shortestTimeIntercept.interceptTime - refPathIntercept.shortestTimeIntercept.interceptTime;

			if (fabs(timeDiff) <= match.mSwitchControlSetup.pathInterceptTimeCompareTolerance) {

				if (compared.mSwitchState.getPlayerScoreInfluence(compared) != ref.mSwitchState.getPlayerScoreInfluence(ref)) {

					return compared.mSwitchState.getPlayerScoreInfluence(compared) > ref.mSwitchState.getPlayerScoreInfluence(ref);
				}

				float distDiff = pathIntercept.shortestTimeIntercept.interceptDist - refPathIntercept.shortestTimeIntercept.interceptDist;

				if (fabs(distDiff) <= match.mSwitchControlSetup.pathInterceptDistCompareTolerance) {

					return timeDiff < 0.0f;

				} else {

					return distDiff < 0.0f;
				}
			} 

			return timeDiff < 0.0f;
		} 
		
		return true;
	} 
	
	
	if (refPathIntercept.shortestTimeIntercept.isValid) {

		float distDiff = pathIntercept.restBallIntercept.interceptDist - refPathIntercept.shortestTimeIntercept.interceptDist;

		return distDiff < 0.0f;

	} else {

		float distDiff = pathIntercept.restBallIntercept.interceptDist - refPathIntercept.restBallIntercept.interceptDist;

		return distDiff < 0.0f;
	}
	*/
}

void ControlSwitcher::insertScoreSorted_ModePathIntercept(Match& match, Footballer& footballer, SoftFootballerArray& sortedTopArray, BallSimulation& simul, const Time& time) {

	SwitchControl::Value_PathIntercept& pathIntercept = dref(footballer.mSwitchState.getValue_PathIntercept(footballer, &simul));

	for (SoftFootballerArray::Index i = 0; i < sortedTopArray.count; ++i) {

		if (compareIsBetterThan_ModePathIntercept(match, footballer, pathIntercept, dref(sortedTopArray.el[i]), time)) {
		
			sortedTopArray.addOneAtReserved(i, &footballer);
			return;
		}
	}


	//if (sortedTopArray.count == 0)
	//	pathIntercept.shortestTimeIntercept.checkValidity(time);

	sortedTopArray.addOneReserved(&footballer);
}

void ControlSwitcher::updateTopFootballerArray_ModePathIntercept(Match& match, const Time& time) {

	SoftPtr<BallSimulation> currSimul = match.getBall().getSimul();

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	FootballerIterator& iterator = getFootballerIterator(match);

	mTopFootballers[_TeamA].count = 0;
	mTopFootballers[_TeamA].reserve(match.getTeam(_TeamA).getFootballerCount());

	mTopFootballers[_TeamB].count = 0;
	mTopFootballers[_TeamB].reserve(match.getTeam(_TeamB).getFootballerCount());
	
	SoftPtr<SwitchControl::Value_PathIntercept> pathIntercept;

	while (iterator.iterateFootballers(footballer.ptrRef(), iter)) {

		pathIntercept = footballer->mSwitchState.getValue_PathIntercept(footballer, currSimul);

		if (pathIntercept.isValid()) {

			insertScoreSorted_ModePathIntercept(match, footballer, mTopFootballers[footballer->mTeam], currSimul, time);
		}
	}
}

bool ControlSwitcher::compareIsBetterThan_ModeBallDist(Match& match, Footballer& compared, Footballer& ref, const Time& time) {

	{
		float compValue;
		float refValue;

		compared.mSwitchState.getValue_BallOwnership(compared, compValue); 
		ref.mSwitchState.getValue_BallOwnership(ref, refValue); 

		if (compValue > refValue) 
			return true;
	}

	{
		float compValue;
		float refValue;

		compared.mSwitchState.getValue_CurrBallDist(compared, compValue); 
		ref.mSwitchState.getValue_CurrBallDist(ref, refValue); 

		float distDiff = compValue - refValue;

		if (fabs(distDiff) < match.mSwitchControlSetup.ballDistCompareTolerance) {

			if (compared.mSwitchState.getPlayerScoreInfluence(compared) != ref.mSwitchState.getPlayerScoreInfluence(ref)) {

				return compared.mSwitchState.getPlayerScoreInfluence(compared) > ref.mSwitchState.getPlayerScoreInfluence(ref);
			}
		
			return distDiff < 0.0f;
		}

		return distDiff < 0.0f;
	}
}

void ControlSwitcher::insertScoreSorted_ModeBallDist(Match& match, Footballer& footballer, SoftFootballerArray& sortedTopArray, const Time& time) {

	for (SoftFootballerArray::Index i = 0; i < sortedTopArray.count; ++i) {

		if (compareIsBetterThan_ModeBallDist(match, footballer, dref(sortedTopArray.el[i]), time)) {
		
			sortedTopArray.addOneAtReserved(i, &footballer);
			return;
		}
	}

	sortedTopArray.addOneReserved(&footballer);
}

void ControlSwitcher::updateTopFootballerArray_ModeBallDist(Match& match, const Time& time) {

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	FootballerIterator& iterator = getFootballerIterator(match);

	mTopFootballers[_TeamA].count = 0;
	mTopFootballers[_TeamA].reserve(match.getTeam(_TeamA).getFootballerCount());

	mTopFootballers[_TeamB].count = 0;
	mTopFootballers[_TeamB].reserve(match.getTeam(_TeamB).getFootballerCount());
	
	while (iterator.iterateFootballers(footballer.ptrRef(), iter)) {

		insertScoreSorted_ModeBallDist(match, footballer, mTopFootballers[footballer->mTeam], time);
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

		

		updateTopFootballerArray_ModePathIntercept(match, time);
		
	} else {

		updateTopFootballerArray_ModeBallDist(match, time);
	}
}
*/

void ControlSwitcher::frameMove(Match& match, const Time& time) {

	if (match.mSwitchControlSetup.mode != FSM_Automatic)
		return;

	//DWORD t = timeGetTime();

	bool doUpdate = false;
	bool modeWasPathIntercept = (mPathInterceptEvaluator->getState() == PathInterceptEvaluator::State_Valid);
	bool modeIsPathIntercept;
	Ball& ball = match.getBall();

	PathInterceptEvaluator::State pathInterceptEvalState = mPathInterceptEvaluator->update(match, time);

	if (pathInterceptEvalState == PathInterceptEvaluator::State_Valid) {

		modeIsPathIntercept = true;

		if (modeWasPathIntercept) {

			if (time > mPathInterceptUpdateTime) {

				doUpdate = true;
			}

		} else {

			doUpdate = true;
			mPathInterceptUpdateTime = FLT_MAX;
			mProcessedPathInterceptSimulID = mPathInterceptEvaluator->getCurrSimulID();
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
	static int ct = 0;
	static String dbgStr;

	dbgStr.assignEx(L"%d switchUpdate %f", ct++, time);

	match.getConsole().show(true, true);
	match.getConsole().print(dbgStr.c_tstr());
	log(dbgStr.c_tstr());
	*/

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);

		for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

			Player& player = team.getPlayer(pi);
			updatePlayerState(match, player);
		}
	}
	
	if (modeIsPathIntercept) {

		updateTopFootballerArray_ModePathIntercept(match, time);

	} else {

		updateTopFootballerArray_ModeBallDist(match, time);
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

	/*
	for (int ti = 0; ti < 2; ++ti) {

		SoftFootballerArray& topFootballers = mTopFootballers[team];

		for (FootballerIndex fi = 0; fi < topFootballers.count; ++fi) {

			if (topFootballers.el[fi]->mTeam == playerTeam && topFootballers.el[fi]->isInAIMode()) {

				player.attachFootballer(dref(topFootballers.el[fi]));

				break;
			}
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
	*/

	//DWORD dt = timeGetTime() - t;
	//if (dt) log(L"%d", (int) dt);
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