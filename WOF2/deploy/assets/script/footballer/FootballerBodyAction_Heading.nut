
class FootballerBodyAction_Heading extends FootballerBodyAction_Base {

	function getType() { return Footballer.Action_Heading; }

	constructor() {
	
		mRequestBallShot = FootballerVarRequest();
		mRequestBallShot.value = FootballerBallShotRequestValue();
	
		mBallShotDir = Vector3();
		
		mJumpPhysicsPhase = PhaseWalker();
		
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
		
	function start(body, t) {
		
		{
			mJumpPhysics = null;
			local momentum = body.getAlignedNonZeroMomentum();

			if (momentum != null) {
			
				local magSq = momentum.magSq();
				
				if (magSq > 0.0) {
				
					mJumpPhysics = FootballerBodyJumpPhysics();
				}
			} 
		}
		
		mDidLand = false;
		
		local impl = body.mImpl;
		local settings = body.mProfile.shoot;

		settings.AnimProfiles[settings.Anim_Header].apply(body.mImpl, false);
				
		if (mJumpPhysics != null) {
		
			mJumpPhysicsPhase.start(this, settings.jumpProfile.PhysicsPhases, t);
			mJumpPhysics.init(body, settings.jumpProfile, body.mImpl.getFacing(), settings.HeaderJumpDistance_Default, null);
		}
		
		if (settings.HeaderSoundName != null)
			impl.playSound(settings.HeaderSoundName, impl.getPos(), settings.HeaderSoundMag);
		
		ballControlOnStart(body, t);
		
		body.setMomentum(null, false);
		return true;
	}
	
	function stop(body, t) {
	
		cancelRequests(body, t);
	
		mJumpPhysics = null;
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
		
		local actionTime = t - body.mActionStartTime;
				
		if (mJumpPhysics != null) {
		
			local physPhase = mJumpPhysicsPhase.advance(this, t);
			local settings = body.mProfile.shoot;
		
			mJumpPhysics.update(body, dt, settings.jumpProfile, physPhase.id, true);
			
			if (physPhase.id >= FootballerJumpProfile.PhysPhase_Ended) {
					
				mJumpPhysics = null;
					
				if (settings.LandSoundName != null) {			
			
					impl.playSound(settings.LandSoundName, impl.getPos(), settings.LandSoundMag);
				}
			}
		}
	
		
		{
			if (impl.nodeIsDirty()) {

				impl.collUpdate();
				body.resolveCollisions(t, false);
			}
		}
		
		handleBallControlState(body, t, actionTime);
		
		return handleActionEnd(body, t);
	}
	
	
	mRequestBallShot = null;
	mBallShotDir = null;
	
	mJumpPhysics = null;
	mJumpPhysicsPhase = null;
	mDidLand = false;
	
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
	
	function onBallSelfCollision(footballer, ball, boneID) { 
	
		if (boneID == footballer.mBody.mProfile.BoneID_Head && mBallControlState != BallControlState_PostShot) 
			doShot(footballer.mBody, ball);
	}	
	
	function onBallPassiveSelfCollision(footballer, ball, boneID, isInFront) { 
	
		if (isInFront && boneID == footballer.mBody.mProfile.BoneID_Head && mBallControlState != BallControlState_PostShot) 
			doShot(footballer.mBody, ball);
	}	
	
	function doShot(body, ball) {
	
		local ballShotInfo = mRequestBallShot.value;
	
		body.doHeaderShot(ball, ballShotInfo.shot, ballShotInfo.shot.speed * ballShotInfo.speedScale, getBallShotDir(body), body.getNonZeroMomentum());
		
		mRequestBallShot.end(body, FootballerBody.Request_Executed);
		mBallControlState = BallControlState_PostShot;
	}
	
	
	function onBallTriggerVolume(footballer, ball, volumeID) {
	
		doShot(footballer.mBody, ball);
	}
		
	function handleBallControlState(body, t, actionTime) {
	
		local isBallOwner = body.mImpl.isBallOwner();
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mBallControlState) {
			
				case BallControlState_None: {
				
					repeat = true;
				
					body.addTriggerVolumeHead(0);
					mBallControlState = BallControlState_PreShot;
					
				} break;
				
				case BallControlState_NotOwner: {
				
					if (isBallOwner) {
						
						repeat = true;
						
						mBallControlState = BallControlState_PreShot;
					}
				
				} break;
				
				case BallControlState_PreShot: {
				
					body.executeTriggerVolumesOnBall();
				
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