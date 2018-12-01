
class FootballerBrain_Player {
	
	static Action_AfterTouch = -2;
	static AfterTouchState_SwitchedOn = 0;
	static AfterTouchState_WaitingForChange = 1;
	static AfterTouchState_Applying = 2;
	static TapInterval = 0.18;
	static ShotPowerBarCycleInterval = 0.5;
	static ShotPowerBarCancelValue = 0.1;
	
	static ButtonAction_None = -1;
	static ButtonAction_Tap = 0;
	static ButtonAction_DoubleTap = 1;
	static ButtonAction_Hold = 2;
	
	constructor() {
		
		constructStatic();
		
		mActionQueue = [];
		mInterpreter = FootballerBrain_Player_ControllerInterpreter();
		
		mSwitcherJumpingState = FootballerBrain_Player_Switcher_Jumping_State();
	}	

	function constructStatic() {
	
		local rootTable = getroottable();
	
		if (!("kFootballerBrain_Player_Loaded" in rootTable)) {
		
			::kFootballerBrain_Player_Loaded <- false;
		}
	}
	
	function initArrays(footballer) {
	
		mSyncers[footballer.Action_Idle] <- FootballerBrain_Player_Syncer_Idle();
		mSyncers[footballer.Action_Running] <- FootballerBrain_Player_Syncer_Running();
		mSyncers[footballer.Action_Tackling] <- FootballerBrain_Player_Syncer_Tackling();
		mSyncers[footballer.Action_Jumping] <- FootballerBrain_Player_Syncer_Jumping();
		mSyncers[footballer.Action_Falling] <- FootballerBrain_Player_Syncer_Falling();
		mSyncers[footballer.Action_Shooting] <- FootballerBrain_Player_Syncer_Shooting();
		mSyncers[Action_AfterTouch] <- FootballerBrain_Player_Syncer_AfterTouch();
			
		mSwitchers[footballer.Action_None] <- FootballerBrain_Player_Switcher();
		mSwitchers[footballer.Action_Idle] <- FootballerBrain_Player_Switcher_Idle();
		mSwitchers[footballer.Action_Running] <- FootballerBrain_Player_Switcher_Running();
		mSwitchers[footballer.Action_Tackling] <- FootballerBrain_Player_Switcher_Tackling();
		mSwitchers[footballer.Action_Jumping] <- FootballerBrain_Player_Switcher_Jumping();
		mSwitchers[footballer.Action_Falling] <- FootballerBrain_Player_Switcher_Falling();		
		mSwitchers[footballer.Action_Shooting] <- FootballerBrain_Player_Switcher_Shooting();		
		mSwitchers[Action_AfterTouch] <- FootballerBrain_Player_Switcher_AfterTouch();		
	}
	
	function loadStatic(footballer, chunk, conv) {
	
		if (::kFootballerBrain_Player_Loaded == false) { 
			
			::kFootballerBrain_Player_Loaded = true;

			initArrays(footballer);
		}
	}
	
	function load(footballer, chunk, conv) {
	
		loadStatic(footballer, chunk, conv);
			
		return true;
	}
	
	function start(footballer, t, controller) {
	
		if (!mIsStarted) {
		
			mInterpreter.init(controller, t);
			mIsStarted = true;
						
			mLastInputActionType = footballer.Action_None;
		}
		
		return true;
	}
	
	function stop(footballer, t) {
	
		mSwitchers[mLastInputActionType].switchedOff(this, footballer, t);
	
		mIsStarted = false;
	}
	
	function isAskingForManualSwitch(footballer) {
	
		return (mInterpreter.getShotType() == footballer.Shot_Pass) && mInterpreter.getShotTypeChanged();
	}

