class FootballerBrain_AI_GoalGoto extends FootballerBrain_AI_Goal {

	static State_None = -1;
	static State_Starting = 0;
	static State_Moving = 1;
	
	static Tolerance_None = -1;
	static Tolerance_Dist = 0; //stops when within distance
	static Tolerance_JustBefore = 1; //stops at last step before passing goal
	static Tolerance_JustAfter = 2; //stops at 1st step after passing goal
	
	
	constructor() {
	
		mMoveDir = Vector3();
		mTargetPos = Vector3();
	}
	
	function getTarget() { return mTargetPos; }
		
	function setEpsilon(epsilon) {
	
		mEpsilonSq = epsilon * epsilon;
	}	
	
	function setToleranceMode(toleranceMode) {
	
		mToleranceMode = toleranceMode;
		
		if (mToleranceMode > Tolerance_Dist) {
		
			mTempVec3 = Vector3();
		} 
	}
	
	//set pos as NULL if it was set using getTarget()
	function setTarget(footballer, pos, isDynamic) {
	
		if (pos != null) {
	
			if (isDynamic && !mTargetPos.equals(pos))
				mMoveDirSlowUpdateLastTime = 0.0;
		
			mTargetPos.set(pos);
			
		} else {
		
			if (isDynamic)
				mMoveDirSlowUpdateLastTime = 0.0;
		}
		
		mTargetPos.setEl(::Scene_Up, footballer.settings.PitchHeight);
	}
	
	function setTargetWithDirectedOffset(footballer, pos, t, offsetDistance, isDynamic) {
	
		if (offsetDistance != 0.0) {
		
			local offset = Vector3();
		
			updateMoveDir(footballer.impl.getMatch(), pos, mMoveDir, t);
			mMoveDir.mul(offsetDistance, offset);
			
			setTargetWithOffset(footballer, pos, offset, isDynamic);
			
		} else {
		
			setTarget(footballer, pos, isDynamic);
		}
	}
	
	function setTargetWithOffset(footballer, pos, offset, isDynamic) {
	
		mTargetPos.set(pos);
		mTargetPos.addToSelf(offset);
		mTargetPos.setEl(::Scene_Up, footballer.settings.PitchHeight);
		
		if (isDynamic)
			mMoveDirSlowUpdateLastTime = 0.0;
	}
	
	function setStrafe(strafe) {
	
		mStrafe = strafe;
	}
	
	function start(brain, footballer, t) {
	
		mState  = State_Starting;
		mMoveDirSlowUpdateLastTime = 0.0;
		
		return doReturn(t, Result_Processing);
	}
	
	function updateMoveDir(footballer, pos, dir, t) {
	
		mMoveDirSlowUpdateLastTime = t;
	
		mTargetPos.subtract(pos, dir);
		footballer.helperFixDir(dir);
	}
	
	function slowUpdateMoveDir(footballer, pos, dir, t) {
	
		if (t - mMoveDirSlowUpdateLastTime > 0.35) {
		
			updateMoveDir(footballer, pos, dir, t);
		}
	}
	
	function requestBodyRun(footballer, t, pos) {
	
		if (mRunStartState == null) {
		
			mRunStartState = BodyActionRunning_StartState();
			
			if (!mStrafe)
				mRunStartState.bodyFacing = mMoveDir;
				
			mRunStartState.runDir = mMoveDir;
			mRunStartState.runSpeedType = footballer.settings.run.RunSpeed_Normal;
			
			local actionRunning = footballer.getActionRunning();
			
			actionRunning.setIsStrafing(mStrafe);
		}

		return footballer.getAction().switchRunning(footballer, t, mRunStartState);
	}
	
