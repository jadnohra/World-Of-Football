

class FootballerBallHandler {

	static Action_None = -1;
	static Action_Control = 0;
	static Action_Shoot = 1;
	static Action_PrepareForShoot = 2;

	static Stage_None = -1;
	static Stage_Handling = 0;
	static Stage_Handling2 = 1;
	static Stage_Handled = 2;
	static Stage_ActiveBroken = 3;
	static Stage_PassiveBroken = 4;
	static Stage_WaitingForOwnership = 5;
	
	constructor() {
	
		mAction = Action_None;
		mStage = Stage_None;
	
		mControlTargetPoint = Vector3();
		mShootVelocity = Vector3();
	
		mBrokenActionListener = null;
		mBallWasBlocked = false;
	}
	
	function load(footballer, chunk, conv) {
	
		return true;
	}

	function onLostBall(footballer) {}
	function onAcquiredBall(footballer, isSolo) {}
		
	function onBallSelfCollision(footballer, ball, boneID) {}	
	function onBallFootballerCollision(footballer, ball, collider, boneID) {}	
	function onBallObstacleCollision(footballer, ball, object) {}	
	function onBallPitchCollision(footballer, ball) {}	
		
	function onBallCommandFeedback(footballer, commandID, accepted) {}	
	function onBallCommand(footballer, command) {}	
		
	function onBallAcquiredAfterTouch(footballer) {}
	function onBallLostAfterTouch(footballer) {}	
	
	function reset(footballer, resetStun) {
	
		mAction = Action_None;
		
		if (resetStun) {
		
			mActionStunTimeLeft = 0.0;
		}
	}
	
	function isActionShoot() {
	
		return mAction == Action_Shoot;
	}
	
	function isActionShootActive() {
	
		return mAction == Action_Shoot && mStage != Stage_Broken;
	}
	
	function isActionShootOrPrepareActive() {
	
		return ((mAction == Action_Shoot) || (mAction == Action_PrepareForShoot)) && mStage != Stage_Broken;
	}
	
	function getShotWasExecuted() {
	
		return mShotWasExecuted;
	}
	
	function isActionFinished() {
	
		return mAction == Action_Handled;
	}
	
	function isActionBroken() {
	
		return mAction == Action_Broken;
	}
	
	function isActionInProgress() {
	
		return mAction == Action_Handling;
	}
	
	function getActionCountWhileOwner() {
	
		return mShotCountWhileOwner + mControlCountWhileOwner;
	}
	
	function getShotCountWhileOwner() {
	
		return mShotCountWhileOwner;
	}
	
	function getControlCountWhileOwner() {
	
		return mControlCountWhileOwner;
	}
	
	function getBallWasBlocked() {
	
		return mBallWasBlocked;
	}
	
	function resetStunned() {
	
		mActionStunTimeLeft = 0.0;
	}
	
	function isStunned() {
	
		return (mActionStunTimeLeft > 0.0);
	}
	
	function setActionSound(soundName, soundMag) {
	
		mHasSound = true;
		mSoundName = soundName;
		mSoundMag = soundMag;
	}
	
	/*
		will handle ball transparency
	*/
	function setActionPrepareForShoot(footballer) {
	
		mBallWasBlocked = false;
	
		mAction = Action_PrepareForShoot;
		mStage = Stage_Handling;
	}
	
	function setActionShoot(footballer, failIfStunned, velocity, shot, shotSwitchControlValue) {
	
		if (failIfStunned && mActionStunTimeLeft > 0.0) {
		
			return false;
		}
	
		mBallWasBlocked = false;
	
		mHasSound = false;
	
		mAction = Action_Shoot;
		mStage = Stage_Handling;
		mShotWasExecuted = false;
	
		mShootVelocity.set(velocity);
		mShootRotSyncRatio = shot.rotSyncRatio;
		mShootEnableAfterTouch = shot.enableAfterTouch;
		
		if (footballer.settings.EnableShotSwitchControlValues)
			mShootSwitchControlValue = shotSwitchControlValue;
		else
			mShootSwitchControlValue = 0.0;
				
		return true;
	}
	
	
	function setActionControl(footballer, failIfStunned, targetPoint, controlInterval) {
	
		if (failIfStunned && mActionStunTimeLeft > 0.0)
			return false;
		
		mBallWasBlocked = false;
	
		mHasSound = false;
	
		mAction = Action_Control;
		mStage = Stage_Handling;

		mControlTargetPoint.set(targetPoint);
		mControlControlInterval = controlInterval;
		mTimeLeft = mControlControlInterval;
		
		return true;
	}	
	
	function frameMove(footballer, t, dt) {
	
		switch (mAction) {
		
			case Action_Control: frameMove_Control(footballer, t, dt); break;
			case Action_Shoot: frameMove_Shoot(footballer, t, dt); break;
			case Action_PrepareForShoot: frameMove_PrepareForShoot(footballer, t, dt); break;
		}
		
		if (!footballer.impl.isBallOwner()) {
		
			mShotCountWhileOwner = 0;
			mControlCountWhileOwner = 0;
		}
		
		if (mActionStunTimeLeft > 0.0) {
		
			mActionStunTimeLeft -= dt;
		}
	}
	
