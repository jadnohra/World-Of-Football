#include "entity/ball/Ball.h"
#include "EventDispatcher.h"

#include "Match.h"

namespace WOF { namespace match {

void EventDispatcher::init(Match& match) {

	mMatch = &match;
}

void EventDispatcher::onBallSwitchController(Ball& ball) {

	//mMatch->getBallSimulManager().onBallSwitchController(ball);
}

void EventDispatcher::onBallCommand(Ball& ball) {

	//mMatch->getBallSimulManager().onBallCommand(ball);
}
void EventDispatcher::onBallOwnershipChange(Ball& ball) {

	//mMatch->getBallSimulManager().onBallOwnershipChange(ball);
}

void EventDispatcher::onActiveFootballersChange() {

	mMatch->onActiveFootballersChange();
	mMatch->getControlSwitcher().onActiveFootballersChange();
}

} }