
class FootballerBodyAction_Shooting extends FootballerBodyAction_Base {

	function getType() { return Footballer.Action_Shooting; }

	constructor() {
	
		mRequestBallShot = FootballerVarRequest();
		mRequestBallShot.value = FootballerBallShotRequestValue();
	
		mBallShotDir = Vector3();
		
		ballControlOnConstructor();
	}

	function load(body, chunk, conv) {
	
		return true;
	}
	
	function setShotDir(body, dir) {
			
		if (dir != null)
			mBallShotDir.set(dir);
		else
			mBallShotDir.zero();
	}
	
	function requestShot(body, requestID, isHeader, shotType, shot, needCopyShot, shotSpeedScale) {
	
		if (!ballControlCanChangeShot(body))
			return ::FootballerBody.Request_Blocked;
	
		mRequestBallShot.init(body, requestID);
	
		if (shot != null) {
			
			if (needCopyShot) {
		
				mRequestBallShot.value.shot = createFootballerBallShot();
				mRequestBallShot.value.shot.set(startState.ballShot);
				
			} else {
			
				mRequestBallShot.value.shot = shot;
			}
			
			mRequestBallShot.value.shotType = shotType;
			
		} else {
		
			mRequestBallShot.value.shotType = body.Shot_High;
			mRequestBallShot.value.shot = body.mProfile.Shots[body.mProfile.DefaultShotLevel][mRequestBallShot.value.shotType];
		}
		
		if (shotSpeedScale != null)
			mRequestBallShot.value.speedScale = shotSpeedScale;
		else 	
			mRequestBallShot.value.speedScale = 1.0;
		
		mRequestBallShot.value.isHeader = isHeader;
		
		return ::FootballerBody.Request_Executing;
	}
	
	function setAnim(body, animIndex) {
	
		if (animIndex == null)
			animIndex = FootballerBodyActionProfile_Idle.Anim_Idle;
	
		body.mProfile.shoot.AnimProfiles[animIndex].apply(body.mImpl, false);
	}
	
	function start(body, t) {
		
		local impl = body.mImpl;
		local settings = body.mProfile.shoot;

		body.mProfile.shoot.AnimProfiles[body.mProfile.shoot.Anim_Kick].apply(body.mImpl, false);
		
		if (settings.KickSoundName != null)
			impl.playSound(settings.KickSoundName, impl.getPos(), settings.KickSoundMag);
		
		ballControlOnStart(body, t);
		
		body.setMomentum(null, false);
		return true;
	}
	
	function stop(body, t) {
	
		cancelRequests(body, t);
	
		ballControlOnStop(body, t);
	
		::FootballerBodyAction_Base.stop(body, t);
	}
	
	function cancelRequests(body, t) {
	
		mRequestBallShot.end(body, FootballerBody.Request_Blocked);
	}
	
	
	function handleActionEnd(body, t) {

		mRequestBallShot.signal(body, FootballerBody.Request_Executing);
	
		if (body.mImpl.isAnimFinished()) {
		
			return false;
		} 
		
		return true;
	}
			
	function frameMove(body, t, dt) {
			
		local impl = body.mImpl;	
				
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		{
			if (impl.nodeIsDirty()) {

				impl.collUpdate();
				body.resolveCollisions(t, false);
			}
		}
		
		handleBallControlState(body, t, t - body.mActionStartTime);
		
		return handleActionEnd(body, t);
	}
	
	
	mRequestBallShot = null;
	mBallShotDir = null;
	
	/*******************
		Ball Handling
	********************/
	
	function ballControlOnConstructor() {
	}
	
	function ballControlOnStart(body, t) {
	
		mBallControlState = BallControlState_None;
	}
	
	function ballControlOnStop(body, t) {
	
		body.clearTriggerVolumes();
	
		if (!body.mImpl.isShootingBall())
			body.mImpl.cancelBallControl();
	}
	
	function ballControlCanChangeShot(body) {
	
		return mBallControlState != BallControlState_PostShot;
	}
	
	function getBallShotDir(body) {
	
		local shotDir;
		local facingDir = body.mImpl.getFacing();
		local ballShotDir = mBallShotDir;

		if (ballShotDir == null || ballShotDir.isZero()) {
		
			shotDir = facingDir;
		
		} else {
		
			if (ballShotDir.dot(facingDir) == 0.0) {
			
				shotDir = Vector3();
			
				lerpv3(facingDir, ballShotDir, 0.5, shotDir);
				
				if (shotDir.dot(facingDir) > 0.0) {
				
					return shotDir;
				}
			} 
			
			if (ballShotDir.dot(facingDir) > 0.0) {
			
				shotDir = ballShotDir;
			
			} else {

				shotDir = facingDir;
			}
		}
		
		return shotDir;
	}
	
	function doShot(body, ball) {
	
		local ballShotInfo = mRequestBallShot.value;
	
		body.shootBall(getMatch().genBallCommandID(), ball, ballShotInfo.shot, ballShotInfo.shot.speed * ballShotInfo.speedScale, getBallShotDir(body), null, 
							true, true, true, body.getSkillBallProtection(), body.getSkillBallControlBodyOrFeet(), false, body.mProfile.BallShotSwitchInfluence, body.mProfile.BallShotSoundName);
	
		mRequestBallShot.end(body, FootballerBody.Request_Executed);
		mBallControlState = BallControlState_PostShot;
	}
	
	function handleBallControlState(body, t, actionTime) {
	
		local isBallOwner = body.mImpl.isBallOwner();
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mBallControlState) {
			
				case BallControlState_None: {
				
					repeat = true;
				
					if (isBallOwner) {
						
						mBallControlState = BallControlState_PreShot;
							
					} else {
						
						mBallControlState = BallControlState_NotOwner;
					}
				
				} break;
				
				case BallControlState_NotOwner: {
				
					if (isBallOwner) {
						
						repeat = true;
						
						mBallControlState = BallControlState_PreShot;
					}
				
				} break;
				
				case BallControlState_PreShot: {
				
					
					if (actionTime >= body.mProfile.shoot.ShootPhase_StartTime) {

						if (isBallOwner) {
						
							local match = body.mImpl.getMatch();
							local ball = match.getBall();
							
							if (body.ballIsFootControlHeight()) {
							
								doShot(body, ball);
							} 
						} 
					} 
				
				} break;
				
				case BallControlState_PostShot: {
				} break;
			}
		
			
		}
	}
	
	mBallControlState = -1;
	
	static BallControlState_None = -1;
	static BallControlState_NotOwner = 0;
	static BallControlState_PreShot = 1;
	static BallControlState_PostShot = 2;
}