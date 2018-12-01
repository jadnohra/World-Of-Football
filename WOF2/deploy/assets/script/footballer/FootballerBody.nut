
class FootballerBody {

	function constructor() {
	
		mMomentum = Vector3();
		mMomentum.zero();
		
		mIntraBodySignals = [];
		
		constructBodyState();
	}

	function getDefaultRunSpeed() { return mDefaultRunSpeed; }
	
	function load(footballer, chunk, conv) {

		mFootballer = footballer;
		mImpl = footballer.mImpl;
	
		mImpl.collEnable(true);
		
		mProfile = mImpl.getMatch().getScriptMatch().getSharedFootballerBodyProfile();
				
		if (!mProfile.onAttached(this, null)){
			
			assrt("Failed: Footballer Profile.onAttached");
			return false;
		}
		
		loadAttributes(chunk, conv);	
					
		mActionIdle = FootballerBodyAction_Idle();
		mActionRunning = FootballerBodyAction_Running();
		mActionTackling = FootballerBodyAction_Tackling();
		mActionFalling = FootballerBodyAction_Falling();
		mActionJumping = FootballerBodyAction_Jumping();
		mActionShooting = FootballerBodyAction_Shooting();
		mActionHeading = FootballerBodyAction_Heading();
		mActionTurning = FootballerBodyAction_Turning();
	
		if (!mActionIdle.load(this, chunk, conv)
			|| !mActionRunning.load(this, chunk, conv)
			|| !mActionTackling.load(this, chunk, conv)
			|| !mActionFalling.load(this, chunk, conv)
			|| !mActionJumping.load(this, chunk, conv)
			|| !mActionShooting.load(this, chunk, conv)
			|| !mActionTurning.load(this, chunk, conv)) {
			
			assrt("Failed: Footballer Body Actions Load");
			return false;
		}
	
		mDefaultRunSpeed = mProfile.run.getSpeedValue(mProfile.run.Run_Normal, mProfile.run.RunSpeed_Normal);
			
		return true;
	}
	
	function prepareScene(footballer) {
	
		if (!mImpl.createPositionProxy(0.75)) {
		
			assrt("Failed: Footballer createPositionProxy");
			return false;
		}
		
		mImpl.setRelativeCOCOffset(mProfile.COCOffset);
		
		{
			if (!mProfile.addTriggerVolumes(mFootballer)) {
			
				assrt("Failed: Footballer addTriggerVolumes");
				return false;
			}
		}
			
		return true;
	}
	
	function start(footballer, t) {
	
		if (!mActionIdle.update(this, t, 0.0, false)) {
		
			assrt("Failed: Footballer Body Action Idle Start");
			return false;
		}
		
		mAction = mActionIdle;
		
		return true;
	}
		
	function preFrameMove(footballer, t, dt) {

		if (mProfile.EnableLimitAfterTouchTime) {
		
			if (mImpl.isBallAfterTouchOwner()) {
			
				if (mEndAfterTouchTime == 0.0) { 
				
					mEndAfterTouchTime = t + mProfile.AfterTouchTimeLimit;
					
				} else if (t >= mEndAfterTouchTime) {
				
					giveUpBallAfterTouch();
				}
			
			} else {
			
				mEndAfterTouchTime = 0.0;
			}
		}
	}
	
	function frameMove(footballer, t, dt) {

		handleRequests(t, dt);
		
		if (!mAction.update(this, t, dt, true)) {
		
			onActionFinished(mAction, t, dt);
		}
		
		resetIntraBodySignals();
		updateStamina(dt);
	}
	
	
	function getIntraBodySignals() { return mIntraBodySignals; }
	function addIntraBodySignal(signal) { mIntraBodySignals.append(signal); }
	function resetIntraBodySignals() { mIntraBodySignals.resize(0); }
	
	function resolveCollisions(t, enableGhostMode) {
	
		mImpl.collResolve(enableGhostMode, false);
	}
	
	function giveUpBallAfterTouch() { 
	
		mImpl.giveUpBallAfterTouch(); 
		mEndAfterTouchTime = 0.0; 
	}
	
	function shouldApplyBallAfterTouch() { 
	
		return mImpl.isBallAfterTouchOwner(); 
	}
	
	function isSwitchLocked(footballer) { 
	
		return mAction.isSwitchLocked(this); 
	}
			
	/***************************
		Body State
	****************************/		
			
	static Request_None = -1;		
	static Request_Invalid = 0;		
	static Request_Executing = 1;
	static Request_Blocked = 2;	
	static Request_Executed = 3;
				
	//mBodydStateRequests = null;	
	mBodyStateRequestResults = null;
			
	function constructBodyState() {
	
		//mBodydStateRequests = [];
		mBodyStateRequestResults = {};
	}

