
class BodyActionRunning_StartState {
	
	bodyFacing = null; //optional
	runDir = null; //optional
	runSpeedType = null; 
}

class BodyAction_Running extends BodyActionBase {

	function getType(footballer) {
	
		return footballer.Action_Running;
	}

	constructor() {
	
		mRequestBodyFacing = FootballerVarRequest();
		mRequestBodyFacing.value = Vector3();
		
		mRequestRunDir = FootballerVarRequest();
		mRequestRunDir.value = Vector3();
		
		mRequestBallShot = FootballerVarRequest();
		mRequestBallShot.value = FootballerBallShot(false, 0.0, 0.0, 0.0, false);
		
		mRequestRunSpeedType = FootballerVarRequest();
		
		mRequestIsStrafing = FootballerVarRequest();
		
		mRunDir = Vector3();
		mLastShotDir = Vector3(); 
		mBallShot = FootballerBallShot(false, 0.0, 0.0, 0.0, false);
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
	function getStaminaGain(footballer) { return footballer.run.getSafeSpeedValue(mRunType, mRunSpeedType) * footballer.run.StaminaGainPerSpeedUnit; }
	
	function getRunDir() { return mRunDir; }
		
	function switchRunning(footballer, t, startState) {
	
		return true;
	}
	
	function cancelSetBodyFacing() {

		mRequestBodyFacing.isValid = false;
	}

	function setBodyFacing(footballer, val) {

		local request = null;
	
		if (delayRequest(footballer)) {
		
			if (!mDelayedRequestBodyFacing.isValid && footballer.impl.getFacing().equals(val))
				return;
		
			request = mDelayedRequestBodyFacing;
		
		} else {
		
			if (footballer.impl.getFacing().equals(val))
				return;
		
			mHasRequests = true;	
		
			request = mRequestBodyFacing;
		}
			
		request.value.set(val);
		request.isValid = true;
	}
	
	function cancelSetRunDir() {

		mRequestRunDir.isValid = false;
	}

	function setRunDir(footballer, val) {

		local request = null;
	
		if (delayRequest(footballer)) {
		
			if (!mDelayedRequestRunDir.isValid && mRunDir.equals(val))
				return;
		
			request = mDelayedRequestRunDir;
		
		} else {
		
			if (mRunDir.equals(val))
				return;

			mHasRequests = true;	
		
			request = mRequestRunDir;
		}
			
		request.value.set(val);
		request.isValid = true;
	}
	
	function setBallShot(shot, copyShot) {
	
		if (shot == null && mBallShot == null)
			return;
	
		if (copyShot) {
		
			mRequestBallShot.value.set(shot);
			
		} else {
		
			mRequestBallShot.value = shot;
		}
		
		mRequestBallShot.isValid = true;
	}
	
	function delayRequest(footballer) {
	
		if (mIsEasyLockDribbling) {
		
			if (mIsStrafing || (mRequestIsStrafing.isValid && mRequestIsStrafing.value)) {
			
				return false;
			}
			
			return mDribbleState != Dribble_Controlling;
		}
		
		return false;
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
	
	function start(footballer, t, startState) {
		
		mIsEasyLockDribbling = false;
		mDribbleState = Dribble_None;
	
		mRunType = footballer.run.Run_None;
		mRunDir.zero();
	
		applyStartState(footballer, startState);
					
		/*	
		if (!handleRequests(footballer, t)) {

			return false;
		}
		
		mRunDirChangeUnderSampleLeft = 0;
	
		validateRunState(footballer);
		*/
				
	
		/*
		footballer.ballHandler.reset(footballer, true);
		*/
		mStrafeDribbleCorrection = 0.0;
	
		/*
		
			ok mStrafeDribbleCorrection has also to be deactivated on ball events and on changeDirs (this means passing ball events to action as well??? )
			and impl the correct way of correcting shotVelocity by simulating
			dribble shot at video start (also dribble while running fast)
			also add the EasyDribble option which blocks dir changes
			until new ball contact, maybe add some fine tuning constraints
		*/
	
		return true;
	}
	
	function stop(footballer, t) {
	
		/*
		footballer.ballHandler.setBrokenActionListener(null);
		*/
		mRunType = footballer.run.Run_None;
		
		/*
		footballer.ballHandler.reset(footballer, true);
		*/
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
	
	function handleDelayedRequests(footballer, t) {
	
		local needUpdateRunState = false;
		local needUpdateAnimSpeed = false;

		if (mDelayedRequestBodyFacing.isValid) {

			footballer.impl.setFacing(mDelayedRequestBodyFacing.value);
			mDelayedRequestBodyFacing.isValid = false;

			needUpdateRunState = true;
		}

		if (mDelayedRequestRunDir.isValid) {

			mRunDir.set(mDelayedRequestRunDir.value);
			mDelayedRequestRunDir.isValid = false;

			//if (mRunDirChangeUnderSampleLeft == 0)
					mRunDirChangeUnderSampleLeft = footballer.run.RunDirUnderSampleCount;
			
			needUpdateRunState = true;
		}
	
		onHandledRequests(footballer, needUpdateRunState, needUpdateAnimSpeed);
	}
	
	function handleRequests(footballer, t) {

		if (mRequestBallShot.isValid) {
		
			mBallShot.set(mRequestBallShot.value);
			mRequestBallShot.isValid = false;
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
			
		} else {

			/*	
			if (mRunType == footballer.run.Run_None)
				updateRunState(footballer);
			*/	
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
			
			/*
			footballer.ballHandler.reset(footballer, false);
			*/
		}
	}
	
	function onBrokenBallAction(footballer, gameObject) {
	
		mIsEasyLockDribbling = false;
	}
	
	function onBallLostAfterTouch(footballer) {
	
		mIsEasyLockDribbling = false;
	}
	
	/*
	function doBallControlOrShoot(footballer, t) {

		if (mBallShot.isValid) {
		
			doBallShot(footballer, null, false);
		
			return Dribble_WaitingToLooseOwnership;
		} 
		
		if (footballer.impl.isBallSharedOwner()) {
		
			local idealBallActionOffset = footballer.settings.getIdealBallActionOffset(footballer);
			doBallShot(footballer, idealBallActionOffset, false);
		
			return Dribble_WaitingToLooseOwnership;
		} 
	
		{
	
			local testVector = footballer.settings.getIdealBallActionPoint(footballer);
			testVector.subtractFromSelf(footballer.impl.getPos());
		}
		
		footballer.ballHandler.setActionControl(footballer, true, footballer.settings.getIdealBallActionPoint(footballer), 0.0);
		//footballer.ballHandler.setActionSound(footballer.settings.BallControlSoundName, footballer.settings.BallControlSoundMag);
		
		return mDribbleState;
	}
	
	function doBallShot(footballer, idealBallActionOffset, useControlSound) {
		
		local ballHandler = footballer.ballHandler;
		local impl = footballer.impl;
					
		local shotVelocity = Vector3(); 
		local ballShot;
		local shotSwitchControlValue;
		
		if (mBallShot.isValid) {
		
			ballShot = mBallShot;
		
			ballShot.getVelocity(footballer, impl.getFacing(), shotVelocity);
			shotSwitchControlValue = footballer.settings.BallShotSwitchInfluence;
			
			if (footballer.run.EnableEasyLockDribble)
				mIsEasyLockDribbling = false;
		
		} else {
		
			local settings = footballer.run;
			local correctionDist = impl.calcBallCorrectionDist(idealBallActionOffset, impl.getFacing(), impl.getRelativeBallPos());
			ballShot = settings.Shots[settings.Shot_Dribble];
			shotSwitchControlValue = settings.DribbleShotSwitchInfluence;
		
			ballShot.getAdjustedVelocity(footballer, impl.getFacing(), settings.getSpeedValue(mRunType, mRunSpeedType), shotVelocity);
			
			impl.correctBallShotVelocity(correctionDist, shotVelocity);
			shotVelocity.normalizeTo(mLastShotDir);
			
			if (footballer.run.EnableStrafeCorrectDribble) 
				mStrafeDribbleCorrection = correctionDist;
			
			if (footballer.run.EnableEasyLockDribble)
				mIsEasyLockDribbling = true;
		}
		
		if (mIsEasyLockDribbling)
			footballer.ballHandler.setBrokenActionListener(this);
			
		ballHandler.setActionShoot(footballer, true, shotVelocity, ballShot, shotSwitchControlValue);
		
		if (useControlSound) {
		
			ballHandler.setActionSound(footballer.settings.BallControlSoundName, footballer.settings.BallControlSoundMag);		
		
		} else {
		
			ballHandler.setActionSound(footballer.settings.BallShotSoundName, shotVelocity.mag());		
		}
	}
	
	function handleBallHelperShootIfNeeded(footballer, t, impl, posDiff, runDirChanged) {
	
		local immediateShot = mBallShot.isValid && footballer.run.ImmediateShots;
				
		local idealBallActionOffset = footballer.settings.getIdealBallActionOffset(footballer);
		local idealBallDir = (footballer.ballHandler.getShotCountWhileOwner() > 0) ? mLastShotDir : impl.getFacing();
			
		if (immediateShot || impl.needsShootBall(idealBallActionOffset, idealBallDir, posDiff, runDirChanged, footballer.settings.BallPositionEpsilon)) {
		
			if (footballer.run.EnableEasyLockDribble) {
			
				if (mDelayedRequestBodyFacing.isValid || mDelayedRequestRunDir.isValid) {
				
					return Dribble_Controlling;
				} 
			}
		
			doBallShot(footballer, idealBallActionOffset, footballer.ballHandler.getShotCountWhileOwner() != 0);
			return Dribble_WaitingToLooseOwnership;
		} 
		
		
		footballer.ballHandler.setActionPrepareForShoot(footballer);
		
		return mDribbleState;
	}
	
	function handleBall(footballer, t, dt, posDiff, runDirChanged) {
	
		local impl = footballer.impl;
	
		if (footballer.run.isStrafing(mRunType)) {
	
			mIsEasyLockDribbling = false;
	
			if (impl.isBallOwner()) {
			
				local repeat = true;
	
				while (repeat) {
	
					repeat = false;
	
					switch (mDribbleState) {
					
						case Dribble_Strafing: {
						
							if (footballer.run.EnableEasyLockDribble) {
			
								if (mDelayedRequestBodyFacing.isValid || mDelayedRequestRunDir.isValid) {
				
									delayedRunDirChanged = true;
									handleDelayedRequests(footballer, t);
								}
							}
												
							if (mBallShot.isValid && !footballer.ballHandler.isStunned()) {
							
								local idealBallActionOffset = footballer.settings.getIdealBallActionOffset(footballer);
								doBallShot(footballer, idealBallActionOffset, false); //for now facing is used for direction ... can be changed!
							
								mDribbleState = Dribble_StrafingWaitingToLooseOwnership;
							
							} else {
		
								mDribbleState = doBallControlOrShoot(footballer, t);
							}
								
							
						} break;
						
						case Dribble_StrafingWaitingToLooseOwnership: {
						
							footballer.ballHandler.setActionPrepareForShoot(footballer);
						
						} break;
					
						default: {
						
							repeat = true;
							mDribbleState = Dribble_Strafing;
						} break;
					}
				}
				
			} else {
			
				//footballer.ballHandler.setBrokenActionListener(null);
				mDribbleState = Dribble_WaitingForOwnership;
			}
			
		} else {
		
			if (impl.isBallOwner()) {
	
				local repeat = true;
				
				while (repeat) {
				
					repeat = false;
					
					switch (mDribbleState) {
					
						case Dribble_PreparingForShot: {
						
							if (runDirChanged) {
							
								mDribbleState = Dribble_Controlling;
								repeat = true;
							
							} else {

								if (!footballer.ballHandler.isStunned()) {
						
									mDribbleState = handleBallHelperShootIfNeeded(footballer, t, impl, posDiff, true);
									
									if (mDribbleState == Dribble_Controlling) {
									
										repeat = true;
									}
								} 
							}
						
						} break;
						
						case Dribble_Controlling: {
						
							local delayedRunDirChanged = false;
						
							if (footballer.run.EnableEasyLockDribble) {
			
								if (mDelayedRequestBodyFacing.isValid || mDelayedRequestRunDir.isValid) {
				
									delayedRunDirChanged = true;
									handleDelayedRequests(footballer, t);
								}
							}
						
							if (runDirChanged || delayedRunDirChanged) {
							
								mDribbleState = doBallControlOrShoot(footballer, t);
							
							} else {
							
								if (!footballer.ballHandler.isStunned()) {
							
									mDribbleState = handleBallHelperShootIfNeeded(footballer, t, impl, posDiff, true);
								}
							}
						
						} break;
						
						case Dribble_WaitingToLooseOwnership: {
						
							if (runDirChanged && !footballer.run.StrictDribbleControl) {
							
								repeat = true;
								mDribbleState = Dribble_Controlling;
							
							} else {
												
								footballer.ballHandler.setActionPrepareForShoot(footballer);
							}
							
						} break;
					
						case Dribble_Strafing: {
						
							repeat = true;
							
							runDirChanged = true;
							mDribbleState = Dribble_Controlling;
						
						} break;
					
						default: {
						
							repeat = true;
						
							if (runDirChanged) {
							
								mDribbleState = Dribble_Controlling;
								
							} else {
							
								mDribbleState = Dribble_PreparingForShot;
							}
						}
					}
				}
								
			} else {
			
				if (mDribbleState != Dribble_WaitingForOwnership) {
			
					//footballer.ballHandler.setBrokenActionListener(null);		
					footballer.ballHandler.reset(footballer, false);
					mDribbleState = Dribble_WaitingForOwnership;
				}
			}
		}
	}
	*/
	
	function frameMove(footballer, t, dt) {

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
			
				if (runDirChanged /*|| footballer.ballHandler.getBallWasBlocked()*/) {
				
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
	}

	mHasRequests = false;
	
	mRequestBodyFacing = null;
	mRequestRunDir = null;
	mRequestRunSpeedType = null;
	mRequestIsStrafing = null;
	mRequestBallShot = null;
	
	//used when mIsEasyLockDribbling is true
	mDelayedRequestBodyFacing = null;
	mDelayedRequestRunDir = null;
	
	mRunDir = null;
	mRunSpeedType = null;
	mRunType = null;
	mIsStrafing = null;
	mBallShot = null;
	mLastShotDir = null;
	
	mRunDirChangeUnderSampleLeft = 0;
	mStrafeDribbleCorrection = 0.0;
	mIsEasyLockDribbling = false;
	
	static Dribble_None = -1;
	static Dribble_WaitingForOwnership = 0;
	static Dribble_Strafing = 1;
	static Dribble_PreparingForShot = 2;
	static Dribble_Controlling = 3;
	static Dribble_WaitingToLooseOwnership = 4;
	static Dribble_StrafingWaitingToLooseOwnership = 5;
	
	mDribbleState = -1;
};