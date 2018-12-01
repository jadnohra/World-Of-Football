
class AI_Goal {

	static Result_None = -1;
	static Result_Processing = 0;
	static Result_Success = 1;
	static Result_Failure = 2;

	static DirEpsilonSq = 0.05 * 0.05;
	
	function start(footballer, t) {}
	function frameMove(footballer, t, dt) { return Result_Success; }
	
	function utilFixDir(match, dir, ) {
	
		dir.setEl(match.Scene_Up, 0.0);
		
		if (dir.magSq() <= DirEpsilonSq)
			dir.zero();
		else
			dir.normalize();
	}
	
	function chainGoal(goal) { mChainedGoal = goal; }
	function getChainedGoal() { return mChainedGoal; }
	
	
	mChainedGoal = null;
}

function AI_helper_Idle(footballer, t) {

	if (footballer.getActionType() == footballer.Action_Idle)
		return true;
		
	footballer.getAction().switchIdle(footballer, t, null);
	
	return false;
}

class AI_Goal_Idle extends AI_Goal {

	function start(footballer, t) { footballer.getAction().switchIdle(footballer, t); }
	
	function frameMove(footballer, t, dt) { 
		
		return AI_helper_Idle(footballer, t) ? Result_Success : Result_Processing;
	}
}

class AI_Goal_Face extends AI_Goal {

	static State_Processing = 0;
	static State_Success = 1;
	static State_Failure = 2;

	constructor() {
	
		mFaceDir = Vector3();
	}
	
	function accessDirectFaceDir() { return mFaceDir; }
	
	function setTargetByDir2(footballer, dir) {
	
		mFaceDir.set(dir);
	}
	
	function setTargetByDir(footballer, dir) {
	
		local match = footballer.impl.getMatch();
	
		mFaceDir.set(dir);
		utilFixDir(match, mFaceDir);
	}
	
	function setTargetByPos(footballer, pos) {
	
		local match = footballer.impl.getMatch();
	
		mFaceDir.set(pos);
		mFaceDir.subtractFromSelf(footballer.impl.getPos());
		utilFixDir(match, mFaceDir);
		
		//if (mFaceDir.isZero())	
		//	mState = State_Success;
	}
	
	function start(footballer, t) { 
	
		if (mFaceDir.isZero())	
			mState = State_Success;
		else
			mState = State_Processing; 
	}
		
	function frameMove(footballer, t, dt) { 
		
		if (mState >= State_Success) 
			return mState == State_Success ? Result_Success : Result_Failure;
			
		local impl = footballer.impl;
		
		if (mFaceDir.isZero() || impl.getFacing().dot(mFaceDir) >= 0.95) {
		
			AI_helper_Idle(footballer, t);
		
			mState = State_Success;
			return Result_Success;
			
		} 

		if (footballer.getActionType() == footballer.Action_Running) {

			if (impl.getCollState() == impl.Coll_Blocked) {
				
				AI_helper_Idle(footballer, t);
				mState = State_Processing;
				
			} else {
			
				footballer.getActionRunning().setBodyFacing(footballer, mFaceDir);
				footballer.getActionRunning().setRunDir(footballer, mFaceDir);
			}
		
		} else {
		
			footballer.getAction().switchTurning(footballer, t, null);
			footballer.getActionTurning().setBodyFacing(footballer, mFaceDir);
			/*
			local startState = BodyActionRunning_StartState();
			
			startState.runSpeedType = footballer.run.RunSpeed_Normal;
			startState.bodyFacing = mFaceDir;
			startState.runDir = mFaceDir;
			
			footballer.getAction().switchRunning(footballer, t, startState);
			*/
		}
		
		if (mState >= State_Success) 
			return mState == State_Success ? Result_Success : Result_Failure;
			
		return Result_Processing;	
	}
	
	mState = null;
	mFaceDir = null;
}

class AI_Goal_ReachPos extends AI_Goal {

	static State_None = -1;
	static State_Starting = 0;
	static State_Moving = 1;
	static State_Succeeded = 2;
	static State_Failed = 3;
	
