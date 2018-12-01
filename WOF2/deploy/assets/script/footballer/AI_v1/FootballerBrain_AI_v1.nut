
class AI_RequestManager {

	static OppRequest_None = -1;
	static OppRequest_ZoneDefend = 0;

	function getOppRequestSelectFunction(requestType) {
	
		switch (requestType) {
		
			case AI_RequestManager.OppRequest_ZoneDefend: {
			
				return AI_RequestManager.select_OppRequest_ZoneDefend;
			
			} break;
		}
		
		return null;
	}
	
	function select_OppRequest_ZoneDefend(bestRequest, request, oppFootballer) {
	
		if (bestRequest == null)
			return request;
		
		local oppPos = oppFootballer.impl.getPos();
		
		local distSqBest = distSq(bestRequest.requestSource.impl.getPos(), oppPos);
		local distSqNew = distSq(request.requestSource.impl.getPos(), oppPos);
		
		return distSqNew < distSqBest ? request : bestRequest;
	}
}

class AI_GoalPool_v1 {

	constructor() {
	
		mPoolTable = {};
	}

	function getPool(classType) {
	
		local pool = classType in mPoolTable ? mPoolTable[classType] : null;
		
		if (pool == null) {
		
			if (!(classType.parent == AI_Goal)) {
			
				return null;
			}
		
			pool = [];
			mPoolTable[classType] <- pool;
		}
		
		return pool;
	}
	
	function get(classType) {
	
		local pool = getPool(classType);
		local retGoal = pool.len() ? pool.pop() : null;
	
		if (retGoal == null) {
		
			retGoal = classType();
			//retGoal.constructor();
		} 
		
		return retGoal;
	}
		
	function put(goal) {
	
		local classType = goal.getclass();
		local pool = getPool(classType);
		
		goal.stop();
		pool.append(goal);
		
		return null;
	}
	
	mPoolTable = null;
}

class AI_Goal {
	
	/*
		the values are negative so that they can be used directly as states as well
	*/
	static Result_None = -1;
	static Result_Processing = -2;
	static Result_Success = -3;
	static Result_Failure = -4;
	
	static DirEpsilonSq = 0.05 * 0.05;
	
	constructor() {
	
		mLastGoalUpdateTime = -1.0;
	}
	
	function start(teamAI, brain, footballer, t) { return doReturn(t, Result_Success); }
	function frameMove(teamAI, brain, footballer, t, dt) { return doReturn(t, Result_Success); }
	
	function update(teamAI, brain, footballer, t, dt) { 
	
		local result = Result_Processing;
	
		if (!isActive(t, dt)) {
			
			result = start(teamAI, brain, footballer, t);
		}
		
		if (result == Result_Processing)
			result = frameMove(teamAI, brain, footballer, t, dt);
			
		return result;			
	}
	
	function setRegion(region) {}
	
	function doReturn(t, result) {
	
		if (result != Result_Processing)
			mLastGoalUpdateTime = -1.0;
		else	
			mLastGoalUpdateTime = t;
			
		return result;	
	}
	
	function isActive(t, dt) {
	
		return mLastGoalUpdateTime >= t - (1.5 * dt);
	}
	
	function calcActiveTime(t, dt) {
	
		return t - (1.5 * dt);
	}
	
	function isActiveTime(activeT) {
	
		return mLastGoalUpdateTime >= activeT;
	}
	
	function stop() {
	
		mLastGoalUpdateTime = -1.0;
	}
	
	function utilFixDir(match, dir) {
	
		dir.setEl(match.Scene_Up, 0.0);
		
		if (dir.magSq() <= DirEpsilonSq)
			dir.zero();
		else
			dir.normalize();
	}
	
	function chainGoal(goal) { mChainedGoal = goal; }
	function getChainedGoal() { return mChainedGoal; }
	
	function stateIsEndResult(state) { return state <= Result_Success; }
	
	mChainedGoal = null;
	mLastGoalUpdateTime = -1.0;
}

class AI_Goal_Idle extends AI_Goal {

	function start(teamAI, brain, footballer, t) { footballer.getAction().switchIdle(footballer, t); }
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
		
		return doReturn(t, brain.body_idle(footballer, t) ? Result_Success : Result_Processing);
	}
}

class AI_Goal_Face extends AI_Goal {

	constructor() {
	
		mFaceDir = Vector3();
	}
	
	function accessDirectFaceDir() { 
	
		mQueuedPos = null; 
		return mFaceDir; 
	}
	
	function setTargetByDir2(footballer, dir) {
	
		mQueuedPos = null;
		
		mFaceDir.set(dir);
	}
	
	function setTargetByDir(footballer, dir) {
	
		mQueuedPos = null;
	
		local match = footballer.impl.getMatch();
	
		mFaceDir.set(dir);
		utilFixDir(match, mFaceDir);
	}
	
	function setTargetByQueuedPos(footballer, pos) {
	
		mQueuedPos = Vector3();
		mQueuedPos.set(pos);
	}
	
	function setTargetByPos(footballer, pos) {
	
		mQueuedPos = null;
	
		local match = footballer.impl.getMatch();
	
		mFaceDir.set(pos);
		mFaceDir.subtractFromSelf(footballer.impl.getPos());
		utilFixDir(match, mFaceDir);
		
		//if (mFaceDir.isZero())	
		//	mState = State_Success;
	}
	
	function start(teamAI, brain, footballer, t) { 
	
		if (mQueuedPos != null) {
		
			setTargetByPos(footballer, mQueuedPos);
			mQueuedPos = null;
		}
	
		if (mFaceDir.isZero())	
			mState = Result_Success;
		else
			mState = Result_Processing; 
			
		return doReturn(t, mState);	
	}
		
	function frameMove(teamAI, brain, footballer, t, dt) { 
		
		if (mState != Result_Processing) 
			return doReturn(t, mState);
			
		local impl = footballer.impl;
		
		if (mFaceDir.isZero() || impl.getFacing().dot(mFaceDir) >= 0.995) {
		
			brain.body_idle(footballer, t);
		
			mState = Result_Success;
			return doReturn(t, mState);
		} 
		
		if (footballer.getActionType() == footballer.Action_Running) {

			if (impl.getCollState() == impl.Coll_Blocked) {
				
				brain.body_idle(footballer, t);
				mState = Result_Processing;
				
			} else {
			
				footballer.getActionRunning().setBodyFacing(footballer, mFaceDir);
				footballer.getActionRunning().setRunDir(footballer, mFaceDir);
			}
		
		} else {
		
			footballer.getAction().switchTurning(footballer, t, null);
			footballer.getActionTurning().setBodyFacing(footballer, mFaceDir);
		}
		
		//if (mState >= State_Success) 
		//	return doReturn(t, mState == State_Success ? Result_Success : Result_Failure);
			
		return doReturn(t, mState);	
	}
	
	mState = -1;
	mFaceDir = null;
	mQueuedPos = null;
}

class AI_Goal_ReachPos extends AI_Goal {

	static State_None = -1;
	static State_Starting = 0;
	static State_Moving = 1;
	
	static Tolerance_None = -1;
	static Tolerance_Dist = 0;
	static Tolerance_Strict = 1;
	static Tolerance_Tolerant = 2;
	
	
	constructor() {
	
		mMoveDir = Vector3();
		mTargetPos = Vector3();
	}
	
	function getTarget() { return mTargetPos; }

