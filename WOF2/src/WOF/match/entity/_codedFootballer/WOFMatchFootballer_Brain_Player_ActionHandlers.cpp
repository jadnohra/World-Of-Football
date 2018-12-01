#include "WOFMatchFootballer_Brain_Player.h"

#include "WOFMatchFootballer_Brain.h"
#include "../../WOFMatch.h"

namespace WOF { namespace match {

Footballer_Brain_Player::ActionHandler* Footballer_Brain_Player::getHandlerFor(const Footballer_Body::ActionType& actionType) {

	/*
	switch (actionType) {

		case Footballer_Body::Action_Idle: return &mHandler_Default;
		case Footballer_Body::Action_Running: return &mHandler_Default;
		case Footballer_Body::Action_Tackling: return &mHandler_Default;
		case Footballer_Body::Action_Falling: return &mHandler_Default;
		case Footballer_Body::Action_Jumping: return &mHandler_Default;
	}

	assrt(false);
	*/

	return &mHandler_Default;
}

const float& Footballer_Brain_Player::InputStateInterpreter::getReactionTime() { 
	
	return dref(mController->getPlayer()).getInputReactionTime(); 
}

/*
void Footballer_Brain_Player::InputStateInterpreter::attachController(InputController& controller, const DiscreeteTime& time) {

	mController = &controller;
	mController->updateDevice(time.t_discreet);
}

void Footballer_Brain_Player::InputStateInterpreter::detachController() {

	mController.destroy();
}
*/

bool Footballer_Brain_Player::InputStateInterpreter::frameMove(Match& match, const DiscreeteTime& time) {

	mController->frameMove(match, time);

	if(getInputState().valid == false) {

		assrt(false);
		return false;
	}

	return true;
}

const Vector3& Footballer_Brain_Player::InputStateInterpreter::getMoveDir(Match& match, Vector3& temp) {

	const Vector3* pDir;

	if (match.mTweakCameraRelativeInput) {

		match.getActiveCamera().relativiseDirection(getMoveDir(), temp, true, match.mTweakCameraRelativeInputDiscreeteDirections);

		pDir = &temp;

	} else {

		pDir = &getMoveDir();
	}

	return *pDir;
}

Footballer_Body::ShotType Footballer_Brain_Player::InputStateInterpreter::getShootType() {

	Footballer_Body::ShotType type = Footballer_Body::Shot_None;
		
	if (getInputState().tackle[2].state) {

		type = Footballer_Body::Shot_Extra;

	} else if (getInputState().tackle[1].state) {

		type = Footballer_Body::Shot_High;

	} else if (getInputState().tackle[0].state) {

		type = Footballer_Body::Shot_Low;
	} 

	return type;
}

Footballer_Body::RunType Footballer_Brain_Player::InputStateInterpreter::getRunType() {

	if (getInputState().move.state) {

		return Footballer_Body::Run_Normal;
	}

	return Footballer_Body::Run_None;
}

Footballer_Body::RunSpeedType Footballer_Brain_Player::InputStateInterpreter::getRunSpeedType() {

	if (getInputState().sprint.state) {

		return Footballer_Body::RunSpeed_Sprint;
	}

	return Footballer_Body::RunSpeed_Normal;
}

Footballer_Body::JumpType Footballer_Brain_Player::InputStateInterpreter::getJumpType() {

	Footballer_Body::JumpType type = Footballer_Body::Jump_None;
		
	if (getInputState().jump[0].state) {

		type = Footballer_Body::Jump_Up;

	} else if (getInputState().jump[1].state) {

		type = Footballer_Body::Jump_UpLeft;

	} else if (getInputState().jump[2].state) {

		type = Footballer_Body::Jump_UpRight;
	} 

	return type;
}

void Footballer_Brain_Player::InputStateInterpreter::syncJumpParams(Match& match, Footballer_Brain& brain, const Time& time, Footballer_BodyAction_Jumping& action, bool force) {

	Vector3 tempDir;
	
	const Vector3& dir = getMoveDir(match, tempDir);
	
	action.setJumpTypeFromTargetBodyDir(brain, time, &dir);
}

void Footballer_Brain_Player::InputStateInterpreter::syncRunParams(Match& match, Footballer_Brain& brain, const Time& time, Footballer_BodyAction_Running& action, bool force) {

	InputControllerState& input = getInputState();

	if (force || !input.move.synced || !input.modifier.synced) {

		if (input.move.state) {

			bool synced = true;

			Vector3 tempDir;
			const Vector3& dir = getMoveDir(match, tempDir);

			if (!getModifier())
				synced = synced && action.setBodyFacing(brain, time, dir);

			synced = synced && action.setRunDir(brain, time, dir);

			if (synced) {

				input.move.synced = true;
				input.modifier.synced = true;

			} else {

				action.cancelSetBodyFacing();
				action.cancelSetRunDir();
			}
		}
	}

	if (force || !input.sprint.synced) {

		bool synced = true;

		synced = synced && action.setRunSpeed(brain, time, getRunSpeedType());

		if (synced)
			input.sprint.synced = true;
	}
}

void Footballer_Brain_Player::InputStateInterpreter::syncTackleParams(Match& match, Footballer_Brain& brain, const Time& time, Footballer_BodyAction_Tackling& action, bool force) {

	InputControllerState& input = getInputState();

	if (force || !input.move.synced) {

		if (input.move.state) {

			bool synced = true;

			Vector3 tempDir;
			const Vector3& dir = getMoveDir(match, tempDir);

			Vector3 vel;

			if (brain.getActionType() == Footballer_Body::Action_Running) {

				dir.mul(brain.getActionRunning().getSpeedValue(Footballer_Body::Run_Normal, getRunSpeedType()), vel);

			} else {

				dir.mul(brain.getActionRunning().getSpeedValue(Footballer_Body::Run_Normal, Footballer_Body::RunSpeed_Normal), vel);
			}

			synced = synced && action.setTackleVel(brain, time, vel);

			if (synced) {

				input.move.synced = true;

			} else {

				action.cancelSetTackleVel();
			}

		} else {

			action.cancelSetTackleVel();
		}
	}
}

/*
Footballer_Body::ActionType Footballer_Brain_Player::ActionHandler_Default::getType() {

	return Footballer_Body::Action_Idle;
}
*/
Footballer_Brain_Player::ActionHandler_Default::ActionHandler_Default() {
}

void Footballer_Brain_Player::ActionHandler_Default::attach(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) {

	mTrackingAction = Footballer_Body::Action_None;
}

void Footballer_Brain_Player::ActionHandler_Default::frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) {

	Footballer::ActionImpl& currAction = curr;

	Footballer_Body::ShotType shootType = inputState.getShootType();
	Footballer_Body::RunType runType = inputState.getRunType();
	Footballer_Body::JumpType jumpType = inputState.getJumpType();

	Vector3 tempDir;

	bool processed = false;

	Footballer_Body::ActionType activeAction = Footballer_Body::Action_None;

	if (!processed) {

		if (shootType != Footballer_Body::Shot_None) {

			processed = true;

			activeAction = Footballer_Body::Action_Running;
		}
	}

	if (!processed) {
	
		if (jumpType != Footballer_Body::Jump_None) {

			processed = true;

			activeAction = Footballer_Body::Action_Jumping;
		}
	}

	if (!processed) {

		processed = true;

		if (runType != Footballer_Body::Run_None) {

			activeAction = Footballer_Body::Action_Running;

		} else {

			activeAction = Footballer_Body::Action_Idle;
		}
	}

	if (mTrackingAction != Footballer_Body::Action_None
		&& activeAction != mTrackingAction) {

		if (time - mTrackingActionStartTime < inputState.getReactionTime()) {

			//tracking action 'cancelled' by new action during reaction time
			//react accordingly, if nothign is done here, the tracking action
			//will not take effect (the player was faster than his own reaction time)
		}
	}

	processed = false;

	
	if (!processed) {

		/*
		if (shootType != Footballer_Body::Shot_None) {

			processed = true;

			Footballer_BodyAction_Tackling& actionTackling = brain.getActionTackling();

			if (runType != Footballer_Body::Run_None) {

				inputState.syncTackleParams(match, brain, time, actionTackling);
				currAction.switchActionTackling(brain, time);

			} else {

				actionTackling.cancelSetTackleVel();
				currAction.switchActionTackling(brain, time);
			}

		} else {
		}
		*/

		if (shootType != Footballer_Body::Shot_None) {

			processed = true;

			if (currAction.getType() != Footballer_Body::Action_Tackling) {

				if (mTrackingAction != Footballer_Body::Action_Tackling) {

					mTrackingAction = Footballer_Body::Action_Tackling;
					mTrackingActionStartTime = time;
				}

				if (time - mTrackingActionStartTime >= inputState.getReactionTime()) {

					mTrackingAction = Footballer_Body::Action_None;

					const Vector3& currDir = inputState.getMoveDir(match, tempDir);

					inputState.syncTackleParams(match, brain, time, brain.getActionTackling());
					currAction.switchActionTackling(brain, time);

				} else {
				}

			} else {

				inputState.syncTackleParams(match, brain, time, brain.getActionTackling(), true);
				currAction.switchActionTackling(brain, time);
			}
			
		} else {
		}
	}

	if (!processed) {
	
		if (jumpType != Footballer_Body::Jump_None) {

			processed = true;

			if (currAction.getType() != Footballer_Body::Action_Jumping) {

				if (mTrackingAction != Footballer_Body::Action_Jumping) {

					mTrackingAction = Footballer_Body::Action_Jumping;
					mTrackingActionStartTime = time;
				}

				if (time - mTrackingActionStartTime >= inputState.getReactionTime()) {

					mTrackingAction = Footballer_Body::Action_None;

					const Vector3& currDir = inputState.getMoveDir(match, tempDir);

					inputState.syncJumpParams(match, brain, time, brain.getActionJumping());
					currAction.switchActionJumping(brain, time);

				} else {
				}

			} else {

				inputState.syncJumpParams(match, brain, time, brain.getActionJumping(), true);
				currAction.switchActionJumping(brain, time);
			}
			
		} else {
		}
	} 
		
	if (!processed) {

		processed = true;

		if (runType != Footballer_Body::Run_None) {

			inputState.syncRunParams(match, brain, time, brain.getActionRunning());
			
			if (currAction.getType() != Footballer_Body::Action_Running) 
				currAction.switchActionRunning(brain, time);

		} else {

			if (currAction.getType() != Footballer_Body::Action_Idle) 
				currAction.switchActionIdle(brain, time);
		}
	}
}


/*
Footballer_Body::ActionType Footballer_Brain_Player::ActionHandler_Running::getType() {

	return Footballer_Body::Action_Running;
}
*/

void Footballer_Brain_Player::ActionHandler_Running::frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) {

	Footballer_BodyAction_Running& currAction = (Footballer_BodyAction_Running&) curr;

	Footballer_Body::ShotType shootType = inputState.getShootType();
	Footballer_Body::RunType runType = inputState.getRunType();

	if (shootType != Footballer_Body::Shot_None) {

		currAction.switchActionTackling(brain, time);
		
	} else {

		Footballer_Body::JumpType jumpType = inputState.getJumpType();

		if (jumpType != Footballer_Body::Jump_None) {

			Footballer_BodyAction_Jumping& actionJumping = brain.getActionJumping();

			if (jumpType != Footballer_Body::Jump_None) {

				inputState.syncJumpParams(match, brain, time, actionJumping);
				currAction.switchActionJumping(brain, time);

			} else {

				actionJumping.cancelSetJumpType();
				currAction.switchActionJumping(brain, time);
			} 

		} else if (runType != Footballer_Body::Run_None) {

			inputState.syncRunParams(match, brain, time, currAction);

		} else {

			currAction.switchActionIdle(brain, time);
		}
	}
}


