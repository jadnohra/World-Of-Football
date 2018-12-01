
class FootballerBodyAction_Tackling extends FootballerBodyAction_Base {

	function getType() { return Footballer.Action_Tackling; }

	constructor() {
	
		mSlideParams = FootballerSettings_Tackle_SlideParams();
		mShotDir = Vector3();
		
		mCurrPhysPhase = PhaseWalker();
		mCurrTacklePhase = PhaseWalker();
		
		ballControlOnConstructor();
	}

	function load(body, chunk, conv) {
	
		mSettings = body.mProfile.tackle;
	
		return true;
	}
	
	function start(body, t) {
	
		mIsSlideBlocked = false;
	
		local impl = body.mImpl;
	
		local speed = 0.0;
		local dir = impl.getFacing();
	
		local momentum = body.getAlignedNonZeroMomentum();
				
		if (momentum != null) {
		
			local magSq = momentum.magSq();
			
			if (magSq > 0.0) {
			
				speed = sqrtf(magSq);
			
				dir = Vector3();
				momentum.div(speed, dir);
			}
		} 	
				
		local changeFacing = !dir.equals(impl.getFacing());
				
		//print("mIsBallControlTackle: " + mIsBallControlTackle);
	
		mIsSlidingTackle = (speed != 0.0);
		
		if (mIsSlidingTackle) {
		
			mSlideParams.init(body, dir, speed);
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
			
		mShotDir.zero();
	
		mProcessedTackleColliders = {};
	
		ballControlOnStart(body, t);
		
		body.setMomentum(null, false);
		
		return true;
	}
	
	function stop(body, t) {
	
		 ballControlOnStop(body, t);
		
		::FootballerBodyAction_Base.stop(body, t);
	}
	
	function setIsBallControlTackle(value) {
	
		if (mBallControlState != BallControlState_BallAction)
			mIsBallControlTackle = value;
	}
	
	function setShotDir(body, dir) {
	
		if (dir != null)
			mShotDir.set(dir);
		else 
			mShotDir.zero();
	}
	
	function handleActionEnd(body) {
	
		if (body.mImpl.isAnimFinished()) {
		
			return false;
		}
		
		return true;
	}
	
	function checkColliders(body, t, dt, isTacklePhase) {
	
		if (!isTacklePhase) {
		
			return;
		}
	
		local impl = body.mImpl;
	
		local index = Int(0);
		local collider;
		local bodyType = body.mFootballer.getTypeInt();
		
		local TacklePowerRandomAmount = mSettings.TacklePowerRandomAmount;
		
		while ((collider = impl.collFindDynVolColliderInt(index, bodyType)) != null) {
	
			if (!(collider in mProcessedTackleColliders)) {
	
				if (body.getPhysicalPowerTackle() + rand2f(TacklePowerRandomAmount) > collider.mBody.getPhysicalAgilityBalance()) {
			
					collider.mBody.requestActionFalling(null, t, dt, FootballerBodyAction_Falling.createFallDirByTargetPos(collider.mBody, impl.getPos()), false);
							
				} else {
				
					mIsSlideBlocked = true;
				}
				
				mProcessedTackleColliders[collider] <- true;
			}
		}
	}
	
	
	function frameMove(body, t, dt) {

		local physPhase = mCurrPhysPhase.advance(this, t);
		local tacklePhase = mCurrTacklePhase.advance(this, t);
		local isTacklePhase = (tacklePhase.id == mSettings.TacklePhase_Tackle);
		local isAnimFrozen = (physPhase.id == mSettings.PhysPhase_SlideMiddle);
		
		local impl = body.mImpl;		
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
				checkColliders(body, t, dt, /*isTacklePhase && */ physPhase && !mIsSlideBlocked);
				body.resolveCollisions(t, /*isTacklePhase && */physPhase && !mIsSlideBlocked);
				
				if (impl.getCollState() == impl.Coll_Blocked) {

					//break slide	
					mIsSlidingTackle = false;
				}
			}
		}
		
		handleBallControlState(body, t, isTacklePhase);