	function switchAction(newAction, t, dt) { 
	
		if (mAction != newAction) {
	
			mLastAction = mAction;
	
			if (mLastAction != null)
				mLastAction.stop(this, t); 
						
			mAction = newAction; 
			
			if (mAction != null)
				mAction.update(this, t, dt, false); 
		}
		
		return true;
	}
	
	function onActionStarted(action, t) {
	
		if (action == mAction) {
		
			mActionStartTime = t;
		}
	}
			
	function onActionFinished(action, t, dt) {
	
		if (action != mActionIdle && action == mAction) {
		
			//could depend on current action
			switchAction(mActionIdle, t, dt);
		}
	}
		
	/*
		we still might need to save requests to make cases work where an 
		action expires and starts with a new action other than idle and 
		uses information from the previous action
	*/
			
	function requestFacing(requestID, t, dt, bodyFacing, needsFixing) {
	
		if (mImpl.getFacing().equals(bodyFacing))
			return Request_Executed;
	
		local doRequest = false;
	
		switch (mAction.getType()) {
		
			case Footballer.Action_Idle: {
			
				if (switchAction(mActionTurning, t, dt)) {
		
					doRequest = true;
					
				} else {
				
					return Request_Blocked;
				}
			
			} break;
			
			case Footballer.Action_Turning:
			case Footballer.Action_Running:	{
			
				doRequest = true;
						
			} break;
		}
		
		if (doRequest)
			return mAction.requestBodyFacing(this, requestID, bodyFacing, needsFixing);
			
		return Request_Invalid;	
	}
	
	function requestIdle(requestID, t, dt) {
	
		requestZeroVelocity(requestID, t, dt);
	}
	
	function requestZeroVelocity(requestID, t, dt) {
	
		switch (mAction.getType()) {
		
			case Footballer.Action_Idle:
			case Footballer.Action_Turning:
			case Footballer.Action_Shooting:
			case Footballer.Action_Heading:
			case Footballer.Action_Tackling:
			case Footballer.Action_Falling:
			case Footballer.Action_Jumping:	{
			
				return Request_Executed;
					
			} break;
			
			case Footballer.Action_Running: {
			
				if (switchAction(mActionIdle, t, dt)) {
		
					return Request_Executed;
				}
				
			} break;
		}
			
		return Request_Invalid;	
	
	}
	
	//all paramaters are optional (old settings are kept if not set)
	function requestVelocity(requestID, t, dt, direction, needsFixing, speedType, strafe) {
	
		local doRequest = false;
	
		switch (mAction.getType()) {
		
			case Footballer.Action_Idle:
			case Footballer.Action_Turning: {
			
				if (switchAction(mActionRunning, t, dt)) {

					doRequest = true;
					
				} else {
				
					return Request_Blocked;
				}
			
			} break;
			
			case Footballer.Action_Running:	{
			
				doRequest = true;
							
			} break;
		}
			
		if (doRequest) {
		
			if (strafe != null) {
		
				if (strafe) {
				
					mActionRunning.requestStrafe(this, null, true);
					
				} else {
				
					mActionRunning.requestStrafe(this, null, false);
					mActionRunning.requestBodyFacing(this, null, direction, needsFixing);
				}
			}
			
			return mActionRunning.requestVelocity(this, requestID, direction, needsFixing, speedType);
		}
			
		return Request_Invalid;	
	}
		
	function requestShot(requestID, t, dt, isHeader, shotType, shot, needCopyShot, shotSpeedScale) {
	
		local doRequest = false;
	
		switch (mAction.getType()) {
		
			case Footballer.Action_Idle:
			case Footballer.Action_Turning:	{
			
				if (isHeader) {
				
					if (switchAction(mActionHeading, t, dt)) {
		
						doRequest = true;
						
					} else {
					
						return Request_Blocked;
					}
				
				} else {
			
					if (switchAction(mActionShooting, t, dt)) {
		
						doRequest = true;
						
					} else {
					
						return Request_Blocked;
					}
				}
			
			} break;
			
			case Footballer.Action_Running: {
			
				if (isHeader) {
				
					if (switchAction(mActionHeading, t, dt)) {
		
						doRequest = true;
						
					} else {
					
						return Request_Blocked;
					}
				
				} else {
				
					return mActionRunning.requestShot(this, requestID, shotType, shot, needCopyShot, shotSpeedScale);
				}
			
			} break;
			
			case Footballer.Action_Shooting: {
			
				doRequest = true;
			
			} break;
			
			case Footballer.Action_Heading: {
			
				doRequest = true;
			
			} break;
		}
		
		if (doRequest) {
		
			if (isHeader) {
				
				return mActionHeading.requestShot(this, requestID, isHeader, shotType, shot, needCopyShot, shotSpeedScale);
				
			} else {
			
				return mActionShooting.requestShot(this, requestID, isHeader, shotType, shot, needCopyShot, shotSpeedScale);
			}
		}
			
		return Request_Invalid;	
	}
	