	function setIsDynamic(isDynamic) {
	
		mIsDynamic = isDynamic;
	}
		
	
	function setEpsilon(epsilon) {
	
		mEpsilonSq = epsilon * epsilon;
	}	
	
	function setToleranceMode(toleranceMode) {
	
		mToleranceMode = toleranceMode;
		
		if (mToleranceMode > Tolerance_Dist) {
		
			mTempVec3 = Vector3();
		} 
	}
	
	function setTarget(footballer, pos) {
	
		local match = footballer.impl.getMatch();
	
		if (mIsDynamic && !mTargetPos.equals(pos))
			mMoveDirSlowUpdateLastTime = 0.0;
	
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
	
	function start(teamAI, brain, footballer, t) {
	
		mState  = State_Starting;
		mMinRunDistSq = 0.0;
		mMoveDirSlowUpdateLastTime = 0.0;
		
		return doReturn(t, Result_Processing);
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
	
		if (mRunStartState == null) {
		
			mRunStartState = BodyActionRunning_StartState();
			mRunStartState.bodyFacing = mMoveDir;
			mRunStartState.runDir = mMoveDir;
			mRunStartState.runSpeedType = footballer.settings.run.RunSpeed_Normal;
		}

		return footballer.getAction().switchRunning(footballer, t, mRunStartState);
	}
	
	function updateBodyRun(brain, footballer, t, actionRunning) {
	
		local impl = footballer.impl;
		local collState = impl.getCollState();

		switch (collState) {
		
			case impl.Coll_Constrained: {
			
				if (mTimeToCollIdle < t)
					brain.body_idle(footballer, t);				
			
			} return true;
			
			case impl.Coll_Blocked: return false;
		}
				
		mTimeToCollIdle = t + randf(0.0, 1.5);
	
		brain.body_updateStraightRunDir(footballer, t, mMoveDir);
			
		return true;
	}
	
	function isWithinTolerance(pos, distanceSq, dt) {
	
		local isWithinSimpleTolerance = ((distanceSq <= mEpsilonSq) || (distanceSq <= mMinRunDistSq));
	
		if (!isWithinSimpleTolerance)
			return false;
	
		switch (mToleranceMode) {
		
			case Tolerance_Strict: {
			
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
			
			case Tolerance_Tolerant: {
			
				pos.subtract(mTargetPos, mTempVec3);
			
				if (mTempVec3.dot(mMoveDir) > 0.0) {
				
					pos.addMultiplication(mMoveDir, dt, mTempVec3);
					local newDistSq = distSq(mTempVec3, mTargetPos);
					
					return (newDistSq > mEpsilonSq || newDistSq > mMinRunDistSq);
					
				} else {
				
					return false;
				}
			
			} break;
		}
			
		return true;
	}
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		if (stateIsEndResult(mState)) 		
			return doReturn(t, mState);
	
		local repeat = true;
		local pos = footballer.impl.getPos();
		local distSq = distSq(pos, mTargetPos);
	
		if (isWithinTolerance(pos, distSq, dt)) {
		
			mState = Result_Success;
			return doReturn(t, mState);
		} 
		
		local match = footballer.impl.getMatch();
	
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_Starting: {
								
					if (mMinRunDistSq == 0.0) {
					
						local settings = footballer.settings.run;
					
						local speed = settings.getSpeedValue(settings.Run_Normal, settings.RunSpeed_Normal);
						mMinRunDistSq = 0.55 * speed * dt;
						mMinRunDistSq *= mMinRunDistSq;
						
						if (mEpsilonSq == 0.0 && isWithinTolerance(pos, distSq, dt)) {
		
							mState = Result_Success;
							return doReturn(t, mState);
						} 
					}
					
					if (footballer.getActionType() == footballer.Action_Running) {
					
						mLastDistSq = distSq;
						initMoveDir(match, pos, mMoveDir, t);
						mState = State_Moving;
						mTimeToCollIdle = t + randf(0.0, 1.5);
						repeat = true;
					
					} else {
					
						initMoveDir(match, pos, mMoveDir, t);
					
						if (requestBodyRun(footballer, t, pos)) {
						
							mState = State_Moving;
							mTimeToCollIdle = t + randf(0.0, 1.5);
						}
					}
				
				} break;
			
				case State_Moving: {
				
					if (footballer.getActionType() == footballer.Action_Running) {

						slowUpdateMoveDir(match, pos, mMoveDir, t);					
				
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
		
		mLastDistSq = distSq;	
			
		if (stateIsEndResult(mState)) 		
			return doReturn(t, mState);
			
		return doReturn(t, Result_Processing);	
	}
	
	mState = -1;
	mTargetPos = null;
	mEpsilonSq = 0.0;
	mMinRunDistSq = 0.0;
	mLastDistSq = null;
	mToleranceMode = -1;
	mTempVec3 = null;

	mRunStartState = null;
	mMoveDir = null;
	mMoveDirSlowUpdateLastTime = 0.0;
	mTimeToCollIdle = 0.0;
	mIsDynamic = false;
}

class AI_Goal_GainBallOwnership extends AI_Goal {

	static State_None = -1;
	static State_Estimating = 0;
	static State_Moving = 1;
	static State_Waiting = 2;
	static State_Facing = 3;
	
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
	
	function start(teamAI, brain, footballer, t) {
		
		mState = State_None;
		mEstimateMode = mEstimate.Analyse_None;
		
		return doReturn(t, Result_Processing);
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
	
		brain.body_idle(footballer, t);
					
		if (resetEstimateMode)
			mEstimateMode = mEstimate.Analyse_None;	
			
		mState = State_Estimating;
		return doReturn(t, Result_Processing);
	}
	
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		if (stateIsEndResult(mState)) 		
			return doReturn(t, mState);

		local impl = footballer.impl;			
		
		if (impl.isBallOwner()) {
		
			brain.body_idle(footballer, t);
			
			mState = Result_Success;
			return doReturn(t, mState);
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
						return doReturn(t, Result_Processing);
				
				} break;
			
				case mEstimate.Update_NewEstimate: {

					if (!mChaseInstantaneousBall && mEstimate.isInstantaneous()) {
					
						return idleAndReturn(footballer, t, false);
					
					} else {
					
						if (prevEstimMode == mEstimate.Analyse_None) {
						
							startMoving(footballer, t);
						
						} else {
					
							mEstimateMode = mEstimate.Analyse_None;
							return doReturn(t, Result_Processing);
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
		
			brain.body_idle(footballer, t);
		}
		
		if (stateIsEndResult(mState)) 		
			return doReturn(t, mState);
	
		return doReturn(t, Result_Processing); 
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
	
	function start(teamAI, brain, footballer, t) {
	
		if (footballer.impl.isBallOwner()) {
	
			mReached = true;
			
		} else {
		
			mReached = false;
			mInterceptor.start(footballer, t);
		}
		
		return doReturn(t, Result_Processing);
	}
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
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
			
		return doReturn(t, Result_Processing); 
	}
	
	mInterceptor = null;
	mReached = false;
	
	mOwnershipLossDelay = 0.25;
	mNextStartTime = 0.0;
}

class AI_Goal_FireShot extends AI_Goal {

	static State_None = -1;
	static State_WaitingToShoot = 0;

	constructor() {
	
		mShot = createFootballerBallShot();
	}

	function setShot(shotType, shot, shotSpeedScale) {
	
		mShotType = shotType;
		mShot.set(shot);
		mShotSpeedScale = shotSpeedScale;
	}
	
	function start(teamAI, brain, footballer, t) { 
	
		mState = State_WaitingToShoot;
		mShootStartState = null;
		
		return doReturn(t, Result_Processing);
	}
		
	function frameMove(teamAI, brain, footballer, t, dt) { 
		
		local impl = footballer.impl;
		
		switch (footballer.getActionType()) {
		
			case footballer.Action_Running: {
			
				footballer.getActionRunning().setActiveBallShot(mShot, mShotSpeedScale, false, false);
				mState = State_Succeeded;
			
			} break;
			
			default: {
				
				if (mShootStartState == null) {
				
					mShootStartState = footballer.getActionShooting().createStartState(footballer, footballer.ballIsHeaderHeight(), mShotType, mShot, false, mShotSpeedScale);
				} 
			
				if (footballer.getAction().switchShooting(footballer, t, mShootStartState)) {
				
					mState = Result_Success;
				}
			
			} break;
		}
		
		if (stateIsEndResult(mState)) 		
			return doReturn(t, mState);
		
		return doReturn(t, Result_Processing);	
	}
	
	mState = null;
	mShotType = -1;
	mShot = null;
	mShotSpeedScale = 0.0;
	mShootStartState = null;
}

class AI_Goal_SetupFireShotGoal extends AI_Goal {

	constructor() {
	
		mTarget = Vector3();
	}

	function setupByTime(goal, target, time, shotType, allowWait, allowUnpreciseShot) {
	
		mFireShotGoal = goal;
		
		mTarget.set(target);
		mTime = time;
		mSpeed = -1.0;
		
		mShotType = shotType;
		
		mAllowWait = allowWait;
		mAllowUnpreciseShot = allowUnpreciseShot;
	}
	
	function setupBySpeed(goal, target, speed, shotType, allowUnpreciseShot) {
	
		mFireShotGoal = goal;
		
		mTarget.set(target);
		mSpeed = speed;
		mTime = -1.0;
		
		mShotType = shotType;
		
		mAllowWait = false;
		mAllowUnpreciseShot = allowUnpreciseShot;
	}
	
	function start(teamAI, brain, footballer, t) { 
	
		mIsWaiting = false;
		
		return doReturn(t, Result_Processing);
	}
		
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
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
		
		utilFixDir(match, dir);
		
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
				mFireShotGoal.setShot(mShotType, shot, result.speed / shot.speed);
						
				return doReturn(t, Result_Success);
			} 
		}
		
		//print("estim fail");
	
		return doReturn(t, Result_Failure);
	}
	
	mFireShotGoal = null;
	mTarget = null;
	mTime = 0.0;
	mSpeed = 0.0;
	mShotType = -1;
	
	mAllowWait = false;
	mWaitDelay = 0.07;
	mIsWaiting = false;
	mNextWaitUpdateTime = 0.0;
	
	mAllowUnpreciseShot = false;
}

class AI_Goal_GoToSafeSpace extends AI_Goal {
	
