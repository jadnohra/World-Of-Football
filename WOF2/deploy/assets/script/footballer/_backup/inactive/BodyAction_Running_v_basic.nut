
class BodyActionRunning_StartState {
	
	bodyFacing = null; //optional
	runDir = null; //optional
	runSpeedType = null; 
}

class BodyAction_Running extends BodyActionBase_BallTriggers {

	function getType(footballer) {
	
		return footballer.Action_Running;
	}

	constructor() {
	
		mRequestBodyFacing = FootballerVarRequest();
		mRequestBodyFacing.value = Vector3();
		
		mRequestRunDir = FootballerVarRequest();
		mRequestRunDir.value = Vector3();
		
		mRequestActiveBallShot = FootballerVarRequest();
		mRequestActiveBallShot.value = null;
		
		mRequestActiveBallShotVelScale = 1.0;
		
		mRequestRunSpeedType = FootballerVarRequest();
		
		mRequestIsStrafing = FootballerVarRequest();
		
		mRunDir = Vector3();
		mLastShotDir = Vector3(); 
		mActiveBallShot = FootballerBallShot(false, ::FootballerBallShot_None, 0.0, 0.0, 0.0, 0.0, false);
		
		ballControlOnConstructor();
	}

	function load(footballer, chunk, conv) {

		if (footballer.run.EnableEasyLockDribble) {
	
			mDelayedRequestBodyFacing = FootballerVarRequest();
			mDelayedRequestBodyFacing.value = Vector3();
			
			mDelayedRequestRunDir = FootballerVarRequest();
			mDelayedRequestRunDir.value = Vector3();
		}
	
		return true;
	}
	
	function isFinished() { return true; }	
	function getStaminaGain(footballer) { return footballer.run.getStaminaGain(mRunType, mRunSpeedType); }
	
	function getRunDir() { return mRunDir; }
		
	function switchRunning(footballer, t, startState) {
	
		return true;
	}
	
	function cancelSetBodyFacing() {

		mRequestBodyFacing.isValid = false;
	}

	function setBodyFacing(footballer, val) {

		local request = null;
	
		if (footballer.impl.getFacing().equals(val))
			return;
		
		mHasRequests = true;	
		
		request = mRequestBodyFacing;
			
		request.value.set(val);
		request.isValid = true;
	}
	
	function cancelSetRunDir() {

		mRequestRunDir.isValid = false;
	}

	function setRunDir(footballer, val) {

		local request = null;
	
		if (mRunDir.equals(val))
			return;

		mHasRequests = true;	
		
		request = mRequestRunDir;
			
		request.value.set(val);
		request.isValid = true;
	}
	
	function setActiveBallShot(shot, shotVelScale, copyShot) {
	
		if (shot == null && mActiveBallShot == null)
			return;
	
		if (copyShot) {
		
			mRequestActiveBallShot.value = FootballerBallShot(false, ::FootballerBallShot_None, 0.0, 0.0, 0.0, 0.0, false);
			mRequestActiveBallShot.value.set(shot);
			
		} else {
		
			mRequestActiveBallShot.value = shot;
		}
		
		mRequestActiveBallShotVelScale = shotVelScale;
		mRequestActiveBallShot.isValid = true;
	}
	
	function cancelSetRunSpeed() {

		mRequestRunSpeedType.isValid = false;
	}

	function setRunSpeed(val) {

		if (mRunSpeedType == val)
			return;
	
		mHasRequests = true;
		
		mRequestRunSpeedType.value = val;
		mRequestRunSpeedType.isValid = true;
	}
	
	function cancelSetIsStrafing() {

		mRequestIsStrafing.isValid = false;
	}

	function setIsStrafing(val) {

		if (mIsStrafing == val)
			return;
	
		mHasRequests = true;
		
		mRequestIsStrafing.value = val;
		mRequestIsStrafing.isValid = true;
	}
	
	function getRunSpeedType() { return mRunSpeedType; } 
	function getRunType() { return mRunType; } 
	
	function validateRunState(footballer) { 
	
		if (mRunType == footballer.run.Run_None) 
			updateRunState(footballer); 
	}
	
	function getCurrSpeed(footballer) {
	
		validateRunState(footballer);

		return footballer.run.getSpeedValue(getRunType(), getRunSpeedType());
	}
	