		return handleActionEnd(body);
	}

	mSettings = null;	
	mSlideParams = null;
	mIsSlidingTackle = false;
	mIsBallControlTackle = false;
	mIsSlideBlocked = false;
	
	mCurrPhysPhase = null;
	mCurrTacklePhase = null;
	
	mShotDir = null;
		
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
	
	function ballControlOnStart(body, t) {
	
		mBallControlState = BallControlState_None;
		body.clearTriggerVolumes();
	}
	
	function ballControlOnStop(body, t) {
	
		body.clearTriggerVolumes();
	
		if (!body.mImpl.isShootingBall())
			body.mImpl.cancelBallControl();
	}
	
	function switchBallControlState(body, nextState, setup) {
	
		local state = mBallControlState;
		endBallControlState(body, nextState, setup);
		startBallControlState(body, nextState, setup, state);
	}
	
	function endBallControlState(body, nextState, setup) {
	
		switch (mBallControlState) {
		
			case BallControlState_Active: {
			
				body.clearTriggerVolumes();
			
			} break;
		}
	}
	
	function onBallTriggerVolume(footballer, ball, volumeID) { 
	
		doBallAction(footballer.mBody, ball);
	
		switchBallControlState(footballer.mBody, BallControlState_BallAction, null);
	}
	
	function doBallAction(body, ball) {
	
		local impl = body.mImpl;
		local match = impl.getMatch();

		/*
			This condition could be a it less strict (height + energy)
		*/
		if (mIsBallControlTackle && !ball.isOnPitch()) {
		
			mIsBallControlTackle = false;
		}
		
		if (mIsBallControlTackle) {
		
			impl.getWorldTransform().invTransformPoint(ball.getPos(), mBallControlRelPos);
		
			impl.setBallControlImmediateTeleport(match.genBallCommandID(), body.getSkillBallTackleClean(), 
													true, mBallControlRelPos, true, true);
		
		} else {
		
			local settings = body.mProfile.tackle;
			local shot = settings.shot;
			local shotDir = mShotDir.isZero() ? impl.getFacing() : mShotDir;
					
			body.shootBall(match.genBallCommandID(), ball, shot, shot.speed, shotDir, null, 
									true, true, true, body.getSkillBallTackleClean(), body.getSkillBallControlBodyOrFeet(), false, settings.ShotSwitchInfluence, settings.ShotSoundName);
														
		}

	}
	
	function startBallControlState(body, newState, setup, prevState) {
	
		switch (newState) {
		
			case BallControlState_Active: {
			
				body.addTriggerVolumesFeet(0, 1);
			
			} break;
		
			default: {
			
			} break;
		}
		
		mBallControlState = newState;
		//print(body.mImpl.getNodeName() + ": " + mBallControlState);
	}
	
	function handleBallControlState(body, t, isTacklePhase) {
	
		local repeat = true;
	
		while (repeat) {
	
			repeat = false;
	
			switch (mBallControlState) {
			
				case BallControlState_None: {
				
					if (isTacklePhase) {
					
						switchBallControlState(body, BallControlState_Active, null);
						repeat = true;
					}
									
				} break;
				
				case BallControlState_Active: {
				
					if (isTacklePhase) {
					
						body.executeTriggerVolumesOnBall();
						
					} else {
					
						switchBallControlState(body, BallControlState_None, null);
					}
				
				} break;
			
				case BallControlState_BallAction: {
				
					if (mIsBallControlTackle) {
					
						local impl = body.mImpl;
						local match = impl.getMatch();
					
						if (isTacklePhase) {
						
							impl.setBallControlImmediateTeleport(match.genBallCommandID(), body.getSkillBallProtection(), 
														true, mBallControlRelPos, true, true);
						
						} else {
						
							body.shootBall(impl.genBallCommandID(), match.getBall(), body.mProfile.ZeroShot, 0.0, impl.getFacing(), null,
												false, false, false, body.getSkillBallProtection(), body.getSkillBallControlBodyOrFeet(), true, 0.0, null);
						}
					}
					
					if (!isTacklePhase) {
					
						switchBallControlState(body, BallControlState_None, null);
					}
					
				} break;
			}
		}
	
	}
	
	mBallControlState = null;
	mBallControlRelPos = null;
}