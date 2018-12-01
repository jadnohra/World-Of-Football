
class BodyActionIdle_StartState {
}

class BodyAction_Idle extends BodyActionBase {

	static State_None = -1;
	static State_WaitingForBallOwnership = 0;
	static State_BallOwner = 1;
	static State_WaitingToTriggerStabilize = 2;
	static State_TriggerStabilizeBall = 3;
	static State_StabilizingBall = 4;
	static State_WaitingForBallPitchTouch = 5;
	static State_SweetSpottingBall = 6;
	static State_GuardingBall = 7;
	static State_FailedToControl = 8;
	
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
	
	function start(footballer, t, startState) {
	
		local impl = footballer.impl;
	
		setAnimIdle(footballer);		
		
		mState = State_None;
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
	
	function onBallPassiveSelfCollision(footballer, ball, boneID, isInFront) {
	
		if (!mIsAlternateAnim) 
			checkBallPainHit(footballer, ball, boneID);
	}	
	
	function onBallSelfCollision(footballer, ball, boneID) {
	
		if (mIsAlternateAnim) 
			return;
	
		switch (mState) {
		
			case State_None: 
			case State_WaitingForBallOwnership: {
			
				if (!checkBallPainHit(footballer, ball, boneID)) {
			
					doBallActionStabilize(footballer);
				}
			
			} break;
			
			case State_BallOwner: 
			case State_WaitingToTriggerStabilize: {
			
				if (!checkBallPainHit(footballer, ball, boneID)) {
			
					mState = State_TriggerStabilizeBall; 
				}
				
			} break;
			
			case State_WaitingForBallPitchTouch: {
			
				if (footballer.settings.boneIsFeet(boneID)) {
				
					mState = State_SweetSpottingBall; 
				}
			
			} break;
		}
	}
	
	function onBallPitchCollision(footballer) {
	
		switch (mState) {
	
			case State_StabilizingBall:
			case State_WaitingToTriggerStabilize:
			case State_WaitingForBallPitchTouch: {
		
				doBallActionSweetSpot(footballer);
				
			} break;
		}
	}
	
	function onBallCommandFeedback(footballer, commandID, accepted) {
	
		if (!accepted) {
		
			mState = State_FailedToControl;
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
	
	function doBallActionSweetSpot(footballer) {
	
		local settings = footballer.settings;
	
		footballer.impl.setBallControlSmoothTeleport(footballer.impl.genBallCommandID(), true, settings.BallSweetSpottingSpeed, settings.BallSweetSpottingMaxDuration, settings.BallSweetSpot, true, true);
		mState = State_GuardingBall;
	}
	
	function doBallActionStabilize(footballer) {
	
		footballer.setBallCommandStabilize();
		mState = State_StabilizingBall;
	}
	
	function handleBall(footballer, t) {
	
		local repeat = true;
		
		if (mIsAlternateAnim) {
		
			mState = State_None;
			return;
		}
		
		if (!footballer.impl.isBallOwner()) {
		
			mState = State_WaitingForBallOwnership;
			return;
		}
	
		local impl = footballer.impl;
		
		while (repeat) {
		
			repeat = false;
	
			switch (mState) {
			
				case State_None:
				case State_WaitingForBallOwnership: {
				
					if (impl.isBallOwner()) {
					
						mState = State_BallOwner;
						repeat = true;
					}
								
				} break;
				
				case State_BallOwner: {
				
					switch (impl.simulEstBallWillTouchPitchInCOC(footballer.settings.BallTouchPitchTolerance)) {
					
						case impl.SimulEst_Wait: break;
						case impl.SimulEst_True: mState = State_WaitingForBallPitchTouch; break;
						case impl.SimulEst_False: mState = State_WaitingToTriggerStabilize; break;
					}
				
				} break;
								
				case State_WaitingToTriggerStabilize: {
				} break;
								
				case State_TriggerStabilizeBall: {
				
					doBallActionStabilize(footballer);
				
				} break;
				
				case State_StabilizingBall: {
				
					switch (impl.simulEstBallWillTouchPitchInCOC(footballer.settings.BallTouchPitchTolerance)) {
					
						case impl.SimulEst_Wait: break;
						case impl.SimulEst_True: mState = State_WaitingForBallPitchTouch; break;
						case impl.SimulEst_False: mState = footballer.idle.EnableSweetSpotHighBalls || footballer.idle.EnableFootStabilizeHighBalls ? State_SweetSpottingBall : State_FailedToControl; break;
					}
				
				} break;
				
				case State_WaitingForBallPitchTouch: {
				} break;
				
				case State_SweetSpottingBall: {
					
					if (footballer.ballIsFootControlHeight()) {
		
						if (footballer.idle.EnableFootStabilizeHighBalls) {
						
							mState = State_TriggerStabilizeBall;
							repeat = true;
						
						} else {
		
							doBallActionSweetSpot(footballer);
						}
					}
									
				} break;
				
				case State_GuardingBall: {
				} break;
			}
		}
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
		
		handleBall(footballer, t);
	}
	
	mState = -1;
	mIsAlternateAnim = false;
}