	function getCurrVelocity(footballer, result) {
	
		mRunDir.mul(getCurrSpeed(footballer), result);
	}
	
	function detectRunType(footballer, facing, dir) {
	
		if (dir == null)
			return footballer.run.Run_Normal;
	
		local type;
	
		local tolerance = 0.0175; //sin(1 degrees)
		local dot = facing.dot(dir);

		if (fabs(1.0 - dot) <= tolerance) {

			type = mIsStrafing ? footballer.run.Run_StrafeFwd : footballer.run.Run_Normal;

		} else {

			local discreeteDir = Vector3();

			local step = discretizeDirection(facing, footballer.impl.getMatch().SceneUpAxis(), dir, 4, discreeteDir);

			switch(step) {

				case 0: type = footballer.run.Run_StrafeFwd; break;
				case 1: type = footballer.run.Run_StrafeRight; break;
				case 2: type = footballer.run.Run_StrafeBack; break;
				case 3: type = footballer.run.Run_StrafeLeft; break;
				default: {
				
					/*
					assrt("Bug: BodyAction_Running discretizeDirection facing(" + facing.get(0) + "," + facing.get(1) + ","+ facing.get(2) + ") type(" + type.get(0) + "," + type.get(1) + ","+ type.get(2) + ") steps(" + step + ")"); 
					local stepDebug = discretizeDirection(facing, impl.getMatch().SceneUpAxis(), dir, 4, discreeteDir);
					*/
					//assrt("Bug: BodyAction_Running discretizeDirection");
					type = footballer.run.Run_StrafeFwd; 
					
				} break;
			}
		}
		
		return type;
	}
	
	function updateRunState(footballer) {

		local runType = detectRunType(footballer, footballer.impl.getFacing(), mRunDir);

		if (runType != mRunType) {

			mRunType = runType;

			local impl = footballer.impl;
			local animIndex = impl.getAnimIndex(footballer.run.AnimNames[mRunType]);

			if (animIndex != impl.getCurrAnimIndex()) {

				impl.setAnimByIndex(animIndex, false);
				impl.setAnimSpeed(footballer.run.speedToAnimSpeed(footballer, footballer.run.getSpeedValue( mRunType, mRunSpeedType)));
				impl.setAnimLoop(true);
			}
			
			/*
			footballer.ballHandler.reset(footballer, false);
			*/
		}
	}
	
	function applyStartState(footballer, startState) {
	
		local runSpeedWasSet = false;
	
		if (startState != null) {
	
			if (startState.bodyFacing != null)
				setBodyFacing(footballer, startState.bodyFacing);
				
			if (startState.runDir != null)
				setRunDir(footballer, startState.runDir);	
				
			if (startState.runSpeedType != null) {
			
				runSpeedWasSet = true;
				setRunSpeed(startState.runSpeedType);
			} 
		} 
	}
	
	function start(footballer, t, startState, prevAction) {
		
		mIsEasyLockDribbling = false;
		
		mRunType = footballer.run.Run_None;
		mRunDir.zero();
	
		applyStartState(footballer, startState);
		
		/*
			mStrafeDribbleCorrection has also to be deactivated on ball events and on changeDirs (this means passing ball events to action as well??? )
			and impl the correct way of correcting shotVelocity by simulating
			dribble shot at video start (also dribble while running fast)
			also add the EasyDribble option which blocks dir changes
			until new ball contact, maybe add some fine tuning constraints
		*/
		mStrafeDribbleCorrection = 0.0;
	
		ballControlOnStart(footballer, t, startState);
		
		return true;
	}
	
	function stop(footballer, t) {
	
		mRunType = footballer.run.Run_None;
		
		ballControlOnStop(footballer, t);
	}
	
	function checkColliders(footballer, t) {
	
		/*
		
		jump over??
		
		local impl = footballer.impl;
	
		local index = Int(0);
		local collider;
		local type = footballer.getTypeInt();
		
		while ((collider = impl.collFindDynVolColliderInt(index, type)) != null) {

			if (collider.getActionType() == footballer.Action_Tackling) {
				
				switchTackled(footballer, t, collider);
			}
		}
		*/
	}
	