	constructor() {
	
		mMoveDir = Vector3();
		mTargetPos = Vector3();
	}
	
	function getTarget() { return mTargetPos; }

	function setEpsilon(epsilon) {
	
		mEpsilonSq = epsilon * epsilon;
	}	
	
	function setTarget(footballer, pos) {
	
		local match = footballer.impl.getMatch();
	
		mTargetPos.set(pos);
		mTargetPos.setEl(match.Scene_Up, footballer.impl.getPos().get(match.Scene_Up));
	}
	
	function setTargetWithDistanceOffset(footballer, pos, t, offsetDistance) {
	
		if (offsetDistance != 0.0) {
		
			local offset = Vector3();
		
			initMoveDir(footballer.impl.getMatch(), pos, mMoveDir, t);
			mMoveDir.mul(offsetDistance, offset);
			
			setTargetWithOffset(footballer, pos, offset);
			
		} else {
		
			setTarget(footballer, pos);
		}
	}
	
	function setTargetWithOffset(footballer, pos, offset) {
	
		local match = footballer.impl.getMatch();
	
		mTargetPos.set(pos);
		mTargetPos.addToSelf(offset);
		mTargetPos.setEl(match.Scene_Up, footballer.impl.getPos().get(match.Scene_Up));
	}

	function start(footballer, t) {
	
		mState  = State_Starting;
		mMinRunDistSq = 0.0;
		mMoveDirSlowUpdateLastTime = 0.0;
	}
	
	function initMoveDir(match, pos, dir, t) {
	
		mMoveDirSlowUpdateLastTime = t;
	
		mTargetPos.subtract(pos, dir);
		utilFixDir(match, dir);
	}
	
	function slowUpdateMoveDir(match, pos, dir, t) {
	
		if (t - mMoveDirSlowUpdateLastTime > 0.35) {
		
			initMoveDir(match, pos, dir, t);
		}
	}
	
	function requestBodyRun(footballer, t, pos) {
	
		//mRunStartState bodyFacing and runDir point to same mMoveDir instance
	
		if (mRunStartState == null) {
		
			mRunStartState = BodyActionRunning_StartState();
			mRunStartState.bodyFacing = mMoveDir;
			mRunStartState.runDir = mMoveDir;
			mRunStartState.runSpeedType = footballer.run.RunSpeed_Normal;
		}

		return footballer.getAction().switchRunning(footballer, t, mRunStartState);
	}
	
	function updateBodyRun(footballer, actionRunning, distSq) {
	
		local impl = footballer.impl;
		local collState = impl.getCollState();

		switch (collState) {
		
			case impl.Coll_Constrained: return true;
			case impl.Coll_Blocked: return false;
		}
		
		if (distSq > mLastDistSq)
			return false;
	
		local currRunDir = actionRunning.getRunDir();
		
		if (!currRunDir.equals(mMoveDir)) {
		
			actionRunning.setBodyFacing(footballer, mMoveDir);
			actionRunning.setRunDir(footballer, mMoveDir);
		}
		
		return true;
	}
	