	function setShotDir(t, dt, shotDir) {
	
		local success = false;
	
		switch (mAction.getType()) {
		
			case Footballer.Action_Shooting: {
			
				mActionShooting.setShotDir(this, shotDir);
				success = true;
			
			} break;
			
			case Footballer.Action_Heading: {
			
				mActionShooting.setShotDir(this, shotDir);
				success = true;
			
			} break;
			
			case Footballer.Action_Tackling: {
			
				mActionShooting.setShotDir(this, shotDir);
				success = true;
			
			} break;
		}
			
		return success ? Request_Executed : Request_Invalid;	
	}
		
		
	function requestActionTackle(requestID, t, dt) {
	
		local doRequest = false;
	
		switch (mAction.getType()) {
		
			case Footballer.Action_Idle:
			case Footballer.Action_Turning:
			case Footballer.Action_Running: {
			
				if (switchAction(mActionTackling, t, dt)) {

					doRequest = true;
					
				} else {
				
					return Request_Blocked;
				}
			
			} break;
					
			case Footballer.Action_Tackling: {
			
				doRequest = true;
							
			} break;
		}
			
		if (doRequest) {
		
			return Request_Executed;
		}
			
		return Request_Invalid;	
	}	
	
	function requestActionFalling(requestID, t, dt, fallDir, needsFixing) {
	
		local doRequest = false;
	
		switch (mAction.getType()) {
		
			case Footballer.Action_Idle:
			case Footballer.Action_Turning:
			case Footballer.Action_Shooting:
			case Footballer.Action_Running: {
			
				if (switchAction(mActionFalling, t, dt)) {

					doRequest = true;
					
				} else {
				
					return Request_Blocked;
				}
			
			} break;
		}
			
		if (doRequest) {
		
			mActionFalling.setStartFallDir(this, t, fallDir, needsFixing);
			return Request_Executed;
		}
			
		return Request_Invalid;	
	}	
	
	function requestActionJumping(requestID, t, dt, optionalJumpDir, optionalJumpDistance, optionalJumpType, optionalJumpHeight) {
	
		local doRequest = false;
	
		switch (mAction.getType()) {
		
			case Footballer.Action_Idle:
			case Footballer.Action_Turning:
			case Footballer.Action_Running: {
			
				if (switchAction(mActionJumping, t, dt)) {

					doRequest = true;
					
				} else {
				
					return Request_Blocked;
				}
			
			} break;
		}
			
		if (doRequest) {
		
			mActionJumping.setStartFallDir(this, t, optionalJumpDir, optionalJumpDistance, optionalJumpType, optionalJumpHeight);
			return Request_Executed;
		}
			
		return Request_Invalid;	
	}	
		
	function setRequestResult(requestID, result) {
	
		mBodyStateRequestResults[requestID] <- result;
	}
	
	function handleRequests(t, dt) {
	
		mBodyStateRequestResults = {};
	
		/*
		foreach (request in mBodydStateRequests) {
		
			request.execute(this, t, dt);
		}
		*/
	}
	
	/***************************
		Body Action
	****************************/
	
	mLastAction = null;
	mAction = null;
	mActionStartTime = 0.0;
	
	mActionIdle = null;
	mActionRunning = null;
	mActionTackling = null;
	mActionFalling = null;
	mActionJumping = null;
	mActionShooting = null;
	mActionHeading = null;
	mActionTurning = null;

	function setActiveAction(action) {
	
		mAction = action;
	}
						
	function getActionType() { return mAction.getType(); }
	function getAction() { return mAction; }
	
	/*
	function setActionIdle() { mAction = getActionIdle(); }
	function setActionRunning() { mAction = getActionRunning(); }
	function setActionTackling() { mAction = getActionTackling(); }
	function setActionJumping() { mAction = getActionJumping(); }
	function setActionFalling() { mAction = getActionFalling(); }
	
	function setAndStartAction(action, t, startState) { local lastAction = mAction; mAction = action; mAction.start(this, t, startState, lastAction); }
	*/
	