	function handleRequests(footballer, t) {

		if (mRequestActiveBallShot.isValid) {
		
			mActiveBallShot.set(mRequestActiveBallShot.value);
			mActiveBallShotVelScale = mRequestActiveBallShotVelScale;
			mRequestActiveBallShot.isValid = false;
		}
	
		if (mHasRequests) {

			mHasRequests = false;

			local needUpdateRunState = false;
			local needUpdateAnimSpeed = false;

			if (mRequestBodyFacing.isValid) {

				footballer.impl.setFacing(mRequestBodyFacing.value);
				mRequestBodyFacing.isValid = false;

				needUpdateRunState = true;
			}

			if (mRequestRunDir.isValid) {

				mRunDir.set(mRequestRunDir.value);
				mRequestRunDir.isValid = false;
				
				//if (mRunDirChangeUnderSampleLeft == 0)
					mRunDirChangeUnderSampleLeft = footballer.run.RunDirUnderSampleCount;

				needUpdateRunState = true;
			}

			if (mRequestRunSpeedType.isValid) {

				mRunSpeedType = mRequestRunSpeedType.value;
				mRequestRunSpeedType.isValid = false;

				needUpdateAnimSpeed = true;
			}
			
			if (mRequestIsStrafing.isValid) {

				mIsStrafing = mRequestIsStrafing.value;
				mRequestIsStrafing.isValid = false;

				if (mIsStrafing)
					mIsEasyLockDribbling = false;
				
				needUpdateRunState = true;
			}

			onHandledRequests(footballer, needUpdateRunState, needUpdateAnimSpeed);
		} 
		
		return true;
	}
	
	function onHandledRequests(footballer, needUpdateRunState, needUpdateAnimSpeed) {
	
		if (needUpdateRunState || mRunType == footballer.run.Run_None) {

			needUpdateAnimSpeed = true;

			updateRunState(footballer);
		}

		if (needUpdateAnimSpeed) {

			footballer.impl.setAnimSpeed(footballer.run.speedToAnimSpeed(footballer, footballer.run.getSpeedValue(mRunType, mRunSpeedType)));
		}
	}
	
	function frameMove(footballer, t, dt) {

		local savedSpatialState = ballControlCreateSavedSpatialState(footballer);
	
		local runDirChanged = mRequestRunDir.isValid;
		
		if (footballer.run.EnableRunDirUnderSampling) {
		
			if (mRunDirChangeUnderSampleLeft > 0) {
		
				runDirChanged = true;
				mRunDirChangeUnderSampleLeft -= 1;
			}
		}
		
		handleRequests(footballer, t);
	
		local impl = footballer.impl;		
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		local posDiff = Vector3();
		
		{	
			mRunDir.mul(dt * footballer.run.getSpeedValue(mRunType, mRunSpeedType), posDiff);
			impl.shiftPos(posDiff);

						
			if (footballer.run.EnableStrafeCorrectDribble && mStrafeDribbleCorrection != 0.0) {
			
				if (runDirChanged) {
				
					mStrafeDribbleCorrection = 0.0;
					
				} else {
			
					local correctionDist = dt * footballer.run.StrafeCorrectDribbleSpeed;
					local correctionOffset = Vector3();
				
					if (fabs(correctionDist) > fabs(mStrafeDribbleCorrection)) {
					
						correctionDist = mStrafeDribbleCorrection;
						
					} else {
					
						if (mStrafeDribbleCorrection < 0.0)
							correctionDist = -correctionDist;
					}
				
					impl.getRight().mul(correctionDist, correctionOffset);
					
					impl.shiftPos(correctionOffset);
				
					mStrafeDribbleCorrection -= correctionDist;
				}
			}
		}
		
		/*
		handleBall(footballer, t, dt, posDiff, runDirChanged);
		*/
		
		{
			if (impl.nodeIsDirty() == true) {

				impl.collUpdate();
				checkColliders(footballer, t);
				footballer.resolveCollisions(footballer, t, false);
			}
		}
		
		handleBallControlState(footballer, t, savedSpatialState);
	}

	mHasRequests = false;
	
	mRequestBodyFacing = null;
	mRequestRunDir = null;
	mRequestRunSpeedType = null;
	mRequestIsStrafing = null;
	mRequestActiveBallShot = null;
	mRequestActiveBallShotVelScale = null;
	
	//used when mIsEasyLockDribbling is true
	mDelayedRequestBodyFacing = null;
	mDelayedRequestRunDir = null;
	