	function frameMove(footballer, t, dt) {
		
		/*
			The correct way to handle after touch is to get footballer.player
			and ask player if its just control footballer or just idle it
			and also pass input to player to allow it to apply aftertouch
		*/
		
		local inputActionType = mInterpreter.frameMove(this, footballer, t);
	
		if (mLastInputActionType != inputActionType) {
		
			mSwitchers[mLastInputActionType].switchedOff(this, footballer, t);
			mSwitchers[inputActionType].switchedOn(this, footballer, t);
		
			mLastInputActionType = inputActionType;
		}
		
		/*
		if (mTrigger != null) {
			
			mTrigger.handle(this, footballer, t, inputActionType);
		}
		*/
	
		mSwitchers[inputActionType].handle(this, footballer, t, dt);
		//mSyncers[inputActionType].sync(this, footballer, t);
		
		if (inputActionType == Action_AfterTouch) {
		
			mSyncers[Action_AfterTouch].sync(this, footballer, t);
		
		} else {
			
			mSyncers[footballer.getActionType()].sync(this, footballer, t);
		}
		
		if (mSwitcherIsInBarMode) {
		
			footballer.impl.showBar(mSwitcherBarValue);
			
		} else {
		
			footballer.impl.hideBar();
		}
	}
	
	function getSyncer(type) { return mSyncers[type]; }
	function getActionQueue() { return mActionQueue; }
	function hasQueuedActions() { return mActionQueue.len() > 0; }
	
	function getTrigger() { return mTrigger; }
	function setTrigger(trigger) { mTrigger = trigger; }
	
	function getSwitcherJumpingState() { return mSwitcherJumpingState; }
			
	function getInput() { return mInterpreter; }
	
	mIsStarted = false;
	mInterpreter = null;
	mSwitchers = {}; //static, stateless
	mSyncers = {}; //static, stateless
	mActionQueue = null;		
	mLastInputActionType = null;
		
	mSwitcherJumpingState = null;
	mSwitcherAfterTouchState = null;
	mSwitcherSwitchOnTime = 0.0;
	mSwitcherIsInBarMode = false;
	mSwitcherBarValueUp = true;
	mSwitcherBarValue = 0.0;
	
	
	function switchToTackling(footballer, t, buttonAction) {
	
		local currActionType = footballer.getActionType();
		local startState = BodyActionTackling_StartState();
	
		if (currActionType == footballer.Action_Running) {
			
			local actionRunning = footballer.getActionRunning();
		
			if (footballer.run.isStrafingHard(actionRunning.getRunType())) {
			
			} else {
			
				local vel = Vector3();
				actionRunning.getCurrVelocity(footballer, vel);
		
				startState.tackleSpeed = vel.mag();
			}
		
		} else {
		
			local input = this.getInput();
			
			if (input.hasDir()) {
			
				startState.tackleDir = input.getDir();
			
			} else {
			}
		}
		
		startState.isBallControlTackle = buttonAction == ButtonAction_Hold ? true : false;
		
		footballer.getAction().switchTackling(footballer, t, startState);
	}
}

/*
	All Switchers have to be stateless, state if any is stored in FootballerBrain_Player
*/
class FootballerBrain_Player_Switcher {

	function handle(brain, footballer, t, dt) {}
	
	function switchedOn(brain, footballer, t) {}
	function switchedOff(brain, footballer, t) {}
}

class FootballerBrain_Player_Switcher_Idle extends FootballerBrain_Player_Switcher {

	function handle(brain, footballer, t, dt) {
	
		if (footballer.getActionType() != Footballer.Action_Idle) {
	
			footballer.getAction().switchIdle(footballer, t, null);
		}
	}
}

class FootballerBrain_Player_Switcher_AfterTouch extends FootballerBrain_Player_Switcher {

	function handle(brain, footballer, t, dt) {
	
		if (footballer.getActionType() != Footballer.Action_Idle) {
	
			footballer.getAction().switchIdle(footballer, t, null);
		}
	}
	
	function switchedOn(brain, footballer, t) {
	
		brain.mSwitcherAfterTouchState = brain.AfterTouchState_SwitchedOn;
	}
}

class FootballerBrain_Player_Switcher_Shooting extends FootballerBrain_Player_Switcher {

