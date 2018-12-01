#include "WOFMatchControlSwitcher.h"

#include "../WOFMatch.h"
#include "../WOFMatchObjectIncludes.h"

#include "WOFMatchControlSwitchEvaluator_BallPathIntercept.h"

namespace WOF { namespace match {


void ControlSwitcher::init(Match& match) {

	mMatch = &match;

	mFrameProcessingSetup.init(match.getTime(), 30.0f, 1, true);

	mEvaluator_BallPathIntercept.destroy();
	WE_MMemNew(mEvaluator_BallPathIntercept.ptrRef(), ControlSwitchEvaluator_BallPathIntercept());
	mEvaluator_BallPathIntercept->init(match);

	onActiveFootballersChange();
}

void ControlSwitcher::onActiveFootballersChange() {

	mEvaluator_BallPathIntercept->resetFootballerIterator(&mMatch->getActiveFootballerIterator());

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

void ControlSwitcher::updatePlayerState(Match& match, Player& player, bool* pWantsToSwitch) {

	if (pWantsToSwitch)
		*pWantsToSwitch = true;

	SoftPtr<Footballer> footballer = player.getAttachedFootballer();
		
	if (footballer.isValid()) {

		SwitchControl::FootballerState& state = footballer->mSwitchState;

		state.playerScoreInfluence = 1.0f;

		#ifdef FOOTBALLER_BODY3
		//if (footballer->getActionType() == Footballer::Action_Running) {
		if (footballer->isTryingToKeepBall()) {
		#endif
		#ifdef FOOTBALLER_BODY2
		if (footballer->getActionType() == Footballer::Action_Running) {
		#endif
		#ifdef FOOTBALLER_BODY1
		if (footballer->body_getCurrState() == Footballer::BS_Running) {
		#endif

			if (pWantsToSwitch)
				*pWantsToSwitch = false;

			return;
		}


		#ifdef FOOTBALLER_BODY3
		//Footballer::BallInteractionType interaction = footballer->brain_getAttachedLastBallInteraction(interactionTime);
		BallInteraction& interaction = footballer->getBallInteraction();
		#else
		Time interactionTime;
		Footballer::BallInteraction interaction = footballer->brain_getAttachedLastBallInteraction(interactionTime);
		#endif

		
		if (interaction.lastInteractionTime < match.mSwitchControlSetup.playerControlEffectInterval) {

			switch (interaction.lastInteraction) {

				case BallInteract_ActiveShot: {

					state.playerScoreInfluence = -10.0f;

				} break;

				case BallInteract_PassiveShot:
				case BallInteract_Controlled: {

					state.playerScoreInfluence = 10.0f;
					
					if (pWantsToSwitch)
						*pWantsToSwitch = false;

				} break;

				case BallInteract_UncontrolledCollision: {

					state.playerScoreInfluence = 1.0f;
					
				} break;

				default: {

				} break;
			}

		} 

	} 
}

bool ControlSwitcher::compareIsBetterThan_ModePathIntercept(Match& match, Footballer& compared, SwitchControl::Value_PathIntercept& pathIntercept, Footballer& ref, const Time& time) {

	SwitchControl::Value_PathIntercept& refPathIntercept = ref.mSwitchState.value_pathIntercept;
	
	if (pathIntercept.shortestTimeIntercept.checkValidity(time)) {

		if (refPathIntercept.shortestTimeIntercept.isValid) {

			float timeDiff = pathIntercept.shortestTimeIntercept.interceptTime - refPathIntercept.shortestTimeIntercept.interceptTime;

			if (fabs(timeDiff) <= match.mSwitchControlSetup.pathInterceptTimeCompareTolerance) {

				if (compared.mSwitchState.playerScoreInfluence != ref.mSwitchState.playerScoreInfluence) {

					return compared.mSwitchState.playerScoreInfluence > ref.mSwitchState.playerScoreInfluence;
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

		return false;

	} else {

		float distDiff = pathIntercept.restBallIntercept.interceptDist - refPathIntercept.restBallIntercept.interceptDist;

		return distDiff < 0.0f;
	}
}

void ControlSwitcher::insertScoreSorted_ModePathIntercept(Match& match, Footballer& footballer, SoftFootballerArray& sortedTopArray, BallSimulManager::Simul& simul, const Time& time) {

	SwitchControl::Value_PathIntercept& pathIntercept = dref(footballer.mSwitchState.getValue_PathIntercept(footballer, &simul));

	for (SoftFootballerArray::Index i = 0; i < sortedTopArray.count; ++i) {

		if (compareIsBetterThan_ModePathIntercept(match, footballer, pathIntercept, dref(sortedTopArray.el[i]), time)) {
		
			sortedTopArray.addOneAtReserved(i, &footballer);
			return;
		}
	}

	sortedTopArray.addOneReserved(&footballer);
}

void ControlSwitcher::updateTopFootballerArray_ModePathIntercept(Match& match, const Time& time) {

	SoftPtr<BallSimulManager::Simul> currSimul = match.getBallSimulManager().getCurrSimul();

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

			if (compared.mSwitchState.playerScoreInfluence != ref.mSwitchState.playerScoreInfluence) {

				return compared.mSwitchState.playerScoreInfluence > ref.mSwitchState.playerScoreInfluence;
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

void ControlSwitcher::updateTopFootballerArray(Match& match, const Time& time) {

	mEvaluator_BallPathIntercept->updateValidity(match, time, mValueValidity);

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	FootballerIterator& iterator = getFootballerIterator(match);

	while (iterator.iterateFootballers(footballer.ptrRef(), iter)) {

		SoftPtr<Player> player = footballer->getPlayer();

		if (!player.isValid()) {

			footballer->mSwitchState.playerScoreInfluence = 0.0f;
		}
	}

	if (mValueValidity.isValidPathIntercept) {

		mEvaluator_BallPathIntercept->frameMove(match, time, mValueValidity);

		updateTopFootballerArray_ModePathIntercept(match, time);
		
	} else {

		updateTopFootballerArray_ModeBallDist(match, time);
	}
}

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
	

	/*
	DWORD dt = timeGetTime() - t;

	if (dt)
		log(L"%d", (int) dt);
		*/
}

Footballer* ControlSwitcher::switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch) {

	if (technique == FST_Default) {

		technique = mMatch->mSwitchControlSetup.technique;
	}

	return NULL;
}

void ControlSwitcher::render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass) {
}

} }