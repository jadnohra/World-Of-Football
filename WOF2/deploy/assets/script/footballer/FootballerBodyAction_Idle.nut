
class FootballerBodyAction_Idle extends FootballerBodyAction_BaseBallTriggers {

	function getType() { return Footballer.Action_Idle; }
	function getStaminaGain(body) { return body.mProfile.idle.StaminaGain; }
	
	constructor() {
	
		ballControlOnConstructor();
	}
	
	function setAnim(body, animIndex) {
	
		if (animIndex == null)
			animIndex = FootballerBodyActionProfile_Idle.Anim_Idle;
	
		body.mProfile.idle.AnimProfiles[animIndex].apply(body.mImpl, false);
	}
	
	function handleInterBodySignals(body, t, dt) {

		/*
		local signals = body.getInterBodySignals();
		
		foreach (signal in signals) {
		
			body.switchActionFalling(true, t, dt);
			return false;
		}
		*/
		
		return true;
	}	
	
	function start(body, t) {
	
		setAnim(body, null);		
		
		ballControlOnStart(body, t);
		
		body.setMomentum(null, false);					
		
		return true;
	}
	
	function stop(body, t) {
		
		ballControlOnStop(body, t);
		
		::FootballerBodyAction_Base.stop(body, t);
	}
	
	function frameMove(body, t, dt) {
			
		handleInterBodySignals(body, t, dt);
			
		local impl = body.mImpl;

		impl.nodeMarkDirty();

		{	
			if (impl.isAnimFinished()) {
			
				setAnim(body, null);
				
			} else {
		
				impl.addAnimTime(dt);
			}
		}
		
		handleBallControlState(body, t);
		
		return true;
	}
		

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
	
	function ballControlOnStart(body, t) {
	
		local wasLastShotActive = false;
		local prevAction = body.mLastAction;
	
		if (prevAction != null && prevAction.getType() == Footballer.Action_Running) {
		
			//print("wasLastShotActive");
			wasLastShotActive = prevAction.wasLastShotActive();
			
		} else {
		
			wasLastShotActive = body.mImpl.isBallOwner() && body.mImpl.isShootingBall();
		}
	
		mBallControlState = BallControlState_None;
		switchBallControlState(body, BallControlState_Undetermined, wasLastShotActive);
	}
	
	function ballControlOnStop(body, t) {
	
		body.mImpl.cancelBallControl();
	}
	
	function onLostBall(body, ball) { switchBallControlState(body, BallControlState_NotOwner, null); }
	function onAcquiredBall(body, ball, isSolo) { 
	
		//because this state could have been set by a fast collision trigger
		if (mBallControlState < BallControlState_Owner)
			switchBallControlState(body, BallControlState_Owner, null); 
	}
	
	function onBallCommandRejected(footballer, ball, commandID) {
	
		switchBallControlState(footballer.mBody, BallControlState_Stunned, null);
	}
	
	function onBallTrigger(triggerID, footballer, ball) {
	
		//print("trigger: " + triggerID);
	
		switch (triggerID) {
		
			case BallTriggerID_Stabilize: {
			
				doBallActionStabilize(footballer.mBody, ball, null);
			
			} break;
		
			case BallTriggerID_Stabilized: {
			
				doBallActionSweetSpot(footballer.mBody, ball);
			
			} break;
			
			case BallTriggerID_SweetSpot: {
			
				doBallActionSweetSpot(footballer.mBody, ball);
			
			} break;
		}
		
		return false;
	}
	
	function onBallCommandRejected(footballer, ball, commandID) {
	
		//print(body.mImpl.getNodeName() + "rejected");
	
		switchBallControlState(footballer.mBody, BallControlState_Stunned, null);
	}	
	
	function doBallActionSweetSpot(body, ball) {
	
		//avoid jittering
		if (getMatch().getTime() - body.mActionStartTime > 0.3) {
	
			setAnim(body, FootballerBodyActionProfile_Idle.Anim_SweetSpotRight);
		}
	
		local settings = body.mProfile;
	
		body.mImpl.setBallControlSmoothTeleport(body.mImpl.genBallCommandID(), body.getSkillBallControlBodyOrFeet(), true, settings.BallSweetSpottingSpeed, settings.BallSweetSpottingMaxDuration, settings.BallSweetSpot, true, true);
		switchBallControlState(body, BallControlState_GuardingBall, null);
	}
	
	function doBallActionStabilize(body, ball, soundName) {
	
		//body.setBallCommandStabilize(body.mImpl.genBallCommandID(), ball, 0.0, body.mImpl.getFacing(), false, true, false, soundName);
		body.shootBall(body.mImpl.genBallCommandID(), ball, body.mProfile.ZeroShot, 0.0, body.mImpl.getFacing(), null,
								false, false, false, body.getSkillBallProtection(), body.getSkillBallControlBodyOrFeet(), true, 0.0, soundName);
		switchBallControlState(body, BallControlState_TriggeredStabilize, null);
	}
	