	function triggerShoot(brain, footballer, t, shotType, shotPowerScale, buttonAction) {
	
		local isHeadShot = footballer.ballIsHeaderHeight();
		local isRunning = footballer.getActionType() == Footballer.Action_Running;
	
		if (!isHeadShot) {
	
			if ((buttonAction == brain.ButtonAction_Tap) 
				&& (!footballer.impl.isBallOwner())) {
				
				local shot = footballer.settings.safeGetShot(shotType);
				
				if (shot == null || shot.tackleIfNotOwner) {
				
					brain.switchToTackling(footballer, t, buttonAction);
					return;
				}
			}
		}
	
		if (isRunning && !isHeadShot) {
			
			local shot = footballer.settings.safeGetShot(shotType);
			
			footballer.getActionRunning().setActiveBallShot(shot, shotPowerScale, false, buttonAction != brain.ButtonAction_Tap);
		
		} else {
		
			local startState = BodyActionShooting_StartState();
		
			startState.copyBallShot = false;
			startState.ballShotType = shotType;
			startState.isHeadShot = isHeadShot;
			startState.ballShot = startState.isHeadShot ? footballer.settings.safeGetHeadShot(shotType) : footballer.settings.safeGetShot(shotType);
			startState.shotSpeedScale = shotPowerScale;
			startState.isDynamicJumping = isRunning && !footballer.getActionRunning().isStrafingHard(footballer);
				
			footballer.getAction().switchShooting(footballer, t, startState);
		}
	}

	function handle(brain, footballer, t, dt) {
				
		if (footballer.getActionType() != Footballer.Action_Shooting) {
		
			if (brain.mSwitcherIsInBarMode) {
			
				local diff = dt / brain.ShotPowerBarCycleInterval;
			
				brain.mSwitcherBarValue += brain.mSwitcherBarValueUp ? diff : -diff;
			
				if (brain.mSwitcherBarValue > 1.0) {
			
					brain.mSwitcherBarValueUp = false;
					brain.mSwitcherBarValue = 1.0;
					
				} else if (brain.mSwitcherBarValue < 0.0) {
				
					brain.mSwitcherBarValueUp = true;
					brain.mSwitcherBarValue = 0.0;
				}
			
			} else if (t - brain.mSwitcherSwitchOnTime > brain.TapInterval) {
			
				local allowPowerBarMode = !footballer.ballIsHeaderHeight();
				
				if (allowPowerBarMode) {
			
					brain.mSwitcherIsInBarMode = true;
					brain.mSwitcherBarValueUp = true;
					brain.mSwitcherBarValue = 0.0;
				}
			}
		}
	}
	
	function switchedOff(brain, footballer, t) {
	
		if (footballer.getActionType() != Footballer.Action_Shooting) {
	
			if (brain.mSwitcherIsInBarMode) {
			
				if (brain.mSwitcherBarValue >= brain.ShotPowerBarCancelValue) {
				
					triggerShoot(brain, footballer, t, brain.getInput().getLastShotType(), 2.0 * brain.mSwitcherBarValue, brain.ButtonAction_Hold);
				}
			
			} else /*if (t - brain.mSwitcherSwitchOnTime <= brain.TapInterval)*/ {
			
				triggerShoot(brain, footballer, t, brain.getInput().getLastShotType(), 1.0, brain.ButtonAction_Tap);
			}
		}
		
		brain.mSwitcherIsInBarMode = false;
	}
	
	function switchedOn(brain, footballer, t) {
	
		brain.mSwitcherSwitchOnTime = t;
	}
}

class FootballerBrain_Player_Switcher_Running extends FootballerBrain_Player_Switcher {

	function handle(brain, footballer, t, dt) {
	
		local actionType = Footballer.Action_Running;
	
		if (footballer.getActionType() != actionType) {
	
			local startState = brain.getSyncer(actionType).getStartState(brain, footballer, t);
			footballer.getAction().switchRunning(footballer, t, startState);
		}
	}
}

class FootballerBrain_Player_Switcher_Tackling extends FootballerBrain_Player_Switcher {

	function handle(brain, footballer, t, dt) {
	
		/*
		local actionType = footballer.Action_Tackling;
		local currActionType = footballer.getActionType();
				
		if (currActionType != actionType || footballer.getActionTackling().isFinished()) {
		
			brain.switchToTackling(footballer, t);
		}
		*/
		if (t - brain.mSwitcherSwitchOnTime > brain.TapInterval) {
		
			brain.switchToTackling(footballer, t, brain.ButtonAction_Hold);
		}
	}
	
	function switchedOff(brain, footballer, t) {
	
		local actionType = footballer.Action_Tackling;
		local currActionType = footballer.getActionType();
	
		if (currActionType != actionType || footballer.getActionTackling().isFinished()) {
	
			if (t - brain.mSwitcherSwitchOnTime > brain.TapInterval) {
			
				brain.switchToTackling(footballer, t, brain.ButtonAction_Hold);
			
			} else {
			
				brain.switchToTackling(footballer, t, brain.ButtonAction_Tap);
			}
		}
	}
	
