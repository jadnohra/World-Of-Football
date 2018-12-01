

class Footballer {
	
	static TypeInt_Footballer = 0;
	static Type_Footballer = "Footballer";
	
	static Action_None = -1;
	static Action_Idle = 0;
	static Action_Running = 1;
	static Action_Shooting = 2;
	static Action_Tackling = 3;
	static Action_Falling = 4;
	static Action_Jumping = 5;
	static Action_Turning = 6;
	static Action_Injured = 7;
	static Action_Transition = 8;
	static ActionTypeCount = 9;

	static Shot_None = -1;
	static Shot_Low = 0;
	static Shot_High = 1;
	static Shot_Pass = 2;
	static Shot_Extra = 3;
	static ShotTypeCount = 5;

	static BallConflictResult_None = -1;
	static BallConflictResult_Lost = 0;
	static BallConflictResult_Tie = 1;
	static BallConflictResult_Won = 2;
	
	/*	
	static TriggerVol_None = -1;
	static TriggerVol_Head = 0;
	static TriggerVol_Belly = 1;
	*/
	
	/*
	static BallControlVol_None = -1;
	static BallControlVol_LeftFoot = 0;
	static BallControlVol_RightFoot = 1;
	static BallControlVol_LeftKnee = 2;
	static BallControlVol_RightKnee = 3;
	static BallControlVol_LeftChest = 4;
	static BallControlVol_RightChest = 5;
	static BallControlVol_Head = 10;
	static BallControlVol_Belly = 11;
	*/
		
	
	
	//those are instantiated here and not in constructor
	//so they are shared between instances
	settings = FootballerSettings();
	idle = FootballerSettings_Idle();
	run = FootballerSettings_Run();
	tackle = FootballerSettings_Tackle();
	fall = FootballerSettings_Fall();
	jump = FootballerSettings_Jump();
	shoot = FootballerSettings_Shoot();

	
	function constructor() {
	
		constructStatic();
	
		mBodyMomentum = Vector3();
		mBodyMomentum.zero();
		
		mInterBodySignals = [];
	}
	
	function destructor() {
	}
	
	function constructStatic() {
	
		local rootTable = getroottable();
	
		if (!("kFootballerStaticLoaded" in rootTable)) {
		
			::kFootballerStaticLoaded <- false;
			::kFootballerStaticPreparedScene <- false;
			::kFootballerCounter <- 0;
		}
		
		::kFootballerCounter = ::kFootballerCounter + 1;
	}
	
	function loadStatic(match, conv) {
	
		if (::kFootballerStaticLoaded == false) { 
			
			::kFootballerStaticLoaded = true;
			
			loadStaticAttributes(match, conv);
			settings.load(match, conv);
			idle.load(match, conv, settings);
			run.load(match, conv, settings);
			tackle.load(match, conv, settings);
			fall.load(match, conv, settings);
			jump.load(match, conv, settings);
			shoot.load(match, conv, settings);
		}
	}
	
	function prepareSceneStatic(match) {
	
		if (::kFootballerStaticPreparedScene == false) { 
			
			::kFootballerStaticPreparedScene = true;
			
			settings.prepareScene(match);
			//idle.load(match, conv);
			//run.load(match, conv);
			//tackle.load(match, conv);
			//fall.load(match, conv);
			//jump.load(match, conv);
			//shoot.load(match, conv);
		}
	}
	
	function getTypeInt() { return TypeInt_Footballer; }
	function getType() { return Type_Footballer; }
	function getRunSpeed() { return run.SpeedTable[run.Run_Normal][run.RunSpeed_Normal]; }
	
	function hasPlayerMode() { return true; }
	function isInAIMode() { return mIsModeAI; }
	function getBrainAI() { return mBrainAI; }
	
	function isAskingForManualSwitch() {return (!mIsModeAI) && mBrainPlayer.isAskingForManualSwitch(this); }
	
	function activatePlayerMode(controller, t) {
	
		if (mIsModeAI) {
		
			if (controller == null || !controller.isValid())
				return false;
		
			mBrain.stop(this, t);
			
			if (mBrainPlayer.start(this, t, controller)) {
			
				mBrain = mBrainPlayer;
				mIsModeAI = false;			
				
			} else {
			
				assrt("Failed Footballer Brain Player Start");
			}
		} 
		
		return mIsModeAI == false;
	}
	