/*
Footballer_Body::ActionType Footballer_Brain_Player::ActionHandler_Tackling::getType() {

	return Footballer_Body::Action_Tackling;
}
*/

void Footballer_Brain_Player::ActionHandler_Tackling::frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) {

	Footballer_BodyAction_Tackling& currAction = (Footballer_BodyAction_Tackling&) curr;

	Footballer_Body::ShotType shootType = inputState.getShootType();
	Footballer_Body::RunType runType = inputState.getRunType();
	Footballer_Body::JumpType jumpType = inputState.getJumpType();

	const Vector3* pTackleStartVel = currAction.getSetTackleVel();
	bool tackleHasStartVel = (pTackleStartVel != NULL && !pTackleStartVel->isZero());
	bool forceSync = inputState.hasMoveDir() != tackleHasStartVel;

	if (forceSync || !inputState.getInputState().move.synced) {

		inputState.syncTackleParams(match, brain, time, currAction, true);
		inputState.syncRunParams(match, brain, time, brain.getActionRunning(), true);
	}

	if (shootType != Footballer_Body::Shot_None) {
		
		currAction.switchActionTackling(brain, time);
		
	} else if (shootType != Footballer_Body::Shot_None) {
	
		Footballer_BodyAction_Jumping& actionJumping = brain.getActionJumping();
		inputState.syncJumpParams(match, brain, time, actionJumping);
		currAction.switchActionJumping(brain, time);

	} else {

		if (runType != Footballer_Body::Run_None) {
			
			currAction.switchActionRunning(brain, time);

		} else {

			currAction.switchActionIdle(brain, time);
		}
	}
}

