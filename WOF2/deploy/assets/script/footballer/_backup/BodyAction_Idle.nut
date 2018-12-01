
class BodyActionIdle_StartState {
}

class BodyAction_Idle extends BodyActionBase_BallTriggers {

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
	
		ballControlOnConstructor();
	}
	
	function start(footballer, t, startState, prevAction) {
	
		setAnimIdle(footballer);		
		
		ballControlOnStart(footballer, t, startState, prevAction);
					
		footballer.setBodyMomentum(null);					
		return true;
	}
	
	function stop(footballer, t) {
	
		ballControlOnStop(footballer, t);
	}
	
	function setAnimIdle(footballer) {
	
		mAlternateAnim = footballer.idle.AlternateAnim_None;
		local impl = footballer.impl;
	
		impl.setAnim(footballer.idle.AnimName, false);
		impl.setAnimLoop(true);
		impl.setAnimSpeed(footballer.idle.AnimSpeed);
	}
	
	function setAlternateAnim(footballer, anim) {
	
		if (mAlternateAnim != anim) {
	
			mAlternateAnim = anim;
			local impl = footballer.impl;
		
			impl.setAnim(footballer.idle.AlternateAnimNames[mAlternateAnim], false);
			impl.setAnimLoop(false);
			impl.setAnimSpeed(footballer.idle.AlternateAnimSpeeds[mAlternateAnim]);
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

		if (mAlternateAnim >= 0 && impl.isAnimFinished()) {
			
				setAnimIdle(footballer);
				
			} else {
		
				impl.addAnimTime(dt);
			}
		}
		
		handleBallControlState(footballer, t);
	}
	
	mAlternateAnim = -1;
	

	/*******************
		Ball Handling
	********************/
	
	static BallControlState_None = -1;
	static BallControlState_Undetermined = 0;
	static BallControlState_NotOwner = 1;
	static BallControlState_Owner = 2;
	static BallControlState_TriggerAction = 3;
	static BallControlState_TriggeredStabilize = 4;
	static BallControlState_WaitingToStabilize = 5;
	static BallControlState_GuardingBall = 6;
	static BallControlState_Stunned = 7;
	static BallControlState_Failed = 8;
	static BallControlState_WaitingForShotEnd = 9;
	
	static BallTriggerID_None = -1;
	static BallTriggerID_Stabilize = 0;
	static BallTriggerID_Stabilized = 1;
	static BallTriggerID_SweetSpot = 2;
	
	function ballControlOnConstructor() {
	
		mBallTrigger_ActiveColl = FootballerBallTrigger_ActiveColl();
		mBallTrigger_Pitch = FootballerBallTrigger_Pitch();
		mBallTrigger_FootControl = FootballerBallTrigger_FootControl();
	}
	
	function ballControlOnStart(footballer, t, startState, prevAction) {
	
		local wasLastShotActive = false;
	
		if (prevAction != null && prevAction.getType(footballer) == footballer.Action_Running) {
		
			wasLastShotActive = prevAction.wasLastShotActive();
			
		} else {
		
			wasLastShotActive = footballer.impl.isBallOwner() && footballer.impl.isShootingBall();
		}
	
		mBallControlState = BallControlState_None;
		switchBallControlState(footballer, BallControlState_Undetermined, wasLastShotActive);
	}
	
	function ballControlOnStop(footballer, t) {
	
		footballer.impl.cancelBallControl();
	}
	
	function onLostBall(footballer, ball) { switchBallControlState(footballer, BallControlState_NotOwner, null); }
	function onAcquiredBall(footballer, ball, isSolo) { 
	
		//because this state could have been set by a fast collision trigger
		if (mBallControlState < BallControlState_Owner)
			switchBallControlState(footballer, BallControlState_Owner, null); 
	}
	
	function onBallCommandRejected(footballer, ball, commandID) {
	
		switchBallControlState(footballer, BallControlState_Stunned, null);
	}
	
	function onBallTrigger(triggerID, footballer, ball) {
	
		//print("trigger: " + triggerID);
	
		switch (triggerID) {
		
			case BallTriggerID_Stabilize: {
			
				doBallActionStabilize(footballer, ball, null);
			
			} break;
		
			case BallTriggerID_Stabilized: {
			
				doBallActionSweetSpot(footballer, ball);
			
			} break;
			
			case BallTriggerID_SweetSpot: {
			
				doBallActionSweetSpot(footballer, ball);
			
			} break;
		}
		
		return false;
	}
	
	function onBallCommandRejected(footballer, ball, commandID) {
	
		//print(footballer.impl.getNodeName() + "rejected");
	
		switchBallControlState(footballer, BallControlState_Stunned, null);
	}	
	
	function doBallActionSweetSpot(footballer, ball) {
	
		setAlternateAnim(footballer, footballer.idle.AlternateAnim_SweetSpotRight);
	
		local settings = footballer.settings;
	
		footballer.impl.setBallControlSmoothTeleport(footballer.impl.genBallCommandID(), footballer.getSkillBallControlBodyOrFeet(), true, settings.BallSweetSpottingSpeed, settings.BallSweetSpottingMaxDuration, settings.BallSweetSpot, true, true);
		switchBallControlState(footballer, BallControlState_GuardingBall, null);
	}
	
	function doBallActionStabilize(footballer, ball, soundName) {
	
		//footballer.setBallCommandStabilize(footballer.impl.genBallCommandID(), ball, 0.0, footballer.impl.getFacing(), false, true, false, soundName);
		footballer.shootBall(footballer.impl.genBallCommandID(), ball, footballer.settings.ZeroShot, 0.0, footballer.impl.getFacing(), null,
								false, false, false, footballer.getSkillBallProtection(), footballer.getSkillBallControlBodyOrFeet(), true, 0.0, soundName);
		switchBallControlState(footballer, BallControlState_TriggeredStabilize, null);
	}
	
	function switchBallControlState(footballer, nextState, setup) {
	
		//print ("switch");
	
		local state = mBallControlState;
		endBallControlState(footballer, nextState, setup);
		startBallControlState(footballer, nextState, setup, state);
	}
	
	function endBallControlState(footballer, nextState, setup) {
	
		mBallTriggers = [];
	}
	
	function startBallControlState(footballer, newState, setup, prevState) {
	
		switch (newState) {
		
			case BallControlState_None:
			case BallControlState_Undetermined: {
			
				mBallControlState = newState;
				local leaveShot = false;
			
				if (setup != null) {
				
					leaveShot = setup;
				}
			
				if (footballer.impl.isBallOwner()) {
				
					if (leaveShot) {
					
						switchBallControlState(footballer, BallControlState_WaitingForShotEnd, null);
					
					} else {
				
						switchBallControlState(footballer, BallControlState_Owner, null);
					}
					
				} else {
				
					switchBallControlState(footballer, BallControlState_NotOwner, null);
				}
			
				return;
			
			} break; 
			
			case BallControlState_NotOwner: {
			
				mBallTriggers = [ 
									mBallTrigger_FootControl.setTriggerID(BallTriggerID_Stabilize),
									mBallTrigger_ActiveColl.setTriggerID(BallTriggerID_Stabilize)
								];
			
			} break;
			
			case BallControlState_Owner: {
			
				mBallTriggers = [ 
									mBallTrigger_ActiveColl.setTriggerID(BallTriggerID_Stabilize)
								];
			
			} break;
			
			case BallControlState_TriggerAction: {
			
				mBallTriggers = [ 
									mBallTrigger_FootControl.setTriggerID(setup ? BallTriggerID_Stabilize : BallTriggerID_SweetSpot),
									mBallTrigger_ActiveColl.setTriggerID(BallTriggerID_Stabilize),
								];
			
			} break;
			
			case BallControlState_TriggeredStabilize:
			case BallControlState_WaitingToStabilize: {
			
				mBallTriggers = [ 
									mBallTrigger_Pitch.setTriggerID(BallTriggerID_Stabilized)
								];
								
				mBallControlState = newState;				
				handleBallControlState(footballer, footballer.impl.getMatch().getTime());
			
			} break;
		
			case BallControlState_Stunned: {
		
				mBallControlStunEndTime = footballer.impl.getMatch().getTime() + footballer.getPhysicalAgilityReflexReactionTime();
				
				//print(footballer.impl.getNodeName() + "react time: " + footballer.getBallControlReactionTime() + " endt: " + mBallControlStunEndTime);
				
			} break;
		
			default: {
			
			} break;
		}
		
		mBallControlState = newState;
		//print(footballer.impl.getNodeName() + ": " + mBallControlState);
	}
	
	function doTriggerHighBallFootAction(footballer, failOnNoAction) {
	
		local settings = footballer.idle;
	
		switch (settings.HighBallFootAction) {
							
			case settings.HighBallFootAction_None: if (failOnNoAction) switchBallControlState(footballer, BallControlState_Failed, null); break;
			case settings.HighBallFootAction_Stabilize: switchBallControlState(footballer, BallControlState_TriggerAction, true); break;
			case settings.HighBallFootAction_SmoothControl: switchBallControlState(footballer, BallControlState_TriggerAction, false); break;
		}
	}
	
	function doHighBallFootAction(footballer, ball) {
	
		local settings = footballer.idle;
	
		switch (settings.HighBallFootAction) {
							
			case settings.HighBallFootAction_None: break;
			case settings.HighBallFootAction_Stabilize: doBallActionStabilize(footballer, ball, footballer.settings.BallShotSoundName); return true; break;
			case settings.HighBallFootAction_SmoothControl: doBallActionSweetSpot(footballer, ball); return true; break;
		}
		
		return false;
	}
	
	function handleBallControlState(footballer, t) {
	
		switch (mBallControlState) {
		
			case BallControlState_Owner: {
			
				local nextSample = CBallTrajSample();
				local impl = footballer.impl;
				local ball = impl.getMatch().getBall();
				
				/*
				if (impl.simulNextBallSample(ball, nextSample)) {
				
					if (footballer.isBallStateImprovingIdle(footballer, ball, nextSample, true)) {
						
						//wait for next sample
						print("improving");
					
					} else {
					
						if (ball.isOnPitch()) {
								
							print("sweetSpot");
							doBallActionSweetSpot(footballer, ball);
								
						} else {

							if (footballer.ballIsFootControlHeight()
								&& doHighBallFootAction(footballer, ball)) {						
								
							} else {	
							
								doTriggerHighBallFootAction(footballer, false);
							}
						}
					}
				}
				*/
				
				if (impl.simulNextBallSample(ball, nextSample) && footballer.isBallStateImprovingIdle(footballer, ball, nextSample, false)) {
				
					//wait for next sample
					//print("improving");
					
				} else {
				
					if (ball.isOnPitch()) {
							
						//print("sweetSpot");
						doBallActionSweetSpot(footballer, ball);
							
					} else {

						if (footballer.ballIsFootControlHeight()
							&& doHighBallFootAction(footballer, ball)) {						
							
						} else {	
						
							doTriggerHighBallFootAction(footballer, false);
						}
					}
				}
		
			} break;
		
			case BallControlState_TriggeredStabilize: {
			
				local impl = footballer.impl;
				local ball = impl.getMatch().getBall();
				
				if (ball.isOnPitch()) {
				
					onBallTrigger(BallTriggerID_Stabilized, footballer, ball);
				
				} else {
				
					switch (impl.simulEstBallWillTouchPitchInCOC()) {
						
						case impl.SimulEst_Wait: break;
						case impl.SimulEst_True: switchBallControlState(footballer, BallControlState_WaitingToStabilize, null); break;
						case impl.SimulEst_False: doTriggerHighBallFootAction(footballer, true); break;
					}
				}
			
			} break;
			
			case BallControlState_GuardingBall: {
			
				footballer.setBallCommandGuard(footballer.impl.genBallCommandID(), true, true);
			
			} break;
			
			case BallControlState_Stunned: {
			
				if (t >= mBallControlStunEndTime) {
				
					//print(footballer.impl.getNodeName() +" stun end");
					switchBallControlState(footballer, BallControlState_Undetermined, null);			
				} /*else {
				
					print(footballer.impl.getNodeName() + " stunned");
				}*/
			
			} break;
			
			case BallControlState_WaitingForShotEnd: {
			
				if (footballer.impl.getMatch().getBall().isResting()) {
				
					switchBallControlState(footballer, BallControlState_Undetermined, null);
				}
			
			} break;
		}
	}
	
	mBallControlState = -1;
	mBallControlStunEndTime = 0.0;
	
	mBallTrigger_ActiveColl = null;
	mBallTrigger_Pitch = null;
	mBallTrigger_FootControl = null;
}