	function getActionIdle() { return mActionIdle; }
	function getActionRunning() { return mActionRunning; }
	function getActionTackling() { return mActionTackling; }
	function getActionFalling() { return mActionFalling; }
	function getActionJumping() { return mActionJumping; }
	function getActionShooting() { return mActionShooting; }
	function getActionHeading() { return mActionHeading; }
	function getActionTurning() { return mActionTurning; }
	
	
	
	
	/*
	function setActionRunning(t, bodyFacing, bodyFacingNeedsFixing, runDir, runDirNeedsFixing, isStrafing, speedType) {
	
		if (switchAction(mActionRunning, t, null)) {
		
			mActionRunning.setIsStrafing(isStrafing);
			
			if (bodyFacing == null) {
			
				if (!isStrafing)
					mActionRunning.setBodyFacing(mFootballer, runDir, runDirNeedsFixing);
			
			} else {
			
				mActionRunning.setBodyFacing(mFootballer, bodyFacing, bodyFacingNeedsFixing);
			}
			
			mActionRunning.setRunDir(mFootballer, runDir, runDirNeedsFixing);
			mActionRunning.setRunSpeed(mFootballer, speedType);
		
			return true;
		}
		
		return false;
	}
	
	function setActionShooting(t, isHeadShot, shotType, shot, needCopyShot, shotSpeedScale, ballShotDir) {
	
		if (mAction == mActionShooting) {
		
			mActionShooting.setBallShotDir(ballShotDir);
		
		} else {
	
			if (switchAction(mActionShooting, null)) {
			
				mActionShooting.setShot(isHeadShot, shotType, shot, needCopyShot, shotSpeedScale);
				mActionShooting.setBallShotDir(ballShotDir);
							
				return true;
			}
		}
		
		return false;
	}
	*/
	
	
	/***************************
		Body State
	****************************/
	
	function getMomentum() { return mMomentum; }
	function getNonZeroMomentum() { return mMomentumIsZero ? null : mMomentum; }
	function hasBodyMomentum() { return !mMomentumIsZero; }
	
	function getAlignedMomentum() { return mMomentumIsAligned ? mMomentum : null; }
	function getAlignedNonZeroMomentum() { return mMomentumIsAligned && !mMomentumIsZero ? mMomentum : null; }
	function isAlignedMomentum() { return mMomentumIsAligned; }
	
	function setMomentum(momentum, isBodyFacing) { 
	
		if (momentum != null) {
		
			mMomentum.set(momentum);
			mMomentumIsZero = mMomentum.isZero();
			mMomentumIsAligned = isBodyFacing;
			
		} else {
				
			mMomentum.zero();
			mMomentumIsZero = true;
			mMomentumIsAligned = false;
		}
	}
	
	//call when u changed mMomentum directly, if isBodyFacing is not known use null
	function onUpdatedMomentum(isBodyFacing) { 
	
		if (!mMomentum.isZero()) {
		
			mMomentumIsZero = false;
			
			if (isBodyFacing == null) {
			
				mMomentumIsAligned = mMomentum.equals(mImpl.getFacing());
			
			} else {
			
				mMomentumIsAligned = isBodyFacing;
			}
			
		} else {
		
			mMomentumIsZero = true;
			mMomentumIsAligned = false;
		}
	}

	/***************************
		Trigger Volumes
	****************************/
	
	function addTriggerVolumesFeet(leftTriggerID, rightTriggerID) { mProfile.addTriggerVolumesFeet(mImpl, leftTriggerID, rightTriggerID); }
	function addTriggerVolumeHead(triggerID) { mProfile.addTriggerVolumeHead(mImpl, triggerID); }
	function clearTriggerVolumes() { mImpl.clearTriggerVolumes(); }
	function executeTriggerVolumesOnBall() { mImpl.executeTriggerVolumesOnBall(); }
	
	/***************************
		Ball State
	****************************/
	
	function ballIsFootControllable() {
	
		if (mImpl.isBallOwner()) {
	
			local relDist = mImpl.getBallInteraction().relativeDist;
	
			return ballIsFootControlHeightEx2(relDist.getFootballerHorizDist(), relDist.getBallHeight());
		}
		
		return false;
	}
	
	function ballIsFootControllableEx(ball) {
	
		local ballPos = ball.getPos();
		local ballRadius = ball.getRadius();
		
		return (ballIsFootControlHeightEx1(mImpl.getPos(), ballPos, ballRadius)
				&& BallRelativeDist.calcIsInCOC(mImpl.getMatch(), mImpl.getWorldTransform(), mImpl.getRelativeCOCOffset(), ballPos, ballRadius));
	}
	
	function ballIsFootControllableEx1(footballerTransform, ball) {
	
		local ballPos = ball.getPos();
		local ballRadius = ball.getRadius();
		
		return (ballIsFootControlHeightEx1(footballerTransform.getPos(), ballPos, ballRadius)
				&& BallRelativeDist.calcIsInCOC(mImpl.getMatch(), footballerTransform, mImpl.getRelativeCOCOffset(), ballPos, ballRadius));
	}
	
