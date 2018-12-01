#include "WOFMatchControlSwitcher.h"

#include "../WOFMatch.h"
#include "../WOFMatchObjectIncludes.h"

#include "WOFMatchControlSwitchEvaluator_BallPathIntercept.h"

namespace WOF { namespace match {


void ControlSwitcher::init(Match& match) {

	mMatch = &match;

	mFrameProcessingSetup.init(match.getTime(), 30.0f, 1, true);
	mFootballerIter.init(match);

	mEvaluator_BallPathIntercept[_TeamA].destroy();
	mEvaluator_BallPathIntercept[_TeamB].destroy();

	WE_MMemNew(mEvaluator_BallPathIntercept[_TeamA].ptrRef(), ControlSwitchEvaluator_BallPathIntercept());
	WE_MMemNew(mEvaluator_BallPathIntercept[_TeamB].ptrRef(), ControlSwitchEvaluator_BallPathIntercept());

	mEvaluator_BallPathIntercept[_TeamA]->init(match);
	mEvaluator_BallPathIntercept[_TeamB]->init(match);

	mEvaluator_BallPathIntercept[_TeamA]->resetFootballerIterator(&match.getTeam(_TeamA));
	mEvaluator_BallPathIntercept[_TeamB]->resetFootballerIterator(&match.getTeam(_TeamB));

	onActiveFootballersChange();
}

FootballerIterator& ControlSwitcher::getFootballerIterator() {

	return mMatch->getTeam(_TeamA); //mFootballerIter;
}

void ControlSwitcher::onActiveFootballersChange() {

	mEvaluator_BallPathIntercept[_TeamA]->resetFootballerIterator(&mMatch->getTeam(_TeamA));
	mEvaluator_BallPathIntercept[_TeamB]->resetFootballerIterator(&mMatch->getTeam(_TeamB));

	mFrameProcessingState.reset(mFrameProcessingSetup, getFootballerIterator().getFootballerCount());
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

/*
float calcEventTimeScore(const Time& currTime, const Time& endTime, const Time& evtTime) {

	float totalValue = 0.0f;

	{
		const float timeInTrajectoryWeight = 50.0f;

		float value = 1.0f - ((evtTime - currTime) / (endTime - currTime));

		totalValue += value * timeInTrajectoryWeight;
	}


	{
		const float currTimeProximityWeight = 50.0f;
		const float cap = 1.0f;

		float value = fabs(evtTime - currTime);

		value = cap - value;

		if (value < 0.0f)
			value = 0.0f;

		value = value / cap;

		totalValue += value * currTimeProximityWeight;
	}

	return totalValue;
	
}

void ControlSwitcher::updateSwitchScore(const Time& currTime, Footballer& footballer, ScoreUpdateState& updateState, BallSimulManager::Simul* pSimul) {

	SwitchControl::FootballerState& switchState = footballer.mSwitchState;

	switchState.switchScore = 0.0f;

	if (mValueValidity.isValidPathIntercept) {

		SoftPtr<SwitchControl::Value_PathIntercept> pathIntercept = switchState.getValue_PathIntercept(footballer, pSimul);

		if (pathIntercept.isValid()) {

			const float timeTolerance = 0.7f; //XXXX also used in WOFMatchControlSwitchEvaluator_BallPathIntercept.cpp

			Time simulEndTime;
			
			dref(pSimul).getEndTime(simulEndTime);

			float scoreTime = 0.0f;
			float scoreDist = 0.0f;
			int scoreCount = 0;

			float baseScore = 0.0f;

			if (pathIntercept->shortestTimeIntercept.checkValidity(currTime)) {

				float value = pathIntercept->shortestTimeIntercept.interceptTime - currTime;
				
				//also cancel events with time to react expired 
				//in PathInterception add maxTimeToReact ... i.e player needs to be reacting by this time

				const float weight = 100.0f;
				const float cap = 2.5f;
				const float base = 30.0f;

				value = cap - value;

				if (value < 0.0f)
					value = 0.0f;

				value = value / cap;

				baseScore += base;
				scoreTime = base + weight * value;
				scoreTime += calcEventTimeScore(currTime, simulEndTime, pathIntercept->shortestTimeIntercept.interceptTime);
				++scoreCount;
			}

			if (pathIntercept->shortestDistIntercept.checkValidity(currTime)) {

				float value = pathIntercept->shortestDistIntercept.interceptDist;

				const float weight = 100.0f;
				const float cap = 10.0f * 100.0f;
				const float base = 30.0f;

				value = cap - value;

				if (value < 0.0f)
					value = 0.0f;

				value = value / cap;

				baseScore += base;
				scoreDist = base + weight * value;
				scoreDist += calcEventTimeScore(currTime, simulEndTime, pathIntercept->shortestDistIntercept.interceptTime);
				++scoreCount;
			}

			if ((scoreCount == 2)
					&& (pathIntercept->shortestTimeIntercept.equals(pathIntercept->shortestDistIntercept, timeTolerance)) ) {

				switchState.switchScore += MMax(scoreTime, scoreDist);

			} else {

				switchState.switchScore += scoreTime + scoreDist;
			}

			if (switchState.switchScore <= baseScore) {

				if (pathIntercept->restBallIntercept.checkValidity(currTime)) {

					float value = pathIntercept->restBallIntercept.interceptDist;

					const float weight = 25.0f;
					const float cap = 30.0f * 100.0f;

					value = cap - value;

					if (value < 0.0f)
						value = 0.0f;

					value = value / cap;

					switchState.switchScore += weight * value;
					//scoreTime += calcEventTimeScore(currTime, simulEndTime, pathIntercept->restBallIntercept.interceptTime);
					++scoreCount;
				}
			}
		}

	} else {

		{
			float value;

			switchState.getValue_CurrBallDist(footballer, value); 

			const float cap = 30.0f * 100.0f;

			value = cap - value;

			if (value < -cap)
				value = -cap;

			value = value / cap;

			switchState.switchScore += 100.0f * value;
		}

		{
			float value;

			switchState.getValue_BallOwnership(footballer, value); 

			switchState.switchScore += 100.0f * value;
		}
	}
}
*/

/*
void ControlSwitcher::considerFootballer(Footballer& footballer) {

	int teamIndex = (int) footballer.mTeam;

	if (!mTop2Footballers[teamIndex][0].isValid() || mTop2Footballers[teamIndex][0]->mSwitchState.switchScore < footballer.mSwitchState.switchScore) {

		if (mTop2Footballers[teamIndex][1] == &footballer)
			mTop2Footballers[teamIndex][1].destroy();
		//here BUG we might need to require an additional footballer or make a fully sorted list
		//actually it is also ok like this since we dont deatch the footballer if topfootballer is null

		mTop2Footballers[teamIndex][0] = &footballer;
		return;
	}

	if (!mTop2Footballers[teamIndex][1].isValid() || mTop2Footballers[teamIndex][1]->mSwitchState.switchScore < footballer.mSwitchState.switchScore) {

		mTop2Footballers[teamIndex][1] = &footballer;
		return;
	}
}
*/

/*
void ControlSwitcher::updatePlayerInfluence(Footballer& footballer, ScoreUpdateState& updateState) {

	SwitchControl::FootballerState& state = footballer.mSwitchState;

	SoftPtr<Player> player = footballer.getAttachedPlayer();

	if (player.isValid()) {

		const float penalty = -100.0f;
		const float reward = 150.0f;
		const float smallReward = 20.0f;

		const float maxEffectTime = 2.0f;

		Time interactionTime;
		Footballer::BallInteraction interaction = footballer.brain_getAttachedLastBallInteraction(interactionTime);

		if (interactionTime < maxEffectTime) {

			switch (interaction) {

				case Footballer::FBI_ActiveShot: {

					state.playerScoreInfluence = penalty;

				} break;

				case Footballer::FBI_PassiveShot:
				case Footballer::FBI_Controlled: {

					state.playerScoreInfluence = reward;

				} break;

				case Footballer::FBI_UncontrolledCollision: {

					state.playerScoreInfluence = smallReward;

				} break;

				default: {

					state.playerScoreInfluence = 0.0f;

				} break;
			}

		} else {

			state.playerScoreInfluence = 0.0f;
		}

		if (footballer.body_getCurrState() == Footballer::BS_Running) {

			state.playerScoreInfluence += reward;
		}

	} else {

		state.playerScoreInfluence = 0.0f;
	}

}

void ControlSwitcher::insertScoreSorted(Footballer& footballer, SoftFootballerArray& sortedTopArray) {

	float footballerScore = footballer.mSwitchState.getTotalScore();

	for (SoftFootballerArray::Index i = 0; i < sortedTopArray.count; ++i) {

		if (footballerScore > sortedTopArray.el[i]->mSwitchState.getTotalScore()) {
		
			sortedTopArray.addOneAtReserved(i, &footballer);
			return;
		}
	}

	sortedTopArray.addOneReserved(&footballer);
}

void ControlSwitcher::updateTopArray(Team& team, ScoreUpdateState& updateState) {

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	SoftFootballerArray& topArray = mTopFootballers[team.getTeamEnum()];

	topArray.count = 0;
	topArray.reserve(team.getFootballerCount());

	while (team.iterateFootballers(footballer.ptrRef(), iter)) {

		updatePlayerInfluence(footballer, updateState);
		insertScoreSorted(footballer, topArray);
	}
}

void ControlSwitcher::updateTopArray_BallPathIntercept(Team& team, ScoreUpdateState& updateState) {

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	SoftFootballerArray& topArray = mTopFootballers[team.getTeamEnum()];

	topArray.count = 0;
	topArray.reserve(team.getFootballerCount());

	while (team.iterateFootballers(footballer.ptrRef(), iter)) {

		updatePlayerInfluence(footballer, updateState);
		insertScoreSorted(footballer, topArray);
	}
}
*/

bool ControlSwitcher::playerWantsToSwitch(Match& match, Player& player) {

	SoftPtr<Footballer> footballer = player.getAttachedFootballer();
		
	if (footballer.isValid()) {

		if (footballer->body_getCurrState() == Footballer::BS_Running) {

			return false;
		}

		SwitchControl::FootballerState& state = footballer->mSwitchState;

		state.playerScoreInfluence = 1.0f;

		Time interactionTime;
		Footballer::BallInteraction interaction = footballer->brain_getAttachedLastBallInteraction(interactionTime);

		if (interactionTime < match.mSwitchControlSetup.playerControlEffectInterval) {

			switch (interaction) {

				case Footballer::FBI_ActiveShot: {

					state.playerScoreInfluence = -10.0f;
					return true;

				} break;

				case Footballer::FBI_PassiveShot:
				case Footballer::FBI_Controlled: {

					state.playerScoreInfluence = 10.0f;
					return false;

				} break;

				case Footballer::FBI_UncontrolledCollision: {

					state.playerScoreInfluence = 1.0f;

					//or better give the footballer a small reward
					return true;

				} break;

				default: {

				} break;
			}

		} 

		return true;
	} 

	return true;
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

	for (int i = 0; i < 2; ++i) {

		Team& team = match.getTeam((TeamEnum) i);

		SoftPtr<Footballer> footballer;
		FootballerIndex iter;

		SoftFootballerArray& topArray = mTopFootballers[i];

		topArray.count = 0;
		topArray.reserve(team.getFootballerCount());

		SoftPtr<SwitchControl::Value_PathIntercept> pathIntercept;

		while (team.iterateFootballers(footballer.ptrRef(), iter)) {

			pathIntercept = footballer->mSwitchState.getValue_PathIntercept(footballer, currSimul);

			if (pathIntercept.isValid()) {

				insertScoreSorted_ModePathIntercept(match, footballer, topArray, currSimul, time);
			}
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

	for (int i = 0; i < 2; ++i) {

		Team& team = match.getTeam((TeamEnum) i);

		SoftPtr<Footballer> footballer;
		FootballerIndex iter;

		SoftFootballerArray& topArray = mTopFootballers[i];

		topArray.count = 0;
		topArray.reserve(team.getFootballerCount());

		SoftPtr<SwitchControl::Value_PathIntercept> pathIntercept;

		while (team.iterateFootballers(footballer.ptrRef(), iter)) {

			insertScoreSorted_ModeBallDist(match, footballer, topArray, time);
		}
	}
}

void ControlSwitcher::updateTopFootballerArray(Match& match, const Time& time) {

	/*
	ok here we have 2 modes, pathIntercept valid mode and default mode
		in pathIntercept mode its useless to assign scores becuadse it 
		all depends on relations between footballer times,
		so we just insertSort them with the appropriate comparison fcntiuons
		where actually all the functionality is and 
		playerinfluence is also considered there probably

	for the default mode we probably use ball disrt andf all the usual stuff

	watch for mode transtions ... all scores need to be invalidted or so ...
	after that try to make it work with multi frame in some efficient way if possible
	also during partial paths ...
	*/

	for (int i = 0; i < 2; ++i) {

		Team& team = match.getTeam((TeamEnum) i);

		mEvaluator_BallPathIntercept[team.getTeamEnum()]->updateValidity(match, time, mValueValidity);

		SoftPtr<Footballer> footballer;
		FootballerIndex iter;

		while (team.iterateFootballers(footballer.ptrRef(), iter)) {

			SoftPtr<Player> player = footballer->brain_getAttachedPlayer();

			if (!player.isValid()) {

				footballer->mSwitchState.playerScoreInfluence = 0.0f;
			}
		}
	}


	if (mValueValidity.isValidPathIntercept) {

		for (int i = 0; i < 2; ++i) {

			Team& team = match.getTeam((TeamEnum) i);
		
			mEvaluator_BallPathIntercept[team.getTeamEnum()]->frameMove(match, time, mValueValidity);
		}

		updateTopFootballerArray_ModePathIntercept(match, time);
		
	} else {

		updateTopFootballerArray_ModeBallDist(match, time);
	}
}

void ControlSwitcher::frameMove(Match& match, const Time& time) {

	DWORD t = timeGetTime();

	PlayerManager::TempPlayers players[2];

	bool needUpdateTopArray = false;

	for (int i = 0; i < 2; ++i) {

		match.getPlayerManager().getTeamPlayers((TeamEnum) i, players[i]);
		PlayerManager::sortBySwitchPriority(players[i], false);

		for (PlayerManager::TempPlayers::Index pi = 0; pi < players[i].count; ++pi) {

			Player& player = dref(players[i].el[pi]);
			SoftPtr<Footballer> attachedFootballer = player.getAttachedFootballer(); 

			player.hSwitchState.doSwitch = false;

			if (attachedFootballer.isValid()) {

				Time attachedInterval = time - attachedFootballer->brain_getAttachTime();

				if (attachedInterval >= match.mSwitchControlSetup.minSwitchInterval) {

					player.hSwitchState.doSwitch = playerWantsToSwitch(match, player);

				} else {

					player.hSwitchState.doSwitch = false;
				}

			} else {

				player.hSwitchState.doSwitch = true;
			}

			if (player.hSwitchState.doSwitch) {

				needUpdateTopArray = true;
			}
		}
	}

	if (!needUpdateTopArray) {

		return;
	}

	//cache the doswitch in player - switchstate struct

	updateTopFootballerArray(match, time);

	for (int i = 0; i < 2; ++i) {

		SoftFootballerArray& topFootballers = mTopFootballers[i];

		for (PlayerManager::TempPlayers::Index pi = 0; pi < players[i].count; ++pi) {

			Player& player = dref(players[i].el[pi]);
			SoftPtr<Footballer> attachedFootballer = player.getAttachedFootballer(); 

			if (player.hSwitchState.doSwitch && (pi < topFootballers.count)) {

				player.attachFootballer(dref(topFootballers.el[pi]));

			} else {

				//not enough footballers
				break;
			}
		}
	}

	DWORD dt = timeGetTime() - t;

	if (dt)
		log(L"%d", (int) dt);
}

Footballer* ControlSwitcher::switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch) {

	if (technique == FST_Default) {

		technique = mMatch->mSwitchControlSetup.technique;
	}

	/*
	SoftPtr<Footballer> owner =  switch_Owner(team, player, pCurrFootballer, isManualSwitch);

	if (owner.isValid()) {

		return owner;
	}

	switch (technique) {

		case FST_ShotAnalysis: {

			return switch_ShotAnalysis(team, player, pCurrFootballer, isManualSwitch, mState.shotAnalysis);

		} break;

		case FST_BallDist: {

			return switch_BallDist(team, player, pCurrFootballer, isManualSwitch);
			
		} break;

		case FST_NextNumber:
		default: {

			return switch_NextNumber(team, player, pCurrFootballer, isManualSwitch);
			
		} break;
	}
	*/


	return NULL;
}

void ControlSwitcher::render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass) {

	/*
	if (flagExtraRenders) {

		for (State_ShotAnalysis::Trajectories::Index i = 0; i < mState.shotAnalysis.trajectories.count; ++i) {

			renderer.draw(mState.shotAnalysis.trajectories.el[i].plane, 100.0f, &Matrix4x3Base::kIdentity);
		}
	}
	*/
}

} }