	function frameMove(footballer, t, dt) { 
	
		if (mState >= State_Succeeded) 		
			return mState == State_Succeeded ? Result_Success : Result_Failure;
	
		local repeat = true;
		local pos = footballer.impl.getPos();
		local distSq = distSq(pos, mTargetPos);
	
		if (distSq <= mEpsilonSq || distSq <= mMinRunDistSq) {

			mState = State_Succeeded;
			return Result_Success;
		} 
		
		local match = footballer.impl.getMatch();
	
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_Starting: {
				
					if (mMinRunDistSq == 0.0) {
					
						local settings = footballer.run;
					
						local speed = settings.getSpeedValue(settings.Run_Normal, settings.RunSpeed_Normal);
						mMinRunDistSq = 0.55 * speed * dt;
						mMinRunDistSq *= mMinRunDistSq;
					}
					
					if (footballer.getActionType() == footballer.Action_Running) {
					
						mLastDistSq = distSq;
						initMoveDir(match, pos, mMoveDir, t);
						mState = State_Moving;
						repeat = true;
					
					} else {
					
						initMoveDir(match, pos, mMoveDir, t);
					
						if (requestBodyRun(footballer, t, pos)) {
						
							mState = State_Moving;
						}
					}
				
				} break;
			
				case State_Moving: {
				
					if (footballer.getActionType() == footballer.Action_Running) {

						slowUpdateMoveDir(match, pos, mMoveDir, t);					
				
						if (!updateBodyRun(footballer, footballer.getActionRunning(), distSq)) {
						
							mState = State_Failed;
						}
					
					} else {
				
						mState = State_Starting;
						repeat = true;
					}
				
				} break;
			}
		}	
		
		mLastDistSq = distSq;	
			
		if (mState >= State_Succeeded) 		
			return mState == State_Succeeded ? Result_Success : Result_Failure;
			
		return Result_Processing;	
	}
	
	mState = -1;
	mTargetPos = null;
	mEpsilonSq = 0.0;
	mMinRunDistSq = 0.0;
	mLastDistSq = null;

	mRunStartState = null;
	mMoveDir = null;
	mMoveDirSlowUpdateLastTime = 0.0;
}

class AI_Goal_GainBallOwnership extends AI_Goal {

	static State_None = -1;
	static State_Estimating = 0;
	static State_Moving = 1;
	static State_Waiting = 2;
	static State_Facing = 3;
	static State_Succeeded = 4;
	static State_Failed = 5;

	constructor() {
	
		mEstimate = CBallInterceptEstimate();
		mEstimate.enableAnalysis(true, true);
		mEstimate.enableBallOutOfPlayEstimates(false);
		mMover = AI_Goal_ReachPos();
		mFacer = AI_Goal_Face();
	}	
	
	function setMaxAnalysisAge(age) {
	
		mMaxAnalysisAge = age;
	}
	
	function setEpsilon(epsilon) {
	
		mMover.setEpsilon(epsilon);
	}
	
	function start(footballer, t) {
		
		mState = State_None;
		mEstimateMode = mEstimate.Analyse_None;
	}
	
	function startMoving(footballer, t) {
	
		mState = State_Moving;
		
		mMover.start(footballer, t);
		
		local ballVel = mEstimate.getModeVel(mEstimateMode);
		local ballDir = null;

		if (ballVel != null && !ballVel.isZero()) {
		
			ballDir = Vector3();
			ballDir.set(ballVel);
			utilFixDir(footballer.impl.getMatch(), ballDir);
			
			if (ballDir.isZero())
				ballDir = null;
		}
		
		if (ballDir != null) {
	
			ballDir.mulToSelf(0.5 * footballer.impl.getCOCFwdReach());
			mMover.setTargetWithOffset(footballer, mEstimate.getModePos(mEstimateMode), ballDir);
			
		} else {
		
			mMover.setTargetWithDistanceOffset(footballer, mEstimate.getModePos(mEstimateMode), t, 0.5 * footballer.impl.getCOCFwdReach());
		}
	}
	
