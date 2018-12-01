
class BodyActionShooting_StartState {

	copyBallShot = true;
	ballShot = null;
}

class BodyAction_Shooting extends BodyActionBase {

	function getType(footballer) {
	
		return footballer.Action_Shooting;
	}

	constructor() {
	
		mBallShotDir = Vector3();
	}

	function load(footballer, chunk, conv) {
	
		return true;
	}
	
	function isFinished() { return mIsFinished; }	
	
	function setBallShotDirection(footballer, dir) {
	
		mBallShotDir.set(dir);
	}
	
	function applyStartState(footballer, startState) {
	
		if (startState != null) {
			
			if (startState.copyBallShot) {
			
				mBallShot = FootballerBallShot(false, 0.0, 0.0, 0.0);
				mBallShot.set(startState.ballShot);
				
			} else {
			
				mBallShot = startState.ballShot;
			}
			
		} else {
		
			mBallShot = footballer.settings.Shots[footballer.settings.DefaultShotLevel][footballer.settings.Shot_High];
		}
		
		mBallShotDir.zero();
	}
	
	function start(footballer, t, startState) {
		
		applyStartState(footballer, startState);
		
		mShootState = Shoot_PreShot;
		mIsFinished = false;
		
		local impl = footballer.impl;
		local settings = footballer.shoot;
		
		impl.setAnim(settings.AnimName, true);
		impl.setAnimLoop(false);
		impl.setAnimSpeed(settings.AnimSpeed);
		
		impl.playSound(settings.SoundName, impl.getPos(), settings.SoundMag);
		
		mActionStartTime = t;
		
		return true;
	}
	
	function stop(footballer, t) {
	
		mIsFinished = true;
	}
	
	function handleActionEnd(footballer) {
	
		if (footballer.impl.isAnimFinished()) {
		
			mIsFinished = true;
		}
	}
	
	/*
	function doBallShot(footballer) {
		
		local ballHandler = footballer.ballHandler;
					
		local shotVelocity = Vector3(); 
				
		local shotDir;
		local facingDir = footballer.impl.getFacing();
				
		if (mBallShotDir.isZero()) {
		
			shotDir = facingDir;
		
		} else {
		
			if (mBallShotDir.dot(facingDir) > 0.0) {
			
				shotDir = mBallShotDir;
			
			} else {

				shotDir = facingDir;
			}
		}
				
		mBallShot.getVelocity(footballer, shotDir, shotVelocity);
		
		ballHandler.setActionShoot(footballer, true, shotVelocity, mBallShot, footballer.settings.BallShotSwitchInfluence);
		ballHandler.setActionSound(footballer.settings.BallShotSoundName, shotVelocity.mag());		
	}
	
	
	function handleBall(footballer, t, dt, actionTime) {
	
		if (footballer.impl.isBallOwner()) {
		
			switch (mShootState) {
			
				case Shoot_PreShot: {
				
					if (actionTime >= footballer.shoot.ShootPhase_StartTime) {
				
						doBallShot(footballer);
						mShootState = Shoot_PostShot;
						
					} else {
					
						footballer.ballHandler.setActionPrepareForShoot(footballer);
					}
				
				} break;
			
				case Shoot_PostShot: {
				
					footballer.ballHandler.setActionPrepareForShoot(footballer);
				
				} break;
				
				case Shoot_WaitingForOwnership: {
				} break;
			}
			
		} else {
		
			mShootState = Shoot_WaitingForOwnership;
		}
	}
	*/
	
	function frameMove(footballer, t, dt) {
			
		if (mIsFinished) {
			
			switchIdle(footballer, t, null);
			return;
		}	
			
		local impl = footballer.impl;	
				
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		local actionTime = t - mActionStartTime;
		
		{
			if (impl.nodeIsDirty()) {

				impl.collUpdate();
				footballer.resolveCollisions(footballer, t, false);
			}
		}
		
		//handleBall(footballer, t, dt, actionTime);
		
		handleActionEnd(footballer);
	}
	
	
	mBallShot = null;
	mBallShotDir = null;
	
	mIsFinished = true;
	mActionStartTime = 0.0;
	
	mShootState = -1;
	
	static Shoot_None = -1;
	static Shoot_WaitingForOwnership = 0;
	static Shoot_PreShot = 0;
	static Shoot_PostShot = 1;
}