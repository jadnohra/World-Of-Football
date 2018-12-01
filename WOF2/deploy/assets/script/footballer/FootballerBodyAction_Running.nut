
/*
class BodyAction_Running_State {
	
	bodyFacing = null; 
	runDir = null;
	isStrafing = null;
	runSpeedType = null;
}
*/

class FootballerBodyAction_Running extends FootballerBodyAction_BaseBallTriggers {

	function getType() { return Footballer.Action_Running; }
		
	constructor() {
	
		mRequestBodyFacing = FootballerVarRequest();
		mRequestBodyFacing.value = Vector3();
		
		mRequestVelocity = FootballerVarRequest();
		mRequestRunDir = FootballerVarRequest();
		mRequestRunDir.value = Vector3();
		mRequestRunSpeedType = FootballerVarRequest();
		
		mRequestBallShot = FootballerVarRequest();
		mRequestBallShot.value = FootballerBallShotRequestValue();
		
		mRequestStrafe = FootballerVarRequest();
		
		mRunDir = Vector3();
		
		ballControlOnConstructor();
	}

	function load(body, chunk, conv) {

		mSettings = body.mProfile.run;
	
		return true;
	}
	
	function getStaminaGain(body) { return mSettings.getStaminaGain(body, mRunType, mRunSpeedType); }
	function isSwitchLocked(body) { return true; }
	
	function isStrafing(body) { return mSettings.isStrafing(mRunType); }
	function isStrafingHard(body) { return mSettings.isStrafingHard(mRunType); }
	function wasLastShotActive() { return mWasLastShotActive; }
	
	function getRunDir() { return mRunDir; }
		
	function requestBodyFacing(body, requestID, bodyFacing, needsFixing) {

		if (body.mImpl.getFacing().equals(bodyFacing))
			return ::FootballerBody.Request_Executed;
		
		mHasRequests = true;
		local request = mRequestBodyFacing;
			
		request.init(body, requestID);
		request.value.set(bodyFacing);
		
		if (needsFixing)
			body.helperFixDir(request.value);
			
		return ::FootballerBody.Request_Executing;
	}
	
	function requestVelocity(body, requestID, dir, needsFixing, speedType) {
	
		local requestedRunDir = false;
		local requestedRunSpeed = false;
		
		if (dir != null) {
		
			if (!mRunDir.equals(dir)) {
				
				requestedRunDir = true;
				
				if (dir.isZero()) {
				
					dir = body.mImpl.getFacing();
					
				} 
				
				mHasRequests = true;	
					
				local request = mRequestRunDir;
						
				request.init(body, null);
				request.value.set(dir);
					
				if (needsFixing)
					body.mFootballer.helperFixDir(request.value);	
			}
		}
		
		if ((speedType != null) || (mRunSpeedType == null)) {
		
			if ((mRunSpeedType != speedType) || (mRunSpeedType == null)) {
			
				requestedRunSpeed = true;
			
				mHasRequests = true;
		
				local request = mRequestRunSpeedType;
		
				request.init(body, null);
				request.value = (speedType == null) ? mSettings.RunSpeed_Normal : speedType;
			}
		}
		
		if (requestedRunDir || requestedRunSpeed) {
			
			mRequestVelocity.init(body, requestID);
			
			return ::FootballerBody.Request_Executing;
		}
		
		return ::FootballerBody.Request_Executed;
	}
	
	function requestShot(body, requestID, shotType, shot, copyShot, speedScale) {
	
		mRequestBallShot.init(body, requestID);
	
		if (shot == null || shot.isValid == false) {

			mRequestBallShot.isValid = true;
			mRequestBallShot.value.shot = null;
			
			return ::FootballerBody.Request_Executed;
		}
		
		if (copyShot) {
		
			mRequestBallShot.value.shot = createFootballerBallShot();
			mRequestBallShot.value.shot.set(shot);
			
		} else {
		
			mRequestBallShot.value.shot = shot;
		}
		
		if (speedScale == null)
			mRequestBallShot.value.speedScale = 1.0;
		else 
			mRequestBallShot.value.speedScale = speedScale;
		
		return ::FootballerBody.Request_Executing;
	}
	
	function cancelSetIsStrafing() {

		mRequestStrafe.isValid = false;
	}