	function updateBodyRun(brain, footballer, t, actionRunning) {
	
		local impl = footballer.impl;
		local collState = impl.getCollState();

		switch (collState) {
		
			case impl.Coll_Constrained: {
			
				mMoveDirSlowUpdateLastTime = t;
			
				if (mTimeToIdleConstrained < 0.0)
					mTimeToIdleConstrained = t + randf(0.0, 1.5);
			
				if (t > mTimeToIdleConstrained) {
				
					brain.body_idle(footballer, t);
					mTimeToIdleConstrained = -1.0;
				}
			
			} break;
			
			case impl.Coll_Blocked: {
			
				//ideally needs vision / navigation / A*
				
				mMoveDirSlowUpdateLastTime = t;
				
				mMoveDir.randomizeToSelf(1.0, 1.0, 0.0, 0.0, 0.1, 1.0, true);
				footballer.helperFixDir(mMoveDir);
			
			} break;
		}

		if (mStrafe) 
			brain.body_updateStrafeRunDir(footballer, t, mMoveDir);
		else
			brain.body_updateStraightRunDir(footballer, t, mMoveDir);
			
		return true;
	}
	
	function isWithinTolerance(footballer, pos, distanceSq, t, dt) {
	
		//if (mEpsilonSq > 0.0) print(distanceSq + " / " + mEpsilonSq);
	
		local isWithinSimpleTolerance = (distanceSq <= mEpsilonSq);
	
		if (!isWithinSimpleTolerance)
			return false;
	
		switch (mToleranceMode) {
		
			case Tolerance_JustBefore: {

				updateMoveDir(footballer, pos, mMoveDir, t);			
		
				pos.subtract(mTargetPos, mTempVec3);
				
				if (mTempVec3.dot(mMoveDir) < 0.0) {
				
					pos.addMultiplication(mMoveDir, dt, mTempVec3);
					mTempVec3.subtractFromSelf(mTargetPos);
					
					if (mTempVec3.dot(mMoveDir) > 0.0) 
						return false;
					
					return true;
					
				} else {
				
					return true;
				}
			
			} break;
			
			case Tolerance_JustAfter: {
			
				updateMoveDir(footballer, pos, mMoveDir, t);
			
				pos.subtract(mTargetPos, mTempVec3);
			
				if (mTempVec3.dot(mMoveDir) > 0.0) {
				
					pos.addMultiplication(mMoveDir, dt, mTempVec3);
					local newDistSq = distSq(mTempVec3, mTargetPos);
					
					return (newDistSq > mEpsilonSq);
					
				} else {
				
					return false;
				}
			
			} break;
		}
			
		return true;
	}
	
	function frameMove(brain, footballer, t, dt) { 
	
		local pos = footballer.impl.getPos();
		local distSq = distSq(pos, mTargetPos);
	
		if (isWithinTolerance(footballer, pos, distSq, t, dt)) {
		
			mState = Result_Success;
			return doReturn(t, mState);
		} 
		
		local repeat = true;
	
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_Starting: {
								
					mTimeToIdleConstrained = -1.0;			
								
					if (mEpsilonSq == 0.0) {
					
						local runSettings = footballer.settings.run;
						mEpsilonSq *= runSettings.getMoveEpsilonSq(runSettings.Run_Normal, runSettings.RunSpeed_Normal);
						
						if (isWithinTolerance(footballer, pos, distSq, t, dt)) {
		
							mState = Result_Success;
							return doReturn(t, mState);
						} 
					}
					
					if (footballer.getActionType() == footballer.Action_Running) {
					
						updateMoveDir(footballer, pos, mMoveDir, t);
						mState = State_Moving;
						repeat = true;
					
					} else {
					
						updateMoveDir(footballer, pos, mMoveDir, t);
					
						if (requestBodyRun(footballer, t, pos)) {
						
							mState = State_Moving;
						}
					}
				
				} break;
			
				case State_Moving: {
				
					if (footballer.getActionType() == footballer.Action_Running) {

						slowUpdateMoveDir(footballer, pos, mMoveDir, t);					
				
						if (!updateBodyRun(brain, footballer, t, footballer.getActionRunning())) {
						
							mState = Result_Failure;
						}
					
					} else {
				
						mState = State_Starting;
						repeat = true;
					}
				
				} break;
			}
		}	
		
		return doReturn(t, stateToResult(mState));	
	}
	
	mState = -1;
	mTargetPos = null;
	mEpsilonSq = 0.0;
	mToleranceMode = -1;
	mTempVec3 = null;

	mRunStartState = null;
	mStrafe = false;
	mMoveDir = null;
	mMoveDirSlowUpdateLastTime = 0.0;
	mTimeToIdleConstrained = -1.0;
}