	function activateAIMode(t) {
	
		if (!mIsModeAI) {
		
			mBrain.stop(this, t);
			
			if (mBrainAI.start(this, t)) {
			
				mBrain = mBrainAI;
				mIsModeAI = true;			
				
			} else {
			
				assrt("Failed Footballer Brain AI Start");
			}
		} 
		
		return mIsModeAI == true;
	}
	


	function load(thisEntity, chunk, conv) {

		loadStatic(thisEntity.getMatch(), conv);	

		impl = thisEntity;	
		impl.collEnable(true);
		
		mBrainAI = FootballerBrain_AI();
		mBrainPlayer = FootballerBrain_Player();
	
		mActionIdle = BodyAction_Idle();
		mActionRunning = BodyAction_Running();
		mActionTackling = BodyAction_Tackling();
		mActionFalling = BodyAction_Falling();
		mActionJumping = BodyAction_Jumping();
		mActionShooting = BodyAction_Shooting();
		mActionTurning = BodyAction_Turning();
	
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
	
		if (!mBrainPlayer.load(this, chunk, conv)) {
			
			assrt("Failed: Footballer BrainPlayer Load");
			return false;
		}
		
		if (!mBrainAI.load(this, chunk, conv)) {
			
			assrt("Failed: Footballer BrainAI Load");
			return false;
		}
		
		loadAttributes(chunk, conv);
			
		return true;
	}
	
	function prepareScene() {
	
		prepareSceneStatic(impl.getMatch());
	
		if (!impl.createPositionProxy(0.75)) {
		
			assrt("Failed: Footballer createPositionProxy");
			return false;
		}
		
		impl.setRelativeCOCOffset(settings.COCOffset);
		

		{
			if (!settings.addTriggerVolumes(this)) {
			
				assrt("Failed: Footballer addTriggerVolumes");
				return false;
			}
		}
		
		if (!mActionIdle.start(this, 0.0, null, null)) {
		
			assrt("Failed: Footballer Body Action Idle Start");
			return false;
		}
		
		mAction = mActionIdle;
		
		if (!mBrainAI.start(this, 0.0)) {
		
			assrt("Failed: Footballer Brain AI Start");
			return false;
		}
		
		mBrain = mBrainAI;
		mIsModeAI == true;
		
		return true;
	}
	
	function frameMove(t, dt) {

		if (settings.EnableLimitAfterTouchTime) {
		
			if (impl.isBallAfterTouchOwner()) {
			
				if (mEndAfterTouchTime == 0.0) { 
				
					mEndAfterTouchTime = t + settings.AfterTouchTimeLimit;
					
				} else if (t >= mEndAfterTouchTime) {
				
					giveUpBallAfterTouch();
				}
			
			} else {
			
				mEndAfterTouchTime = 0.0;
			}
		}
			
		mBrain.frameMove(this, t, dt);
		mAction.frameMove(this, t, dt);
		
		resetInterBodySignals();
		
		updateStamina(dt);
	}
	
	function setActiveAction(action) {
	
		mAction = action;
	}
			
	function getActionType() { return mAction.getType(this); }
	function getAction() { return mAction; }
	
	function setActionIdle() { mAction = getActionIdle(); }
	function setActionRunning() { mAction = getActionRunning(); }
	function setActionTackling() { mAction = getActionTackling(); }
	function setActionJumping() { mAction = getActionJumping(); }
	function setActionFalling() { mAction = getActionFalling(); }
	
	function setAndStartAction(action, t, startState) { local lastAction = mAction; mAction = action; mAction.start(this, t, startState, lastAction); }
	
	function getActionIdle() { return mActionIdle; }
	function getActionRunning() { return mActionRunning; }
	function getActionTackling() { return mActionTackling; }
	function getActionFalling() { return mActionFalling; }
	function getActionJumping() { return mActionJumping; }
	function getActionShooting() { return mActionShooting; }
	function getActionTurning() { return mActionTurning; }
	
	function getInterBodySignals() { return mInterBodySignals; }
	function addInterBodySignal(signal) { mInterBodySignals.append(signal); }
	
	function resetInterBodySignals() { mInterBodySignals.resize(0); }
	
	function resolveCollisions(footballer, t, enableGhostMode) {
	
		impl.collResolve(enableGhostMode, false);
	}
	
	function giveUpBallAfterTouch() {  impl.giveUpBallAfterTouch(); mEndAfterTouchTime = 0.0; }
	function shouldApplyBallAfterTouch() { return impl.isBallAfterTouchOwner(); }
	