	function ballIsHeaderHeight() {
	
		local interaction = mImpl.getBallInteraction();
		
		return mImpl.getBallInteraction().relativeDist.getBallHeight() >= mProfile.BallHeaderHeight;
	}
	
	function ballIsFootControlHeight() {
	
		local interaction = mImpl.getBallInteraction();
		
		return mImpl.getBallInteraction().relativeDist.getBallHeight() <= mProfile.BallFootControlHeight;
	}
	
	function ballIsFootControlHeightEx(footballerPos, ball) {
	
		return ballIsFootControlHeightEx1(footballerPos, ball.getPos(), ball.getRadius());
	}
	
	function ballIsFootControlHeightEx1(footballerPos, ballPos, ballRadius) {
	
		return ballIsFootControlHeightEx2(
			sqrtf(BallRelativeDist.calcFootballerBallHorizDistSq(footballerPos, ballPos, ballRadius)),
			BallRelativeDist.calcFootballerBallHeight(footballerPos, ballPos, ballRadius));
	}
	
	function ballIsFootControlHeightEx2(horizDist, height) {
	
		local controlHeight;
			
		if (mProfile.AdaptiveBallFootControlHeight) {
			
			local factor = (horizDist - mProfile.BallFootControlDistMin) / (mProfile.BallFootControlDistMax - mProfile.BallFootControlDistMin);
			controlHeight = mProfile.BallFootControlHeight * (1.0 - factor);
							
		} else {
			
			controlHeight = mProfile.BallFootControlHeight;
		}
		
		return height <= controlHeight;
	}
	
	function isBallStateImprovingIdle(ball, nextSample, dbg) {
	
		local match = mImpl.getMatch();
		local ballPos = ball.getPos();
		local nextPos = nextSample.getPos();
		local ballRadius = ball.getRadius();
		local COCOffset = mImpl.getRelativeCOCOffset();
	
		local footballerTransform = mImpl.getWorldTransform();
	
		//if (dbg) {
		//	print("ballPos : " + ball.getPos().getEl(0) + "," + ball.getPos().getEl(1) + "," + ball.getPos().getEl(2));
		//	print("nextPos : " + nextPos.getEl(0) + "," + nextPos.getEl(1) + "," + nextPos.getEl(2));
		//}
		
		local nextIsInCOC = BallRelativeDist.calcIsInCOC(match, footballerTransform, COCOffset, ballPos, ballRadius);
	
		if (!nextIsInCOC) {
		
			//if (dbg) print("not nextIsInCOC");
		
			return false;
		}

		local footballerPos = mImpl.getPos();
		
		local currIsFootControllable = ballIsFootControlHeightEx1(footballerPos, ballPos, ballRadius);
		local nextIsFootControllable = ballIsFootControlHeightEx1(footballerPos, nextPos, ballRadius);
	
		if (nextIsFootControllable != currIsFootControllable) {
		
			//if (dbg) print("footControllable: " + nextIsFootControllable);
		
			return nextIsFootControllable;
		}
	
		local sweetSpotPos = Vector3();
		
		footballerTransform.transformPoint(mProfile.BallSweetSpot, sweetSpotPos);
	
		//if (dbg) print("nextDist: " + distSq(nextPos, sweetSpotPos) + " currDist: " + distSq(ball.getPos(), sweetSpotPos));
	
		return distSq(nextPos, sweetSpotPos) < distSq(ballPos, sweetSpotPos);
	}
	
	
	
	/***************************
		Ball Commands
	****************************/
	
	function shootBall(commandID, ball, shot, _shotSpeed, shotDir, addedVel, 
										useGostedShot, ghostBall, 
										useAccuracy, controlStrength, controlSkill, allowDownDir, 
										switchControlInfluence, soundName) {
	
		//local controlStrength = getSkillBallProtection();
		//local controlSkill = useSkill ? getSkillBallControlBodyOrFeet() : 1.0;
		local shotSpeed = minf(_shotSpeed, getPhysicalPowerShotMaxSpeed());
		local isHeader = (shot.type == ::FootballerBallShot_Header);
		local speedVariance = useAccuracy ? getSkillShootAccuracySpeedVariance(isHeader, shotSpeed) : 0.0;
		local dirVariance = useAccuracy ? getSkillShootAccuracyDirVariance(isHeader, shotSpeed) : 0.0;
		
		local shotV = Vector3();
		local shotW = Vector3();
		
		local shotSoundMag = shot.getVelocity(getPhysicalPowerShotMaxSpeed(), ball, shotDir, shotSpeed, addedVel, shotV, shotW, speedVariance, dirVariance, controlSkill, allowDownDir);
			
		//printV3("shotV", shotV);
		//print("shotSoundMag: " + shotSoundMag);
			
		if (useGostedShot) {
		
			mImpl.setBallControlGhostedShot(commandID, controlStrength, shotV, shotW, switchControlInfluence, shot.enableAfterTouch);
			
		} else {
		
			mImpl.setBallControlImmediateVel(commandID, controlStrength, ghostBall, shotV, shotW, switchControlInfluence, shot.enableAfterTouch);		
		}
		
		if (soundName != null) {
		
			mImpl.playSound2(soundName, mImpl.getMatch().getBall().getPos(), shotSoundMag, randf(0.85, 1.4));
		}
	}
	
