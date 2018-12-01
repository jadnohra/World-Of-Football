
class BodyActionRunning_StartState {
	
	bodyFacing = null; //optional
	runDir = null; //optional
	runSpeedType = null; //optional
	
	function init(isStrafing, dir, speedType) {
	
		if (isStrafing) {
		
			bodyFacing = null;
			runDir = Vector3();
			runDir.set(dir);
		
		} else {
	
			bodyFacing = Vector3();
			bodyFacing.set(dir);
			runDir = bodyFacing;
		}
		
		runSpeedType = speedType;
	}
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
		mRequestActiveBallShotQueue = false;
		
		mRequestRunSpeedType = FootballerVarRequest();
		
		mRequestIsStrafing = FootballerVarRequest();
		
		mRunDir = Vector3();
		mActiveBallShot = FootballerBallShot(false, ::FootballerBallShot_None, 0.0, 0.0, 0.0, 0.0, false);
		
		ballControlOnConstructor();
	}

	function load(footballer, chunk, conv) {

		return true;
	}
	
	function isFinished() { return true; }	
	function getStaminaGain(footballer) { return footballer.run.getStaminaGain(footballer, mRunType, mRunSpeedType); }
	
	function isStrafing(footballer) { return footballer.run.isStrafing(mRunType); }
	function isStrafingHard(footballer) { return footballer.run.isStrafingHard(mRunType); }
	function wasLastShotActive() { return mWasLastShotActive; }
	
	function createStartState(isStrafing, dir, speedType) {
	
		local startState = BodyActionRunning_StartState();
		
		startState.init(isStrafing, dir, speedType);
	
		return startState;
	}
	
	function getRunDir() { return mRunDir; }
	
	function switchRunning(footballer, t, startState) {
	
		return true;
	}
	
	function cancelSetBodyFacing() {

		mRequestBodyFacing.isValid = false;
	}

	function setBodyFacing(footballer, val) {

		if (footballer.impl.getFacing().equals(val) || val.isZero())
			return;
		
		mHasRequests = true;	
		
		local request = mRequestBodyFacing;
			
		request.value.set(val);
		request.isValid = true;
	}
	
	function cancelSetRunDir() {

		mRequestRunDir.isValid = false;
	}

	function setRunDir(footballer, val) {
	
		if (mRunDir.equals(val))
			return;
			
		mHasRequests = true;	
		
		local request = mRequestRunDir;
			
		request.value.set(val);
		request.isValid = true;
	}
	
	function hasActiveBallShot() {
	
		return mActiveBallShot.isValid;
	}
	
	function cancelActiveBallShot() {
	
		return mActiveBallShot.isValid = false;
	}
	
	function setActiveBallShot(shot, shotVelScale, copyShot, queue) {
	
		if (shot == null || shot.isValid == false) {

			mRequestActiveBallShot.isValid = false;
			mRequestActiveBallShot.value = null;
			return;		
		}
		
		mRequestActiveBallShot.isValid = true;
		
		if (copyShot) {
		
			mRequestActiveBallShot.value = FootballerBallShot(false, ::FootballerBallShot_None, 0.0, 0.0, 0.0, 0.0, false);
			mRequestActiveBallShot.value.set(shot);
			
		} else {
		
			mRequestActiveBallShot.value = shot;
		}
		
		mRequestActiveBallShotVelScale = shotVelScale;
		mRequestActiveBallShotQueue = queue;
	}
	
	function cancelSetRunSpeed() {

		mRequestRunSpeedType.isValid = false;
	}

	function setRunSpeed(footballer, val) {

		if (mRunSpeedType == val)
			return;
	
		mHasRequests = true;
		
		mRequestRunSpeedType.value = val == null ? footballer.run.RunSpeed_Normal : val;
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
	
	function cancelSetIsStrafing() {

		mRequestIsStrafing.isValid = false;
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
		
		if (type == footballer.run.Run_Normal && footballer.impl.isBallOwner())
			type = footballer.run.Run_Dribble;
				
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
				setRunSpeed(footballer, startState.runSpeedType);
			} 
		} 
		
		if (!runSpeedWasSet)
			setRunSpeed(footballer, footballer.run.RunSpeed_Normal);
	}
	
	function start(footballer, t, startState, prevAction) {
		
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
		//mStrafeDribbleCorrection = 0.0;
	
		ballControlOnStart(footballer, t, startState);
		
		footballer.setBodyMomentum(null);
		return true;
	}
	
	function stop(footballer, t) {
	
		mRunType = footballer.run.Run_None;
		mIsStrafing = false;
				
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
	
	function hasQueuedActiveBallShotRequest() {
	
		return mRequestActiveBallShot.isValid;
	}
	
	function acceptActiveBallShotRequest(footballer) {
	
		if (mRequestActiveBallShot.isValid) {

			//print("accepeted");		
	
			mActiveBallShot.set(mRequestActiveBallShot.value);
			mActiveBallShotVelScale = mRequestActiveBallShotVelScale;
			mRequestActiveBallShot.isValid = false;
			mRequestActiveBallShot.value = null;
		}
	}
	
	function handleRequests(footballer, t) {

		if (mRequestActiveBallShot.isValid) {
		
			if (!mRequestActiveBallShotQueue) {
		
				//print("executed");
		
				acceptActiveBallShotRequest(footballer);
				
			} else {
			
				//print("delayed");
			}
		}
		
		local switchRunDribble = false;
		
		{
			local settings = footballer.run;
			local isBallOwner = footballer.impl.isBallOwner();
		
			if (mRunType == settings.Run_Normal && isBallOwner) {
			
				switchRunDribble = true;
				
			} else if (mRunType == settings.Run_Dribble && !isBallOwner) {
			
				switchRunDribble = true;
			}
		}
	
		if (mHasRequests || switchRunDribble) {

			mHasRequests = false;

			local needUpdateRunState = false;
			local needUpdateAnimSpeed = false;

			if (switchRunDribble) {
			
				needUpdateRunState = true;
			}
			
			if (mRequestBodyFacing.isValid) {

				footballer.impl.setFacing(mRequestBodyFacing.value);
				mRequestBodyFacing.isValid = false;

				needUpdateRunState = true;
			}

			if (mRequestRunDir.isValid) {

				mRunDir.set(mRequestRunDir.value);
				mRequestRunDir.isValid = false;
				
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
	
	function updateBodyMomentum(footballer, speedVector) {
	
		if (!footballer.run.isStrafingHard(mRunType)) {
		
			footballer.setBodyMomentum(speedVector);
			
		} else {
		
			footballer.setBodyMomentum(null);
		}
	}
	
	//printT = 0.0;
	
	function frameMove(footballer, t, dt) {

		local savedSpatialState = ballControlCreateSavedSpatialState(footballer);
	
		local runDirChanged;

		if (mRequestRunDir.isValid) {
			
			/*
			local dirDot = mRequestRunDir.value.dot(footballer.impl.getFacing());
			
			if (dirDot != 0.0) {
			
				print("dirDot: " + dirDot + ", eps: " + footballer.run.RunDirChangeEpsilone);
			}
			*/
		
			//if (printT == 0.0)
			//	printT = t;
		
			local changeAngleSine = (1.0 - mRequestRunDir.value.dot(footballer.impl.getFacing()));
			runDirChanged = fabs(changeAngleSine) >= footballer.run.RunDirChangeEpsilon;
			
			//printV3("" + changeAngleSine, mRequestRunDir.value);
			//print("FootballerBrain_Player_DirTestItem(" + (t - printT) + "," + mRequestRunDir.value.getEl(0) + "," + mRequestRunDir.value.getEl(1) + "," + mRequestRunDir.value.getEl(2) + ")" );
			
			//printT = t;
			
		} else {
		
			runDirChanged = false;
		}
		
		handleRequests(footballer, t);
		
		local speedVector = Vector3();
		local posDiff = Vector3();
		
		mRunDir.mul(footballer.run.getSpeedValue(mRunType, mRunSpeedType), speedVector);
		speedVector.mul(dt, posDiff);
		
		updateBodyMomentum(footballer, speedVector);
	
		local impl = footballer.impl;		
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		{	
			impl.shiftPos(posDiff);

			/*			
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
			*/
		}
		
		{
			if (impl.nodeIsDirty() == true) {

				impl.collUpdate();
				checkColliders(footballer, t);
				footballer.resolveCollisions(footballer, t, false);
			}
		}
		
		handleBallControlState(footballer, t, savedSpatialState, runDirChanged);
		cancelActiveBallShot();
	}

	mHasRequests = false;
	
	mRequestBodyFacing = null;
	mRequestRunDir = null;
	mRequestRunSpeedType = null;
	mRequestIsStrafing = null;
	mRequestActiveBallShot = null;
	mRequestActiveBallShotVelScale = null;
	mRequestActiveBallShotQueue = null;
	
	mRunDir = null;
	mRunSpeedType = null;
	mRunType = null;
	mIsStrafing = null;
	mActiveBallShot = null;
	mActiveBallShotVelScale = null;
	mWasLastShotActive = false;
		
	
	/*******************
		Ball Handling
	********************/
	
	static BallControlState_None = -1;
	static BallControlState_Undetermined = 0;
	static BallControlState_NotOwner = 1;
	static BallControlState_OwnerGroup_Start = 2;
	static BallControlState_OwnerNonStrafeGroup_Start = 2;
	static BallControlState_Owner = 2;
	static BallControlState_Turning = 3;
	static BallControlState_EndStrafe1 = 4;
	static BallControlState_EndStrafe2 = 5;
	static BallControlState_OwnerNonStrafeGroup_End = 5;
	static BallControlState_Strafing = 6;
	static BallControlState_OwnerGroup_End = 6;
	static BallControlState_Shooting = 10;
	static BallControlState_Stunned = 11;
	static BallControlState_Failed = 12;
	
	static BallTriggerID_None = -1;
	static BallTriggerID_Coll = 0;
	static BallTriggerID_Control = 1;
	
	function ballControlIsOwnerNonStrafeState(state) {
	
		return state >= BallControlState_OwnerNonStrafeGroup_Start && state <= BallControlState_OwnerNonStrafeGroup_End;
	}
	
	function ballControlIsOwnerNonTruning(state) {
	
		return state != BallControlState_Turning && state >= BallControlState_OwnerGroup_Start && state <= BallControlState_OwnerGroup_End;
	}
	
	function ballControlIsEndStrafe(state) {
	
		return state == BallControlState_EndStrafe1 || state == BallControlState_EndStrafe2;
	}
	
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
			
				doBallAction(footballer, ball, true, null);
			
			} break;
			
			case BallTriggerID_Control: {
			
				doBallActionShoot(footballer, ball, true);
			
			} break;
		}
		
		return false;
	}
	
	function onBallCommandRejected(footballer, ball, commandID) {
	
		//print(footballer.impl.getNodeName() + "rejected");
	
		switchBallControlState(footballer, BallControlState_Stunned, null);
	}	
	
	function doBallAction(footballer, ball, allowBodyAction, bodySoundName) {
	
		if (footballer.ballIsFootControllableEx(ball)) {
		
			doBallActionShoot(footballer, ball, true);
			
		} else {
		
			if (allowBodyAction) {
			
				doBallActionStabilize(footballer, ball, bodySoundName);
			}
		}
	}
	
	function getBallActionDir(footballer) {
	
		return mIsStrafing ? footballer.impl.getFacing() : mRunDir;
	}
	
	function doBallActionStabilize(footballer, ball, soundName) {
	
		//print("stabilize " + soundName);
	
		local shot = footballer.run.Shots[footballer.run.Shot_Stabilize];
		local addedVel = Vector3();
			
		mRunDir.mul(footballer.run.getSpeedValue(mRunType, mRunSpeedType), addedVel);
			
		footballer.shootBall(footballer.impl.genBallCommandID(), ball, shot, shot.speed, getBallActionDir(footballer), addedVel,
								true, true, false, footballer.getSkillBallProtection(), footballer.getSkillBallControlBodyOrFeet(), true, footballer.run.DribbleShotSwitchInfluence, soundName);	
			
		//footballer.setBallCommandGhostedShot(footballer.impl.genBallCommandID(), ball, shot, shot.speed, mRunDir, addedVel, footballer.run.DribbleShotSwitchInfluence, true, footballer.settings.BallSoftShotSoundName);
	}
	
	function doBallActionShoot(footballer, ball, allowActiveShot) {
	
		//print("shoot");
	
		local settings = footballer.settings;
		
		if (allowActiveShot && hasActiveBallShot()) {
		
			mWasLastShotActive = true;
		
			//print("activeShot");
		
			local match = footballer.impl.getMatch();
			local shot = mActiveBallShot;
						
			//footballer.setBallCommandGhostedShot(footballer.impl.genBallCommandID(), ball, shot, shot.speed, mRunDir, null, footballer.settings.BallShotSwitchInfluence, true, footballer.settings.BallShotSoundName);
			
			footballer.shootBall(match.genBallCommandID(), ball, shot, shot.speed * mActiveBallShotVelScale, getBallActionDir(footballer), null, 
													true, true, true, footballer.getSkillBallProtection(), footballer.getSkillBallControlBodyOrFeet(), false, footballer.settings.BallShotSwitchInfluence, footballer.settings.BallShotSoundName);

		} else {
		
			mWasLastShotActive = false;
		
			//print("dribbleShot");	

			local match = footballer.impl.getMatch();
			local shot = footballer.run.Shots[footballer.run.Shot_Dribble];
			local addedVel = Vector3();
			
			mRunDir.mul(footballer.run.getSpeedValue(mRunType, mRunSpeedType), addedVel);
			
			//printV3("addedVel", addedVel);	
			//print("shotSpeed" + shot.speed);	
			
			//footballer.setBallCommandGhostedShot(footballer.impl.genBallCommandID(), ball, shot, shot.speed, mRunDir, addedVel, footballer.run.DribbleShotSwitchInfluence, false, footballer.settings.BallShotSoundName);
			
			footballer.shootBall(match.genBallCommandID(), ball, shot, shot.speed, getBallActionDir(footballer), addedVel, 
													true, true, false, footballer.getSkillBallProtection(), 1.0, false, footballer.settings.BallShotSwitchInfluence, footballer.settings.BallShotSoundName);
		}
		
		cancelActiveBallShot();
		
		//switchBallControlState(footballer, BallControlState_Owner, null);
	}
	
	function doBallActionSweetSpotControl(footballer, t) {
	
		local impl = footballer.impl;
		local match = impl.getMatch();
	
		impl.setBallControlImmediateTeleport(match.genBallCommandID(), footballer.getSkillBallProtection(), 
												true, footballer.settings.BallSweetSpot, true, true);
			
		footballer.playBallControlSound(t);
		//we need to wait for sound to finish!			
		//impl.playSound(footballer.settings.BallControlSoundName, match.getBall().getPos(), footballer.settings.BallControlSoundMag);
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
									mBallTrigger_FootControl.setTriggerID(BallTriggerID_Control),
									mBallTrigger_ActiveColl.setTriggerID(BallTriggerID_Coll),
								];
			} break; 
			
			case BallControlState_Owner:
			case BallControlState_Turning:
			case BallControlState_Strafing: 
			case BallControlState_EndStrafe1: 
			case BallControlState_EndStrafe2:
			case BallControlState_Shooting: {

				mBallTurningFrameLeft = footballer.run.RunDirUnderSampleCount;
			
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
	
		if (mBallControlState == BallControlState_Owner || mBallControlState == BallControlState_Strafing) {
		
			local ret = CFootballerSavedSpatialState();
			ret.init(footballer.impl);
		
			return ret;
		}
		
		return null;
	}
	
	function handleBallControlState(footballer, t, savedSpatialState, dirChanged) {
	
		local impl = footballer.impl;
		
		if ((hasQueuedActiveBallShotRequest() || hasActiveBallShot())
			&& mBallControlState != BallControlState_Shooting && impl.isBallOwner()) {
		
			acceptActiveBallShotRequest(footballer);
		
			doBallAction(footballer, impl.getMatch().getBall(), true, footballer.settings.BallSoftShotSoundName);
			switchBallControlState(footballer, BallControlState_Shooting, null);
			return;
		}
	
		if (mBallControlState == BallControlState_Shooting) {
		
			if (impl.getMatch().getBall().isResting())
				switchBallControlState(footballer, BallControlState_Undetermined, null);
		}
	
		if (mIsStrafing) {
		
			if (ballControlIsOwnerNonStrafeState(mBallControlState)) {
			
				switchBallControlState(footballer, BallControlState_Strafing, null);
			} 
			
		} else {
		
			if (mBallControlState == BallControlState_Strafing) {
			
				switchBallControlState(footballer, BallControlState_EndStrafe1, null);
			
			} else {
			
				if (dirChanged && !ballControlIsEndStrafe(mBallControlState) && ballControlIsOwnerNonTruning(mBallControlState))
					switchBallControlState(footballer, BallControlState_Turning, null);
			}
		}
		
	
		switch (mBallControlState) {
				
			case BallControlState_Owner: {
			
				local ball = impl.getMatch().getBall();
			
				local shouldTriggerShot = impl.shouldTriggerBallDribbleAction(savedSpatialState.getWorldTransform(), impl.getWorldTransform(),
																		footballer.settings.BallSweetSpot, mRunDir, footballer.settings.BallPositionEpsilon, footballer.run.RunDirChangeEpsilon);
			
				if (shouldTriggerShot) {
			
					//print("doShoot");
			
					/*
					if (footballer.ballIsFootControllableEx1(savedSpatialState.getWorldTransform(), ball) != footballer.ballIsFootControllable()) {
					
						print("incomp!: " + footballer.ballIsFootControllable());
					}
					*/

					doBallAction(footballer, ball, false, footballer.settings.BallSoftShotSoundName);
					
					/*
					if (footballer.ballIsFootControlHeightEx(impl.getPos(), ball))
						doBallActionShoot(footballer, ball, true);
					else 
						print("not foot controllable");
					*/	
					
					//print("didShoot");
						
				} else {
				
					//print("improving");
					//still in our wanted direction
				}
			
			} break;
		
			case BallControlState_Turning: {
				
				if (!dirChanged)
					mBallTurningFrameLeft -= 1;
				
				if (mBallTurningFrameLeft > 0) {
				
					//print("turn");
					
					local match = impl.getMatch();
				
					if (footballer.ballIsFootControlHeight()) {
					
						doBallActionSweetSpotControl(footballer, t);
					}
				
				} else {
				
					local ball = impl.getMatch().getBall();
				
					if (footballer.ballIsFootControlHeightEx(impl.getPos(), ball))
						doBallActionShoot(footballer, ball, true);
					//else 
					//	print("not foot controllable");
				
					switchBallControlState(footballer, BallControlState_Owner, null);
				}
			
			} break;
			
			case BallControlState_Strafing: {
			
				if (footballer.ballIsFootControlHeight()) {
					
					doBallActionSweetSpotControl(footballer, t);
				}
			
			} break;
			
			case BallControlState_EndStrafe1: {
			
				if (footballer.ballIsFootControlHeight()) {
					
					doBallActionSweetSpotControl(footballer, t);
				}
				
				switchBallControlState(footballer, BallControlState_EndStrafe2, null);
				
			} break;
			
			case BallControlState_EndStrafe2: {
			
				local ball = impl.getMatch().getBall();
			
				if (footballer.ballIsFootControlHeight()) {
				
					//print("end strafe shoot");
					doBallActionShoot(footballer, ball, true);
					
				} else {
					
					//print("not foot controllable");
				}
			
				switchBallControlState(footballer, dirChanged ? BallControlState_Turning : BallControlState_Owner, null);
				return;
				
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
	mBallTurningFrameLeft = 0;
	
	mBallTrigger_ActiveColl = null;
	mBallTrigger_ActiveUpperBodyColl = null;
	mBallTrigger_Pitch = null;
	mBallTrigger_FootControl = null;
};