	static State_Thinking = -1;
	static State_Going = 0;
	static State_ReachingBall = 1;
		
	constructor() {	
		
		mSafePos = Vector3();	
		
		mGoalReachPos = AI_Goal_ReachPos();
		mGoalReachPos.setIsDynamic(false);
		mGoalReachPos.setEpsilon(50.0);								
	}
		
	function setRegion(region) {
	
		mRegion = region;
	}	
		
	function start(teamAI, brain, footballer, t) { 
	
		mState = State_Thinking;
	
		return doReturn(t, Result_Processing); 
	}
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_Thinking: {
				
					local fbImpl = footballer.impl;
					local runSettings = footballer.settings.run;
					local runSpeed = runSettings.getSpeedValue(runSettings.Run_Normal, runSettings.RunSpeed_Normal)
				
					local safetyTime = Float();
				
					if (fbImpl.getMatch().findFootballerFreeSpace(fbImpl, mSearchDir, true, 
																	fbImpl.getCOCFwdReach() * 1.5, runSpeed, fbImpl.getMatch().conv(5.0), 0.5, 4, mRegion, mSafePos, safetyTime)) {
									
						
						mTryKeepBall = fbImpl.isBallOwner();
									
						mGoalReachPos.setTarget(footballer, mSafePos);
						mState = State_Going;
						repeat = true;
						
					} else {
					
						brain.body_idle(footballer, t);
					}
					
				} break;
				
				case State_Going: {
				
					local result = mGoalReachPos.update(teamAI, brain, footballer, t, dt);
						
					if (result != Result_Processing) {
					
						local acceptEnd = true;
					
						if (mTryKeepBall && result == Result_Success) {
						
							mState = State_ReachingBall;
							repeat = true;
							
						} else {
						
							mState = result;
						}
					} 
			
				} break;
				
				case State_ReachingBall: {
				
					if (brain.body_isIdle(footballer)) {
					
						mState = Result_Success;
					
					} else {
				
						//if owner end, otherwize keep running in same dir until owner or somebody else 'steals' the ball command
						if (footballer.impl.isBallOwner() || !footballer.impl.isLastBallOwner()) {
						
							mState = Result_Success;
						} 
					}
				
				} break;
			}
		}
	
		if (stateIsEndResult(mState)) 		
			return doReturn(t, mState);
		
		return doReturn(t, Result_Processing);	
	}
	
	mState = -1;
	mSearchDir = null;
	mSafePos = null;
	mRegion = null;
	mGoalReachPos = null;
	mTryKeepBall = false;
}

class AI_Goal_CoverStaticPos_From_Footballer extends AI_Goal {
	
	constructor() {
	
		mTarget = Vector3();
		mAttackerWatchMomentum = Vector3();
		
		mCoverEstimate = CCoverEstimate2D();
		mCoverEstimate.holdIdealPos3D(true);
		
		//use a pool ...
		mGoalReachPos = AI_Goal_ReachPos();
		mGoalReachPos.setIsDynamic(true);
		
		mGoalFace = AI_Goal_Face();
		
		mAttackerWatchReflexTime = 0.0;
	}
	
	function setTarget(point) {
	
		mTarget.set(point);
	}
	
	function setAttacker(footballer) {
	
		mAttacker = footballer;
		mAttackerWatchUpdateTime = 0.0;
	}
	
	function setRegion(region) {
	
		mRegion = region;
	}
	
	function setReflexTime(time) {
	
		mAttackerWatchReflexTime = time;
	}
	
	function setBallOwnerActionGoal(actionGoal) {
	
		mBallOwnerActionGoal = actionGoal;
	}
	
	function start(teamAI, brain, footballer, t) {
	
		{
			local match = footballer.impl.getMatch();
		
			mCoverEstimate.blockOffset = match.conv(1.5);
			mCoverEstimate.toleranceDist = match.conv(1.0);
			mCoverEstimate.frameInterval = match.getFrameInterval();
			
			mGoalReachPos.setEpsilon(mCoverEstimate.toleranceDist);
			mGoalReachPos.setToleranceMode(mGoalReachPos.Tolerance_Strict);
		}
	
		mIsInSlowWalkMode = false;
		mGoalReachPos.stop();
		mGoalFace.stop();
		
		return doReturn(t, Result_Processing);
	}
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		if (mBallOwnerActionGoal != null) {

			local useBallOwnerGoal;
		
			if (footballer.impl.isBallOwner()) {
			
				useBallOwnerGoal = true;
				
			} else {
			
				if (mBallOwnerActionGoal.isActive(t, dt) && footballer.impl.isLastBallOwner()) {
				
					useBallOwnerGoal = true;
				} 
			}
				
			if (useBallOwnerGoal) {
			
				local result = mBallOwnerActionGoal.update(teamAI, brain, footballer, t, dt);
			
				if (result != Result_Processing)
					brain.body_idle(footballer, t);
					
				return Result_Processing;
			}
		}
	