	function switchBallControlState(body, nextState, setup) {
	
		//print ("switch");
	
		local state = mBallControlState;
		endBallControlState(body, nextState, setup);
		startBallControlState(body, nextState, setup, state);
	}
	
	function endBallControlState(body, nextState, setup) {
	
		mBallTriggers.resize(0);
	}
	
	function startBallControlState(body, newState, setup, prevState) {
	
		switch (newState) {
		
			case BallControlState_None:
			case BallControlState_Undetermined: {
			
				mBallControlState = newState;
				local leaveShot = false;
			
				if (setup != null) {
				
					leaveShot = setup;
				}
			
				if (body.mImpl.isBallOwner()) {
				
					if (leaveShot) {
					
						switchBallControlState(body, BallControlState_WaitingForShotEnd, null);
					
					} else {
				
						switchBallControlState(body, BallControlState_Owner, null);
					}
					
				} else {
				
					switchBallControlState(body, BallControlState_NotOwner, null);
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
				handleBallControlState(body, body.mImpl.getMatch().getTime());
			
			} break;
		
			case BallControlState_Stunned: {
		
				mBallControlStunEndTime = body.mImpl.getMatch().getTime() + body.getPhysicalAgilityReflexReactionTime();
				
				//print(body.mImpl.getNodeName() + "react time: " + body.getBallControlReactionTime() + " endt: " + mBallControlStunEndTime);
				
			} break;
		
			default: {
			
			} break;
		}
		
		mBallControlState = newState;
		//print(body.mImpl.getNodeName() + ": " + mBallControlState);
	}
	
	function doTriggerHighBallFootAction(body, failOnNoAction) {
	
		switch (body.mProfile.idle.HighBallFootAction) {
							
			case FootballerBodyActionProfile_Idle.HighBallFootAction_None: if (failOnNoAction) switchBallControlState(body, BallControlState_Failed, null); break;
			case FootballerBodyActionProfile_Idle.HighBallFootAction_Stabilize: switchBallControlState(body, BallControlState_TriggerAction, true); break;
			case FootballerBodyActionProfile_Idle.HighBallFootAction_SmoothControl: switchBallControlState(body, BallControlState_TriggerAction, false); break;
		}
	}
	
	function doHighBallFootAction(body, ball) {
	
		switch (body.mProfile.idle.HighBallFootAction) {
							
			case FootballerBodyActionProfile_Idle.HighBallFootAction_None: break;
			case FootballerBodyActionProfile_Idle.HighBallFootAction_Stabilize: doBallActionStabilize(body, ball, body.mProfile.BallShotSoundName); return true; break;
			case FootballerBodyActionProfile_Idle.HighBallFootAction_SmoothControl: doBallActionSweetSpot(body, ball); return true; break;
		}
		
		return false;
	}
	
	function handleBallControlState(body, t) {
	
		switch (mBallControlState) {
		
			case BallControlState_Owner: {
			
				local nextSample = CBallTrajSample();
				local impl = body.mImpl;
				local ball = impl.getMatch().getBall();
				
				if (impl.simulNextBallSample(ball, nextSample) && body.isBallStateImprovingIdle(ball, nextSample, false)) {
				
					//wait for next sample
					//print("improving");
					
				} else {
				
					if (ball.isOnPitch()) {
							
						//print("sweetSpot");
						doBallActionSweetSpot(body, ball);
							
					} else {

						if (body.ballIsFootControlHeight()
							&& doHighBallFootAction(body, ball)) {						
							
						} else {	
						
							doTriggerHighBallFootAction(body, false);
						}
					}
				}
		
			} break;
		
			case BallControlState_TriggeredStabilize: {
			
				local impl = body.mImpl;
				local ball = impl.getMatch().getBall();
				
				if (ball.isOnPitch()) {
				
					onBallTrigger(BallTriggerID_Stabilized, body.mFootballer, ball);
				
				} else {
				
					switch (impl.simulEstBallWillTouchPitchInCOC()) {
						
						case impl.SimulEst_Wait: break;
						case impl.SimulEst_True: switchBallControlState(body, BallControlState_WaitingToStabilize, null); break;
						case impl.SimulEst_False: doTriggerHighBallFootAction(body, true); break;
					}
				}
			
			} break;
			
			case BallControlState_GuardingBall: {
			
				body.setBallCommandGuard(body.mImpl.genBallCommandID(), true, true);
			
			} break;
			
			case BallControlState_Stunned: {
			
				if (t >= mBallControlStunEndTime) {
				
					//print(body.mImpl.getNodeName() +" stun end");
					switchBallControlState(body, BallControlState_Undetermined, null);			
				} /*else {
				
					print(body.mImpl.getNodeName() + " stunned");
				}*/
			
			} break;
			
			case BallControlState_WaitingForShotEnd: {
			
				if (body.mImpl.getMatch().getBall().isResting()) {
				
					switchBallControlState(body, BallControlState_Undetermined, null);
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