	function isSwitchLocked() { return settings.isSwitchLockingActionType(this, mAction.getType(this)); }
		
		
	impl = null;
	mIsModeAI = true;
	
	mBrain = null;
	
	mBrainAI = null;
	mBrainPlayer = null;
	
	mAction = null;
	
	mActionIdle = null;
	mActionRunning = null;
	mActionTackling = null;
	mActionFalling = null;
	mActionJumping = null;
	mActionShooting = null;
	mActionTurning = null;
	
	mInterBodySignals = null;
	
	mBodyMomentum = null;
	
	mEndAfterTouchTime = 0.0;
	mLastBallControlSoundPlayTime = -10.0;
	
	/***************************
		Body State
	****************************/
	
	function getBodyNonZeroMomentum() { return mBodyMomentum.isZero() ? null : mBodyMomentum; }
	function setBodyMomentum(momentum) { 
	
		if (momentum != null) 
			mBodyMomentum.set(momentum);
		else
			mBodyMomentum.zero();
	}
	
	/***************************
		Notifications
	****************************/
	
	function onLostBall(ball) { mAction.onLostBall(this, ball); }
	function onAcquiredBall(ball, isSolo) { mAction.onAcquiredBall(this, ball, isSolo); }
		
	function onBallSelfCollision(ball, boneID) { mAction.onBallSelfCollision(this, ball, boneID); }	
	function onBallPassiveSelfCollision(ball, boneID, isInFront) { mAction.onBallPassiveSelfCollision(this, ball, boneID, isInFront); }	
	function onBallFootballerCollision(ball, footballer, boneID) { mAction.onBallFootballerCollision(this, ball, footballer, boneID); }	
	function onBallObstacleCollision(ball, object) { mAction.onBallObstacleCollision(this, ball, object); }	
	function onBallPitchCollision(ball) { mAction.onBallPitchCollision(this, ball); }	
		
	function onBallCommandAccepted(command) { mAction.onBallCommandAccepted(this, command); }	
	function onBallCommandRejected(ball, commandID) { mAction.onBallCommandRejected(this, ball, commandID); }	
	function onBallCommand(command) { mAction.onBallCommand(this, command); }	
	function onBallCommandConflictFeedback(command, result) { mAction.onBallCommandConflictFeedback(this, command, result); }	
		
	function onBallAcquiredAfterTouch(ball) { mAction.onBallAcquiredAfterTouch(this, ball); }
	function onBallLostAfterTouch(ball) { mAction.onBallLostAfterTouch(this, ball); }	
		
	function onBallTriggerVolume(ball, volumeID) { mAction.onBallTriggerVolume(this, ball, volumeID); }
	
	function onSwitchedTeamFormation(t) { mBrain.onSwitchedTeamFormation(this, t); }
	
	/***************************
		Trigger Volumes
	****************************/
	
	function addTriggerVolumesFeet(leftTriggerID, rightTriggerID) { settings.addTriggerVolumesFeet(this, leftTriggerID, rightTriggerID); }
	function addTriggerVolumeHead(triggerID) { settings.addTriggerVolumeHead(this, triggerID); }
	function clearTriggerVolumes() { impl.clearTriggerVolumes(); }
	function executeTriggerVolumesOnBall() { impl.executeTriggerVolumesOnBall(); }
	
	/***************************
		Ball State
	****************************/
	
	function ballIsFootControllable() {
	
		if (impl.isBallOwner()) {
	
			local relDist = impl.getBallInteraction().relativeDist;
	
			return ballIsFootControlHeightEx2(relDist.getFootballerHorizDist(), relDist.getBallHeight());
		}
		
		return false;
	}
	
	function ballIsFootControllableEx(ball) {
	
		local ballPos = ball.getPos();
		local ballRadius = ball.getRadius();
		
		return (ballIsFootControlHeightEx1(impl.getPos(), ballPos, ballRadius)
				&& BallRelativeDist.calcIsInCOC(impl.getMatch(), impl.getWorldTransform(), impl.getRelativeCOCOffset(), ballPos, ballRadius));
	}
	
	function ballIsFootControllableEx1(footballerTransform, ball) {
	
		local ballPos = ball.getPos();
		local ballRadius = ball.getRadius();
		
		return (ballIsFootControlHeightEx1(footballerTransform.getPos(), ballPos, ballRadius)
				&& BallRelativeDist.calcIsInCOC(impl.getMatch(), footballerTransform, impl.getRelativeCOCOffset(), ballPos, ballRadius));
	}
	