		local doMove = false;
		
		local isThinking;
		
		if (mAttackerWatchReflexTime != 0.0) {
		
			local momentum = mAttacker.getBodyMomentum();
			
			if (t - mAttackerWatchUpdateTime > mAttackerWatchReflexTime) {
			
				isThinking = false;
			
			} else {
			
				local change = momentum.dot(mAttackerWatchMomentum);
				
				if (change == 0.0) {
				
					isThinking = mAttacker.hasBodyMomentum() || !mAttackerWatchMomentum.isZero();
				
				} else {
				
					isThinking = change < 0.0;
				}
			}
			
		} else {
		
			isThinking = false;
		}
		
		if (!isThinking) {
			
			mAttackerWatchUpdateTime = t;
			mAttackerWatchMomentum.set( mAttacker.getBodyMomentum());
		}
		
		if (isThinking) {
		
			doMove = false;
		
		} else {
		
			local runSettings = footballer.settings.run;
			local maxRunSpeed = runSettings.getSpeedValue(runSettings.Run_Normal, runSettings.RunSpeed_Normal);
			
			if (updateIdealPos2D(brain, footballer, t, dt, mAttacker.impl.getPos(), mAttackerWatchMomentum, maxRunSpeed)) {
				
				doMove = true;
				
				mGoalReachPos.setTarget(footballer, mCoverEstimate.idealPos3D());
											
			} else {
			
				doMove = false;
			}
		}
	
		if (doMove) {
		
			local result = mGoalReachPos.update(teamAI, brain, footballer, t, dt);
		
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
			
				mGoalFace.setTargetByPos(footballer, mAttacker.impl.getPos());
			}
		
			mGoalFace.update(teamAI, brain, footballer, t, dt);
			
		} else {
		
			brain.body_updateSetIsStrafing(footballer, t, false);
		}
							
		return doReturn(t, Result_Processing); 
	}
	
	function enterSlowWalkMode(brain, footballer, t) {
	
		if (mAttackerWatchMomentum.isZero()) {
		
			return false;
		}
		
		local minDot = 0.5 * mAttackerWatchMomentum.mag();
							
		if (mAttackerWatchMomentum.dot(footballer.impl.getFacing()) > minDot) {
		
			brain.body_updateSetIsStrafing(footballer, t, true);
		
			return true;
		}
		
		return false;
	}
	
	function updateIdealPos2D(brain, footballer, t, dt, attackerPos, attackerVel, maxRunSpeed) { 
	
		mCoverEstimate.estimate3DWithRegion(footballer.impl.getMatch(), mRegion, attackerPos, attackerVel, mTarget, getZeroVector3(), 
											footballer.impl.getPos(), maxRunSpeed, mCoverEstimate.idealPos3D(), footballer.settings.PitchHeight);

		return true;										
	}
	
	mAttacker = null;
	mTarget = null;
	mRegion = null;
	
	mAttackerWatchReflexTime = 0.0;
	mAttackerWatchUpdateTime = 0.0;
	mAttackerWatchMomentum = null;
	
	mCoverEstimate = null;
	
	mGoalReachPos = null;
	mGoalFace = null;
	
	mBallOwnerActionGoal = null;
	
	mIsInSlowWalkMode = -1;
}

class AI_Goal_GoalieJumpAtBall extends AI_Goal {

	constructor() {
	
		mJumpStartState = BodyActionJumping_StartState();
		mJumpStartState.jumpDir = Vector3();
	}
		
	function start(teamAI, brain, footballer, t) {
	
		local match = footballer.impl.getMatch();
		local pos = footballer.impl.getPos();
		local dim = match.Scene_Fwd;
		local jumpSettings = footballer.settings.jump;
		
		local targetTimeHolder = Float(0.0);
		local targetPos = Vector3();
				
		if (!match.estimateBallStateAtPlane(pos, footballer.impl.getFacing(), targetPos, targetTimeHolder, true)) {
		
			return doReturn(t, Result_Failure);
		}
	
		local targetTime = targetTimeHolder.get();
	
		mIsJumping = false;
		mJumpStartTime = t;
		
		local jumpVect = Vector3();
		
		targetPos.subtract(pos, jumpVect);
		jumpVect.setEl(match.Scene_Up, 0.0);
		
		local apogeeDistance = jumpVect.mag();
		local jumpDistance = 0.0;
		
		if (apogeeDistance <= 0.0)
			return doReturn(t, Result_Failure);
		
		jumpVect.div(apogeeDistance, mJumpStartState.jumpDir);
		mJumpStartState.jumpType = footballer.getActionJumping().detectJumpType(footballer, null, mJumpStartState.jumpDir);			
		
		{
			local handPosAtApogee = jumpSettings.FlyPhase_HandPosAtApogee;
		
			mJumpStartState.jumpHeight = targetPos.getEl(match.Scene_Up) - handPosAtApogee.getEl(match.Scene_Up);
				
			if (mJumpStartState.jumpHeight < 0.0) {
			
				//print(mJumpStartState.jumpHeight);
				mJumpStartState.jumpHeight = 0.0;
			}
			
			local cappedJumpHeight = minf(mJumpStartState.jumpHeight, footballer.settings.jump.JumpDistance_MaxHeight);
			mJumpStartState.jumpHeight = cappedJumpHeight;
			
			jumpDistance = jumpSettings.jumpProfile.calcTotalDistForApogeeDist(apogeeDistance);
		}
		
		switch (mJumpStartState.jumpType) {
		
			case FootballerSettings_Jump.Jump_Left:
			case FootballerSettings_Jump.Jump_Right: {
			
				local cappedJumpedDistance = minf(jumpDistance, footballer.settings.jump.JumpDistance_Default);
				mJumpStartTime = (targetTime - jumpSettings.jumpProfile.ApogeeTime) - (match.getFrameInterval() * 1.5); 
				mJumpStartState.jumpDistance = cappedJumpedDistance;
			
				local reactionTime = mJumpStartTime - t;
				local minReactionTime = footballer.getAIReactionTime();
				
				if (reactionTime < minReactionTime) {
				
					mJumpStartTime = t + minReactionTime;
				}
			
			} break;
			
					
			default: {
			
				return doReturn(t, Result_Failure);
			
			} break;
		}
		
		return doReturn(t, Result_Processing);
	}
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		if (t >= mJumpStartTime) {
		
			if (mIsJumping) {

				if (brain.body_isIdle(footballer))
					return doReturn(t, Result_Success);
				
			} else {
			
				if (footballer.getAction().switchJumping(footballer, t, mJumpStartState)) {
				
					mIsJumping = true;
				}
				
				return doReturn(t, Result_Processing);
			}
		} 
		
		brain.body_idle(footballer, t);
		
		return doReturn(t, Result_Processing); 
	}
	
	mJumpStartTime = 0.0;
	mIsJumping = false;
	mJumpStartState = null;
}

