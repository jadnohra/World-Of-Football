#include "WOFMatchControlSwitchStates.h"

#include "../entity/footballer/WOFMatchFootballer.h"

namespace WOF { namespace match {

SwitchControl::PathInterception::PathInterception() : isValid(false) {
}

bool SwitchControl::PathInterception::checkValidity(const Time& currTime) {

	if (isValid && currTime > interceptTime)
		isValid = false;
	
	return isValid;
}

bool SwitchControl::PathInterception::equals(const PathInterception& comp, const Time& tolerance) {

	if (isValid && comp.isValid) {

		return fabs(interceptTime - comp.interceptTime) <= tolerance;
	}

	return false;
}

SwitchControl::FootballerState::FootballerState() : switchScore(-FLT_MAX), playerScoreInfluence(0.0f) {
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

bool SwitchControl::Value_PathIntercept::isValid(BallSimulManager::Simul* pSimul) const {

	return ((pSimul != NULL) && (pSimul->getID() == simulID));
}

SwitchControl::Value_PathIntercept* SwitchControl::FootballerState::getValue_PathIntercept(Footballer& footballer, BallSimulManager::Simul* pSimul) {

	return value_pathIntercept.isValid(pSimul) ? &value_pathIntercept : NULL;
}


} }