	mRunDir = null;
	mRunSpeedType = null;
	mRunType = null;
	mIsStrafing = null;
	mActiveBallShot = null;
	mActiveBallShotVelScale = null;
	mLastShotDir = null;
	
	mRunDirChangeUnderSampleLeft = 0;
	mStrafeDribbleCorrection = 0.0;
	mIsEasyLockDribbling = false;
	
	/*
	static Dribble_None = -1;
	static Dribble_WaitingForOwnership = 0;
	static Dribble_Strafing = 1;
	static Dribble_PreparingForShot = 2;
	static Dribble_Controlling = 3;
	static Dribble_WaitingToLooseOwnership = 4;
	static Dribble_StrafingWaitingToLooseOwnership = 5;
	
	mDribbleState = -1;
	*/
	
	
	/*******************
		Ball Handling
	********************/
	
	static BallControlState_None = -1;
	static BallControlState_Undetermined = 0;
	static BallControlState_NotOwner = 1;
	static BallControlState_Owner = 2;
	static BallControlState_Stunned = 7;
	static BallControlState_Failed = 8;
	
	static BallTriggerID_None = -1;
	static BallTriggerID_Coll = 0;
	
	
	function ballControlOnConstructor() {
	
		mBallTrigger_ActiveUpperBodyColl = FootballerBallTrigger_ActiveUpperBodyColl();
		mBallTrigger_ActiveColl = FootballerBallTrigger_ActiveColl();
		mBallTrigger_Pitch = FootballerBallTrigger_Pitch();
		mBallTrigger_FootControl = FootballerBallTrigger_FootControl();
	}
	
	function ballControlOnStart(footballer, t, startState) {
	
		mBallControlState = BallControlState_None;
		switchBallControlState(footballer, BallControlState_Undetermined, null);
	}
	
	function ballControlOnStop(footballer, t) {
	
		if (!footballer.impl.isShootingBall())
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
	
		//print("triggerID: " + triggerID);
	
		switch (triggerID) {
		
			case BallTriggerID_Coll: {
			
				doBallAction(footballer, ball, true);
			
			} break;
		}
		
		return false;
	}
	
	function onBallCommandRejected(footballer, ball, commandID) {
	
		//print(footballer.impl.getNodeName() + "rejected");
	
		switchBallControlState(footballer, BallControlState_Stunned, null);
	}	
	
	function doBallAction(footballer, ball, allowBodyAction) {
	
		if (footballer.ballIsFootControllableEx(ball)) {
		
			doBallActionShoot(footballer, ball, true);
			
		} else {
		
			if (allowBodyAction)
				doBallActionStabilize(footballer, ball, null);
		}
	}
	
	function doBallActionStabilize(footballer, ball, soundName) {
	
		//print("stabilize");
	
		local shot = footballer.run.Shots[footballer.run.Shot_Stabilize];
		local addedVel = Vector3();
			
		mRunDir.mul(footballer.run.getSpeedValue(mRunType, mRunSpeedType), addedVel);
			
		footballer.shootBall(footballer.impl.genBallCommandID(), ball, shot, shot.speed, mRunDir, addedVel,
								false, false, false, footballer.getSkillBallProtection(), footballer.getSkillBallControlBodyOrFeet(), true, footballer.run.DribbleShotSwitchInfluence, soundName);	
			
		//footballer.setBallCommandGhostedShot(footballer.impl.genBallCommandID(), ball, shot, shot.speed, mRunDir, addedVel, footballer.run.DribbleShotSwitchInfluence, true, footballer.settings.BallSoftShotSoundName);
	}
	