/*
class AI_Goal_StaticGoalie1 extends AI_Goal {

	static State_Thinking = 0;
	static State_TriggeringJump = 1;
	static State_Jumping = 2;
	static State_Idle = 3;
	
	constructor() {
	
		mGoalJumpAtBall = AI_Goal_GoalieJumpAtBall();
	}

	function start(teamAI, brain, footballer, t) { 
	
		mState = State_Thinking;

		brain.body_idle(footballer, t);		
		
		return doReturn(t, Result_Processing); 
	}
	
	function setIdleActionGoal(actionGoal) {
	
		mIdleActionGoal = actionGoal;
	}
		
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		local repeat = true;
		local ballState = footballer.impl.getMatch().getSpatialBallState();
		
		while (repeat) {
		
			repeat = false;
	
			switch (mState) {
			
				case State_Thinking: {
				
					local doTriggerJump = false;
				
					switch (ballState.finalGetState()) {
					
						case ballState.FinalBall_Goalie: {
						
							if (ballState.finalGetPitchInTeam() == footballer.impl.getTeamEnum()) {
							
								doTriggerJump = true;
							}
							
						} break;
						
						case ballState.FinalBall_Corner: {
						
							if (ballState.finalGetPitchInTeam() != footballer.impl.getTeamEnum()) {
							
								doTriggerJump = true;
							}
							
						} break;
						
						case ballState.FinalBall_InPlay:
						case ballState.FinalBall_ThrowIn: {
						
							mState = State_Idle;
							
						} break;
						
						default: {
						
							mState = State_Idle;
						}
					}
					
					if (doTriggerJump) {
					
						mState = State_TriggeringJump;
						repeat = true;
					} 
					
					if (mState == State_Thinking) {
					
						brain.body_idle(footballer, t);
						
					} else {
					
						mBallStateID = ballState.finalGetID();
					}
				
				} break;
				
				case State_TriggeringJump: {
				
					if (mBallStateID == ballState.finalGetID()) {
					
						if (mGoalJumpAtBall.start(teamAI, brain, footballer, t) == Result_Processing) {
							
							mState = State_Jumping;
							repeat = true;
							
						} else {
						
							brain.body_idle(footballer, t);
						}
						
					} else {
					
						mState = State_Thinking;
						repeat = true;
					}
				
				} break;

			
				case State_Jumping: {
				
					if (mGoalJumpAtBall.frameMove(teamAI, brain, footballer, t, dt) != Result_Processing) {
					
						mState = State_Idle;
					}
				
				} break;
			
				case State_Idle: {
				
					if (mBallStateID != ballState.finalGetID()) {
					
						mState = State_Thinking;
						repeat = true;
						
					} else {
					
						if (mIdleActionGoal != null) {
						
							local result = mIdleActionGoal.update(teamAI, brain, footballer, t, dt);
						
							if (result != Result_Processing) {
							
								brain.body_idle(footballer, t);
							}
						
						} else {
					
							brain.body_idle(footballer, t);
						}
					}
				
				} break;
			}
					
		}
		
		return doReturn(t, Result_Processing); 
	}
	
	mGoalJumpAtBall = null;
	mState = -1;
	mBallStateID = -1;
	
	mIdleActionGoal = null;
}
*/

class AI_Job_Goalie extends AI_Goal {

	static State_Thinking = 0;
	static State_TriggeringJump = 1;
	static State_Jumping = 2;
	static State_Idle = 3;
	
	constructor() {
	
		mGoalJumpAtBall = AI_Goal_GoalieJumpAtBall();
	}

	function start(teamAI, brain, footballer, t) { 
	
		mState = State_Thinking;

		brain.body_idle(footballer, t);		
		
		return doReturn(t, Result_Processing); 
	}
	
			
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		local repeat = true;
		local ballState = footballer.impl.getMatch().getSpatialBallState();
		
		while (repeat) {
		
			repeat = false;
	
			switch (mState) {
			
				case State_Thinking: {
				
					local doTriggerJump = false;
				
					switch (ballState.finalGetState()) {
					
						case ballState.FinalBall_Goalie: {
						
							if (ballState.finalGetPitchInTeam() == footballer.impl.getTeamEnum()) {
							
								doTriggerJump = true;
							}
							
						} break;
						
						case ballState.FinalBall_Corner: {
						
							if (ballState.finalGetPitchInTeam() != footballer.impl.getTeamEnum()) {
							
								doTriggerJump = true;
							}
							
						} break;
						
						case ballState.FinalBall_InPlay:
						case ballState.FinalBall_ThrowIn: {
						
							mState = State_Idle;
							
						} break;
						
						default: {
						
							mState = State_Idle;
						}
					}
					
					if (doTriggerJump) {
					
						mState = State_TriggeringJump;
						repeat = true;
					} 
					
					if (mState == State_Thinking) {
					
						brain.body_idle(footballer, t);
						
					} else {
					
						mBallStateID = ballState.finalGetID();
					}
				
				} break;
				
				case State_TriggeringJump: {
				
					if (mBallStateID == ballState.finalGetID()) {
					
						if (mGoalJumpAtBall.start(teamAI, brain, footballer, t) == Result_Processing) {
							
							mState = State_Jumping;
							repeat = true;
							
						} else {
						
							mState = State_Idle;
							repeat = true;
						}
						
					} else {
					
						mState = State_Thinking;
						repeat = true;
					}
				
				} break;

			
				case State_Jumping: {
				
					if (mGoalJumpAtBall.frameMove(teamAI, brain, footballer, t, dt) != Result_Processing) {
					
						mState = State_Idle;
						repeat = true;
					}
				
				} break;
			
				case State_Idle: {
				
					if (mBallStateID != ballState.finalGetID()) {
					
						mState = State_Thinking;
						repeat = true;
						
					} else {
					
						brain.frameMoveIdle(teamAI, footballer, t, dt);
					}
				
				} break;
			}
					
		}
		
		return doReturn(t, Result_Processing); 
	}
	
	mGoalJumpAtBall = null;
	mState = -1;
	mBallStateID = -1;
}

class AI_Job_ZoneDefend extends AI_Goal {

	static State_Waiting = -1;
	static State_ChoosingFootballer = 0;
	static State_Covering = 1;
	static State_HandlingBall = 2;

	function setRegion(region) {
	
		mRegion = region;
	}
	
	function setTrackedRegion(region) {
	
		mTrackedRegion = region;
	}

	function start(teamAI, brain, footballer, t) { 
	
		mCovering_Goal = AI_Goal_CoverStaticPos_From_Footballer();
	
		mCoveredFootballer = null;
		mState = State_Waiting;
	
		return doReturn(t, Result_Processing); 
	}
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		local repeat = true;
		local justQuit = false;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_Waiting: {
					
					if (mWaiting_RegionTrack != null) {
					
						if (!mWaiting_RegionTrack.isTracking()) {	
						
							if (mWaiting_RegionTrack.isValid()) {

								local fbArray = mWaiting_RegionTrack.getFootballerArray();
								
								if (fbArray != null && fbArray.len() > 0) {
								
									mChoosing_Requests = [];
								
									foreach (opp in fbArray) {

										//print("request " + footballer.impl.getNodeName() + " -> " + opp.impl.getNodeName());
									
										local request = teamAI.addOpponentFlagRequest(t, dt, footballer, opp, AI_RequestManager.OppRequest_ZoneDefend, true);
										
										if (request != null) {
										
											//print("requested " + footballer.impl.getNodeName() + " -> " + opp.impl.getNodeName());
											mChoosing_Requests.append(request);
										}
									}

									if (mChoosing_Requests.len() > 0) {
									
										mState = State_ChoosingFootballer;
									}
								}
							}
							
							mWaiting_RegionTrack = null;
						}						
					}
				
					if (mState == State_Waiting) {
					
						if (mWaiting_RegionTrack == null) 
							mWaiting_RegionTrack = teamAI.addRegionTrack(t, footballer, mTrackedRegion);
					
						brain.frameMoveIdle(teamAI, footballer, t, dt);
					}
					
				} break;
				