	function switchedOn(brain, footballer, t) {
	
		brain.mSwitcherSwitchOnTime = t;
	}
}

class FootballerBrain_Player_Switcher_Jumping_State {

	isActive = false;
	activateTime = 0.0;
	startDir = null;
}

class FootballerBrain_Player_Switcher_Jumping extends FootballerBrain_Player_Switcher {

	function switchedOn(brain, footballer, t) {}
	
	function switchedOff(brain, footballer, t) {
	
		triggerJump(brain, footballer, t);
	}

	function handle(brain, footballer, t, dt) {
	
		local state = brain.getSwitcherJumpingState();
		
		if (state.isActive) {
					
			if (t - state.activateTime >= 0.3) {
							
				triggerJump(brain, footballer, t);			
			}
		
		} else {
		
			state.isActive = true;
			state.activateTime = t;
			
			state.startDir = Vector3();
			state.startDir.set(brain.getInput().getDir());
		}
	}
	
	function triggerJump(brain, footballer, t) {
	
		local state = brain.getSwitcherJumpingState();
	
		local actionJumping = footballer.getActionJumping();
		local startState = actionJumping.getStartStateFromJumpDirs(footballer, state.startDir, brain.getInput().getDir(), footballer.jump.JumpDistance_Default);

		footballer.getAction().switchJumping(footballer, t, startState);
		
		state.isActive = false;	
		state.startDir = null;
	}
}

class FootballerBrain_Player_Switcher_Falling extends FootballerBrain_Player_Switcher {
}


/*
	All Syncers have to be stateless, state if any is stored in FootballerBrain_Player
*/
class FootballerBrain_Player_Syncer {

	function sync(brain, footballer, t) {}
	function getStartState(brain, footballer, t) { return null; }
}

class FootballerBrain_Player_Syncer_Idle extends FootballerBrain_Player_Syncer {
}

class FootballerBrain_Player_Syncer_Shooting extends FootballerBrain_Player_Syncer {

	function sync(brain, footballer, t) {
	
		local input = brain.getInput();
		
		footballer.getActionShooting().setBallShotDirection(footballer, input.getDir());
	}
}

class FootballerBrain_Player_Syncer_AfterTouch extends FootballerBrain_Player_Syncer {

	function sync(brain, footballer, t) {
	
		if (brain.mSwitcherAfterTouchState == brain.AfterTouchState_SwitchedOn) {

			brain.mSwitcherAfterTouchState = brain.AfterTouchState_WaitingForChange;
			return;
		} 
		
		local input = brain.getInput();
		
		if (brain.mSwitcherAfterTouchState == brain.AfterTouchState_WaitingForChange) {
		
			if (input.afterTouchDirChanged()) {
			
				brain.mSwitcherAfterTouchState = brain.AfterTouchState_Applying;
			}
		} 
	
		if ((brain.mSwitcherAfterTouchState == brain.AfterTouchState_Applying) && input.hasAfterTouchDir()) {
		
			footballer.impl.requestBallAfterTouch(0, input.getAfterTouchDir(), 
													footballer.settings.BallAfterTouchSideMag, 
													footballer.settings.BallAfterTouchUpMag, 
													footballer.settings.BallAfterTouchDownMag, 
													footballer.settings.BallAfterTouchRotSyncRatio, true);
		}
	}
}

class FootballerBrain_Player_Syncer_Running extends FootballerBrain_Player_Syncer {

	function sync(brain, footballer, t) {
	
		local actionRunning = footballer.getActionRunning();
		local input = brain.getInput();
		local isStrafing = input.isStrafing();
		local shotType = input.getShotType();
		
		//actionRunning.setActiveBallShot(shotType == footballer.Shot_None ? null : footballer.settings.Shots[shotType], false);
					
		actionRunning.setIsStrafing(isStrafing);	
					
		if (isStrafing) {
						
			actionRunning.setRunDir(footballer, input.getDir());
			
		} else {
			
			actionRunning.setBodyFacing(footballer, input.getDir());
			actionRunning.setRunDir(footballer, input.getDir());
		}
							
		actionRunning.setRunSpeed(input.getRunSpeedType(footballer));
	}
	
