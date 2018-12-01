class FootballerBrain_AI_GoalEstimateShot extends FootballerBrain_AI_Goal {

	constructor() {
	
		mTarget = Vector3();
	}

	function setupByTime(startState, target, time, shotType, allowWait, allowUnpreciseShot) {
	
		mShotStartState = startState;
		
		mTarget.set(target);
		mTime = time;
		mSpeed = -1.0;
		
		mShotType = shotType;
		
		mAllowWait = allowWait;
		mAllowUnpreciseShot = allowUnpreciseShot;
	}
	
	function setupBySpeed(startState, target, speed, shotType, allowUnpreciseShot) {
	
		mShotStartState = startState;
		
		mTarget.set(target);
		mSpeed = speed;
		mTime = -1.0;
		
		mShotType = shotType;
		
		mAllowWait = false;
		mAllowUnpreciseShot = allowUnpreciseShot;
	}
	
	function start(brain, footballer, t) { 
	
		mIsWaiting = false;
		
		return doReturn(t, Result_Processing);
	}
		
	function frameMove(brain, footballer, t, dt) { 
	
		if (mIsWaiting) {
		
			if (t >= mNextWaitUpdateTime) {
			
				mNextWaitUpdateTime = t + mWaitDelay;
				mIsWaiting = false;
							
			} else {
			
				brain.body_idle(footballer, t);
				return doReturn(t, Result_Processing);
			}
		}
	
		local match = footballer.impl.getMatch();
		local from = match.getBall().getPos();

		local estimator = match.getBallShotEstimator();
		
		local dir = Vector3();
		mTarget.subtract(from, dir);
		dir.normalize();
		
		footballer.helperFixDir(dir);
		
		if (!dir.isZero()) {
		
			local outSpeed = Float(0.0);
			local result = CBallShotEstimatorResult();
			local settings = footballer.settings;
			local estimateSuccess;
			
			if (mTime > 0.0) {
			
				estimateSuccess = estimator.estimateNeededShotSpeedByTime(settings.ShotNames[mShotType], dir, from, mTarget, match.conv(0.1), mTime - t, 0.1, result);
			
			} else {
			
				estimateSuccess = estimator.estimateNeededShotSpeedBySpeed(settings.ShotNames[mShotType], dir, from, mTarget, match.conv(0.1), mSpeed, match.conv(0.1), result);
			}
			
			if (estimateSuccess) {
				
				if (!result.isConstraintSatisfied) {
				
					if (mAllowWait) {
					
						brain.body_idle(footballer, t);
					
						//print("waiting: max" + result.minWaitTime + "," + mWaitDelay);
						mIsWaiting = true;
						mNextWaitUpdateTime = t + maxf(result.minWaitTime, mWaitDelay);
						return doReturn(t, Result_Processing);
					
					} else {
					
						if (!mAllowUnpreciseShot)
							return doReturn(t, Result_Failure);
					}
				}
				
				//print("estim speed: " + result.speed + " timing ok: " + result.isConstraintSatisfied);
				
				local shot = settings.getShot(settings.DefaultShotLevel, mShotType);
				//mShotStartState.setShot(mShotType, shot, result.speed / shot.speed);
				
				mShotStartState.isHeadShot = footballer.ballIsHeaderHeight();
				mShotStartState.copyBallShot = false;
				mShotStartState.ballShotType = mShotType;
				mShotStartState.ballShot = shot;
				mShotStartState.shotSpeedScale = result.speed / shot.speed;
				
				return doReturn(t, Result_Success);
			} 
		}
		
		//print("estim fail");
	
		return doReturn(t, Result_Failure);
	}
	
	mShotStartState = null;
	mTarget = null;
	mTime = 0.0;
	mSpeed = 0.0;
	mShotType = -1;
	
	mAllowWait = false;
	static mWaitDelay = 0.07;
	mIsWaiting = false;
	mNextWaitUpdateTime = 0.0;
	
	mAllowUnpreciseShot = false;
}