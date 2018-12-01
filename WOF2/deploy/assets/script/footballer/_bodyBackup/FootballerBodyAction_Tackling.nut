
class BodyActionTackling_StartState {
	
	isBallControlTackle = null; //optional
}

class FootballerBodyAction_Tackling extends FootballerBodyAction_Base {

	function getType() { return Footballer.Action_Tackling; }

	constructor() {
	
		mSlideParams = FootballerSettings_Tackle_SlideParams();
		mBallTackleDir = Vector3();
		
		mCurrPhysPhase = PhaseWalker();
		mCurrTacklePhase = PhaseWalker();
		
		ballControlOnConstructor();
	}

	function load(footballer, chunk, conv) {
	
		mSettings = footballer.settings.tackle;
	
		return true;
	}
	
	function isFinished() { return mIsFinished; }	
	
	function createStartState(footballer, isBallControlTackle) {
	
		local startState = BodyActionTackling_StartState();
		
		startState.isBallControlTackle = isBallControlTackle;
		
		return startState;
	}
	
	function start(footballer, t, startState, prevAction) {
	
		mIsFinished = false;
		mIsSlideBlocked = false;
	
		local impl = footballer.impl;
	
		local speed = 0.0;
		local dir = impl.getFacing();
	
		local momentum = footballer.getBodyFacingNonZeroMomentum();
				
		if (momentum != null) {
		
			local magSq = momentum.magSq();
			
			if (magSq > 0.0) {
			
				speed = sqrtf(magSq);
			
				dir = Vector3();
				momentum.div(speed, dir);
			}
		} 	
				
		local changeFacing = !dir.equals(impl.getFacing());
		
		if (startState != null && startState.isBallControlTackle != null) {
		
			mIsBallControlTackle = startState.isBallControlTackle;
			
		} else {
		
			mIsBallControlTackle = false;
		}
	
		//print("mIsBallControlTackle: " + mIsBallControlTackle);
	
		mIsSlidingTackle = (speed != 0.0);
		
		if (mIsSlidingTackle) {
		
			mSlideParams.init(footballer, dir, speed);
		}
		
		if (changeFacing) {
			
			impl.setFacing(dir);
		}
		
		mCurrPhysPhase.start(this, mSettings.PhysicsPhases, t);
		mCurrTacklePhase.start(this, mSettings.TacklePhases, t);
				
		impl.setAnim(mSettings.AnimName, true);
		impl.setAnimLoop(false);
		impl.setAnimSpeed(mSettings.AnimSpeed);
		
		impl.playSound(mSettings.SoundName, impl.getPos(), mSettings.SoundMag);
			
		mBallTackleDir.zero();
	
		mProcessedTackleColliders = {};
	
		ballControlOnStart(footballer, t, startState);
		/*
		footballer.ballHandler.reset(footballer, true);
		*/
		
		footballer.setBodyMomentum(null, false);
		return true;
	}
	
	function stop(footballer, t) {
	
		mIsFinished = true;
		
		 ballControlOnStop(footballer, t);
		/*
		footballer.ballHandler.reset(footballer, true);
		*/
	}
	
	function setIsBallControlTackle(value) {
	
		if (mBallControlState != BallControlState_BallAction)
			mIsBallControlTackle = value;
	}
	
	function setBallTackleDirection(footballer, dir) {
	
		if (dir != null)
			mBallTackleDir.set(dir);
		else 
		mBallTackleDir.zero();
	}
	
	function handleActionEnd(footballer) {
	
		if (footballer.impl.isAnimFinished()) {
		
			mIsFinished = true;
		}
	}
	
	function checkColliders(footballer, t, isTacklePhase) {
	
		if (!isTacklePhase) {
		
			return;
		}
	
		local impl = footballer.impl;
	
		local index = Int(0);
		local collider;
		local footballerType = footballer.getTypeInt();
		
		local TacklePowerRandomAmount = mSettings.TacklePowerRandomAmount;
		
		while ((collider = impl.collFindDynVolColliderInt(index, footballerType)) != null) {
	
			if (!(collider in mProcessedTackleColliders)) {
	
				if (footballer.getPhysicalPowerTackle() + rand2f(TacklePowerRandomAmount) > collider.getPhysicalAgilityBalance()) {
			
					collider.getAction().switchTackled(collider, t, footballer);
				} else {
				
					mIsSlideBlocked = true;
				}
				
				mProcessedTackleColliders[collider] <- true;
			}
		}
	}
	
	
	function frameMove(footballer, t, dt) {

		if (mIsFinished) {
			
			switchIdle(footballer, t, null);
			return;
		}
		
		local physPhase = mCurrPhysPhase.advance(this, t);
		local tacklePhase = mCurrTacklePhase.advance(this, t);
		local isTacklePhase = (tacklePhase.id == mSettings.TacklePhase_Tackle);
		local isAnimFrozen = (physPhase.id == mSettings.PhysPhase_SlideMiddle);
		
		local impl = footballer.impl;		
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(isAnimFrozen ? 0.0 : dt); //HAS! to be called with 0.0 when frozen
		}
		
