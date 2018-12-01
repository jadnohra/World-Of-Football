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

	return mInputController.ptr();
}

Player* Footballer_Brain_Player::getAttachedPlayer() {

	if (mInputController.isValid())
		return mInputController->getPlayer();
	
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




#ifdef FOOTBALLER_BODY3
void Footballer_Brain_Player::syncRunSpeed(Match& match, Footballer_Brain& brain, const Time& time, InputControllerState& inputState, const FootballerBodyDefs::RunSpeedType& speedType) {

	bool failedAny = false;

	Footballer_BodyAction_Running& action = brain.getActionRunning();

	if (!action.setRunSpeed(brain, time, speedType))
		failedAny = true;

	if (failedAny)
		mForceUpdateRunParams = true;
}

void Footballer_Brain_Player::syncDir(Match& match, Footballer_Brain& brain, const Time& time, InputControllerState& inputState, const bool& bodyDir, const bool& runDir) {

	bool failedAny = false;

	Footballer_BodyAction_Running& action = brain.getActionRunning();

	const Vector3* pDir;
	Vector3 relMoveDir;

	if (match.mTweakCameraRelativeInput) {

		match.getActiveCamera().relativiseDirection(inputState.moveDir, relMoveDir, true, match.mTweakCameraRelativeInputDiscreeteDirections);

		pDir = &relMoveDir;

	} else {

		pDir = &inputState.moveDir;
	}

	if (bodyDir && !action.setBodyFacing(brain, time, relMoveDir)) {

		failedAny = true;
	}
	
	if (runDir && !action.setRunDir(brain, time, relMoveDir)) {

		failedAny = true;
	}

	if (failedAny) {

		if (bodyDir)
			action.cancelSetBodyFacing();
		
		if (runDir)
			action.cancelSetRunDir();

		mForceUpdateRunParams = true;
	}
}
#endif

#ifdef FOOTBALLER_BODY2
void Footballer_Brain_Player::syncRunSpeed(Match& match, Footballer_Brain& brain, InputControllerState& inputState, const FootballerBodyDefs::RunType& runType) {

	brain.requestRunSpeed(brain.getRunTypeSpeed(runType));
}

void Footballer_Brain_Player::syncDir(Match& match, Footballer_Brain& brain, InputControllerState& inputState, const bool& bodyDir, const bool& runDir) {

	if (match.mTweakCameraRelativeInput) {

		Vector3 relMoveDir;

		match.getActiveCamera().relativiseDirection(inputState.moveDir, relMoveDir, true, match.mTweakCameraRelativeInputDiscreeteDirections);

		if (bodyDir)
			brain.requestBodyFacing(relMoveDir);
		if (runDir)
			brain.requestRunDir(relMoveDir);

	} else {

		if (bodyDir)
			brain.requestBodyFacing(inputState.moveDir);
		if (runDir)
			brain.requestRunDir(inputState.moveDir);
	}
}
#endif

void Footballer_Brain_Player::syncHard(Match& match, Team& team, Footballer_Brain& brain) {

	#ifdef FOOTBALLER_BODY3

	const Time& time = match.getTime().getTime();
	InputControllerState& inputState = mInputController->mState;

	if (inputState.move.state) {

		syncRunSpeed(match, brain, time, inputState, Footballer::RunSpeed_Normal);
		syncDir(match, brain, time, inputState, true, true);

		brain.getAction()->switchActionRunning(brain, time);

	} else {

		brain.getAction()->switchActionIdle(brain, time);
	}

	#endif

	#ifdef FOOTBALLER_BODY2

	InputControllerState& inputState = mInputController->mState;

	if (inputState.move.state) {

		brain.requestActionRunning();

		syncRunSpeed(match, brain, inputState, Footballer::Run_Normal);
		syncDir(match, brain, inputState, true, true);

	} else {

		brain.requestActionIdleIfNeeded();
	}

	#endif
}


#ifdef FOOTBALLER_BODY3
void Footballer_Brain_Player::syncIncremental(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputControllerState& inputState) {

	bool forceUpdateRunParams = mForceUpdateRunParams;
	mForceUpdateRunParams = false;

	if (forceUpdateRunParams || inputState.desynced) {

		inputState.desynced = false;

		bool resyncSpeed = false;

		if (forceUpdateRunParams || inputState.modifier.desynced) {

			inputState.modifier.desynced = false;

			resyncSpeed = true;
			inputState.move.desynced = true;
		}

		SoftPtr<Footballer::ActionImpl> active = brain.getAction();

		if (forceUpdateRunParams || inputState.move.desynced) {

			inputState.move.desynced = false;

			if (inputState.move.state) {

				if (active->getType() != Footballer::Action_Running) {

					resyncSpeed = true;
				}

				if (forceUpdateRunParams || resyncSpeed) 
					syncRunSpeed(match, brain, time, inputState, Footballer::RunSpeed_Normal);

				syncDir(match, brain, time, inputState, inputState.modifier.state ? false : true, true);
				
				if (active->getType() != Footballer::Action_Running) {

					active->switchActionRunning(brain, time);
					active = brain.getAction();
				}

			} else {

				if (active->getType() != Footballer::Action_Idle) {

					active->switchActionIdle(brain, time);
					active = brain.getAction();
				}
			}
		}

		Footballer_Body::ShotType shot = Footballer_Body::Shot_None;
		
		if (inputState.tackle[2].desynced) {

			inputState.tackle[2].desynced = 0;

			if (inputState.tackle[2].state) {

				shot = Footballer_Body::Shot_Extra;

			} 
		}

		if (inputState.tackle[1].desynced) {

			inputState.tackle[1].desynced = 0;

			if (inputState.tackle[1].state) {

				shot = Footballer_Body::Shot_High;
			} 
		}

		if (inputState.tackle[0].desynced) {

			inputState.tackle[0].desynced = 0;

			if (inputState.tackle[0].state) {

				shot = Footballer_Body::Shot_Low;
			} 
		}

		bool doSwitch = false;
		
		if (shot != Footballer_Body::Shot_None) {
			
			if (brain.getBallOwnership().ballControl) {

				if ((match.mSwitchControlSetup.mode == FSM_OnShootAction) &&
					brain.getBallOwnership().ballControl == 1.0f) {
				
					doSwitch = true;
				}

			} else {

				if (active->getType() != Footballer::Action_Tackling) {

					active->switchActionTackling(brain, time);
					active = brain.getAction();
				}
			}
		}

		if (inputState.footballerSwitch.desynced) {

			inputState.footballerSwitch.desynced = 0;

			if (inputState.footballerSwitch.state) {

				if ((match.mSwitchControlSetup.mode == FSM_OnShootAction) || 
					(match.mSwitchControlSetup.mode == FSM_Manual)) {

					doSwitch = true;
				}

				if (brain.getBallOwnership().ballControl == 1.0f) {

					//brain.body_setTackling(Footballer_Body::ST_ShotPass);

				} else {
					
				}

			} else {

			}
		}

		if (doSwitch) {

			brain.brain_playerSwitchFootballer();

			return;
		}

		mReceivedAnyPlayerCommands = (brain.getActionType() != Footballer_Body::Action_Idle);
	}
}
#endif


#ifdef FOOTBALLER_BODY2
void Footballer_Brain_Player::syncIncremental(Match& match, Team& team, Footballer_Brain& brain, InputControllerState& inputState) {

	if (inputState.desynced) {

		inputState.desynced = false;

		bool resyncSpeed = false;

		if (inputState.modifier.desynced) {

			inputState.modifier.desynced = false;

			resyncSpeed = true;
			inputState.move.desynced = true;
		}

		if (inputState.move.desynced) {

			inputState.move.desynced = false;

			if (inputState.move.state) {

				if (brain.requestActionRunningIfNeeded()) {

					resyncSpeed = true;
				}

				if (inputState.modifier.state) {

					if (resyncSpeed) 
						syncRunSpeed(match, brain, inputState, Footballer::Run_Modifier);

					syncDir(match, brain, inputState, false, true);

				} else {

					if (resyncSpeed) 
						syncRunSpeed(match, brain, inputState, Footballer::Run_Normal);

					syncDir(match, brain, inputState, true, true);
				}

			} else {

				brain.requestActionIdleIfNeeded();
			}
		}

		Footballer_Body::ShotType shot = Footballer_Body::Shot_None;
		
		if (inputState.tackle[2].desynced) {

			inputState.tackle[2].desynced = 0;

			if (inputState.tackle[2].state) {

				shot = Footballer_Body::Shot_Extra;

			} 
		}

		if (inputState.tackle[1].desynced) {

			inputState.tackle[1].desynced = 0;

			if (inputState.tackle[1].state) {

				shot = Footballer_Body::Shot_High;
			} 
		}

		if (inputState.tackle[0].desynced) {

			inputState.tackle[0].desynced = 0;

			if (inputState.tackle[0].state) {

				shot = Footballer_Body::Shot_Low;
			} 
		}

		bool doSwitch = false;
		
		if (shot != Footballer_Body::Shot_None) {
			
			if (brain.getBallOwnership().ballControl) {

				if ((match.mSwitchControlSetup.mode == FSM_OnShootAction) &&
					brain.getBallOwnership().ballControl == 1.0f) {
				
					doSwitch = true;
				}

			} else {

				brain.requestActionTacklingIfNeeded();
			}
		}

		if (inputState.footballerSwitch.desynced) {

			inputState.footballerSwitch.desynced = 0;

			if (inputState.footballerSwitch.state) {

				if ((match.mSwitchControlSetup.mode == FSM_OnShootAction) || 
					(match.mSwitchControlSetup.mode == FSM_Manual)) {

					doSwitch = true;
				}

				if (brain.getBallOwnership().ballControl == 1.0f) {

					//brain.body_setTackling(Footballer_Body::ST_ShotPass);

				} else {
					
				}

			} else {

			}
		}

		if (doSwitch) {

			brain.brain_playerSwitchFootballer();

			return;
		}

		mReceivedAnyPlayerCommands = (brain.getActionType() != Footballer_Body::Action_Idle);
	}
}
#endif

void Footballer_Brain_Player::frameMove(Match& match, Team& team, Footballer_Brain& brain, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

#ifdef FOOTBALLER_BODY3

	if (tickIndex != 0) {

		return;
	}

	mInputController->frameMove(match, time);

	InputControllerState& inputState = mInputController->mState;

	if(inputState.valid == false) {

		assrt(false);
		return;
	}

	syncIncremental(match, team, brain, time.t_discreet, inputState);

#endif

#ifdef FOOTBALLER_BODY2

	if (tickIndex != 0) {

		return;
	}

	mInputController->frameMove(match, time);

	InputControllerState& inputState = mInputController->mState;

	if(inputState.valid == false) {

		assrt(false);
		return;
	}

	syncIncremental(match, team, brain, inputState);

	brain.clearBodyEvents();

#endif

#ifdef FOOTBALLER_BODY1
	if (tickIndex != 0) {

		return;
	}

	mInputController->frameMove(match, time);

	InputControllerState& inputState = mInputController->mState;

	if(inputState.valid == false) {

		assrt(false);
		return;
	}

	bool forceRunningDirUpdate = false;

	Footballer_Body::EventIndex eventCount = brain.body_getEventCount();

	for (Footballer_Body::EventIndex i = 0; i < eventCount; ++i) {

		Footballer_Body::Event& evt = brain.body_getEvent(i);

		switch (evt.type) {

			case Footballer_Body::ET_MoveBlockEnded: {

				inputState.desynced = true;
				inputState.move.desynced = true;
				forceRunningDirUpdate = true;
			} break;

			case Footballer_Body::ET_TackleEnded:
			case Footballer_Body::ET_ShootEnded: {

				if (inputState.move.state) {

					brain.body_setRunning();
				}

			} break;
		}
	}

	if (inputState.desynced) {

		inputState.desynced = false;

		if (inputState.move.desynced) {

			inputState.move.desynced = false;

			if (inputState.move.state) {

				if (brain.body_getCurrState() != Footballer_Body::BS_Running) {

					brain.body_setRunning_type(Footballer_Body::BR_Normal);
					brain.body_setRunning();
				}

				if (match.mTweakCameraRelativeInput) {

					Vector3 relMoveDir;

					match.getActiveCamera().relativiseDirection(inputState.moveDir, relMoveDir, true, match.mTweakCameraRelativeInputDiscreeteDirections);

					brain.body_setRunning_dir(relMoveDir, forceRunningDirUpdate);

				} else {

					brain.body_setRunning_dir(inputState.moveDir, forceRunningDirUpdate);
				}

			} else {

				brain.body_setIdle();
			}
		}

		if (inputState.sprint.desynced) {

			inputState.sprint.desynced = 0;

			if (inputState.sprint.state) {

				brain.body_setRunning_type(Footballer_Body::BR_Sprint);

			} else {

				brain.body_setRunning_type(Footballer_Body::BR_Normal);
			}
		}

		Footballer_Body::ShotType shot = Footballer_Body::ST_ShotNone;
		bool tackling = false;

		if (inputState.tackle[2].desynced) {

			inputState.tackle[2].desynced = 0;

			if (inputState.tackle[2].state) {

				tackling = true;
				shot = Footballer_Body::ST_ShotExtra;

			} else {

			}
		}

		if (inputState.tackle[1].desynced) {

			inputState.tackle[1].desynced = 0;

			if (inputState.tackle[1].state) {

				tackling = true;
				shot = Footballer_Body::ST_ShotHigh;

			} else {

			}
		}

		if (inputState.tackle[0].desynced) {

			inputState.tackle[0].desynced = 0;

			if (inputState.tackle[0].state) {

				tackling = true;
				shot = Footballer_Body::ST_ShotLow;

			} else {

			}
		}

		bool doSwitch = false;
		
		if (tackling) {
			
			brain.body_setTackling(shot);

			if ((match.mSwitchControlSetup.mode == FSM_OnShootAction) &&
					brain.getBallOwnership().ballControl == 1.0f) {
				
				doSwitch = true;
			}
		}


		if (inputState.footballerSwitch.desynced) {

			inputState.footballerSwitch.desynced = 0;

			if (inputState.footballerSwitch.state) {

				if ((match.mSwitchControlSetup.mode == FSM_OnShootAction) || 
					(match.mSwitchControlSetup.mode == FSM_Manual)) {

					doSwitch = true;
				}

				if (brain.getBallOwnership().ballControl == 1.0f) {

					brain.body_setTackling(Footballer_Body::ST_ShotPass);

				} else {
					
				}

			} else {

			}
		}

		if (doSwitch) {

			brain.brain_playerSwitchFootballer();

			return;
		}

		mReceivedAnyPlayerCommands = (brain.body_getCurrState() != Footballer_Body::BS_Idle);
	}

	brain.body_clearEvents();
#endif
}

void Footballer_Brain_Player::render(Match& match, Team& team, Footballer_Brain& brain, Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass) {

#ifdef FOOTBALLER_BODY1

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

#endif
}

void Footballer_Brain_Player::attach(Match& match, Team& team, Footballer_Brain& brain, InputController& inputController) {

	assrt(mInputController.isNull());

	mInputController = &inputController;
	mInputController->mState.resetNoneMovementKeys();


#ifdef FOOTBALLER_BODY3
	mForceUpdateRunParams = true;
	syncHard(match, team, brain);
#endif

#ifdef FOOTBALLER_BODY2
	syncHard(match, team, brain);
#endif
#ifdef FOOTBALLER_BODY1
	brain.body_setIdle();
#endif

	mReceivedAnyPlayerCommands = false;
}

void Footballer_Brain_Player::detach(Match& match, Team& team, Footballer_Brain& brain) {

	mInputController.destroy();
}

bool Footballer_Brain_Player::receivedAnyPlayerCommands() {

	return mReceivedAnyPlayerCommands;
}

} }