	function ballIsHeaderHeight() {
	
		local interaction = impl.getBallInteraction();
		
		return impl.getBallInteraction().relativeDist.getBallHeight() >= settings.BallHeaderHeight;
	}
	
	function ballIsFootControlHeight() {
	
		local interaction = impl.getBallInteraction();
		
		return impl.getBallInteraction().relativeDist.getBallHeight() <= settings.BallFootControlHeight;
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
			
		if (settings.AdaptiveBallFootControlHeight) {
			
			local factor = (horizDist - settings.BallFootControlDistMin) / (settings.BallFootControlDistMax - settings.BallFootControlDistMin);
			controlHeight = settings.BallFootControlHeight * (1.0 - factor);
							
		} else {
			
			controlHeight = settings.BallFootControlHeight;
		}
		
		return height <= controlHeight;
	}
	
	function isBallStateImprovingIdle(footballer, ball, nextSample, dbg) {
	
		local impl = footballer.impl;
		local match = impl.getMatch();
		local ballPos = ball.getPos();
		local nextPos = nextSample.getPos();
		local ballRadius = ball.getRadius();
		local COCOffset = impl.getRelativeCOCOffset();
	
		local footballerTransform = impl.getWorldTransform();
	
		//if (dbg) {
		//	print("ballPos : " + ball.getPos().getEl(0) + "," + ball.getPos().getEl(1) + "," + ball.getPos().getEl(2));
		//	print("nextPos : " + nextPos.getEl(0) + "," + nextPos.getEl(1) + "," + nextPos.getEl(2));
		//}
		
		local nextIsInCOC = BallRelativeDist.calcIsInCOC(match, footballerTransform, COCOffset, ballPos, ballRadius);
	
		if (!nextIsInCOC) {
		
			//if (dbg) print("not nextIsInCOC");
		
			return false;
		}

		local footballerPos = impl.getPos();
		
		local currIsFootControllable = ballIsFootControlHeightEx1(footballerPos, ballPos, ballRadius);
		local nextIsFootControllable = ballIsFootControlHeightEx1(footballerPos, nextPos, ballRadius);
	
		if (nextIsFootControllable != currIsFootControllable) {
		
			//if (dbg) print("footControllable: " + nextIsFootControllable);
		
			return nextIsFootControllable;
		}
	
		local sweetSpotPos = Vector3();
		
		footballerTransform.transformPoint(settings.BallSweetSpot, sweetSpotPos);
	
		//if (dbg) print("nextDist: " + distSq(nextPos, sweetSpotPos) + " currDist: " + distSq(ball.getPos(), sweetSpotPos));
	
		return distSq(nextPos, sweetSpotPos) < distSq(ballPos, sweetSpotPos);
	}
	
	/*
	function isBallStateImproving(footballer, footballerCurrTransform, footballerNextTransform, ball, ballNextSample, dbg) {
										
		return isBallStateImprovingEx(footballer, footballerCurrTransform, footballerNextTransform, true, 
										ball, ballNextSample, dbg)
	}
	
	function isBallStateImprovingEx(footballer, footballerCurrTransform, footballerNextTransform, currBallIsInCOC, 
										ball, ballNextSample, dbg) {
	
		//suffers from accuracy ballSimul problems, maybe some tolerance can be used
		//to put in COC when out and put out of COC when in ...
	
		assrt(false); 
	
		local impl = footballer.impl;
		local match = impl.getMatch();
		local ballPos = ball.getPos();
		local ballRadius = ball.getRadius();
		local COCOffset = impl.getRelativeCOCOffset();
		local ballNextPos = ballNextSample.getPos();
	
		local footballerTransform = impl.getWorldTransform();
	
		//if (dbg) {
		//	printV3("ballPos", ballPos);
		//	printV3("ballNextPos", ballNextPos);
		//	
		//	printV3("footballerCurrTransform", footballerCurrTransform.getPos());
		//	printV3("footballerNextTransform", footballerNextTransform.getPos());
		//}
		
		local currIsInCOC = currBallIsInCOC != null ? currBallIsInCOC : BallRelativeDist.calcIsInCOC(match, footballerCurrTransform, COCOffset, ballPos, ballRadius);
		local nextIsInCOC = BallRelativeDist.calcIsInCOC(match, footballerNextTransform, COCOffset, ballNextPos, ballRadius);
	
		if (currIsInCOC != nextIsInCOC) {
		
			//if (dbg) { assrt("nextIsInCOC: " + nextIsInCOC); BallRelativeDist.calcIsInCOC(match, footballerNextTransform, COCOffset, ballNextPos, ballRadius); }
		
			return nextIsInCOC;
		}

		local currIsFootControllable = ballIsFootControlHeightEx1(footballerCurrTransform.getPos(), ballPos, ballRadius);
		local nextIsFootControllable = ballIsFootControlHeightEx1(footballerNextTransform.getPos(), ballNextPos, ballRadius);
	
		if (nextIsFootControllable != currIsFootControllable) {
		
			//if (dbg) print("footControllable: " + nextIsFootControllable);
		
			return nextIsFootControllable;
		}
	
		local sweetSpotPos = Vector3();
				
		footballerCurrTransform.transformPoint(settings.BallSweetSpot, sweetSpotPos);
		local currDistSq = distSq(ballPos, sweetSpotPos);
		
		footballerNextTransform.transformPoint(settings.BallSweetSpot, sweetSpotPos);
		local nextDistSq = distSq(ballNextPos, sweetSpotPos);
	
		//if (dbg) print("nextDist: " + nextDistSq + " currDist: " + currDistSq);
	
		return nextDistSq < currDistSq;
	}
	*/
	