				case State_ChoosingFootballer: {
				
					if (mChoosing_Requests == null) {
					
						mState = State_Waiting;
						repeat = true;
						
					} else {
					
						local grantedRequest = null;
						local refusedCount = 0;
					
						for (local i = 0; i < mChoosing_Requests.len(); i += 1) {
						
							local request = mChoosing_Requests[i];
						
							if (!request.isWaiting()) {
							
								if (request.isGranted() && (request.getTargetFootballer() != null)) {
								
									grantedRequest = request;
									break;
									
								} else {
								
									refusedCount += 1;
								}
							}
						}
						
						if (refusedCount == mChoosing_Requests.len()) {
						
							mChoosing_Requests = null;
							repeat = true;
							
						} else {
						
							if (grantedRequest != null) {
							
								mCoveredFootballer = grantedRequest.getTargetFootballer();
								mFlagZoneDefend = teamAI.addOpponentFlag(t, footballer, mCoveredFootballer, AI_RequestManager.OppRequest_ZoneDefend);
								mState = State_Covering;
								repeat = true;
								
								//print("granted " + footballer.impl.getNodeName() + " -> " + mCoveredFootballer.impl.getNodeName());
							} 
						}
					}
					
					if (mState == State_ChoosingFootballer)
						brain.frameMoveIdle(teamAI, footballer, t, dt);
				
				} break;
				
				case State_Covering: {
				
					local match = footballer.impl.getMatch();
				
					if (!mCovering_Goal.isActive(t, dt)) {
					
						local targetPos = Vector3();
		
						sceneSetRUF(targetPos, 0.0, 0.0, -match.getPitchHalfLength());
						footballer.impl.getTeam().teamLocalToWorld3DInPlace(targetPos);
		
						mCovering_Goal.setAttacker(mCoveredFootballer);
						mCovering_Goal.setTarget(targetPos);
						mCovering_Goal.setRegion(mRegion);
						mCovering_Goal.setReflexTime(footballer.getAIReactionTime());
						mCovering_Goal.setBallOwnerActionGoal(null);
					}
					
					if (mCovering_Goal.update(teamAI, brain, footballer, t, dt) == Result_Processing) {
					
						mFlagZoneDefend.update(t);
						
					} else {
					
						mFlagZoneDefend.end();
						brain.body_idle(footballer, t);
					}
					
					if (!mTrackedRegion.contains(match, mCoveredFootballer.impl.getPos())) {
					
						//print("quit " + footballer.impl.getNodeName() + " -> " + mCoveredFootballer.impl.getNodeName());
					
						mFlagZoneDefend.end();
						mCovering_Goal.stop();
						mState = State_Waiting;
						repeat = true;
					}
					
				} break;
				
				case State_HandlingBall: {
				} break;
			}
		}
	
		return doReturn(t, Result_Processing); 
	}
		
	mState = -1;	
	mRegion = null;
	mTrackedRegion = null;
	
	mWaiting_RegionTrack = null;
	mChoosing_Requests = null;
	
	mCoveredFootballer = null;
	mFlagZoneDefend = null;
	mCovering_Goal = null;
}

/*
	such a state can be transformed into a generic goal state machine
	with options per state such as pool usage, parameters, constrcutor, goal state...
*/
class AI_Job_GotoFormation extends AI_Goal {

	static State_None = -1;
	static State_Going = 0;
	static State_Idling = 1;
	static State_Facing = 2;
	static State_Ended = 3;
	
	function start(teamAI, brain, footballer, t) { 
	
		mState = State_None;
		
		if (mCurrGoal != null) {
		
			mCurrGoal = teamAI.getAIGoalPool().put(mCurrGoal);
		}
	
		return doReturn(t, Result_Processing); 
	}
	
	function frameMove(teamAI, brain, footballer, t, dt) { 
	
		local repeat = true;
		
		while (repeat) {
		
			repeat = false;
		
			switch (mState) {
			
				case State_None: {
				
					local targetPos = Vector3();
					
					if (footballer.impl.getFormationPos(targetPos)) {
				
						local pool = teamAI.getAIGoalPool();
												
						local goalReachPos = pool.get(AI_Goal_ReachPos);
						goalReachPos.setTarget(footballer, targetPos);
						goalReachPos.setEpsilon(0.0);
						goalReachPos.setIsDynamic(false);
						goalReachPos.setToleranceMode(goalReachPos.Tolerance_Dist);
					
						if (mCurrGoal != null)
							mCurrGoal = pool.put(mCurrGoal);
					
						mCurrGoal = goalReachPos;
						mState = State_Going;
						repeat = true;
						
					} else {
					
						brain.body_idle(footballer, t);
					}
									
				} break;
				
				case State_Going: {
				
					local result = mCurrGoal.update(teamAI, brain, footballer, t, dt);
					
					if (result == Result_Processing) {
					
						brain.body_updateSetIsStrafing(footballer, t, false);
					
					} else {
					
						local pool = teamAI.getAIGoalPool();
						mCurrGoal = pool.put(mCurrGoal);
					
						if (result == Result_Success) {
						
							mState = State_Idling;
							repeat = true;
												
						} else {
						
							mState = State_None;
						}
					}
				
				} break;
				
				case State_Idling: {
				
					if (brain.body_isIdle(footballer)) {
					
						local pool = teamAI.getAIGoalPool();
					
						local goalFace = pool.get(AI_Goal_Face);
						goalFace.setTargetByDir(footballer, footballer.impl.getTeamFwdDir());
					
						mCurrGoal = goalFace;
						mState = State_Facing;
						repeat = true;
					
					} else {
					
						brain.body_idle(footballer, t);
					}
				
				} break;
				
				case State_Facing: {
				
					local result = mCurrGoal.update(teamAI, brain, footballer, t, dt);
					
					if (result != Result_Processing) {
					
						local pool = teamAI.getAIGoalPool();
						mCurrGoal = pool.put(mCurrGoal);
					
						if (result == Result_Success) {
						
							mState = State_Ended;
							repeat = true;
						
						} else {
						
							mState = State_None;
						}
					}
				
				} break;
				
				case State_Ended: {
				
					return doReturn(t, Result_Success);
				
				} break;
			}
		}
	
		return doReturn(t, Result_Processing);									
	}
	
	mState = -1;
	mCurrGoal = null;
}

class FootballerBrain_AI_BrainBodyInterface extends FootballerBrainBase {

	static mReflexTimeDir = 0.15;
	static mReflexTimeIdle = 0.34;
	static mReflexTimeStrafe = 0.15;
	
	function body_isIdle(footballer) {

		return (footballer.getActionType() == footballer.Action_Idle);
	}
	
