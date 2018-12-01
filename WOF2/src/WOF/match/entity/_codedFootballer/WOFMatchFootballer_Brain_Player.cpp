#include "WOFMatchFootballer_Brain_Player.h"

#include "../../WOFMatch.h"
#include "WOFMatchFootballer.h"
#include "../../WOFMatchTeam.h"
#include "../../WOFMatchPlayer.h"
#include "../camera/WOFMatchSceneCamera.h"


namespace WOF { namespace match {

Footballer_Brain_Player::Footballer_Brain_Player() {
}

InputController* Footballer_Brain_Player::getAttachedInputController() {

	return mInputInterpreter.getController();
}

Player* Footballer_Brain_Player::getAttachedPlayer() {

	if (mInputInterpreter.isValid())
		return mInputInterpreter.getController()->getPlayer();
	
	return NULL;
}

void Footballer_Brain_Player::cancel_init_create() {
} 

bool Footballer_Brain_Player::init_create(Match& match, Footballer_Brain& brain, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return true;
}

bool Footballer_Brain_Player::init_nodeAttached(Match& match, Footballer_Brain& brain, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return true;
}

void Footballer_Brain_Player::init_prepareScene(Match& match, Team& team, Footballer& footballer) {
}

void Footballer_Brain_Player::frameMove(Match& match, Team& team, Footballer_Brain& brain, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

	if (tickIndex != 0) {

		return;
	}

	if(!mInputInterpreter.frameMove(match, time)) {

		return;
	}

	SoftPtr<Footballer_Body::ActionImpl> currAction = brain.getAction();

	if (currAction.ptr() != mLastAction.ptr()) {

		if (mHandler.isValid()) {

			mHandler->detach(match, team, brain, time.t_discreet, mInputInterpreter, mLastAction);
		}

		mHandler = getHandlerFor(currAction->getType());

		if (mHandler.isValid()) {

			mHandler->attach(match, team, brain, time.t_discreet, mInputInterpreter, currAction);
			mHandler->frameMove(match, team, brain, time.t_discreet, mInputInterpreter, currAction);
		}

	} else {

		if (currAction.isValid()) {

			if (!mHandler.isValid()) {

				mHandler = getHandlerFor(currAction->getType());

				if (mHandler.isValid())
					mHandler->attach(match, team, brain, time.t_discreet, mInputInterpreter, currAction);
			}

			if (mHandler.isValid()) {

				mHandler->frameMove(match, team, brain, time.t_discreet, mInputInterpreter, currAction);
			}
		}
	}

	mLastAction = currAction;
}

void Footballer_Brain_Player::render(Match& match, Team& team, Footballer_Brain& brain, Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass) {

	/*

	int dbgX = (brain.mTeam == _TeamA) ? 100 : 600;
	int dbgY = 200 + brain.mNumber * 25;
	
	String str;

	Footballer::BallOwnership& own = brain.getBallOwnership();

	if (own.isBallOwner) {

		str.assignEx(L">[%d-%d] Body State [%s] Ball [%g - %g] [%f]", 
					(int) brain.mTeam, (int) brain.mNumber, 
					Footballer_Body::toString(brain.body_getCurrState()),
					match.getCoordSys().invConvUnit(brain.getRelBallDist()),
					radToDeg(brain.getRelBallAngle()),
					own.ballControl
					);

	} else {

		str.assignEx(L">[%d-%d] Body State [%s] Ball [%g]", 
					(int) brain.mTeam, (int) brain.mNumber, 
					Footballer_Body::toString(brain.body_getCurrState()),
					match.getCoordSys().invConvUnit(brain.getRelBallDist())
					);
	}

	{
		String switchStr;

		match.getControlSwitcher().getFootballerDebugText(match, brain, switchStr);

		if (switchStr.isValid()) {
			
			str.append(L" Switch ");
			str.append(switchStr);
		}
	}

	renderer.drawText(str.c_tstr(), dbgX, dbgY);

	*/
}

/*
void Footballer_Brain_Player::attach(Match& match, Team& team, Footballer_Brain& brain, InputController& inputController) {

	assrt(mInputInterpreter.getController() == NULL);

	mInputInterpreter.attachController(inputController, match.getTime().mTime);

	mLastAction.destroy();
	mHandler.destroy();

	mReceivedAnyPlayerCommands = false;
}

void Footballer_Brain_Player::detach(Match& match, Team& team, Footballer_Brain& brain) {

	mInputInterpreter.detachController();

	mLastAction.destroy();
	mHandler.destroy();
}
*/

bool Footballer_Brain_Player::receivedAnyPlayerCommands() {

	return mReceivedAnyPlayerCommands;
}

} }

