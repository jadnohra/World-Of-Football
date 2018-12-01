
class BodyActionIdle_StartState {
}

class BodyAction_Idle extends BodyActionBase_BallTriggers {

	static BallControlState_None = -1;
	static BallControlState_Waiting = 0;
	static BallControlState_Failed = 1;
	static BallControlState_TriggeredStabilize = 2;
	static BallControlState_WaitingToStabilize = 3;
	static BallControlState_WaitingForFootControl = 4;
	static BallControlState_GuardingBall = 5;
	
	static BallTriggerID_None = -1;
	static BallTriggerID_Stabilize = 0;
	static BallTriggerID_Stabilized = 1;
	static BallTriggerID_SweetSpot = 2;
	
	function getType(footballer) {
	
		return footballer.Action_Idle;
	}

	function load(footballer, chunk, conv) {
	
		return true;
	}

	function isFinished() { return true; }	
	
	function getStaminaGain(footballer) { return footballer.idle.StaminaGain; }
	
	function switchIdle(footballer, t, startState) {
	
		return true;
	}
	
	constructor() {
	
		mBallTrigger_ActiveColl = FootballerBallTrigger_ActiveColl();
		mBallTrigger_Pitch = FootballerBallTrigger_Pitch();
		mBallTrigger_FootControl = FootballerBallTrigger_FootControl();
	}
	
	function start(footballer, t, startState) {
	
		local impl = footballer.impl;
	
		setAnimIdle(footballer);		
		
		mBallControlState = BallControlState_None;
		switchBallControlState(BallControlState_Waiting, null);
		/*
		footballer.ballHandler.setActionControl(footballer, true, footballer.settings.getIdealBallActionPoint(footballer), 0.7);
		footballer.ballHandler.setActionSound(footballer.settings.BallControlSoundName, footballer.settings.BallControlSoundMag);
		*/
		
		return true;
	}
	
	function stop(footballer, t) {
	
		/*
		footballer.ballHandler.reset(footballer, true);
		*/
	}
	
	function setAnimIdle(footballer) {
	
		mIsAlternateAnim = false;
		local impl = footballer.impl;
	
		impl.setAnim(footballer.idle.AnimName, false);
		impl.setAnimLoop(true);
		impl.setAnimSpeed(footballer.idle.AnimSpeed);
	}
	
	function setAnimHit(footballer, hitterPos) {
	
		mIsAlternateAnim = true;
		local impl = footballer.impl;
	
		local animName = footballer.idle.HitAnimNames[detectHitAnimType(footballer, hitterPos)];
		
		//print(animName);
		
		impl.setAnim(animName, false);
		impl.setAnimLoop(false);
		impl.setAnimSpeed(footballer.idle.HitAnimSpeed);
	}
	
	function detectHitAnimType(footballer, hitterPos) {
	
		local dir = Vector3();
		local facing = footballer.impl.getFacing();
		hitterPos.subtract(footballer.impl.getPos(), dir);
	
		if (dir == null)
			return footballer.idle.HitAnim_Front;
	
		local type;
	
		local tolerance = 0.0175; //sin(1 degrees)
		local dot = facing.dot(dir);

		if (fabs(1.0 - dot) <= tolerance) {

			type = footballer.idle.HitAnim_Front;

		} else {

			local discreeteDir = Vector3();

			local step = discretizeDirection(facing, footballer.impl.getMatch().SceneUpAxis(), dir, 4, discreeteDir);

			switch(step) {

				case 0: type = footballer.idle.HitAnim_Front; break;
				case 1: type = footballer.idle.HitAnim_Right; break;
				case 2: type = footballer.idle.HitAnim_Back; break;
				case 3: type = footballer.idle.HitAnim_Left; break;
				default: {
				
					type = footballer.idle.HitAnim_Front; 
					
				} break;
			}
		}
		
		return type;
	}
	
	function checkBallPainHit(footballer, ball, boneID) {
	
		if (footballer.settings.boneIsBallPain(boneID) 
			&& ball.getVel().magSq() >= footballer.settings.BallPainSpeedSq) {
			
			setAnimHit(footballer, ball.getPos());
			//switchPainHitDir(footballer, footballer.impl.getMatch().getTime(), ball.getVel());
			return true;
		}
		
		return false;
	}
	
	function onLostBall(footballer) { mBallControlState = BallControlState_None; }
	