	function requestStrafe(body, requestID, strafe) {

		if (mIsStrafing == strafe)
			return FootballerBody.Request_Executed;
	
		mHasRequests = true;
		
		mRequestStrafe.init(body, requestID);
		mRequestStrafe.value = strafe;
	}
	

	function getIsStrafing() {
	
		return mIsStrafing;
	}
	
	function getRunSpeedType() { return mRunSpeedType; } 
	function getRunType() { return mRunType; } 
	function getRunTypeIsStraight() { return mRunType == mSettings.Run_Normal || mRunType == mSettings.Run_StrafeFwd; } 
	
	function validateRunState(body) { 
	
		if (mRunType == mSettings.Run_None) 
			updateRunState(body); 
	}
	
	function getCurrSpeed(body) {
	
		validateRunState(body);

		return mSettings.getSpeedValue(getRunType(), getRunSpeedType());
	}
	
	function getCurrVelocity(body, result) {
	
		mRunDir.mul(getCurrSpeed(body), result);
	}
	
	function detectRunType(body, facing, dir, isStrafing, isBallOwner) {
	
		if (dir == null)
			return isBallOwner ? mSettings.Run_Dribble : mSettings.Run_Normal;
	
		local type;
	
		local tolerance = 0.0175; //sin(1 degrees)
		local dot = facing.dot(dir);

		if (fabs(1.0 - dot) <= tolerance) {

			type = isStrafing ? mSettings.Run_StrafeFwd : mSettings.Run_Normal;

		} else {

			local divCount = 200;
			local divCountMiddle = divCount / 2.0;
			local focusDivCount = 10;
		
			local discreeteDir = Vector3();

			local step = discretizeDirection(facing, body.mImpl.getMatch().SceneUpAxis(), dir, divCount, discreeteDir);
			step = step.tofloat();

			if (fabs(step) <= focusDivCount) {
			
				type = mSettings.Run_StrafeFwd; 
				
			} else if (fabs(divCountMiddle - step) <= focusDivCount) {
			
				type = mSettings.Run_StrafeBack; 
				
			} else {
			
				if (step < divCountMiddle) {
				
					type = mSettings.Run_StrafeRight; 
					
				} else {
				
					type = mSettings.Run_StrafeLeft; 
				}
			}
		}
		
		if (type == mSettings.Run_Normal && isBallOwner)
			type = mSettings.Run_Dribble;
				
		return type;
	}
	
	function updateRunState(body) {

		local runType = detectRunType(body, body.mImpl.getFacing(), mRunDir, mIsStrafing, body.mImpl.isBallOwner());

		if (runType != mRunType) {

			mRunType = runType;

			local impl = body.mImpl;
			local animIndex = impl.getAnimIndex(mSettings.AnimNames[mRunType]);

			if (animIndex != impl.getCurrAnimIndex()) {

				impl.setAnimByIndex(animIndex, false);
				impl.setAnimSpeed(mSettings.speedToAnimSpeed(body, mSettings.getSpeedValue(mRunType, mRunSpeedType)));
				impl.setAnimLoop(true);
			}
		}
	}
	
	function start(body, t) {
		
		mIsStrafing = false;
		mRunType = mSettings.Run_None;
		mRunDir.zero();
		
		ballControlOnStart(body, t);
		
		body.setMomentum(null, false);
		return true;
	}
	
	function stop(body, t) {
	
		cancelRequests(body, t);
	
		mRunType = mSettings.Run_None;
		mIsStrafing = false;
				
		ballControlOnStop(body, t);
		
		::FootballerBodyAction_Base.stop(body, t);
	}
	
	function cancelRequests(body, t) {
	
		mRequestStrafe.end(body, FootballerBody.Request_Blocked);
		mRequestVelocity.end(body, FootballerBody.Request_Blocked);
		mRequestBallShot.end(body, FootballerBody.Request_Blocked);
	}
	