/*
Footballer_Body::ActionType Footballer_Brain_Player::ActionHandler_Falling::getType() {

	return Footballer_Body::Action_Falling;
}

void Footballer_Brain_Player::ActionHandler_Falling::frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) {

	Footballer_BodyAction_Falling& currAction = (Footballer_BodyAction_Falling&) curr;

	Footballer_Body::ShotType shootType = inputState.getShootType();
	Footballer_Body::RunType runType = inputState.getRunType();

	if (shootType != Footballer_Body::Shot_None) {

		Footballer_BodyAction_Tackling& actionTackling = brain.getActionTackling();

		if (runType != Footballer_Body::Run_None) {

			inputState.syncTackleParams(match, brain, time, actionTackling);
			currAction.switchActionTackling(brain, time);

		} else {

			actionTackling.cancelSetTackleVel();
			currAction.switchActionTackling(brain, time);
		}

	} else {

		if (runType != Footballer_Body::Run_None) {

			inputState.syncRunParams(match, brain, time, brain.getActionRunning());
			currAction.switchActionRunning(brain, time);

		} else {

			currAction.switchActionIdle(brain, time);
		}
	}
}


Footballer_Body::ActionType Footballer_Brain_Player::ActionHandler_Jumping::getType() {

	return Footballer_Body::Action_Jumping;
}

void Footballer_Brain_Player::ActionHandler_Jumping::frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) {

	Footballer_BodyAction_Jumping& currAction = (Footballer_BodyAction_Jumping&) curr;

	Footballer_Body::ShotType shootType = inputState.getShootType();
	Footballer_Body::RunType runType = inputState.getRunType();
	Footballer_Body::JumpType jumpType = inputState.getJumpType();

	if (shootType != Footballer_Body::Shot_None) {

		Footballer_BodyAction_Tackling& actionTackling = brain.getActionTackling();

		if (runType != Footballer_Body::Run_None) {

			inputState.syncTackleParams(match, brain, time, actionTackling);
			currAction.switchActionTackling(brain, time);

		} else {

			actionTackling.cancelSetTackleVel();
			currAction.switchActionTackling(brain, time);
		}

	} else if (jumpType != Footballer_Body::Jump_None) {

		Footballer_BodyAction_Jumping& actionJumping = brain.getActionJumping();

		if (runType != Footballer_Body::Run_None) {

			inputState.syncJumpParams(match, brain, time, actionJumping);
			currAction.switchActionJumping(brain, time);

		} else {

			actionJumping.cancelSetJumpType();
			currAction.switchActionJumping(brain, time);
		}


	} else {

		if (runType != Footballer_Body::Run_None) {

			inputState.syncRunParams(match, brain, time, brain.getActionRunning());
			currAction.switchActionRunning(brain, time);

		} else {

			currAction.switchActionIdle(brain, time);
		}
	}
}
*/

} }