	function onAcquiredBall(footballer, isSolo) { 
	
		if (mBallControlState == BallControlState_None) 
			switchBallControlState(BallControlState_Waiting, null); 
	}
	
	function onBallTrigger(triggerID, footballer) {
	
		switch (triggerID) {
		
			case BallTriggerID_Stabilize: {
			
				doBallActionStabilize(footballer);
			
			} break;
		
			case BallTriggerID_Stabilized: {
			
				doBallActionSweetSpot(footballer);
			
			} break;
			
			case BallTriggerID_SweetSpot: {
			
				doBallActionSweetSpot(footballer);
			
			} break;
		}
	}
	
	function onBallCommandFeedback(footballer, commandID, accepted) {
	
		if (!accepted) {
		
			switchBallControlState(BallControlState_Failed, null);
		}
	}	
	
	function doBallActionSweetSpot(footballer) {
	
		local settings = footballer.settings;
	
		footballer.impl.setBallControlSmoothTeleport(footballer.impl.genBallCommandID(), true, settings.BallSweetSpottingSpeed, settings.BallSweetSpottingMaxDuration, settings.BallSweetSpot, true, true);
		switchBallControlState(BallControlState_GuardingBall, null);
	}
	
	function doBallActionStabilize(footballer) {
	
		footballer.setBallCommandStabilize();
		switchBallControlState(BallControlState_TriggeredStabilize, null);
	}
	
	function switchBallControlState(nextState, setup) {
	
		local state = mBallControlState;
		endBallControlState(nextState, setup);
		startBallControlState(nextState, setup, state);
	}
	
	function endBallControlState(nextState, setup) {
	
		mBallTriggers = [];
	}
	
	function startBallControlState(newState, setup, prevState) {
	
		switch (newState) {
		
			case BallControlState_Waiting: {
			
				mBallTriggers = [ 
									mBallTrigger_ActiveColl.setTriggerID(BallTriggerID_Stabilize),
									mBallTrigger_Pitch.setTriggerID(BallTriggerID_Stabilize)
								];
			
			} break;
		
			case BallControlState_WaitingToStabilize: {
			
				mBallTriggers = [ 
									mBallTrigger_Pitch.setTriggerID(BallTriggerID_Stabilized)
								];
			
			} break;
		
			case BallControlState_WaitingForFootControl: {
			
				mBallTriggers = [ 
									mBallTrigger_FootControl.setTriggerID(setup ? BallTriggerID_Stabilize : BallTriggerID_SweetSpot)
								];
			
			} break;
		
			default: {
			
			} break;
		}
		
		mBallControlState = newState;
	}
	
	function handleBallControlState(footballer, t) {
	
		switch (mBallControlState) {
		
			case BallControlState_TriggeredStabilize: {
			
				local impl = footballer.impl;
			
				switch (impl.simulEstBallWillTouchPitchInCOC()) {
					
						case impl.SimulEst_Wait: break;
						case impl.SimulEst_True: switchBallControlState(BallControlState_WaitingToStabilize, null); break;
						case impl.SimulEst_False: {
						
							local settings = footballer.idle;
						
							switch (settings.HighBallFootAction) {
							
								case settings.HighBallFootAction_None: switchBallControlState(BallControlState_Failed, null); break;
								case settings.HighBallFootAction_Stabilize: switchBallControlState(BallControlState_WaitingForFootControl, true); break;
								case settings.HighBallFootAction_SmoothControl: switchBallControlState(BallControlState_WaitingForFootControl, false); break;
							}
						
						} break;
					}
			
			} break;
		}
	}
	
	function handleInterBodySignals(footballer, t, dt) {

		/*
		local signals = footballer.getInterBodySignals();
		
		foreach (signal in signals) {
		
			footballer.switchActionFalling(true, t, dt);
			return false;
		}
		*/
		
		return true;
	}	
	
	function frameMove(footballer, t, dt) {
			
		if (!handleInterBodySignals(footballer, t, dt))
			return;
			
		local impl = footballer.impl;

		impl.nodeMarkDirty();

		{	

		if (mIsAlternateAnim && impl.isAnimFinished()) {
			
				setAnimIdle(footballer);
				
			} else {
		
				impl.addAnimTime(dt);
			}
		}
		
		handleBallControlState(footballer, t);
	}
	
	mBallControlState = -1;
	mIsAlternateAnim = false;
	
	mBallTrigger_ActiveColl = null;
	mBallTrigger_Pitch = null;
	mBallTrigger_FootControl = null;
}