	function setBallCommandGhostedShot(commandID, ball, shot, _shotSpeed, shotDir, addedVel, switchControlInfluence, useSkill, soundName) {
	
		local controlStrength = getSkillBallProtection();
		local shotSpeed = minf(_shotSpeed, getPhysicalPowerShotMaxSpeed());
		local isHeader = (shot.type == ::FootballerBallShot_Header);
		local speedVariance = getSkillShootAccuracySpeedVariance(isHeader, shotSpeed);
		local dirVariance = getSkillShootAccuracyDirVariance(isHeader, shotSpeed);
		
		local shotV = Vector3();
		local shotW = Vector3();
		
		local shotSoundMag = shot.getVelocity(getPhysicalPowerShotMaxSpeed(), ball, shotDir, shotSpeed, addedVel, shotV, shotW, speedVariance, dirVariance, 1.0, false);
		
		mImpl.setBallControlGhostedShot(commandID, controlStrength, shotV, shotW, switchControlInfluence, shot.enableAfterTouch);
		
		if (soundName != null) {
		
			mImpl.playSound2(soundName, mImpl.getMatch().getBall().getPos(), shotSoundMag, randf(0.85, 1.4));
		}
	}
	
	function setBallCommandGuard(commandID, useFeetOnly, onlyIfNoOtherCommands) {
	
		if (onlyIfNoOtherCommands && mImpl.getFrameBallCommandCount())
			return false;
			
		local footControllable = ballIsFootControlHeight();
			
		if (useFeetOnly && !footControllable) {
		
			return false;
		}		
		
		mImpl.setBallControlImmediateGuard(commandID, getSkillBallControlBodyOrFeet());
		
		return true;
	}
	
	function playBallControlSound(t) {
	
		if (t - mLastBallControlSoundPlayTime >= randf(mProfile.BallControlSoundMinTime, mProfile.BallControlSoundMaxTime)) {
		
			mLastBallControlSoundPlayTime = t;
			mImpl.playSound(mProfile.BallControlSoundName, mImpl.getMatch().getBall().getPos(), mProfile.BallControlSoundMag);
		}
	}
	
	/***************************
		Ball Headers
	****************************/
	
	function doHeaderShot(ball, shot, shotSpeed, shotDir, addedVel) {
	
		//print(shotSpeed);
	
		shootBall(mImpl.getMatch().genBallCommandID(), ball, shot, shotSpeed, shotDir, addedVel, 
							true, true, true, getSkillBallProtection(), getSkillBallControlBodyOrFeet(), true, mProfile.BallShotSwitchInfluence, mProfile.BallShotSoundName);
	}
	

	/***************************
		Attributes
	****************************/
	
	static StaticAttributePhysicalAgilityReflexReactionMinTime = 0.5;
	static StaticAttributePhysicalPowerShotMaxSpeed = 80.0;
	static StaticAttributeSkillShootAccuracyToSpeedVariance = 15.0; //meters per sec
	static StaticAttributeSkillShootAccuracyToDirVariance = 2.0; //radians
	static StaticAttributeAIReactionMinTime = 0.0;
	static StaticAttributeAIReactionMaxTime = 0.0;
	
	AttributePhysicalPowerShotMaxSpeed = 80.0;
	AttributeSkillShootAccuracyToSpeedVariance = 10.0; 
	
	function getSkillShootAccuracyToSpeedVariance() { return AttributeSkillShootAccuracyToSpeedVariance; }
	function getSkillShootAccuracyToDirVariance() { return StaticAttributeSkillShootAccuracyToDirVariance; }
	function getPhysicalAgilityReflexReactionMinTime() { return StaticAttributePhysicalAgilityReflexReactionMinTime; }
		
	
	//AttributePhysicalAgilityReflexTime = 0.0;
	AttributePhysicalPowerShot = 100;
	AttributePhysicalPowerTackle = 100;
	AttributePhysicalAgilityBalance = 100;
	AttributeSkillBallProtection = 100;
	AttributeSkillBallTackleClean = 100;
	AttributeSkillBallControlFeet = 100;
	AttributeSkillBallControlBody = 100;
	AttributeSkillBallControlConsistancy = 100;
	AttributeSkillShootAccuracy = 100;
	