	function doBallActionShoot(footballer, ball, allowActiveShot) {
	
		local settings = footballer.settings;
		
		if (allowActiveShot && mActiveBallShot.isValid) {
		
			local match = footballer.impl.getMatch();
			local shot = mActiveBallShot;
						
			//footballer.setBallCommandGhostedShot(footballer.impl.genBallCommandID(), ball, shot, shot.speed, mRunDir, null, footballer.settings.BallShotSwitchInfluence, true, footballer.settings.BallShotSoundName);
			
			footballer.shootBall(match.genBallCommandID(), ball, shot, shot.speed * mActiveBallShotVelScale, mRunDir, null, 
													true, true, true, footballer.getSkillBallProtection(), footballer.getSkillBallControlBodyOrFeet(), false, footballer.settings.BallShotSwitchInfluence, footballer.settings.BallShotSoundName);

		
		} else {

			local match = footballer.impl.getMatch();
			local shot = footballer.run.Shots[footballer.run.Shot_Dribble];
			local addedVel = Vector3();
			
			mRunDir.mul(footballer.run.getSpeedValue(mRunType, mRunSpeedType), addedVel);
			
			//footballer.setBallCommandGhostedShot(footballer.impl.genBallCommandID(), ball, shot, shot.speed, mRunDir, addedVel, footballer.run.DribbleShotSwitchInfluence, false, footballer.settings.BallShotSoundName);
			
			footballer.shootBall(match.genBallCommandID(), ball, shot, shot.speed, mRunDir, addedVel, 
													true, true, false, footballer.getSkillBallProtection(), 1.0, false, footballer.settings.BallShotSwitchInfluence, footballer.settings.BallShotSoundName);
		}
		
		mActiveBallShot.isValid = false;
		
		//switchBallControlState(footballer, BallControlState_Owner, null);
	}
	
	function switchBallControlState(footballer, nextState, setup) {
	
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
			
				if (footballer.impl.isBallOwner()) {
				
					switchBallControlState(footballer, BallControlState_Owner, null);
					
				} else {
				
					switchBallControlState(footballer, BallControlState_NotOwner, null);
				}
			
				return;
			
			} break; 
			
			case BallControlState_NotOwner: {
			
				mBallTriggers = [ 
									mBallTrigger_FootControl.setTriggerID(BallTriggerID_Coll),
									mBallTrigger_ActiveColl.setTriggerID(BallTriggerID_Coll),
								];
			} break; 
			
			case BallControlState_Owner: {

				mBallTriggers = [ 
									mBallTrigger_ActiveUpperBodyColl.setTriggerID(BallTriggerID_Coll)
								];
			
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
	
	
	function ballControlCreateSavedSpatialState(footballer) {
	
		if (mBallControlState == BallControlState_Owner) {
		
			local ret = CFootballerSavedSpatialState();
			ret.init(footballer.impl);
		
			return ret;
		}
		
		return null;
	}
	
	function handleBallControlState(footballer, t, savedSpatialState) {
	
		switch (mBallControlState) {
		
			case BallControlState_Owner: {
			
				local impl = footballer.impl;
				local ball = impl.getMatch().getBall();
			
				local shouldTriggerShot = impl.shouldTriggerBallDribbleAction(savedSpatialState.getWorldTransform(), impl.getWorldTransform(),
																		footballer.settings.BallSweetSpot, mRunDir, footballer.settings.BallPositionEpsilon);
			
				if (shouldTriggerShot) {
			
					//print("doShoot");
			
					if (footballer.ballIsFootControllableEx1(savedSpatialState.getWorldTransform(), ball) != footballer.ballIsFootControllable()) {
					
						print("incomp!: " + footballer.ballIsFootControllable());
					}
									
					if (footballer.ballIsFootControlHeightEx(impl.getPos(), ball))
						doBallActionShoot(footballer, ball, true);
					else 
						print("not foot controllable");
			
					/*
					if (ball.isOnPitch()) {
					
						print("shoot");
																			
							
					} else {

						if (footballer.ballIsFootControlHeight()
							&& doHighBallFootAction(footballer, ball)) {						
								
							print("doStabilize");						
										
						} else {	
									
							print("doTriggerStabilize");
							doTriggerHighBallFootAction(footballer, false);
						}					
					}
					*/
						
				} else {
				
					//print("improving");
					//still in our wanted direction
				}
			
			} break;
		
			case BallControlState_Stunned: {
			
				if (t >= mBallControlStunEndTime) {
				
					//print(footballer.impl.getNodeName() +" stun end");
					switchBallControlState(footballer, BallControlState_Undetermined, null);			
				} /*else {
				
					print(footballer.impl.getNodeName() + " stunned");
				}*/
			
			} break;
		}
	}
	
	mBallControlState = -1;
	mBallControlStunEndTime = 0.0;
	
	mBallTrigger_ActiveColl = null;
	mBallTrigger_ActiveUpperBodyColl = null;
	mBallTrigger_Pitch = null;
	mBallTrigger_FootControl = null;
};