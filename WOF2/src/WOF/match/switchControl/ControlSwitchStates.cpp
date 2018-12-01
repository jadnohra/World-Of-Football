#include "ControlSwitchStates.h"

#include "../Match.h"
#include "../entity/footballer/Footballer.h"
#include "../Player.h"
#include "../entity/ball/simul/BallSimulation.h"

namespace WOF { namespace match {


SwitchControl::FootballerState::FootballerState() {
}

float SwitchControl::FootballerState::getTotalScore(Footballer& footballer) { 

	SoftPtr<Player> player = footballer.getPlayer();

	if (player.isValid()) {

		return player->hSwitchState.footballerScoreInfluence; 
	}

	return 0.0f; 
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

void SwitchControl::FootballerState::getValue_CurrBallDist(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.getFootballerDist();
}

void SwitchControl::FootballerState::getValue_CurrBallRelHeight(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.getHeight();
}
void SwitchControl::FootballerState::getValue_CurrBallRelAngle(Footballer& footballer, float& value) const {

	value = footballer.getBallInteraction().relativeDist.getFootballerHorizAngle();
}


} }