		if (mIsSlidingTackle) 
		{	
			local posDiff = Vector3();
		
			if (physPhase.id == mSettings.PhysPhase_SlideEnd) 
				mSlideParams.velocity.subtractNoReverse(mSlideParams.deceleration * dt);
			
			mSlideParams.velocity.mul(dt, posDiff);
			impl.shiftPos(posDiff);
		}
				
		
		if (isTacklePhase) {
		
			/*
			impl.setBodyCollVelocity(mSlideParams.velocity, true);
			impl.disableLimbGroupCollVelocity("feet");
			*/
		} 
		
		
		{
			if (impl.nodeIsDirty()) {

				impl.collUpdate();
				checkColliders(footballer, t, /*isTacklePhase && */ physPhase && !mIsSlideBlocked);
				footballer.resolveCollisions(footballer, t, /*isTacklePhase && */physPhase && !mIsSlideBlocked);
			}
		}
		
		handleBallControlState(footballer, t, isTacklePhase);

		handleActionEnd(footballer);
	}

	mSettings = null;	
	mSlideParams = null;
	mIsFinished = true;
	mIsSlidingTackle = false;
	mIsBallControlTackle = false;
	mIsSlideBlocked = false;
	
	mCurrPhysPhase = null;
	mCurrTacklePhase = null;
	
	mBallTackleDir = null;
		
	mProcessedTackleColliders = null;

	
	/*******************
		Ball Handling
	********************/
	
	static BallControlState_None = 0;
	static BallControlState_Active = 1;
	static BallControlState_BallAction = 2;
	
	function ballControlOnConstructor() {
	
		mBallControlRelPos = Vector3();
	}
	
	function ballControlOnStart(footballer, t, startState) {
	
		mBallControlState = BallControlState_None;
		footballer.clearTriggerVolumes();
	}
	
	function ballControlOnStop(footballer, t) {
	
		footballer.clearTriggerVolumes();
	
		if (!footballer.impl.isShootingBall())
			footballer.impl.cancelBallControl();
	}
	
	function switchBallControlState(footballer, nextState, setup) {
	
		local state = mBallControlState;
		endBallControlState(footballer, nextState, setup);
		startBallControlState(footballer, nextState, setup, state);
	}
	
	function endBallControlState(footballer, nextState, setup) {
	
		switch (mBallControlState) {
		
			case BallControlState_Active: {
			
				footballer.clearTriggerVolumes();
			
			} break;
		}
	}
	
	function onBallTriggerVolume(footballer, ball, volumeID) { 
	
		doBallAction(footballer, ball);
	
		switchBallControlState(footballer, BallControlState_BallAction, null);
	}
	
	function doBallAction(footballer, ball) {
	
		local impl = footballer.impl;
		local match = impl.getMatch();

		/*
			This condition could be a it less strict (height + energy)
		*/
		if (mIsBallControlTackle && !ball.isOnPitch()) {
		
			mIsBallControlTackle = false;
		}
		
		if (mIsBallControlTackle) {
		
			impl.getWorldTransform().invTransformPoint(ball.getPos(), mBallControlRelPos);
		
			impl.setBallControlImmediateTeleport(match.genBallCommandID(), footballer.getSkillBallTackleClean(), 
													true, mBallControlRelPos, true, true);
		
		} else {
		
			local settings = footballer.settings.tackle;
			local shot = settings.shot;
			local shotDir = mBallTackleDir.isZero() ? impl.getFacing() : mBallTackleDir;
					
			footballer.shootBall(match.genBallCommandID(), ball, shot, shot.speed, shotDir, null, 
									true, true, true, footballer.getSkillBallTackleClean(), footballer.getSkillBallControlBodyOrFeet(), false, settings.ShotSwitchInfluence, settings.ShotSoundName);
														
		}

	}
	
	function startBallControlState(footballer, newState, setup, prevState) {
	
		switch (newState) {
		
			case BallControlState_Active: {
			
				footballer.addTriggerVolumesFeet(0, 1);
			
			} break;
		
			default: {
			
			} break;
		}
		
		mBallControlState = newState;
		//print(footballer.impl.getNodeName() + ": " + mBallControlState);
	}
	
	function handleBallControlState(footballer, t, isTacklePhase) {
	
		local repeat = true;
	
		while (repeat) {
	
			repeat = false;
	
			switch (mBallControlState) {
			
				case BallControlState_None: {
				
					if (isTacklePhase) {
					
						switchBallControlState(footballer, BallControlState_Active, null);
						repeat = true;
					}
									
				} break;
				
				case BallControlState_Active: {
				
					if (isTacklePhase) {
					
						footballer.executeTriggerVolumesOnBall();
						
					} else {
					
						switchBallControlState(footballer, BallControlState_None, null);
					}
				
				} break;
			
				case BallControlState_BallAction: {
				
					if (mIsBallControlTackle) {
					
						local impl = footballer.impl;
						local match = impl.getMatch();
					
						if (isTacklePhase) {
						
							impl.setBallControlImmediateTeleport(match.genBallCommandID(), footballer.getSkillBallProtection(), 
														true, mBallControlRelPos, true, true);
						
						} else {
						
							footballer.shootBall(impl.genBallCommandID(), match.getBall(), footballer.settings.ZeroShot, 0.0, impl.getFacing(), null,
												false, false, false, footballer.getSkillBallProtection(), footballer.getSkillBallControlBodyOrFeet(), true, 0.0, null);
						}
					}
					
					if (!isTacklePhase) {
					
						switchBallControlState(footballer, BallControlState_None, null);
					}
					
				} break;
			}
		}
	
	}
	
	mBallControlState = null;
	mBallControlRelPos = null;
}