	/***************************
		Ball Commands
	****************************/
	
	/*
	function setBallCommandStabilize(commandID, ball, ghostBall, useSkill, soundName) {
	
		local controlStrength = getSkillBallControlProtection();
		local controlSkill = useSkill ? getSkillBallControlBodyOrFeet() : 1.0;
		
		local ballV = Vector3();
		local ballW = Vector3();
		local syncW = Vector3();
		
		lerpv3(ball.getVel(), ::kZeroVector3, controlSkill, ballV);
		
		local ballW = ball.getW();
		local rotAxis = Vector3();
		
		ballW.normalizeTo(rotAxis);
		
		ball.calcVelocitySyncedW(ballV, rotAxis, syncW);
		syncW.mulToSelf(0.2);
		lerpv3(ball.getW(), syncW, controlSkill, ballW);
		
		impl.setBallControlImmediateVel(commandID, controlStrength, ghostBall, ballV, ballW, 0.0, false);
		
		if (soundName != null) {
		
			local temp = rotAxis;
			ballV.subtract(ball.getVel(), temp);
		
			impl.playSound(soundName, impl.getMatch().getBall().getPos(), temp.mag());
		}
	}
	*/
	
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
		
		local shotSoundMag = shot.getVelocity(this, ball, shotDir, shotSpeed, addedVel, shotV, shotW, speedVariance, dirVariance, controlSkill, allowDownDir);
			
		//printV3("shotV", shotV);
		//print("shotSoundMag: " + shotSoundMag);
			
		if (useGostedShot) {
		
			impl.setBallControlGhostedShot(commandID, controlStrength, shotV, shotW, switchControlInfluence, shot.enableAfterTouch);
			
		} else {
		
			impl.setBallControlImmediateVel(commandID, controlStrength, ghostBall, shotV, shotW, switchControlInfluence, shot.enableAfterTouch);		
		}
		