	function handleRequests(body, t) {

		local switchRunDribble = false;
		
		{
			local isBallOwner = body.mImpl.isBallOwner();
		
			if (mRunType == mSettings.Run_Normal && isBallOwner) {
			
				switchRunDribble = true;
				
			} else if (mRunType == mSettings.Run_Dribble && !isBallOwner) {
			
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

				body.mImpl.setFacing(mRequestBodyFacing.value);
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
						
			if (mRequestStrafe.isValid) {

				mIsStrafing = mRequestStrafe.value;
				mRequestStrafe.isValid = false;
				
				needUpdateRunState = true;
			}

			if (needUpdateRunState || mRunType == mSettings.Run_None) {

				needUpdateAnimSpeed = true;

				updateRunState(body);
			}

			if (needUpdateAnimSpeed) {

				body.mImpl.setAnimSpeed(mSettings.speedToAnimSpeed(body, mSettings.getSpeedValue(mRunType, mRunSpeedType)));
			}
		} 
		
		mRequestVelocity.end(body, FootballerBody.Request_Executed);		
		
		return true;
	}
	
	function updateBodyMomentum(body, speedVector) {
	
		body.setMomentum(speedVector, !mSettings.isStrafingHard(mRunType));
	}
	
	function handleActionEnd(body, t) {

		mRequestStrafe.signal(body, FootballerBody.Request_Executing);
		mRequestVelocity.signal(body, FootballerBody.Request_Executing);
		mRequestBallShot.signal(body, FootballerBody.Request_Executing);
	
		return true;
	}
	
	function checkColliders(body, t) {
	
		/*
		
		jump over??
		
		local impl = body.mImpl;
	
		local index = Int(0);
		local collider;
		local type = body.getTypeInt();
		
		while ((collider = impl.collFindDynVolColliderInt(index, type)) != null) {

			if (collider.getActionType() == body.Action_Tackling) {
				
				switchTackled(body, t, collider);
			}
		}
		*/
	}
	
	
	function frameMove(body, t, dt) {

		local savedSpatialState = ballControlCreateSavedSpatialState(body);
	
		local runDirChanged;

		if (mRequestRunDir.isValid) {
				
			local changeAngleSine = (1.0 - mRequestRunDir.value.dot(body.mImpl.getFacing()));
			runDirChanged = fabs(changeAngleSine) >= mSettings.RunDirChangeEpsilon;
			
		} else {
		
			runDirChanged = false;
		}
		
		handleRequests(body, t);
		
		local speedVector = Vector3();
		local posDiff = Vector3();
		
		mRunDir.mul(mSettings.getSpeedValue(mRunType, mRunSpeedType), speedVector);
		speedVector.mul(dt, posDiff);
		
		updateBodyMomentum(body, speedVector);
	
		local impl = body.mImpl;		
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		{	
			impl.shiftPos(posDiff);
		}
		
		{
			if (impl.nodeIsDirty() == true) {

				impl.collUpdate();
				checkColliders(body, t);
				body.resolveCollisions(t, false);
			}
		}
		
		handleBallControlState(body, t, savedSpatialState, runDirChanged);
		
		return handleActionEnd(body, t);
	}

	mSettings = null;
	mHasRequests = false;
	
	mRequestBodyFacing = null;
	mRequestVelocity = null;
	mRequestRunDir = null;
	mRequestRunSpeedType = null;
	mRequestStrafe = null;
	mRequestBallShot = null;
	
	mRunDir = null;
	mRunSpeedType = null;
	mRunType = null;
	mIsStrafing = null;
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
	
	function ballControlOnStart(body, t) {
	
		mBallControlState = BallControlState_None;
		switchBallControlState(body, BallControlState_Undetermined, null);
	}
	
	function ballControlOnStop(body, t) {
	
		if (!body.mImpl.isShootingBall())
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
	
		//print("triggerID: " + triggerID);
	
		switch (triggerID) {
		
			case BallTriggerID_Coll: {
			
				doBallAction(footballer.mBody, ball, true, null);
			
			} break;
			
			case BallTriggerID_Control: {
			
				doBallActionShoot(footballer.mBody, ball, true);
			
			} break;
		}
		
		return false;
	}
	
	function onBallCommandRejected(footballer, ball, commandID) {
	
		//print(body.mImpl.getNodeName() + "rejected");
	
		switchBallControlState(footballer.mBody, BallControlState_Stunned, null);
	}	
	
	function doBallAction(body, ball, allowBodyAction, bodySoundName) {
	
		if (body.ballIsFootControllableEx(ball)) {
		
			doBallActionShoot(body, ball, true);
			
		} else {
		
			if (allowBodyAction) {
			
				doBallActionStabilize(body, ball, bodySoundName);
			}
		}
	}
	
	function getBallActionDir(body) {
	
		return mIsStrafing ? body.mImpl.getFacing() : mRunDir;
	}
	
	function doBallActionStabilize(body, ball, soundName) {
	
		//print("stabilize " + soundName);
	
		local shot = mSettings.Shots[mSettings.Shot_Stabilize];
		local addedVel = Vector3();
			
		mRunDir.mul(mSettings.getSpeedValue(mRunType, mRunSpeedType), addedVel);
			
		body.shootBall(body.mImpl.genBallCommandID(), ball, shot, shot.speed, getBallActionDir(body), addedVel,
								true, true, false, body.getSkillBallProtection(), body.getSkillBallControlBodyOrFeet(), true, mSettings.DribbleShotSwitchInfluence, soundName);	
			
		//body.setBallCommandGhostedShot(body.mImpl.genBallCommandID(), ball, shot, shot.speed, mRunDir, addedVel, mSettings.DribbleShotSwitchInfluence, true, body.mProfile.BallSoftShotSoundName);
	}
	
	function doBallActionShoot(body, ball, allowActiveShot) {
	
		//print("shoot");
	
		if (allowActiveShot && mRequestBallShot.isValid) {
		
			mWasLastShotActive = true;
		
			//print("activeShot");
		
			local match = body.mImpl.getMatch();
			local shotInfo = mRequestBallShot.value;
						
			//body.setBallCommandGhostedShot(body.mImpl.genBallCommandID(), ball, shot, shot.speed, mRunDir, null, body.mProfile.BallShotSwitchInfluence, true, body.mProfile.BallShotSoundName);
			
			body.shootBall(match.genBallCommandID(), ball, shotInfo.shot, shotInfo.shot.speed * shotInfo.speedScale, getBallActionDir(body), null, 
													true, true, true, body.getSkillBallProtection(), body.getSkillBallControlBodyOrFeet(), false, body.mProfile.BallShotSwitchInfluence, body.mProfile.BallShotSoundName);
			
			mRequestBallShot.end(body, FootballerBody.Request_Executed);
										
		} else {
		
			mWasLastShotActive = false;
		
			//print("dribbleShot");	

			local match = body.mImpl.getMatch();
			local shot = mSettings.Shots[mSettings.Shot_Dribble];
			local addedVel = Vector3();
			
			mRunDir.mul(mSettings.getSpeedValue(mRunType, mRunSpeedType), addedVel);
			
			//printV3("addedVel", addedVel);	
			//print("shotSpeed" + shot.speed);	
			
			//body.setBallCommandGhostedShot(body.mImpl.genBallCommandID(), ball, shot, shot.speed, mRunDir, addedVel, mSettings.DribbleShotSwitchInfluence, false, body.mProfile.BallShotSoundName);
			
			body.shootBall(match.genBallCommandID(), ball, shot, shot.speed, getBallActionDir(body), addedVel, 
													true, true, false, body.getSkillBallProtection(), 1.0, false, body.mProfile.BallShotSwitchInfluence, body.mProfile.BallShotSoundName);
		}
		
		//switchBallControlState(body, BallControlState_Owner, null);
	}
	
	function doBallActionSweetSpotControl(body, t) {
	
		local impl = body.mImpl;
		local match = impl.getMatch();
	
		impl.setBallControlImmediateTeleport(match.genBallCommandID(), body.getSkillBallProtection(), 
												true, body.mProfile.BallSweetSpot, true, true);
			
		body.playBallControlSound(t);
		//we need to wait for sound to finish!			
		//impl.playSound(body.mProfile.BallControlSoundName, match.getBall().getPos(), body.mProfile.BallControlSoundMag);
	}
	
	function switchBallControlState(body, nextState, setup) {
	
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
			
				if (body.mImpl.isBallOwner()) {
				
					switchBallControlState(body, BallControlState_Owner, null);
					
				} else {
				
					switchBallControlState(body, BallControlState_NotOwner, null);
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

				mBallTurningFrameLeft = mSettings.RunDirUnderSampleCount;
			
				mBallTriggers = [ 
									mBallTrigger_ActiveUpperBodyColl.setTriggerID(BallTriggerID_Coll)
								];
			
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
	
	
	function ballControlCreateSavedSpatialState(body) {
	
		if (mBallControlState == BallControlState_Owner || mBallControlState == BallControlState_Strafing) {
		
			local ret = CFootballerSavedSpatialState();
			ret.init(body.mImpl);
		
			return ret;
		}
		
		return null;
	}
	
	function handleBallControlState(body, t, savedSpatialState, dirChanged) {
	
		local impl = body.mImpl;
		
		if ((mRequestBallShot.isValid)
			&& mBallControlState != BallControlState_Shooting && impl.isBallOwner()) {
		
			doBallAction(body, impl.getMatch().getBall(), true, body.mProfile.BallSoftShotSoundName);
			switchBallControlState(body, BallControlState_Shooting, null);
			return;
		}
	
		if (mBallControlState == BallControlState_Shooting) {
		
			if (impl.getMatch().getBall().isResting())
				switchBallControlState(body, BallControlState_Undetermined, null);
		}
	
		if (mIsStrafing) {
		
			if (ballControlIsOwnerNonStrafeState(mBallControlState)) {
			
				switchBallControlState(body, BallControlState_Strafing, null);
			} 
			
		} else {
		
			if (mBallControlState == BallControlState_Strafing) {
			
				switchBallControlState(body, BallControlState_EndStrafe1, null);
			
			} else {
			
				if (dirChanged && !ballControlIsEndStrafe(mBallControlState) && ballControlIsOwnerNonTruning(mBallControlState))
					switchBallControlState(body, BallControlState_Turning, null);
			}
		}
		
	
		switch (mBallControlState) {
				
			case BallControlState_Owner: {
			
				local ball = impl.getMatch().getBall();
			
				local shouldTriggerShot = impl.shouldTriggerBallDribbleAction(savedSpatialState.getWorldTransform(), impl.getWorldTransform(),
																		body.mProfile.BallSweetSpot, mRunDir, body.mProfile.BallPositionEpsilon, mSettings.RunDirChangeEpsilon);
			
				if (shouldTriggerShot) {
			
					//print("doShoot");
			
					/*
					if (body.ballIsFootControllableEx1(savedSpatialState.getWorldTransform(), ball) != body.ballIsFootControllable()) {
					
						print("incomp!: " + body.ballIsFootControllable());
					}
					*/

					doBallAction(body, ball, false, body.mProfile.BallSoftShotSoundName);
					
					/*
					if (body.ballIsFootControlHeightEx(impl.getPos(), ball))
						doBallActionShoot(body, ball, true);
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
				
					if (body.ballIsFootControlHeight()) {
					
						doBallActionSweetSpotControl(body, t);
					}
				
				} else {
				
					local ball = impl.getMatch().getBall();
				
					if (body.ballIsFootControlHeightEx(impl.getPos(), ball))
						doBallActionShoot(body, ball, true);
					//else 
					//	print("not foot controllable");
				
					switchBallControlState(body, BallControlState_Owner, null);
				}
			
			} break;
			
			case BallControlState_Strafing: {
			
				if (body.ballIsFootControlHeight()) {
					
					doBallActionSweetSpotControl(body, t);
				}
			
			} break;
			
			case BallControlState_EndStrafe1: {
			
				if (body.ballIsFootControlHeight()) {
					
					doBallActionSweetSpotControl(body, t);
				}
				
				switchBallControlState(body, BallControlState_EndStrafe2, null);
				
			} break;
			
			case BallControlState_EndStrafe2: {
			
				local ball = impl.getMatch().getBall();
			
				if (body.ballIsFootControlHeight()) {
				
					//print("end strafe shoot");
					doBallActionShoot(body, ball, true);
					
				} else {
					
					//print("not foot controllable");
				}
			
				switchBallControlState(body, dirChanged ? BallControlState_Turning : BallControlState_Owner, null);
				return;
				
			} break;
		
			case BallControlState_Stunned: {
			
				if (t >= mBallControlStunEndTime) {
				
					//print(body.mImpl.getNodeName() +" stun end");
					switchBallControlState(body, BallControlState_Undetermined, null);			
				} /*else {
				
					print(body.mImpl.getNodeName() + " stunned");
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