	function onBallPassiveCollision(footballer, gameObject) {
	
		onBallAction(footballer, gameObject);
	}
	
	function onBallControlCommand(footballer, gameObject) {
	
		//not really needed
		//onBallAction(footballer, gameObject);
	}
	
	function setBrokenActionListener(listener) {
	
		mBrokenActionListener = listener;
	}
	
	function onBallControlCommandBroken(footballer, t) {
	
		if (mStage == Stage_Handling || mStage == Stage_Handled) {
		
			footballer.impl.setBallTransparent(false);
			mStage = Stage_ActiveBroken;
			
			footballer.giveUpBallAfterTouch();
			
			mActionStunTimeLeft = footballer.AttributeBallConflictLossStunTime;
			mBallWasBlocked = false;
			
			if (mBrokenActionListener != null) {
				
				mBrokenActionListener.onBrokenBallAction(footballer, null);
			}
		}
	}
	
	function onBallAction(footballer, gameObject) {
	
		if (!footballer.impl.isSelf(gameObject)) {
			
			if (!gameObject.isPitch()) {
			
				if (mStage == Stage_Handling || mStage == Stage_Handled) {
				
					footballer.impl.setBallTransparent(false);
					mStage = gameObject.isFootballer() ? Stage_ActiveBroken : Stage_PassiveBroken;
					
					mBallWasBlocked = false;
				}
				
				if (mBrokenActionListener != null) {
					
					mBrokenActionListener.onBrokenBallAction(footballer, gameObject);
				}
			}
			
			footballer.giveUpBallAfterTouch();
		}
	}
	
	function frameMove_PrepareForShoot(footballer, t, dt) {
	
		if (footballer.impl.isBallOwner()) {
	
			if (mStage == Stage_WaitingForOwnership) {
				
				mStage = Stage_Handling;
			}
				
		} else {
			
			mStage = Stage_WaitingForOwnership;
		}
			
		
		if (mStage == Stage_Handling) {
			
			footballer.impl.setBallTransparent(true);
		} 
	}
	
	function frameMove_Shoot(footballer, t, dt) {
	
		if (footballer.impl.isBallOwner()) {
	
			if (mStage == Stage_WaitingForOwnership) {
				
				mStage = Stage_Handling;
				mShotWasExecuted = false;
			}
				
		} else {
			
			mStage = Stage_WaitingForOwnership;
		}
			
		
		if (mStage == Stage_Handling) {
		
			footballer.impl.setBallTransparent(true);
		
			if (!mShotWasExecuted) {
			
				local impl = footballer.impl;
				mShotWasExecuted = true;
			
				impl.shootBall(mCommandID, mShootVelocity, mShootRotSyncRatio, 0.0, mShootSwitchControlValue, mShootEnableAfterTouch);
				mShotCountWhileOwner += 1;
				
				if (mHasSound) {
				
					impl.playSound(mSoundName, impl.getPos(), mSoundMag);
				}
			}
		} 
	}
	
	function frameMove_Control(footballer, t, dt) {
	
		if (footballer.impl.isBallOwner()) {
	
			if (mStage == Stage_WaitingForOwnership) {
			
				mStage = Stage_Handling;
				mTimeLeft = mControlControlInterval;
			}
			
		} else {
		
			mStage = Stage_WaitingForOwnership;
		}
		
		
		if (mStage == Stage_Handling) {
		
			local impl = footballer.impl;
		
			impl.setBallTransparent(true);
		
			if (mHasSound && mTimeLeft == mControlControlInterval) {
				
				impl.playSound(mSoundName, impl.getPos(), mSoundMag);
			}
		
			impl.controlBall(mCommandID, mControlTargetPoint, mControlControlInterval, mTimeLeft, true);
			mTimeLeft -= dt;
					
			if (mTimeLeft <= 0.0) {
					
				mStage = Stage_Handled;
				mControlCountWhileOwner += 1;
			}
		}
	}
	

	mAction = -1;
	mStage = -1;

	mActionSource = null;
	mCommandID = 0;
	
	mControlTargetPoint = null;
	mControlControlInterval = 0.0;
		
	mShootVelocity = null;
	mShootRotSyncRatio = 0.0;
	mShootEnableAfterTouch = false;
	mShotWasExecuted = false;
	mShootSwitchControlValue = 0.0;
	
	mShotCountWhileOwner = 0;
	mControlCountWhileOwner = 0;
	
	mHasSound = false;
	mSoundName = null;
	mSoundMag = 0.0;
	
	mTimeLeft = 0.0;
	
	mBrokenActionListener = null;
	mBallWasBlocked = false;
	mActionStunTimeLeft = 0.0;
}