		if (soundName != null) {
		
			impl.playSound2(soundName, impl.getMatch().getBall().getPos(), shotSoundMag, randf(0.85, 1.4));
		}
	}
	
	/*
	function setBallCommandStabilize(commandID, ball, _shotSpeed, shotDir, ghostBall, useSkill, useAccuracy, soundName) {
	
		local shot = settings.ZeroShot;
		local controlStrength = getSkillBallProtection();
		local controlSkill = useSkill ? getSkillBallControlBodyOrFeet() : 1.0;
		local shotSpeed = minf(_shotSpeed, getPhysicalPowerShotMaxSpeed());
		local speedVariance = useAccuracy ? getSkillShootAccuracySpeedVariance(shotSpeed) : 0.0;
		local dirVariance = useAccuracy ? getSkillShootAccuracyDirVariance(shotSpeed) : 0.0;
		
		local shotV = Vector3();
		local shotW = Vector3();
		
		local shotSoundMag = shot.getVelocity(this, ball, shotDir, shotSpeed, null, shotV, shotW, speedVariance, dirVariance, 1.0, true);
		
		local finalShotW = Vector3();
		local residualShotW = Vector3();
		ball.getW().mul(0.2, residualShotW);
		
		lerpv3(residualShotW, shotW, controlSkill, finalShotW);
		
		impl.setBallControlImmediateVel(commandID, controlStrength, ghostBall, shotV, finalShotW, 0.0, shot.enableAfterTouch);
		
		if (soundName != null) {
		
			impl.playSound(soundName, impl.getMatch().getBall().getPos(), shotSoundMag);
		}
	}
	*/
	
	function setBallCommandGhostedShot(commandID, ball, shot, _shotSpeed, shotDir, addedVel, switchControlInfluence, useSkill, soundName) {
	
		local controlStrength = getSkillBallProtection();
		local shotSpeed = minf(_shotSpeed, getPhysicalPowerShotMaxSpeed());
		local isHeader = (shot.type == ::FootballerBallShot_Header);
		local speedVariance = getSkillShootAccuracySpeedVariance(isHeader, shotSpeed);
		local dirVariance = getSkillShootAccuracyDirVariance(isHeader, shotSpeed);
		
		local shotV = Vector3();
		local shotW = Vector3();
		
		local shotSoundMag = shot.getVelocity(this, ball, shotDir, shotSpeed, addedVel, shotV, shotW, speedVariance, dirVariance, 1.0, false);
		
		impl.setBallControlGhostedShot(commandID, controlStrength, shotV, shotW, switchControlInfluence, shot.enableAfterTouch);
		
		if (soundName != null) {
		
			impl.playSound2(soundName, impl.getMatch().getBall().getPos(), shotSoundMag, randf(0.85, 1.4));
		}
	}
	
	function setBallCommandGuard(commandID, useFeetOnly, onlyIfNoOtherCommands) {
	
		if (onlyIfNoOtherCommands && impl.getFrameBallCommandCount())
			return false;
			
		local footControllable = ballIsFootControlHeight();
			
		if (useFeetOnly && !footControllable) {
		
			return false;
		}		
		
		impl.setBallControlImmediateGuard(commandID, getSkillBallControlBodyOrFeet());
		
		return true;
	}
	
	function playBallControlSound(t) {
	
		if (t - mLastBallControlSoundPlayTime >= randf(settings.BallControlSoundMinTime, settings.BallControlSoundMaxTime)) {
		
			mLastBallControlSoundPlayTime = t;
			impl.playSound(settings.BallControlSoundName, impl.getMatch().getBall().getPos(), settings.BallControlSoundMag);
		}
	}
	
	/***************************
		Headers
	****************************/
	
	function doHeaderShot(ball, shot, shotSpeed, shotDir, addedVel) {
	
		//print(shotSpeed);
	
		shootBall(impl.getMatch().genBallCommandID(), ball, shot, shotSpeed, shotDir, addedVel, 
							true, true, true, getSkillBallProtection(), getSkillBallControlBodyOrFeet(), true, settings.BallShotSwitchInfluence, settings.BallShotSoundName);
	}
	
	
	/***************************
		Attributes
	****************************/
	
	static StaticAttributePhysicalAgilityReflexReactionMinTime = 0.5;
	static StaticAttributePhysicalPowerShotMaxSpeed = 80.0;
	static StaticAttributeSkillShootAccuracyToSpeedVariance = 15.0; //meters per sec
	static StaticAttributeSkillShootAccuracyToDirVariance = 2.0; //radians
	
	AttributePhysicalPowerShotMaxSpeed = 80.0;
	AttributeSkillShootAccuracyToSpeedVariance = 10.0; 
	
	function getSkillShootAccuracyToSpeedVariance() { return AttributeSkillShootAccuracyToSpeedVariance; }
	function getSkillShootAccuracyToDirVariance() { return StaticAttributeSkillShootAccuracyToDirVariance; }
	function getPhysicalAgilityReflexReactionMinTime() { return StaticAttributePhysicalAgilityReflexReactionMinTime; }
	
	AttributePhysicalAgilityReflex = 100;
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
		
	currStamina2SecsLeft = 0.0;
	currStaminaBase = 1.0;
	currStamina = 1.0;
	
	//those are instantiated here and not in constructor
	//so they are shared between instances
	/*
	BallControlStrengthFeetCurve = DefaultUnitResponseCurve();
	BallControlStrengthBodyCurve = UnitResponseCurve(0.5, 0.0, 1.0, 0.0, 1.0);
	BallControlSkillFeetCurve = DefaultUnitResponseCurve();
	BallControlSkillBodyCurve = UnitResponseCurve(0.5, 0.0, 1.0, 0.0, 1.0);
	BallControlReactionTimeCurve = DefaultUnitResponseCurve();
	*/
	
	function loadStaticAttributes(match, conv) {
	}
	
	function loadAttributes(chunk, conv) {
	
		loadStaminaAttributes(chunk, conv);
	
		AttributePhysicalPowerShotMaxSpeed = conv.convUnit(StaticAttributePhysicalPowerShotMaxSpeed);
		AttributeSkillShootAccuracyToSpeedVariance = conv.convUnit(StaticAttributeSkillShootAccuracyToSpeedVariance);
			
		if (chunk.isValid() && chunk.toChild("attributes")) {
		
			AttributePhysicalAgilityReflex = chunk.geti("PhysicalAgilityReflex", AttributePhysicalAgilityReflex);
			AttributePhysicalPowerTackle = chunk.geti("PhysicalPowerTackle", AttributePhysicalPowerTackle);
			AttributePhysicalAgilityBalance = chunk.geti("PhysicalAgilityBalance", AttributePhysicalAgilityBalance);
			AttributePhysicalPowerShot = chunk.geti("PhysicalPowerShot", AttributePhysicalPowerShot);
			AttributeSkillBallProtection = chunk.geti("SkillBallProtection", AttributeSkillBallProtection);
			AttributeSkillBallTackleClean = chunk.geti("SkillBallTackleClean", AttributeSkillBallTackleClean);
			AttributeSkillBallControlFeet = chunk.geti("SkillBallControlFeet", AttributeSkillBallControlFeet);
			AttributeSkillBallControlBody = chunk.geti("SkillBallControlBody", AttributeSkillBallControlBody);
			AttributeSkillBallControlConsistancy = chunk.geti("SkillBallControlConsistancy", AttributeSkillBallControlConsistancy);
			AttributeSkillShootAccuracy = chunk.geti("SkillShootAccuracy", AttributeSkillShootAccuracy);
		}
		
		AttributePhysicalAgilityReflex /= 100.0;
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
	
		switch (settings.StaminaModel) {
		
			case settings.StaminaModel_1: loadStaminaAttributes_Model1(chunk, conv); break;
			case settings.StaminaModel_2: loadStaminaAttributes_Model2(chunk, conv); break;
		}
	}
	
	function updateStamina(dt) {
	
		switch (settings.StaminaModel) {
		
			case settings.StaminaModel_1: updateStamina_Model1(dt); break;
			case settings.StaminaModel_2: updateStamina_Model2(dt); break;
		}
		
		//print(currStamina);
	}
	
	function loadStaminaAttributes_Model1(chunk, conv) {
	
		local AttributePhysicalStaminaFitness = 100;
			
		if (chunk.isValid() && chunk.toChild("attributes")) {
		
			AttributePhysicalStaminaFitness = chunk.geti("PhysicalStaminaFitness", AttributePhysicalStaminaFitness);
		}
		
		currStaminaBase = 1.0;
		currStamina = 1.0;
		
		AttributePhysicalStaminaFitness /= 100.0;
		PhysicalStaminaCurve = ResponseCurve((1.0 - AttributePhysicalStaminaFitness), 0.0, 1.0, 0.0, 1.0, false);
	}
	
	function updateStamina_Model1(dt) {
	
		local gainedStamina = (mAction.getStaminaGain(this) * dt);
						
		currStaminaBase += (gainedStamina / 100.0);
		
		if (currStaminaBase < 0.0)
			currStaminaBase = 0.0;
		else if (currStaminaBase > 100.0)
			currStaminaBase = 100.0;
		
		currStamina = PhysicalStaminaCurve.map(currStaminaBase);
	}
	
	function loadStaminaAttributes_Model2(chunk, conv) {
	
		local AttributePhysicalStaminaFitness = 100;
			
		if (chunk.isValid() && chunk.toChild("attributes")) {
		
			AttributePhysicalStaminaFitness = chunk.geti("PhysicalStaminaFitness", AttributePhysicalStaminaFitness);
		}
		
		currStamina2SecsLeft = AttributePhysicalStaminaFitness * 60.0; //minutes to seconds
		currStaminaBase = 1.0;
		currStamina = 1.0;
		
		{
			local fitness = AttributePhysicalStaminaFitness /= 100.0;
			PhysicalStaminaCurve = ResponseCurve((1.0 - fitness), 0.0, 1.0, 0.0, 1.0, false);
		}
	}
	
	function updateStamina_Model2(dt) {
	
		if (currStamina2SecsLeft > 0.0) {
		
			currStamina2SecsLeft -= dt;
			
		} else {
		
			local gainedStamina = (mAction.getStaminaGain(this) * dt);
			
			if (gainedStamina < 0.0) {
			
				currStaminaBase += (gainedStamina / 100.0);
			
				if (currStaminaBase < 0.0)
					currStaminaBase = 0.0;
			
				currStamina = PhysicalStaminaCurve.map(currStaminaBase);
			} 
		}
	}
	
	function getSkillBallProtection() {	return currStamina * AttributeSkillBallProtection; }
	function getSkillBallTackleClean() {	return currStamina * AttributeSkillBallTackleClean; }
	function getSkillBallControlBodyOrFeet() {	return ballIsFootControlHeight() ? getSkillBallControlFeet() : getSkillBallControlBody(); }
	function getSkillBallControlBody() {	return currStamina * AttributeSkillBallControlBody; }
	function getSkillBallControlFeet() {	return currStamina * AttributeSkillBallControlFeet; }
	function getPhysicalPowerShotMaxSpeed() {	return AttributePhysicalPowerShot * AttributePhysicalPowerShotMaxSpeed; }
	function getPhysicalAgilityBalance() {	return currStamina * AttributePhysicalAgilityBalance; }
	function getPhysicalPowerTackle() {	return currStamina * AttributePhysicalPowerTackle; }
	function mapByShotSpeed(shotSpeed, val) { return val * (shotSpeed / getPhysicalPowerShotMaxSpeed()); }
	function getSkillShootAccuracySpeedVariance(isHeader, shotSpeed) { 

		local variance = (1.1 - currStamina) * (1.0 - AttributeSkillShootAccuracy) * getSkillShootAccuracyToSpeedVariance();
	
		if (isHeader) 
			return variance;
	
		return mapByShotSpeed(shotSpeed, variance); 
	}
	
	function getSkillShootAccuracyDirVariance(isHeader, shotSpeed) { 
	
		local variance = (1.1 - currStamina) * (1.0 - AttributeSkillShootAccuracy) * getSkillShootAccuracyToDirVariance();
	
		if (isHeader) 
			return variance;
	
		return mapByShotSpeed(shotSpeed, variance);  
	}
	
	function getSkillShootAccuracy(shotSpeed) { return mapByShotSpeed(shotSpeed, currStamina * AttributeSkillShootAccuracy); }
	function getPhysicalAgilityReflexReactionTime() { 
	
		local time = getPhysicalAgilityReflexReactionMinTime();
		
		return time + time * (1.0 - currStamina);
	}
	
	/*
	function getBallControlStrengthFeet() {	return BallControlStrengthFeetCurve.fixAndMap((currStamina) * (AttributeBallControlStrengthFeet)); }
	function getBallControlStrengthBody() {	return BallControlStrengthBodyCurve.fixAndMap((currStamina) * (AttributeBallControlStrengthBody)); }
	function getBallControlSkillTolerance() { return randf(0.0, 1.0 - AttributeBallControlSkillStability); }
	function getBallControlSkillFeet() {	return BallControlSkillFeetCurve.fixAndMap((currStamina) * (AttributeBallControlSkillFeet + getBallControlSkillTolerance())); }
	function getBallControlSkillBody() {	return BallControlSkillBodyCurve.fixAndMap((currStamina) * (AttributeBallControlSkillBody + getBallControlSkillTolerance())); }
	function getBallControlReactionTime() {	return BallControlReactionTimeCurve.fixAndMap(lerpf(AttributeBallControlReactionTimeMax, AttributeBallControlReactionTimeMin, currStamina)); }
	function getBallFullControlMaxSpeed() { return AttributeBallFullControlMaxSpeed;  }
	function getBallShotMaxSpeed() { return AttributeBallShotMaxSpeed; }
	
	function getBallShotSpeedVariance(skill, shotSpeed)	{
	
		return 0.0;
	}
	
	function getBallShotDirVariance(skill, shotSpeed)	{
	
		if (skill >= 1.0)
			return 0.0;
	
		local maxVariance = AttributeBallShotSkillToVarianceCurve.fixAndMap(1.0 - skill);
		
		print("maxVariance: " + maxVariance);
		
		return lerpf(0.0, maxVariance, (shotSpeed - AttributeBallShotEasySpeed) / (AttributeBallShotMaxSpeed - AttributeBallShotEasySpeed));
	}
	*/
}