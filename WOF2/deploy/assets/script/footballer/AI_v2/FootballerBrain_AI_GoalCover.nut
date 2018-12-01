class FootballerBrain_AI_GoalCover extends FootballerBrain_AI_Goal {
	
	constructor() {
	
		mCoveredPoint = Vector3();
		mOpponentWatchMomentum = Vector3();
		
		mCoverEstimate = CCoverEstimate2D();
		mCoverEstimate.holdIdealPos3D(true);
		
		mGoalGoto = FootballerBrain_AI_GoalGoto();
		mGoalGoto.setToleranceMode(mGoalGoto.Tolerance_JustBefore);
		mGoalGoto.setStrafe(false);
		
		mGoalFace = FootballerBrain_AI_GoalFace();
		
		mOpponentWatchReflexTime = 0.0;
	}
	
	function setCovedredPoint(point) {
	
		mCoveredPoint.set(point);
	}
	
	function setOpponent(footballer) {
	
		mOpponent = footballer;
		mOpponentWatchUpdateTime = 0.0;
	}
	
	function setRegion(region) {
	
		mRegion = region;
	}
	
	function setReflexTime(time) {
	
		mOpponentWatchReflexTime = time;
	}
	
	function start(brain, footballer, t) {
	
		{
			local match = footballer.impl.getMatch();
		
			mCoverEstimate.blockOffset = match.conv(1.5);
			mCoverEstimate.toleranceDist = match.conv(1.0);
			mCoverEstimate.frameInterval = match.getFrameInterval();
			
			mGoalGoto.setEpsilon(mCoverEstimate.toleranceDist);
		}
	
		mIsInSlowWalkMode = false;
		mGoalGoto.stop();
		mGoalFace.stop();
		
		return doReturn(t, Result_Processing);
	}
	
	function frameMove(brain, footballer, t, dt) { 
	
		local doMove = false;
		local isThinking;
		
		if (mOpponentWatchReflexTime != 0.0) {
		
			local momentum = mOpponent.getBodyMomentum();
			
			if (t - mOpponentWatchUpdateTime > mOpponentWatchReflexTime) {
			
				isThinking = false;
			
			} else {
			
				local change = momentum.dot(mOpponentWatchMomentum);
				
				if (change == 0.0) {
				
					isThinking = mOpponent.hasBodyMomentum() || !mOpponentWatchMomentum.isZero();
				
				} else {
				
					isThinking = change < 0.0;
				}
			}
			
		} else {
		
			isThinking = false;
		}
		
		if (!isThinking) {
			
			mOpponentWatchUpdateTime = t;
			mOpponentWatchMomentum.set( mOpponent.getBodyMomentum());
		}
		
		if (isThinking) {
		
			doMove = false;
		
		} else {
		
			local runSettings = footballer.settings.run;
			local maxRunSpeed = runSettings.getSpeedValue(runSettings.Run_Normal, runSettings.RunSpeed_Normal);
			
			if (updateIdealPos2D(brain, footballer, t, dt, mOpponent.impl.getPos(), mOpponentWatchMomentum, maxRunSpeed)) {
				
				doMove = true;
				
				mGoalGoto.setTarget(footballer, mCoverEstimate.idealPos3D(), true);
											
			} else {
			
				doMove = false;
			}
		}
	
		if (doMove) {
		
			local result = mGoalGoto.update(brain, footballer, t, dt, null, null);
		
			if (result == Result_Processing) {
			
			} else {
			
				doMove = false;
			
				if (result == Result_Success) {
					
					if (enterSlowWalkMode(brain, footballer, t)) {
					
						doMove = true;
					}
				}	
			} 
		}
		
		if (!doMove) {
		
			brain.body_idle(footballer, t);
		}
		
		if (brain.body_isIdle(footballer)) {
		
			if (!mGoalFace.isActive(t, dt)) {
			
				mGoalFace.setTargetByPos(footballer, mOpponent.impl.getPos());
			}
		
			mGoalFace.update(brain, footballer, t, dt, null, null);
			
		} else {
		
			brain.body_updateSetIsStrafing(footballer, t, false);
		}
							
		return doReturn(t, Result_Processing); 
	}
	
	function enterSlowWalkMode(brain, footballer, t) {
	
		if (mOpponentWatchMomentum.isZero()) {
		
			return false;
		}
		
		local minDot = 0.5 * mOpponentWatchMomentum.mag();
							
		if (mOpponentWatchMomentum.dot(footballer.impl.getFacing()) > minDot) {
		
			brain.body_updateSetIsStrafing(footballer, t, true);
		
			return true;
		}
		
		return false;
	}
	
	function updateIdealPos2D(brain, footballer, t, dt, attackerPos, attackerVel, maxRunSpeed) { 
	
		mCoverEstimate.estimate3DWithRegion(footballer.impl.getMatch(), mRegion, attackerPos, attackerVel, mCoveredPoint, getZeroVector3(), 
											footballer.impl.getPos(), maxRunSpeed, mCoverEstimate.idealPos3D(), footballer.settings.PitchHeight);

		return true;										
	}
	
	mOpponent = null;
	mCoveredPoint = null;
	mRegion = null;
	
	mOpponentWatchReflexTime = 0.0;
	mOpponentWatchUpdateTime = 0.0;
	mOpponentWatchMomentum = null;
	
	mCoverEstimate = null;
	
	mGoalGoto = null;
	mGoalFace = null;
	
	mIsInSlowWalkMode = -1;
}