	PhysicalStaminaCurve = null;	
		
	mCurrStamina2SecsLeft = 0.0;
	mCurrStaminaBase = 1.0;
	mCurrStamina = 1.0;
	
	//those are instantiated here and not in constructor
	//so they are shared between instances
	/*
	BallControlStrengthFeetCurve = DefaultUnitResponseCurve();
	BallControlStrengthBodyCurve = UnitResponseCurve(0.5, 0.0, 1.0, 0.0, 1.0);
	BallControlSkillFeetCurve = DefaultUnitResponseCurve();
	BallControlSkillBodyCurve = UnitResponseCurve(0.5, 0.0, 1.0, 0.0, 1.0);
	BallControlReactionTimeCurve = DefaultUnitResponseCurve();
	*/
	
	function loadAttributes(chunk, conv) {
	
		loadStaminaAttributes(chunk, conv);
	
		AttributePhysicalPowerShotMaxSpeed = conv.convUnit(StaticAttributePhysicalPowerShotMaxSpeed);
		AttributeSkillShootAccuracyToSpeedVariance = conv.convUnit(StaticAttributeSkillShootAccuracyToSpeedVariance);

		local attrChunk = null;
		
		if (chunk.isValid()) 
			attrChunk = chunk.getChild("attributes");
					
		if (attrChunk.isValid()) {
		
			//AttributePhysicalAgilityReflexTime = chunk.getf("PhysicalAgilityReflexTime", AttributePhysicalAgilityReflex);
			AttributePhysicalPowerTackle = attrChunk.geti("PhysicalPowerTackle", AttributePhysicalPowerTackle);
			AttributePhysicalAgilityBalance = attrChunk.geti("PhysicalAgilityBalance", AttributePhysicalAgilityBalance);
			AttributePhysicalPowerShot = attrChunk.geti("PhysicalPowerShot", AttributePhysicalPowerShot);
			AttributeSkillBallProtection = attrChunk.geti("SkillBallProtection", AttributeSkillBallProtection);
			AttributeSkillBallTackleClean = attrChunk.geti("SkillBallTackleClean", AttributeSkillBallTackleClean);
			AttributeSkillBallControlFeet = attrChunk.geti("SkillBallControlFeet", AttributeSkillBallControlFeet);
			AttributeSkillBallControlBody = attrChunk.geti("SkillBallControlBody", AttributeSkillBallControlBody);
			AttributeSkillBallControlConsistancy = attrChunk.geti("SkillBallControlConsistancy", AttributeSkillBallControlConsistancy);
			AttributeSkillShootAccuracy = attrChunk.geti("SkillShootAccuracy", AttributeSkillShootAccuracy);
		}
		
		//AttributePhysicalAgilityReflex /= 100.0;
		AttributePhysicalPowerTackle /= 100.0;
		AttributePhysicalAgilityBalance /= 100.0;
		AttributePhysicalPowerShot /= 100.0;
		AttributeSkillBallProtection /= 100.0;
		AttributeSkillBallTackleClean /= 100.0;
		AttributeSkillBallControlFeet /= 100.0;
		AttributeSkillBallControlBody /= 100.0;
		AttributeSkillBallControlConsistancy /= 100.0;
		AttributeSkillShootAccuracy /= 100.0;
	}
	
	function loadStaminaAttributes(chunk, conv) {
	
		local profile = mProfile;
	
		switch (profile.StaminaModel) {
		
			case profile.StaminaModel_1: loadStaminaAttributes_Model1(chunk, conv); break;
			case profile.StaminaModel_2: loadStaminaAttributes_Model2(chunk, conv); break;
		}
	}
	
	function updateStamina(dt) {
	
		local profile = mProfile;
	
		switch (profile.StaminaModel) {
		
			case profile.StaminaModel_1: updateStamina_Model1(dt); break;
			case profile.StaminaModel_2: updateStamina_Model2(dt); break;
		}
		
		//print(mCurrStamina);
	}
	
	function loadStaminaAttributes_Model1(chunk, conv) {
		
		local AttributePhysicalStaminaFitness = 100;

		local attrChunk = null;
		
		if (chunk.isValid()) 
			attrChunk = chunk.getChild("attributes");		
	
		if (attrChunk.isValid()) {
		
			AttributePhysicalStaminaFitness = attrChunk.geti("PhysicalStaminaFitness", AttributePhysicalStaminaFitness);
		}
		
		mCurrStaminaBase = 1.0;
		mCurrStamina = 1.0;
		
		AttributePhysicalStaminaFitness /= 100.0;
		PhysicalStaminaCurve = ResponseCurve((1.0 - AttributePhysicalStaminaFitness), 0.0, 1.0, 0.0, 1.0, false);
	}
	