	function getStartState(brain, footballer, t) {
	
		local startState = BodyActionRunning_StartState();
		local input = brain.getInput();
			
		if (input.isStrafing()) {
			
			startState.runDir = input.getDir();
			
		} else {
			
			startState.bodyFacing = input.getDir();
			startState.runDir = input.getDir();
		}
			
		startState.runSpeedType = input.getRunSpeedType(footballer);
		
		return startState;
	}
}

class FootballerBrain_Player_Syncer_Tackling extends FootballerBrain_Player_Syncer {

	function sync(brain, footballer, t) {
	
		local input = brain.getInput();
		
		footballer.getActionTackling().setBallTackleDirection(footballer, input.getDir());
	}
}

class FootballerBrain_Player_Syncer_Jumping extends FootballerBrain_Player_Syncer {
}

class FootballerBrain_Player_Syncer_Falling extends FootballerBrain_Player_Syncer {
}

class FootballerBrain_Player_ActionCommand {

	function getType(footballer) { return footballer.Action_None; }
	
	function activate(brain, footballer, t) {}
	function deactivate(brain, footballer, t) {}
	function frameMove(brain, footballer, t) {}
	
	mQueueInsertTime = -1.0;
}


class FootballerBrain_Player_ControllerInterpreter {

	constructor() {
	
		mDir = Vector3();
		mAfterTouchDir = Vector3();
	}

	function init(controller, t) {
	
		if (controller == null || !controller.isValid()) {
		
			assrt("invalid controller FootballerBrain_PlayerControllerInterpreter::init");
			return false;
		}
	
		mKeyShotLow = controller.getVariable("ShotLow", t);
		if (!mKeyShotLow.isValid()) { assrt("ShotLow not in Controller"); return false; }
		
		mKeyShotHigh = controller.getVariable("ShotHigh", t);
		if (!mKeyShotHigh.isValid()) { assrt("ShotHigh not in Controller"); return false; }
		
		mKeyShotExtra = controller.getVariable("ShotExtra", t);
		if (!mKeyShotExtra.isValid()) { assrt("ShotExtra not in Controller"); return false; }
		
		mKeyShotPass = controller.getVariable("Pass", t);
		if (!mKeyShotPass.isValid()) { assrt("Pass not in Controller"); return false; }
		
		mKeySprint = controller.getVariable("Sprint", t);
		if (!mKeySprint.isValid()) { assrt("Sprint not in Controller"); return false; }
	
		mKeyStrafe = controller.getVariable("Strafe", t);
		if (!mKeyStrafe.isValid()) { assrt("Strafe not in Controller"); return false; }
		
		mKeyFwdAxis = controller.getVariable("FwdAxis", t);
		if (!mKeyFwdAxis.isValid()) { assrt("FwdAxis not in Controller"); return false; }
		
		mKeyRightAxis = controller.getVariable("RightAxis", t);
		if (!mKeyRightAxis.isValid()) { assrt("RightAxis not in Controller"); return false; }
		
		mKeyAfterTouchUpAxis = controller.getVariable("FwdAxis", t);
		if (!mKeyAfterTouchUpAxis.isValid()) { assrt("FwdAxis not in Controller"); return false; }
		
		mKeyAfterTouchRightAxis = controller.getVariable("RightAxis", t);
		if (!mKeyAfterTouchRightAxis.isValid()) { assrt("RightAxis not in Controller"); return false; }
		
		/*
		mKeyAfterTouchUpAxis = controller.getVariable("AfterTouchUpAxis", t);
		if (!mKeyAfterTouchUpAxis.isValid()) { assrt("AfterTouchUpAxis not in Controller"); return false; }
		
		mKeyAfterTouchRightAxis = controller.getVariable("AfterTouchRightAxis", t);
		if (!mKeyAfterTouchRightAxis.isValid()) { assrt("AfterTouchRightAxis not in Controller"); return false; }
		*/
		
		mKeyJump = controller.getVariable("Jump", t);
		if (!mKeyJump.isValid()) { assrt("Jump not in Controller"); return false; }
		
		mController = controller;
		
		return true;
	}
	
	function frameMove(brain, footballer, t) {
	
		if (footballer.shouldApplyBallAfterTouch()) {
		
			updateAfterTouchDir(footballer);
			return brain.Action_AfterTouch;
		}

		updateDeadZone();
		updateShotType(footballer);
		updateDir(footballer, t);
	
		return getInputActionType(brain, footballer, t);
	}
	