	function idleAndReturn(footballer, t, resetEstimateMode) {
	
		AI_helper_Idle(footballer, t);
					
		if (resetEstimateMode)
			mEstimateMode = mEstimate.Analyse_None;	
			
		mState = State_Estimating;
		return Result_Processing;
	}
	
	
	function frameMove(footballer, t, dt) { 
	
		if (mState >= State_Succeeded) 		
			return mState == State_Succeeded ? Result_Success : Result_Failure;

		local impl = footballer.impl;			
		
		if (impl.isBallOwner()) {
		
			AI_helper_Idle(footballer, t);
			
			mState = State_Succeeded;
			return Result_Success;
		}
		
		local estimResult = mEstimate.Update_Invalidated;
		
		{
			local prevEstimMode = mEstimateMode;
		
			switch (mEstimateMode) {
			
				case mEstimate.Analyse_None: {
				
					if (t < mNextAnalysisTime) {
					
						return idleAndReturn(footballer, t, false);
					}
				
					mNextAnalysisTime = t + mAnalysisDelay;
				
					local estimResultPath = mEstimate.updatePath(footballer.impl.getMatch(), footballer.impl, t, true, true, true);
					local estimResultColl = mEstimate.updateColl(footballer.impl.getMatch(), footballer.impl, t);
					
					switch (mEstimate.analyseValidity()) {
					
						case mEstimate.Analyse_None: {
						
							return idleAndReturn(footballer, t, false);
						
						} break;
					
						case mEstimate.Analyse_Path: {	

							if (mEstimate.isPathInstantaneous()) {
							
								if (mChaseInstantaneousBall) {
							
									//no analysis delay for instantaneous ball analysis
									mNextAnalysisTime = t;
								
								} else {
							
									return idleAndReturn(footballer, t, false);
								}
							}
							
							estimResult = estimResultPath;
							mEstimateMode = mEstimate.Analyse_Path;
							
						} break;
						
						case mEstimate.Analyse_Coll: {	
						
							estimResult = estimResultColl;
							mEstimateMode = mEstimate.Analyse_Coll;
							
						} break;
						
						case mEstimate.Valid_Both: {	
						
							local interceptTimeDiff = mEstimate.getCollInterceptTime() - mEstimate.getPathInterceptTime();
						
							if (interceptTimeDiff > 1.5) {
							
								estimResult = estimResultPath;
								mEstimateMode = mEstimate.Analyse_Path;
							
							} else {
							
								estimResult = estimResultColl;
								mEstimateMode = mEstimate.Analyse_Coll;
							}
						
						} break;
					}
					
					//becuase we updated without invalidating, a no change could have resulted
					if (estimResult == mEstimate.Update_NoChange)
						estimResult = mEstimate.Update_NewEstimate;
					
				} break;
				
				case mEstimate.Analyse_Path: {
				
					estimResult = mEstimate.updatePath(footballer.impl.getMatch(), footballer.impl, t, true, true, true);
					
				} break;
				
				case mEstimate.Analyse_Coll: {
				
					estimResult = mEstimate.updateColl(footballer.impl.getMatch(), footballer.impl, t);
				
				} break;
			}
			
			switch (estimResult) {
		
				case mEstimate.Update_NoChange: {
				
					if (mState == State_Estimating)
						return Result_Processing;
				
				} break;
			
				case mEstimate.Update_NewEstimate: {

					if (!mChaseInstantaneousBall && mEstimate.isInstantaneous()) {
					
						return idleAndReturn(footballer, t, false);
					
					} else {
					
						if (prevEstimMode == mEstimate.Analyse_None) {
						
							startMoving(footballer, t);
						
						} else {
					
							mEstimateMode = mEstimate.Analyse_None;
							return Result_Processing;
						}
					}
				
				} break;
				
				default: {
				
					return idleAndReturn(footballer, t, true);
				
				} break;
			}
		}
			
	
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_None: {
				
					startMoving(footballer, t);
					repeat = true;
					
				} break;
				
				case State_Moving: {
				
					local result = mMover.frameMove(footballer, t, dt);
					
					switch (result) {
					
						case Result_Success: {
						
							mState = State_Facing;
							mFacer.start(footballer, t);
							mFacer.setTargetByPos(footballer, mEstimate.getModePos(mEstimateMode));
							
							repeat = true;
						
						} break;
						
						case Result_Failure: {
						
							mState = State_Failed;
						
						} break;
					}
				
				} break;
				
				case State_Facing: {
				
					local result = mFacer.frameMove(footballer, t, dt);
					
					switch (result) {
					
						case Result_Success: {
						
							mState = State_Waiting;
							repeat = true;
						
						} break;
						
						case Result_Failure: {
						
							mState = State_Failed;
							
						} break;
					}
						
				} break;
			}
		}
		
		if (mState == State_Waiting) {
		
			AI_helper_Idle(footballer, t);
		}
		
		if (mState >= State_Succeeded) 		
			return mState == State_Succeeded ? Result_Success : Result_Failure;
	
		return Result_Processing; 
	}

	mState = -1;
	mEstimate = null;
	mEstimateMode = -1;
	mMover = null;
	mFacer = null;
	mMaxAnalysisAge = 0.0;
	
	mAnalysisDelay = 0.15;
	mNextAnalysisTime = 0.0;
	
	mChaseInstantaneousBall = true;
}