	function body_idle(footballer, t) {

		if (footballer.getActionType() == footballer.Action_Idle)
			return true;

		if (t - mIdleRunUpdateTime > mReflexTimeIdle) {
		
			mIdleRunUpdateTime = t;
		
			footballer.getAction().switchIdle(footballer, t, null);
		}
		
		return false;
	}

	function body_updateSetIsStrafing(footballer, t, isStrafing) {
	
		local actionRunning = footballer.getActionRunning();
	
		if (actionRunning.getIsStrafing == isStrafing)
			return true;
	
		if (t - mStrafeUpdateTime > mReflexTimeStrafe) {
	
			local actionRunning = footballer.getActionRunning();
			actionRunning.setIsStrafing(isStrafing);
			
			mStrafeUpdateTime = t;
			
			return true;
		}
		
		return false;
	}
	
	function body_updateStraightRunDir(footballer, t, dir) {
		
		if (t - mRunDirUpdateTime > mReflexTimeDir) {
		
			mRunDirUpdateTime = t;
		
			local actionRunning = footballer.getActionRunning();
			
			local currRunDir = actionRunning.getRunDir();
			local runTypeIsStraight = actionRunning.getRunTypeIsStraight();
			
			if (!runTypeIsStraight || (!currRunDir.equals(dir))) {
			
				actionRunning.setBodyFacing(footballer, dir);
				actionRunning.setRunDir(footballer, dir);
			}
			
			return true;
			
		} else {
		
			return false;
		}
	}

	mRunDirUpdateTime = 0.0;
	mStrafeUpdateTime = 0.0;
	mIdleRunUpdateTime = 0.0;
}


class FootballerBrain_AI_v1 extends FootballerBrain_AI_BrainBodyInterface {

	static IdleAction_None = -1;
	static IdleAction_Idle = 0;
	static IdleAction_Formation = 1;

	constructor() {
	
		//mActiveGoals = {};
	}	
	
	function canSwitchOff(teamAI, footballer, t) {
	
		return !mIsSwitchLocked;
	}

	function start(teamAI, footballer, t) {
	
		if (t == 0.0) {
		
			if (!loadAction(teamAI, footballer, t))
				return false;
		}
	
		body_idle(footballer, t);
	
		return true;
	}
	
	function stop(teamAI, footballer, t) {
	
		body_idle(footballer, t);
	}
	
	function onSwitchedTeamFormation(teamAI, footballer, t) {
	
		/*
		if (mCurrGoal == null) {
		
			local pos = Vector3();
			
			if (footballer.impl.getFormationWorldPos(pos)) {
			
				setGoalReachPos(footballer, t, pos, 0.0);
				
				local goalFace = AI_Goal_Face();
				goalFace.setTargetByDir(footballer, footballer.impl.getTeamForwardWorldDir());
				
				mCurrGoal.chainGoal(goalFace);
			}
		}
		*/
	}
	
	/*
	function signalActiveGoal(goal) {
	
		if (!(goal in mActiveGoals)) {
		
			mActiveGoals[goal] <- 0;
		}
	}
	
	function signalStoppedGoal(goal) {
	
		goal.stop();
	}
	
	function checkZombieGoals(t, dt) {
	
		local activeT = AI_Goal.calcActiveTime(t, dt);
	
		foreach (goal in mActiveGoals) {
		
			if (!goal.isActiveTime(activeT)) {
			
				signalStoppedGoal(goal);
				delete mActiveGoals[goal]; //is this allowed inside foreach?
			}
		}
	}
	*/
	
	function frameMoveIdle(teamAI, footballer, t, dt) {
	
		local executedAction = false;
	
		switch (mIdleActionType) {
		
			case IdleAction_Formation: {
			
				if (mIdleActionGoal == null) {
				
					if ((mLastIdleActionUpdateTime + 1.5 * dt) < t) {
				
						mIdleActionFormationNextTime = t + randf(0.0, 8.0);
					}
					
					if (t > mIdleActionFormationNextTime) {
					
						mIdleActionGoal = teamAI.getAIGoalPool().get(AI_Job_GotoFormation);
						
					} 
				}
			
				if (mIdleActionGoal != null) {
					
					executedAction = true;
					mIdleActionGoal = frameMoveGoal(mIdleActionGoal, teamAI, footballer, t, dt, teamAI.getAIGoalPool());
				}
			
			} break;
		}
		
		if (!executedAction)
			body_idle(footballer, t);
	
		mLastIdleActionUpdateTime = t;
	}
	
	function signalNotIdle() {
	}
	
	function frameMoveGoal(goal, teamAI, footballer, t, dt, pool) {
	
		if (goal != null) {
		
			local result = goal.update(teamAI, this, footballer, t, dt);
				
			if (result != AI_Goal.Result_Processing) {
			
				if (result == AI_Goal.Result_Success) {
				
					if (pool != null)
						pool.put(goal);
						
					goal = goal.getChainedGoal();
											
				} else {
			
					if (pool != null) {
			
						local chainedGoal = goal.getChainedGoal();
						pool.put(goal);
						
						while (chainedGoal != null) {
						
							pool.put(chainedGoal);
							chainedGoal = chainedGoal.getChainedGoal();
						}
					}
									
					goal = null;
				}
			}
		}
		
		return goal;
	}
	
	function frameMove(teamAI, footballer, t, dt) {

		mCurrGoal = frameMoveGoal(mCurrGoal, teamAI, footballer, t, dt, null);
	
		if (mCurrGoal == null) {
		
			frameMoveIdle(teamAI, footballer, t, dt);
		}
		
		if ((mLastIdleActionUpdateTime + 1.5 * dt) < t) {
		
			signalNotIdle();
		}
	}

	function cancelGoal(footballer, t) {
	
		mCurrGoal = null;
	}
	
	function load(teamAI, footballer, chunk, conv) {
	
		return true;
	}
	
	function loadAction(teamAI, footballer, t) {
	
		mIdleActionType = IdleAction_Formation;
	
		local match = footballer.impl.getMatch();
		
		local formationChunk = footballer.impl.getFormationPositionChunk();
		local tryUseDefaultJob = true;
		
		if (formationChunk.isValid()) {

			formationChunk.toChild("job");
		
			if (formationChunk.isValid()) {
			
				tryUseDefaultJob = false;
			
				switch (formationChunk.get("type")) {
				
					case "ZoneDefence": {
					
						local region = footballer.impl.getRegion(formationChunk.get("region"));
						local trackedRegion = footballer.impl.getRegion(formationChunk.get("trackedRegion"));
						
						if (region != null && trackedRegion != null) {
						
							setJobZoneDefend(teamAI, footballer, t, region, trackedRegion);
							
						} else {
						
							if (region == null)
								log("region '" + formationChunk.get("region") + "' not found for footballer " + footballer.impl.getNodeName());
								
							if (trackedRegion == null)
								log("trackedRegion '" + formationChunk.get("region") + "' not found for footballer " + footballer.impl.getNodeName());
								
							return false;	
						}
					
					} break;
				}
			}
			
		} 
		
		if (tryUseDefaultJob) {
		
			local code = footballer.impl.getFormationPositionCode();
							
			switch (code) {
			
				case "goalie": {
			
					mIsSwitchLocked = true;
					setJobGoalie(teamAI, footballer, t);
					
				} break;
			}
		}
		
		return true;
	}
		