	function getInputActionType(brain, footballer, t) {
	
		local shotType = getShotType();
		local isRunning = isRunning();
		local isJumping = isJumping();
	
		local inputActionType = footballer.Action_None;
		
		if (/*getShotTypeChanged() &&*/ (shotType != footballer.Shot_None)) {
		
			if (footballer.impl.isBallOwner()) {
			
				if (footballer.getActionType() != footballer.Action_Tackling) {
			
					inputActionType = footballer.Action_Shooting;
				}
			
			} else {
			
				if (brain.mSwitcherIsInBarMode) {
				
					inputActionType = footballer.Action_Shooting;
				
				} else {
			
					if (footballer.ballIsHeaderHeight()) {
					
						inputActionType = footballer.Action_Shooting;
					
					} else {
			
						if (footballer.settings.Shots[shotType].tackleIfNotOwner) {
				
							inputActionType = footballer.Action_Tackling;
						
						} else {
						
							//inputActionType = footballer.Action_Shooting;
						}
					}
				}
			}
		
			/*
			inputActionType = footballer.Action_Shooting;
			*/
		
			/*
			if (footballer.impl.isBallOwner()) {
			
				inputActionType = footballer.Action_Shooting;
			
			} else {
			
				if (footballer.settings.Shots[shotType].tackleIfNotOwner) {
		
					inputActionType = footballer.Action_Tackling;
				
				} else {
				
					inputActionType = footballer.Action_Shooting;
				}
			}
			*/
						
			/*
		
			switch (footballer.getActionType()) {
				
				case footballer.Action_Running: {
				
					if (footballer.impl.isBallOwner()) {
											
					
					} else {
					
						if (footballer.settings.Shots[shotType].tackleIfNotOwner) {
				
							inputActionType = footballer.Action_Tackling;
						} 
					}
					
				} break;
				
				default: {
				
					if (footballer.impl.isBallOwner()) {
					
						inputActionType = footballer.Action_Shooting;
					
					} else {
					
						if (footballer.settings.Shots[shotType].tackleIfNotOwner) {
				
							inputActionType = footballer.Action_Tackling;
						
						} else {
						
							inputActionType = footballer.Action_Shooting;
						}
					}
					
				} break;
			}
			*/
		
			/*
			if (footballer.impl.isBallOwner()) {
				
				switch (footballer.getActionType()) {
				
					case footballer.Action_Running: break;
					default: inputActionType = footballer.Action_Shooting; break;
				}
			
			} else {
			
				if (footballer.settings.Shots[shotType].tackleIfNotOwner) {
			
					inputActionType = footballer.Action_Tackling;
					
				} else {
				
					inputActionType = footballer.Action_Shooting;
				}
			}
			*/
		}  
		
		if (inputActionType != footballer.Action_None)
			return inputActionType;
		
		if (isJumping) {

			inputActionType = footballer.Action_Jumping;
			
		} else if (isRunning) {
			
			inputActionType = footballer.Action_Running;

		} else {
			
			inputActionType = footballer.Action_Idle;
		}
		
		
		return inputActionType;
	}
		
	function getShotType() {
	
		return mShotType;
	}
	
	function getLastShotType() {
	
		return mLastShotType;
	}
	
	function getShotTypeChanged() {
	
		return mShotTypeChanged;
	}
	
	function updateShotType(footballer) {
	
		mLastShotType = mShotType;
		
		mShotType = readShotType(footballer);
		
		mShotTypeChanged = (mShotType != mLastShotType);
	}
	
	function readShotType(footballer) {
	
		if (mKeyShotExtra.getValue() > 0.0) {
			
			return footballer.Shot_Extra;
			
		} else if (mKeyShotHigh.getValue() > 0.0) {
		
			return footballer.Shot_High;
			
		} else if (mKeyShotLow.getValue() > 0.0) {
		
			return footballer.Shot_Low;
			
		} else if (mKeyShotPass.getValue() > 0.0) {
		
			return footballer.Shot_Pass;
		}
		
		return footballer.Shot_None;
	}
	
	function isStrafing() {
	
		return (mKeyStrafe.getValue() != 0.0);
	}
	