	function updateStamina_Model1(dt) {
	
		local gainedStamina = (mAction.getStaminaGain(this) * dt);
						
		mCurrStaminaBase += (gainedStamina / 100.0);
		
		if (mCurrStaminaBase < 0.0)
			mCurrStaminaBase = 0.0;
		else if (mCurrStaminaBase > 100.0)
			mCurrStaminaBase = 100.0;
		
		mCurrStamina = PhysicalStaminaCurve.map(mCurrStaminaBase);
	}
	
	function loadStaminaAttributes_Model2(chunk, conv) {
	
		local attrChunk = null;
		
		if (chunk.isValid()) 
			attrChunk = chunk.getChild("attributes");	
	
		local AttributePhysicalStaminaFitness = 100;
			
		if (attrChunk.isValid()) {
		
			AttributePhysicalStaminaFitness = attrChunk.geti("PhysicalStaminaFitness", AttributePhysicalStaminaFitness);
		}
		
		mCurrStamina2SecsLeft = AttributePhysicalStaminaFitness * 60.0; //minutes to seconds
		mCurrStaminaBase = 1.0;
		mCurrStamina = 1.0;
		
		{
			local fitness = AttributePhysicalStaminaFitness /= 100.0;
			PhysicalStaminaCurve = ResponseCurve((1.0 - fitness), 0.0, 1.0, 0.0, 1.0, false);
		}
	}
	
	function updateStamina_Model2(dt) {
	
		if (mCurrStamina2SecsLeft > 0.0) {
		
			mCurrStamina2SecsLeft -= dt;
			
		} else {
		
			local gainedStamina = (mAction.getStaminaGain(this) * dt);
			
			if (gainedStamina < 0.0) {
			
				mCurrStaminaBase += (gainedStamina / 100.0);
			
				if (mCurrStaminaBase < 0.0)
					mCurrStaminaBase = 0.0;
			
				mCurrStamina = PhysicalStaminaCurve.map(mCurrStaminaBase);
			} 
		}
	}
	
	function getSkillBallProtection() {	return mCurrStamina * AttributeSkillBallProtection; }
	function getSkillBallTackleClean() {	return mCurrStamina * AttributeSkillBallTackleClean; }
	function getSkillBallControlBodyOrFeet() {	return ballIsFootControlHeight() ? getSkillBallControlFeet() : getSkillBallControlBody(); }
	function getSkillBallControlBody() {	return mCurrStamina * AttributeSkillBallControlBody; }
	function getSkillBallControlFeet() {	return mCurrStamina * AttributeSkillBallControlFeet; }
	function getPhysicalPowerShotMaxSpeed() {	return AttributePhysicalPowerShot * AttributePhysicalPowerShotMaxSpeed; }
	function getPhysicalAgilityBalance() {	return mCurrStamina * AttributePhysicalAgilityBalance; }
	function getPhysicalPowerTackle() {	return mCurrStamina * AttributePhysicalPowerTackle; }
	function mapByShotSpeed(shotSpeed, val) { return val * (shotSpeed / getPhysicalPowerShotMaxSpeed()); }
	function getSkillShootAccuracySpeedVariance(isHeader, shotSpeed) { 

		local variance = (1.1 - mCurrStamina) * (1.0 - AttributeSkillShootAccuracy) * getSkillShootAccuracyToSpeedVariance();
	
		if (isHeader) 
			return variance;
	
		return mapByShotSpeed(shotSpeed, variance); 
	}
	
	function getSkillShootAccuracyDirVariance(isHeader, shotSpeed) { 
	
		local variance = (1.1 - mCurrStamina) * (1.0 - AttributeSkillShootAccuracy) * getSkillShootAccuracyToDirVariance();
	
		if (isHeader) 
			return variance;
	
		return mapByShotSpeed(shotSpeed, variance);  
	}
	
	function getSkillShootAccuracy(shotSpeed) { return mapByShotSpeed(shotSpeed, mCurrStamina * AttributeSkillShootAccuracy); }
	function getPhysicalAgilityReflexReactionTime() { 
	
		return getPhysicalAgilityReflexReactionMinTime(); //need to define min and max times + (1.0 - mCurrStamina) * (AttributePhysicalAgilityReflexTime);
	}

	
	/***************************
		Member Variables
	****************************/
		
	mFootballer = null;
	mImpl = null;
	mProfile = null;
		
	mIntraBodySignals = null;
	
	mDefaultRunSpeed = null;
	mEndAfterTouchTime = 0.0;
	mLastBallControlSoundPlayTime = 0.0;
	
	mMomentum = null;
	mMomentumIsZero = true;
	mMomentumIsAligned = false;
	
}