	function createGoalFace(footballer, t, faceDir) {
	
		local goal = AI_Goal_Face();
		goal.setTargetByDir(footballer, t, faceDir);
		
		return goal;
	}
	
	function createGoalFaceByQueuePos(footballer, t, queuedPos) {
	
		local goal = AI_Goal_Face();
		goal.setTargetByQueuedPos(footballer, queuedPos);
		
		return goal;
	}
	
	function createGoalReachPos(footballer, t, pos, epsilon) {
	
		local goal = AI_Goal_ReachPos();
		goal.setEpsilon(epsilon);
		goal.setTarget(footballer, pos);
		
		return goal;
	}
	
	function createGoalInterceptBall(footballer, t, maxAnalysisAge, epsilon) {
	
		local goal = AI_Goal_GainBallOwnership();
		goal.setMaxAnalysisAge(maxAnalysisAge);
		goal.setEpsilon(epsilon);
		
		return goal;
	}
	
	function createGoalFireShot(footballer, t, shotType, shotSpeedScale) {
	
		local goal = AI_Goal_FireShot();
		goal.setShot(shotType, footballer.settings.getShot(footballer.settings.DefaultShotLevel, shotType), shotSpeedScale);
		
		return goal;
	}
	
	function createGoalOwnBall(footballer, t, maxAnalysisAge, epsilon) {
	
		local goal = AI_Goal_OwnBall();
		goal.setMaxAnalysisAge(maxAnalysisAge);
		goal.setEpsilon(epsilon);
		
		return goal;
	}
	
	function createGoalFireShot(footballer, t, shotType, shotSpeedScale) {
	
		local goal = AI_Goal_FireShot();
		goal.setShot(shotType, footballer.settings.getShot(footballer.settings.DefaultShotLevel, shotType), shotSpeedScale);
		
		return goal;
	}
	
	function createSetupFireShotGoal(footballer, t, fireShotGoal, target, shotType, allowWait, allowUnpreciseShot, isByTime, targetTime, targetSpeed) {
	
		local goal = AI_Goal_SetupFireShotGoal();
		
		if (isByTime)
			goal.setupByTime(fireShotGoal, target, targetTime, shotType, allowWait, allowUnpreciseShot);
		else
			goal.setupBySpeed(fireShotGoal, target, targetSpeed, shotType, allowUnpreciseShot);
		
		return goal;
	}
	
	function createGoalShootTo(footballer, t, target, shotType, allowWait, allowUnpreciseShot, isByTime, targetTime, targetSpeed) {
	
		local goal = createGoalInterceptBall(footballer, t, 0.0, 0.0);
		local topGoal = goal;
		local chainedGoal;
		
		chainedGoal = createGoalFaceByQueuePos(footballer, t, target);
		goal.chainGoal(chainedGoal);
		goal = chainedGoal;
		
		local fireShotGoal = createGoalFireShot(footballer, t, shotType, 1.0);
		
		chainedGoal = createSetupFireShotGoal(footballer, t, fireShotGoal, target, shotType, allowWait, allowUnpreciseShot, isByTime, targetTime, targetSpeed);
		goal.chainGoal(chainedGoal);
		goal = chainedGoal;
		
		chainedGoal = fireShotGoal;
		goal.chainGoal(chainedGoal);
		goal = chainedGoal;
		
		return topGoal;
	}
	
	function createGoalGoToSafeSpace(footballer, t) {
	
		local goal = AI_Goal_GoToSafeSpace();
		
		return goal;
	}
	
	function createGoalCoverStaticPosFromFootballer(footballer, t, attackingFootballer, targetPos, region, reflexTime, ballOwnerActionGoal) {
	
		local goal = AI_Goal_CoverStaticPos_From_Footballer();
		
		goal.setAttacker(attackingFootballer);
		goal.setTarget(targetPos);
		goal.setRegion(region);
		goal.setReflexTime(reflexTime);
		goal.setBallOwnerActionGoal(ballOwnerActionGoal);
				
		return goal;
	}
	
	function createGoalGoalieJumpAtBall(footballer, t) {
	
		local goal = AI_Goal_GoalieJumpAtBall();
		
		return goal;
	}
	
	/*
	function createGoalStaticGoalie1(footballer, t, idleActionGoal) {
	
		local goal = AI_Goal_StaticGoalie1();
		
		goal.setIdleActionGoal(idleActionGoal);
		
		return goal;
	}
	*/
	
	function createJobGoalie(footballer, t) {
	
		local goal = AI_Job_Goalie();
		
		return goal;
	}
	
	function createJobZoneDefend(footballer, t, region, trackedRegion) {
	
		local goal = AI_Job_ZoneDefend();
		
		goal.setRegion(region);
		goal.setTrackedRegion(trackedRegion);
		
		return goal;
	}
		
	function setAndStartCurrGoal(teamAI, footballer, t, goal) {
	
		mCurrGoal = goal;
		
		//if (mCurrGoal.start(teamAI, footballer.getBrainAI(), footballer, t) != AI_Goal.Result_Processing) {
		//	mCurrGoal = null;
		//}
	}
	
	function setGoalReachPos(teamAI, footballer, t, pos, epsilon) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalReachPos(footballer, t, pos, epsilon));
	}
	
	function setGoalInterceptBall(teamAI, footballer, t, maxAnalysisAge, epsilon) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalInterceptBall(footballer, t, maxAnalysisAge, epsilon));
	}
	
	function setGoalOwnBall(teamAI, footballer, t, maxAnalysisAge, epsilon) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalOwnBall(footballer, t, maxAnalysisAge, epsilon));
	}
	
	function setGoalFireShot(teamAI, footballer, t, shotType, shotSpeedScale) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalFireShot(footballer, t, shotType, shotSpeedScale));
	}
	
	function setGoalShootTo(teamAI, footballer, t, target, shotType, allowWait, allowUnpreciseShot, isByTime, targetTime, targetSpeed) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalShootTo(footballer, t, target, shotType, allowWait, allowUnpreciseShot, isByTime, targetTime, targetSpeed));
	}
	
	function setGoalGoToSafeSpace(teamAI, footballer, t) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalGoToSafeSpace(footballer, t));
	}
	
	function setGoalCoverStaticPosFromFootballer(teamAI, footballer, t, attackingFootballer, targetPos, region, reflexTime, ballOwnerActionGoal) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalCoverStaticPosFromFootballer(footballer, t, attackingFootballer, targetPos, region, reflexTime, ballOwnerActionGoal));
	}
		
	function setGoalGoalieJumpAtBall(teamAI, footballer, t) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalGoalieJumpAtBall(footballer, t));
	}	
	
	/*
	function setGoalStaticGoalie1(teamAI, footballer, t, idleActionGoal) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createGoalStaticGoalie1(footballer, t, idleActionGoal));
	}
	*/
	
	function setJobGoalie(teamAI, footballer, t) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createJobGoalie(footballer, t));
	}	
	
	function setJobZoneDefend(teamAI, footballer, t, region, trackedRegion) {
	
		setAndStartCurrGoal(teamAI, footballer, t, createJobZoneDefend(footballer, t, region, trackedRegion));
	}
		
	mCurrGoal = null;
	mIsSwitchLocked = false;
	//mActiveGoals = null;

	mIdleActionType = -1;
	mIdleActionGoal = null;
	mLastIdleActionUpdateTime = -1.0;
	mIdleActionFormationNextTime = -1.0;
}