	function isRunning() {
	
		return (!mDir.isZero());
	}
	
	function getRunSpeedType(footballer) {
	
		if (mKeySprint.getValue() > 0.0) {
		
			return footballer.run.RunSpeed_Sprint;
		}
		
		return footballer.run.RunSpeed_Normal;
	}
	
	function isJumping() {
	
		return (mKeyJump.getValue() != 0.0);
	}
	
	function hasDir() {
	
		return (!mDir.isZero());
	}
	
	function getDir() {
		
		return mDir;
	}
	
	function dirChanged() {
		
		return mDirChanged;
	}
	
	function hasAfterTouchDir() {
	
		return (!mAfterTouchDir.isZero());
	}
	
	function getAfterTouchDir() {
		
		return mAfterTouchDir;
	}
	
	function afterTouchDirChanged() {
		
		return mAfterTouchDirChanged;
	}
	
	function getSameDirCounter() {
	
		return mSameDirCounter;
	}
	
	function getSmoothedDirChanged(smoothFrames) {
	
		return mDirChanged ? true : mSameDirCounter <= smoothFrames;
	}
	
	function updateDeadZone() {
	
		if (mKeyShotLow.getValue() > 0.0
			&& mKeyShotExtra.getValue() > 0.0) {
			
			mDirCurve.setDeadZone(maxf(mKeyFwdAxis.getValue(), mKeyRightAxis.getValue()));
			log("DeadZone updated to " + mDirCurve.getDeadZone());
		}
	}
	
	function updateDir(footballer, t) {
	
		local match = footballer.impl.getMatch();
		mDirChanged = updateDir2D(footballer, mKeyFwdAxis, match.SceneFwdAxis(), mKeyRightAxis, match.SceneRightAxis(), true, mDir);
		
		if (mDirChanged) {
		
			mSameDirCounter = 0;
			
		} else {
		
			if (mSameDirCounter < 1000)
				mSameDirCounter += 1;
		}
	}
	
	function updateAfterTouchDir(footballer) {
	
		local match = footballer.impl.getMatch();
		
		mAfterTouchDirChanged = updateDir2D(footballer, mKeyAfterTouchUpAxis, match.SceneFwdAxis(), mKeyAfterTouchRightAxis, match.SceneRightAxis(), true, mAfterTouchDir);
	}
	
	function updateDir2D(footballer, key1, axis1, key2, axis2, camRel, inOutDir) {
	
		local impl = footballer.impl;
	
		local temp = Vector3();
		local oldDir = Vector3();
		local match = impl.getMatch();
		
		oldDir.set(inOutDir);
		
		local value1 = mDirCurve.map(key1.getValue());
		
		axis1.mul(value1, inOutDir);
		
		local value2 = mDirCurve.map(key2.getValue());
		
		axis2.mul(value2, temp);
		
		inOutDir.addToSelf(temp);
		
		inOutDir.normalize();
		
		if (camRel)
			impl.camRelativizeInPlace(inOutDir, true, false);
		
		local dirChanged;
		
		if (footballer.settings.EnableInputDirSmoothing) {
		
			if (fabs(1.0 - inOutDir.dot(oldDir)) <= footballer.settings.InputDirSmoothingValue) {
			
				inOutDir.set(oldDir);
				dirChanged = false;
				
			} else {
			
				dirChanged = true;
			}
		
		} else {
		
			dirChanged = !inOutDir.equals(oldDir);
		}
		
		return dirChanged;
	}
	
	
	mController = null;
	
	mKeyFwdAxis = null;
	mKeyRightAxis = null;
	
	mKeyAfterTouchUpAxis = null;
	mKeyAfterTouchRightAxis = null;
	
	mKeyShotLow = null;
	mKeyShotHigh = null;
	mKeyShotExtra = null;
	mKeyShotPass = null;
	
	mKeySprint = null;
	
	mKeyStrafe = null;
	
	mKeyJump = null;
	
	mShotType = -1;
	mLastShotType = -1;
	mShotTypeChanged = false;
	
	mDir = null;
	mDirChanged = false;
	mSameDirCounter = 0; //capped at 30
	
	mAfterTouchDir = null;
	mAfterTouchDirChanged = false;
	
	mDirCurve = ResponseCurve(1.0, 0.0, 1.0, 0.1, 1.0, true);
}