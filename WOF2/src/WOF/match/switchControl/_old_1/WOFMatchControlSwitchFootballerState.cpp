#include "WOFMatchControlSwitchFootballerState.h"

#include "../entity/footballer/WOFMatchFootballer.h"

namespace WOF { namespace match {

bool SwitchControl::FootballerState::getValue(Footballer_Brain& footballer, const ValueID& ID, float& value) const {

	return false;
}

bool SwitchControl::FootballerState::getValue(Footballer_Brain& footballer, const ValueID& ID, const Time& currTime, float& value, Time& time) const {

	return false;
}

void SwitchControl::FootballerState::getValue_PlayerControlled(Footballer_Brain& footballer, float& value) const {

	value = footballer.brain_isAI() ? 0.0f : 1.0f;
}

void SwitchControl::FootballerState::getValue_PlayerControlled(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const {

	time = currTime;

	value = footballer.brain_isAI() ? 0.0f : 1.0f;
}

void SwitchControl::FootballerState::getValue_BallOwnership(Footballer_Brain& footballer, float& value) const {

	Footballer::BallOwnership& ownership = footballer.getBallOwnership();

	value = ownership.isBallOwner ? ownership.ballControl : 0.0f;
}

void SwitchControl::FootballerState::getValue_BallOwnership(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const {

	time = currTime;

	Footballer::BallOwnership& ownership = footballer.getBallOwnership();

	value = ownership.isBallOwner ? ownership.ballControl : 0.0f;
}

void SwitchControl::FootballerState::getValue_CurrBallDist(Footballer_Brain& footballer, float& value) const {

	value = footballer.getRelBallDist();
}

void SwitchControl::FootballerState::getValue_CurrBallDist(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const {

	time = currTime;

	value = footballer.getRelBallDist();
}

void SwitchControl::FootballerState::getValue_CurrBallRelHeight(Footballer_Brain& footballer, float& value) const {

	value = footballer.getRelBallPos().el[Scene_Up];
}

void SwitchControl::FootballerState::getValue_CurrBallRelHeight(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const {

	time = currTime;

	value = footballer.getRelBallPos().el[Scene_Up];
}

void SwitchControl::FootballerState::getValue_CurrBallRelAngle(Footballer_Brain& footballer, float& value) const {

	value = footballer.getRelBallAngle();
}

void SwitchControl::FootballerState::getValue_CurrBallRelAngle(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const {

	time = currTime;

	value = footballer.getRelBallAngle();
}

bool SwitchControl::FootballerState::getValue_BallPathInterceptTime(Footballer_Brain& footballer, float& value) const {

	const Value& val = computed[VID_BallPathInterceptTime];

	if (val.isValid) {

		value = val.val;

		return true;
	}

	return false;
}

bool SwitchControl::FootballerState::getValue_BallPathInterceptTime(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const {

	const Value& val = computed[VID_BallPathInterceptTime];

	if (val.isValid) {

		value = val.val;
		time = val.time;

		return true;
	}

	return false;
}


bool SwitchControl::FootballerState::getValue_BallPathInterceptDist(Footballer_Brain& footballer, float& value) const {

	const Value& val = computed[VID_BallPathInterceptDist];

	if (val.isValid) {

		value = val.val;

		return true;
	}

	return false;
}

bool SwitchControl::FootballerState::getValue_BallPathInterceptDist(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const {

	const Value& val = computed[VID_BallPathInterceptDist];

	if (val.isValid) {

		value = val.val;
		time = val.time;

		return true;
	}

	return false;
}


bool SwitchControl::FootballerState::getValue_RestBallDist(Footballer_Brain& footballer, float& value) const {

	const Value& val = computed[VID_RestBallDist];

	if (val.isValid) {

		value = val.val;

		return true;
	}

	return false;
}

bool SwitchControl::FootballerState::getValue_RestBallDist(Footballer_Brain& footballer, const Time& currTime, float& value, Time& time) const {

	const Value& val = computed[VID_RestBallDist];

	if (val.isValid) {

		value = val.val;
		time = val.time;

		return true;
	}

	return false;
}

} }