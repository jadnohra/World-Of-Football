#include "WOFMatchControlSwitchStates.h"

#include "../WOFMatch.h"
#include "../entity/footballer/WOFMatchFootballer.h"
#include "../WOFMatchPlayer.h"

namespace WOF { namespace match {

SwitchControl::PathInterception::PathInterception() : isValid(false), isExpired(false) {
}

/*
bool SwitchControl::PathInterception::checkValidity(const Time& currTime) {

	if (isExpired || !isValid)
		return false;

	if (currTime > interceptTime)
		isExpired = false;
	
	return isExpired;
}
*/

bool SwitchControl::PathInterception::equals(const PathInterception& comp, const Time& tolerance) {

	if (isValid && comp.isValid) {

		return fabs(interceptTime - comp.interceptTime) <= tolerance;
	}

	return false;
}


SwitchControl::FootballerState::FootballerState() : switchScore(-FLT_MAX) {
}

float SwitchControl::FootballerState::getTotalScore(Footballer& footballer) { 

	SoftPtr<Player> player = footballer.getPlayer();

	if (player.isValid()) {

		return switchScore + player->hSwitchState.footballerScoreInfluence; 
	}

	return switchScore; 
}

float SwitchControl::FootballerState::getPlayerScoreInfluence(Footballer& footballer) {

	SoftPtr<Player> player = footballer.getPlayer();

	if (player.isValid()) {

		return player->hSwitchState.footballerScoreInfluence; 
	}

	return 0.0f; 
}

void SwitchControl::FootballerState::getValue_PlayerControlled(Footballer& footballer, float& value) const {

	value = footballer.isInAIMode() ? 0.0f : 1.0f;
}

void SwitchControl::FootballerState::getValue_BallOwnership(Footballer& footballer, float& value) const {

	BallOwnership& ownership = footballer.getBallInteraction().ownership;

	value = ownership.type == BallOwnership_None ? 0.0f : 1.0f;
}

#ifdef FOOTBALLER_BODY3

void SwitchControl::FootballerState::getValue_CurrBallDist(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.dist;
}

void SwitchControl::FootballerState::getValue_CurrBallRelHeight(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.pos.el[Scene_Up];
}
void SwitchControl::FootballerState::getValue_CurrBallRelAngle(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.angle;
}

#endif

#ifdef FOOTBALLER_BODY2

void SwitchControl::FootballerState::getValue_CurrBallDist(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.dist;
}

void SwitchControl::FootballerState::getValue_CurrBallRelHeight(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.pos.el[Scene_Up];
}
void SwitchControl::FootballerState::getValue_CurrBallRelAngle(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.angle;
}

#endif

#ifdef FOOTBALLER_BODY1

void SwitchControl::FootballerState::getValue_CurrBallDist(Footballer& footballer, float& value) const {

	value = footballer.getRelBallDist();
}

void SwitchControl::FootballerState::getValue_CurrBallRelHeight(Footballer& footballer, float& value) const {

	value = footballer.getRelBallPos().el[Scene_Up];
}
void SwitchControl::FootballerState::getValue_CurrBallRelAngle(Footballer& footballer, float& value) const {

	value = footballer.getRelBallAngle();
}

#endif

SwitchControl::Value_PathIntercept* SwitchControl::FootballerState::getValue_PathIntercept(Footballer& footballer) {

	return &value_pathIntercept;
}

bool SwitchControl::Value_PathIntercept::isValid(BallSimulation* pSimul) const {

	return ((pSimul != NULL) && (pSimul->getID() == simulID));
}

float SwitchControl::Value_PathIntercept::getInterceptTime(const Time& currTime) const {

	if (shortestTimeIntercept.isValid) {

		return MMax(shortestTimeIntercept.interceptTime, currTime);
	}

	if (restBallIntercept.isValid) {

		return restBallIntercept.interceptTime;
	}

	return FLT_MAX;
}

float SwitchControl::Value_PathIntercept::getInterceptDist(const Time& currTime) const {

	if (shortestTimeIntercept.isValid) {

		if (currTime > shortestTimeIntercept.interceptTime) {

			if (restBallIntercept.isValid) {

				return lerp(shortestTimeIntercept.interceptDist, restBallIntercept.interceptDist, (currTime - shortestTimeIntercept.interceptTime) / (restBallIntercept.interceptTime - shortestTimeIntercept.interceptTime));
			} 

		} else {

			return shortestTimeIntercept.interceptDist;
		}
	}

	if (restBallIntercept.isValid) {

		return restBallIntercept.interceptDist;
	}

	return FLT_MAX;
}

SwitchControl::Value_PathIntercept* SwitchControl::FootballerState::getValue_PathIntercept(Footballer& footballer, BallSimulation* pSimul) {

	return value_pathIntercept.isValid(pSimul) ? &value_pathIntercept : NULL;
}


} }