class AI_Goal_OwnBall extends AI_Goal {

	constructor() {
	
		mInterceptor = AI_Goal_GainBallOwnership();
	}

	
	function setEpsilon(epsilon) {
	
		mInterceptor.setEpsilon(epsilon);
	}
	
	function setMaxAnalysisAge(age) {
	
		mInterceptor.setMaxAnalysisAge(age);
	}
	
	function start(footballer, t) {
	
		if (footballer.impl.isBallOwner()) {
	
			mReached = true;
			
		} else {
		
			mReached = false;
			mInterceptor.start(footballer, t);
		}
	}
	
	function frameMove(footballer, t, dt) { 
	
		local repeat = true;
		
		while (repeat) {
		
			repeat = false;
	
			if (mReached) {
			
				if (!footballer.impl.isBallOwner() && mNextStartTime <= t) {
				
					mReached = false;
					mInterceptor.start(footballer, t);
					repeat = true;
				}
			
			} else {
			
				if (mInterceptor.frameMove(footballer, t, dt) >= Result_Success) {
				
					mReached = true; 
					mNextStartTime = t + mOwnershipLossDelay; 
				}
			}
		}
			
		return Result_Processing; 
	}
	
	mInterceptor = null;
	mReached = false;
	
	mOwnershipLossDelay = 0.25;
	mNextStartTime = 0.0;
}

class FootballerBrain_AI_v1 extends FootballerBrainBase {

	constructor() {
	}	

	function start(footballer, t) {
	
		footballer.getAction().switchIdle(footballer, t, null);
	
		return true;
	}
	
	function stop(footballer, t) {
	}

	function onSwitchedTeamFormation(footballer, t) {
	
		if (mCurrGoal == null) {
		
			local pos = Vector3();
			
			if (footballer.impl.getFormationPos(pos)) {
			
				setGoalReachPos(footballer, t, pos, 0.0);
				
				local goalFace = AI_Goal_Face();
				footballer.impl.getFormationDir(goalFace.accessDirectFaceDir());
				
				mCurrGoal.chainGoal(goalFace);
			}
		}
	}
	
	function frameMove(footballer, t, dt) {
	
		if (mCurrGoal != null) {
		
			local result = mCurrGoal.frameMove(footballer, t, dt);
			
			if (result != AI_Goal.Result_Processing) {
			
				if (result == AI_Goal.Result_Success) {
				
					mCurrGoal = mCurrGoal.getChainedGoal();
					
					if (mCurrGoal != null) {
					
						mCurrGoal.start(footballer, t);
					}
					
				} else {
			
					mCurrGoal = null;
				}
			}
		}
		
		if (mCurrGoal == null) {
		
			footballer.getAction().switchIdle(footballer, t, null);
		}
	}

	function cancelGoal(footballer, t) {
	
		mCurrGoal = null;
	}
	
	function setGoalReachPos(footballer, t, pos, epsilon) {
	
		mCurrGoal = AI_Goal_ReachPos();
		mCurrGoal.setEpsilon(epsilon);
		mCurrGoal.setTarget(footballer, pos);
		mCurrGoal.start(footballer, t);
	}
	
	function setGoalInterceptBall(footballer, t, maxAnalysisAge, epsilon) {
	
		mCurrGoal = AI_Goal_GainBallOwnership();
		mCurrGoal.setMaxAnalysisAge(maxAnalysisAge);
		mCurrGoal.setEpsilon(epsilon);
		mCurrGoal.start(footballer, t);
	}
	
	function setGoalOwnBall(footballer, t, maxAnalysisAge, epsilon) {
	
		mCurrGoal = AI_Goal_OwnBall();
		mCurrGoal.setMaxAnalysisAge(maxAnalysisAge);
		mCurrGoal.setEpsilon(epsilon);
		mCurrGoal.start(footballer, t);
	}
	
	mCurrGoal = null;
}