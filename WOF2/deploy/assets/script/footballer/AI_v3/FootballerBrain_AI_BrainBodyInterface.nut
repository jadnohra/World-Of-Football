

class FootballerBrain_AI_BrainBodyInterface_BallOwnState {

	constructor() {
	
		mEstimate = CBallInterceptEstimate();
		mEstimate.enableAnalysis(true, true);
		mEstimate.enableBallOutOfPlayEstimates(false);
		mEstimateMode = mEstimate.Analyse_None;
		mState = ::FootballerBrain_AI_BrainBodyInterface.OwnBall_None;
		mBodyFacing = null;
	}

	static mAnalysisDelay = 0.15;
	mEstimate = null;
	mEstimateMode = -1;
	mNextAnalysisTime = -1.0;
	mChaseInstantaneousBall = false;
	mState = -1;
	mBodyFacing = null;
}

class FootballerBrain_AI_BrainBodyInterface_JumpState {

	constructor(body) {
	
		local match = body.mImpl.getMatch();
	
		mReachabilityProfile = FootballerBrain_AI_JobGoalie_ReachabilityProfile();
		mReachabilityProfile.setup(match.getFrameTickLength() * 2.0, 1.5, null);
	}
	
	function setup(body, t, targetPos, targetTime, targetRadius, checkRedundant) {
	
		return mReachabilityProfile.analyze(match, body, t, targetPos, targetTime, targetRadius, checkRedundant);
	}
	
	function frameMove(brain, body, t, dt) {
	
		//print(mReachabilityProfile.toString(t));
		local state = mReachabilityProfile.process(match, brain, body, t, dt);
	
		return state;
	}

	mReachabilityProfile = null;
}

class FootballerBrain_AI_BrainBodyInterface extends FootballerBrainBase {

	static mReflexTimeDir = 0.15;
	static mReflexTimeIdle = 0.34;
	static mReflexTimeStrafe = 0.15;
	
	static BodyFacing_None = -1;
	static BodyFacing_Dir = 0;
	static BodyFacing_LookAt = 1;
	
	static Destination_None = -1;
	static Destination_Pos = 0;
	static Destination_OwnBall = 1;
	static Destination_Reaching = 2;
	static Destination_Reached = 3;
	static Destination_ReachedExpired = 4;
	
	static OwnBall_None = -1;
	static OwnBall_Estimating = 0;
	static OwnBall_Moving = 1;
		
	
	constructor() {
	
		mTempVect3 = Vector3();
	
		mDestination = Vector3();
		mDestinationDir = Vector3();

		mBodyFacing = Vector3();
		mBodyFacingLookAt = Vector3();
	}
	
	function load(body, chunk, conv) {
	
		mDefaultHumanizeRadius = body.mImpl.getMatch().conv(0.5);
	}
	
	function getDefaultHumanizeRadius() { return mDefaultHumanizeRadius; }
	
	function isIdle(body) {
	
		return (body.getActionType() == ::FootballerBody.Action_Idle);
	}
	
	function idle(body, t) {

		cancelDestination(body, t);
		cancelBodyFacing(body, t);
	}
	
	function cancelDestination(body, t) {
	
		mHasDestination = false;
		mDestinationOwnBallState = null;
	}
	
	function hasDestination() {
	
		return mHasDestination;
	}
	
	function hasDestinationPos() {
	
		return (mHasDestination == true && mDestinationType == Destination_Pos);
	}
	
	function getDestinationPos() {
	
		return mDestination;
	}
		
	function getDestinationIDState(destinationID, expireOnReached) {
	
		if (mDestinationID == destinationID) {
		
			switch (mDestinationIDState) {

				case Destination_Reaching: {
			
					return hasDestinationPos() ? mDestinationIDState : Destination_None;
					
				} break;
				
				case Destination_Reached: {
			
					if (expireOnReached)
						mDestinationIDState = Destination_ReachedExpired;
			
					return Destination_Reached;
					
				} break;
			
			} 
			
			return mDestinationIDState;
		}
		
		return Destination_None;
	}
	
	function cancelGoalieRunJump(body, t) {
	
		mGoalieRunJumpState = null;
	}
	
	function setGoalieRunJumpLookAt(lookAt) {
	
		if (mGoalieRunJumpState != null) {
		
			mGoalieRunJumpState.mReachabilityProfile.setLookAt(lookAt);
		}
	}
	
	function setGoalieRunJump(body, t, targetPos, targetTime, targetRadius) {
	
		if (mGoalieRunJumpState == null) {
	
			mGoalieRunJumpState = FootballerBrain_AI_BrainBodyInterface_JumpState(body);
		}
		
		return mGoalieRunJumpState.setup(body, t, targetPos, targetTime, targetRadius, true);
	}
	
	function setDestination(body, t, destination, needsFixing, arrivalTime, arrivalTimeImportance, humanizeRadius) {
		
		cancelGoalieRunJump(body, t);
		
		{
			local checkArrival;
		
			if (mHasDestination && mDestinationType != Destination_Pos)
				checkArrival = false;
			else
				checkArrival = true;
		
			if (checkArrival) {
		
				local runSettings = body.mProfile.run;
				
				if (isWithinRunTolerance(runSettings, body, t, destination, runSettings.Run_Normal, humanizeRadius == null ? null : humanizeRadius * humanizeRadius)) {
				
					return mDestinationID;
				}
			}
		}
		
		mDestinationID += 1;
		mDestinationIDState = Destination_Reaching;
				
		mHasDestination = true;
		changeDestinationPos(body, t, destination, null, needsFixing);
		mDestinationType = Destination_Pos;
				
		mDestinationArrivalTime = arrivalTime;
		mDestinationArrivalTimeImprotance = arrivalTimeImportance;
		
		mDestinationOwnBallState = null;
		mDestinationEpsilonSq = 0.0;
		
		return mDestinationID;
	}
	
	function hasDestinationOwnBall() {
	
		return (mHasDestination == true && mDestinationType == Destination_OwnBall);
	}
	
	function hasValidDestinationOwnBall() {
	
		if (mHasDestination == true && mDestinationType == Destination_OwnBall) {
		
			return ((mDestinationOwnBallState != null) 
					&& (mDestinationOwnBallState.mState == OwnBall_Moving));
		}
		
		return false;
	}
	
	function willHaveValidDestinationOwnBall(body, maxWaitTime) {
	
		if (mHasDestination == true && mDestinationType == Destination_OwnBall && mDestinationOwnBallState != null) {
		
			switch (mDestinationOwnBallState.mState) {
			
				case OwnBall_Estimating: {
				
					local timeUntilValidity = mDestinationOwnBallState.mEstimate.estimateTimeUntilValidity(body.mImpl);
					
					if (timeUntilValidity < 0.0)
						return false;
							
					if (timeUntilValidity < maxWaitTime)	
						return true;							
				
				} break;
				
				case OwnBall_Moving: {
				
					return true;
				
				} break;
			}
		}
		
		return false;
	}
	
	function getOwnBallState() {
	
		return mDestinationOwnBallState.mState;
	}
	
	function setDestinationOwnBall(body, t, chaseInstantaneousBall) {
		
		cancelGoalieRunJump(body, t);
		
		mHasDestination = true;
		mDestinationType = Destination_OwnBall;
		
		if (mDestinationOwnBallState == null) {
		
			mDestinationOwnBallState = FootballerBrain_AI_BrainBodyInterface_BallOwnState();
		}
		
		mDestinationOwnBallState.mChaseInstantaneousBall = chaseInstantaneousBall;
	}
	
	function cancelBodyFacing(body, t) {
	
		mHasBodyFacing = false;
	}
	
	function setBodyFacing(body, t, bodyFacing, needsFixing, tolerance, importance) {
	
		cancelGoalieRunJump(body, t);
	
		mHasBodyFacing = true;
		
		mBodyFacing.set(bodyFacing);
	
		if (needsFixing) 
			body.mFootballer.helperFixDir(mBodyFacing);
					
		mBodyFacingType = BodyFacing_Dir;			
					
		mBodyFacingTolerance = tolerance;
		mBodyFacingImportance = importance;
		
		if (mBodyFacingTolerance == null)
			mBodyFacingTolerance = 0.995;
	}
	
	function setBodyFacingLookAt(body, t, lookAt, tolerance, importance) {
	
		mHasBodyFacing = true;
	
		mBodyFacingLookAt.set(lookAt);
	
		mBodyFacingType = BodyFacing_LookAt;		
					
		mBodyFacingTolerance = tolerance;
		mBodyFacingImportance = importance;
		
		if (mBodyFacingTolerance == null)
			mBodyFacingTolerance = 0.995;
	}

	function updateBodyFacing(body, t) {
	
		if (mHasBodyFacing && (mBodyFacingType == BodyFacing_LookAt)) {
		
			mBodyFacingLookAt.subtract(body.mImpl.getPos(), mBodyFacing);
			body.mFootballer.helperFixDir(mBodyFacing);
			
			if (mBodyFacing.isZero())
				mBodyFacing.set(body.mImpl.getFacing());
		}
	}
	
	function changeDestinationPos(body, t, pos, offset, needsFixing) {
	
		mDestination.set(pos);
		
		if (offset != null)
			mDestination.addToSelf(offset);
		
		mDestinationChangeTime = t;
		
		if (needsFixing)
			body.mFootballer.helperFixFootballerGroundPos(mDestination);
	}
	
	function updateDestinationDir(body, t) {
	
		if (mHasDestination && (mDestinationDirChangeTime != mDestinationChangeTime)) {
		
			mDestinationDirChangeTime = mDestinationChangeTime;
		
			mDestination.subtract(body.mImpl.getPos(), mDestinationDir);
			body.mFootballer.helperFixDir(mDestinationDir);
		}
	}
	
	function getFreeTimeBeforeRunDist(runSettings, runDist, refTime, interceptTime, runType, reactionTime) {
	
		if (runDist <= 0.0) {
		
			return (interceptTime - refTime);
		}
	
		local runSpeed = runSettings.getSpeedValue(runType, runSettings.RunSpeed_Normal);
		local timeToIntercept = runDist / runSpeed;
		local freeTimeBeforeIntercept = interceptTime - (refTime + timeToIntercept);
								
		freeTimeBeforeIntercept -= reactionTime;
					
		return freeTimeBeforeIntercept;			
	}
	
	function getFreeTimeBeforeRunDestination(runSettings, refPos, refTime, destination, interceptTime, runType, reactionTime, radius) {
	
		return getFreeTimeBeforeRunDist(runSettings, dist(refPos, destination) - radius, refTime, interceptTime, runType, reactionTime);	
	}
	
	function getFreeTimeBeforeRunDistDim(runSettings, refPos, refTime, interceptPos, interceptTime, runType, reactionTime, radius) {
	
		return getFreeTimeBeforeRunDist(runSettings, fabs(refPos - interceptPos) - radius, refTime, interceptTime, runType, reactionTime);
	}
	
	function isWithinRunTolerance(runSettings, body, t, destination, runType, espilonSq) {
	
		local distanceSq = distSq2Dim(body.mImpl.getPos(), destination, ::Scene_Right, ::Scene_Fwd);
		local distToleranceSq = runSettings.getMoveEpsilonSq(runType, runSettings.RunSpeed_Normal);
	
		if (espilonSq != null) {
		
			distToleranceSq = maxf(espilonSq, distToleranceSq);
		}
	
		if (distanceSq <= distToleranceSq) {
		
			return true;
		}
		
		return false;
	}
	
	function frameMoveDestination_checkArrival(runSettings, body, t, dt, runType, espilonSq) {
	
		if (isWithinRunTolerance(runSettings, body, t, mDestination, runType, espilonSq)) {
		
			local distanceSq = distSq2Dim(body.mImpl.getPos(), mDestination, ::Scene_Right, ::Scene_Fwd);
		
			local newPos = mTempVect3;
			body.mImpl.getPos().addMultiplication(mDestinationDir, dt, newPos);
			
			local newDestDir = newPos;
			newDestDir.subtractFromSelf(mDestination);
			
			if (newDestDir.dot(mDestinationDir) < 0.0) {
			
				mDestinationIDState = Destination_Reached;
			
				return true;
			}
		}
		
		return false;
	}
	
	function frameMoveDestination_OwnBall_PrepareDestination(body, t, dt) {
		
		local estimate = mDestinationOwnBallState.mEstimate;
		local ballVel = estimate.getModeVel(mDestinationOwnBallState.mEstimateMode);
		local ballDir = null;
		
		
		if (ballVel != null && !ballVel.isZero()) {
		
			ballDir = Vector3();
			ballDir.set(ballVel);
			body.mFootballer.helperFixDir(ballDir);
			
			if (ballDir.isZero())
				ballDir = null;
		}
		
		if (ballDir != null) {
	
			if (mDestinationOwnBallState.mBodyFacing == null)
				mDestinationOwnBallState.mBodyFacing = Vector3();
				
			ballDir.negateTo(mDestinationOwnBallState.mBodyFacing);
	
			local offset = ballDir;
			offset.mulToSelf(0.5 * body.mImpl.getCOCFwdReach());

			changeDestinationPos(body, t, estimate.getModePos(mDestinationOwnBallState.mEstimateMode), offset, true);			
			mDestinationEpsilonSq = 0.0;
			
		} else {
		
			changeDestinationPos(body, t, estimate.getModePos(mDestinationOwnBallState.mEstimateMode), null, true);			
		
			mDestinationEpsilonSq = 0.5 * body.mImpl.getCOCFwdReach();
			mDestinationEpsilonSq *= mDestinationEpsilonSq;
			
			mDestinationOwnBallState.mBodyFacing = null;
		}
		
		mDestinationArrivalTime = estimate.getModeInterceptTime(mDestinationOwnBallState.mEstimateMode);
		mDestinationArrivalTimeImprotance = 1.0;
		
		updateDestinationDir(body, t);
	}
	
	function frameMoveDestination_OwnBall(body, t, dt) {
	
		if (body.mImpl.isBallOwner()) {
		
			mDestinationOwnBallState.reset();
			return false;
		}
		
		{
			local estimate = mDestinationOwnBallState.mEstimate;
			local estimResult = estimate.Update_Invalidated;
			local prevEstimMode = mDestinationOwnBallState.mEstimateMode;
		
			switch (mDestinationOwnBallState.mEstimateMode) {
			
				case estimate.Analyse_None: {
				
					if (t < mDestinationOwnBallState.mNextAnalysisTime) {
					
						mDestinationOwnBallState.mState = OwnBall_Estimating;
						return true;
					}
				
					mDestinationOwnBallState.mNextAnalysisTime = t + mDestinationOwnBallState.mAnalysisDelay;
				
					local estimResultPath = estimate.updatePath(body.mImpl, t, true, true, true);
					local estimResultColl = estimate.updateColl(body.mImpl, t);
					
					switch (estimate.analyseValidity()) {
					
						case estimate.Analyse_None: {
						
							mDestinationOwnBallState.mState = OwnBall_Estimating;
							return true;
						
						} break;
					
						case estimate.Analyse_Path: {	

							if (estimate.isPathInstantaneous()) {
							
								if (mDestinationOwnBallState.mChaseInstantaneousBall) {
							
									//no analysis delay for instantaneous ball analysis
									mDestinationOwnBallState.mNextAnalysisTime = t;
								
								} else {
							
									mDestinationOwnBallState.mState = OwnBall_Estimating;
									return true;
								}
							}
							
							estimResult = estimResultPath;
							mDestinationOwnBallState.mEstimateMode = estimate.Analyse_Path;
							
						} break;
						
						case estimate.Analyse_Coll: {	
						
							estimResult = estimResultColl;
							mDestinationOwnBallState.mEstimateMode = estimate.Analyse_Coll;
							
						} break;
						
						case estimate.Valid_Both: {	
						
							local interceptTimeDiff = estimate.getCollInterceptTime() - estimate.getPathInterceptTime();
						
							if (interceptTimeDiff > 1.5) {
							
								estimResult = estimResultPath;
								mDestinationOwnBallState.mEstimateMode = estimate.Analyse_Path;
							
							} else {
							
								estimResult = estimResultColl;
								mDestinationOwnBallState.mEstimateMode = estimate.Analyse_Coll;
							}
						
						} break;
					}
					
					//because we updated without invalidating, a no change could have resulted
					if (estimResult == estimate.Update_NoChange)
						estimResult = estimate.Update_NewEstimate;
					
				} break;
				
				case estimate.Analyse_Path: {
				
					estimResult = estimate.updatePath(body.mImpl, t, true, true, true);
					
				} break;
				
				case estimate.Analyse_Coll: {
				
					estimResult = estimate.updateColl(body.mImpl, t);
				
				} break;
			}
			
			switch (estimResult) {
		
				case estimate.Update_NoChange: {
				
					return true;
				
				} break;
			
				case estimate.Update_NewEstimate: {

					if (!mDestinationOwnBallState.mChaseInstantaneousBall && estimate.isInstantaneous()) {
					
						mDestinationOwnBallState.mState = OwnBall_Estimating;
						return true;
					
					} else {
					
						if (prevEstimMode == estimate.Analyse_None) {
						
							frameMoveDestination_OwnBall_PrepareDestination(body, t, dt);
							mDestinationOwnBallState.mState = OwnBall_Moving;
							return true;
						
						} else {
					
							mDestinationOwnBallState.mEstimateMode = estimate.Analyse_None;
							mDestinationOwnBallState.mState = OwnBall_Estimating;
							return true;
						}
					}
				
				} break;
				
				default: {
				
					mDestinationOwnBallState.mState = OwnBall_Estimating;
					return true;
				
				} break;
			}
		}	
		
	}
	
	function frameMoveDestination(body, t, dt) {
	
		if (mHasDestination) {
				
			local isActionRunning = (body.getActionType() == ::Footballer.Action_Running);
			local actionRunning = body.getActionRunning();
			local runSettings = body.mProfile.run;
			local doRun = true;
			local doStrafe = null;
			local chosenRunType = null;
			local targetBodyFacing = null;
			local targetBodyFacingImportance = 0.0;
			
			switch (mDestinationType) {
			
				case Destination_OwnBall: {
				
					if (!frameMoveDestination_OwnBall(body, t, dt))
						return false;
					
					//print("ownball");
					
					//better run at max velocity...
					/*
					targetBodyFacing = mDestinationOwnBallState.mBodyFacing;
					targetBodyFacingImportance = 0.5;
					*/
					
				} break;
				
				default: {
				
					mDestinationOwnBallState = null;
				
					targetBodyFacing = mHasBodyFacing ? mBodyFacing : null;
					targetBodyFacingImportance = mBodyFacingImportance;
				
				} break;
			}
			
			if (isActionRunning) {
			
				if (frameMoveDestination_checkArrival(runSettings, body, t, dt, actionRunning.getRunType(), mDestinationEpsilonSq)) {
				
					//print("reached");
				
					doRun = false;
				}
				
				if (doRun) {
				
					//Handle Collisions
				
					/*
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
					*/
				}
			}
						
			if (doRun) {
			
				local idealRunType = null;
				local fallbackRunType = null;
				
				if (targetBodyFacing != null) {
					
					idealRunType = actionRunning.detectRunType(body, targetBodyFacing, mDestinationDir, false, body.mImpl.isBallOwner());
					
					if ((mDestinationArrivalTime != null) && (mDestinationArrivalTimeImprotance > targetBodyFacingImportance))
						fallbackRunType = actionRunning.detectRunType(body, targetBodyFacing, null, false, body.mImpl.isBallOwner());
					
				} else {
					
					//idealRunType = actionRunning.detectRunType(body, body.mImpl.getFacing(), null, false, body.mImpl.isBallOwner());
					idealRunType = ::FootballerBodyActionProfile_Run.Run_Normal;
				}			
				
				local currRunType = null;	
							
				if (isActionRunning) {
				
					currRunType = actionRunning.getRunType();
				} 
				
				chosenRunType = currRunType;
				
				if (currRunType == idealRunType) {
				
				} else {
				
					local tryNext = true;
				
					if (currRunType != null) {
					
						if ((targetBodyFacing == null) && runSettings.isStrafingHard(currRunType)) {
						
							//ignore curr run type
						
						} else {
					
							local availableTime = getFreeTimeBeforeRunDestination(runSettings, body.mImpl.getPos(), t, mDestination, mDestinationArrivalTime, currRunType, 0.0, 0.0);
							
							if (availableTime >= 0.0) {
							
								chosenRunType = currRunType;
								tryNext = false;
							}
						}
					}
					
					if (tryNext) {
					
						if (fallbackRunType == null) {
						
							chosenRunType = idealRunType;
							tryNext = false;
							
						} else {
						
							local availableTime = getFreeTimeBeforeRunDestination(runSettings, body.mImpl.getPos(), t, mDestination, mDestinationArrivalTime, idealRunType, 0.0, 0.0);
						
							//print("availableTime " + availableTime + " t " + t + " arrivalTime " + mDestinationArrivalTime);
						
							if (availableTime >= 0.0) {
							
								chosenRunType = idealRunType;
								tryNext = false;
							} 
						}
					}
					
					if (tryNext) {
					
						if (fallbackRunType != null) {
						
							chosenRunType = fallbackRunType;
						}
					}
					
					doStrafe = runSettings.isStrafing(chosenRunType);
				}
			}
			

			if (!isActionRunning) {
			
				if (isWithinRunTolerance(runSettings, body, t, mDestination, chosenRunType, mDestinationEpsilonSq)) {
				
					doRun = false;
				}
			}			
			
			if (doRun) {
				
				{
					body.mImpl.getMatch().queueRenderSphere(mDestination, body.mImpl.getMatch().conv(0.35), null);		
				}
						
				body.requestVelocity(null, t, dt, mDestinationDir, false, ::FootballerBodyActionProfile_Run.RunSpeed_Normal, doStrafe);
				
				if (doStrafe == true) {
				
					body.requestFacing(null, t, dt, mBodyFacing, false);
				}
				
			} else {
			
				body.requestZeroVelocity(null, t, dt);
				
				switch (mDestinationType) {
			
					case Destination_OwnBall: {
											
						if (mDestinationOwnBallState.mBodyFacing != null) {
						
							body.requestFacing(null, t, dt, mDestinationOwnBallState.mBodyFacing, false);
						}
						
					} break;
					
					default: {
					
						mHasDestination = false;
					
					} break;
				}
			}
		}
		
		return mHasDestination;
	}
	
	function frameMoveBodyFacing(body, t, dt) {
	
		if (mHasBodyFacing && (mHasDestination == false)) {
		
			if (body.mImpl.getFacing().dot(mBodyFacing) >= mBodyFacingTolerance) {
			
				mHasBodyFacing = false;
				
			} else {
			
				if (body.requestFacing(null, t, dt, mBodyFacing, false) == ::FootballerBody.Request_Executed) {
				
					mHasBodyFacing = false;
				}
			}
		}
		
		return mHasBodyFacing;
	}
	
	function frameMoveGoalieJumpRun(body, t, dt) {
	
		if (mGoalieRunJumpState == null) {
		
			return false;
			
		} else {
		
			switch (mGoalieRunJumpState.frameMove(this, body, t, dt)) {
			
				case ::FootballerBrain_AI_JobGoalie_ReachabilityProfile.State_Executed: {
			
					idle(body, t);
					mGoalieRunJumpState = null;
				
					return false;
					
				} break;
				
				case ::FootballerBrain_AI_JobGoalie_ReachabilityProfile.State_None: 
				case ::FootballerBrain_AI_JobGoalie_ReachabilityProfile.State_Waiting: {
			
					idle(body, t);
					return false;
					
				} break;
			}
		}
		
		return true;
	}
	
	function frameMove(body, t, dt) {
	
		local didAnyAction = false;
		
		if (frameMoveGoalieJumpRun(body, t, dt))
			didAnyAction = true;
		
		updateBodyFacing(body, t);
		updateDestinationDir(body, t);
				
		if (frameMoveDestination(body, t, dt))
			didAnyAction = true;
			
		if (frameMoveBodyFacing(body, t, dt))
			didAnyAction = true;
	
		if (didAnyAction == false) {
		
			body.requestZeroVelocity(null, t, dt);
		}
	}
	
	mTempVect3 = null;
	
	mHasDestination = false;
	mDestination = null;
	mDestinationID = 0.0;
	mDestinationIDState = -1;
	mDestinationChangeTime = -1.0;
	mDestinationType = null;
	mDestinationDir = null;
	mDestinationDirChangeTime = -2.0;
	mDestinationArrivalTime = null;
	mDestinationArrivalTimeImprotance = null;
	mDestinationEpsilonSq = null;
	mDestinationOwnBallState = null;
		
	mDefaultHumanizeRadius = 0.0;
	
	mHasBodyFacing = false;
	mBodyFacingType = null;
	mBodyFacing = null;
	mBodyFacingLookAt = null;
	mBodyFacingTolerance = null;
	mBodyFacingImportance = null;

	mGoalieRunJumpState = null;	
	
	mRunDirUpdateTime = 0.0;
	mStrafeUpdateTime = 0.0;
	mIdleRunUpdateTime = 0.0;
	
	
	function helperDepthToFwd(footballer, depth) {
		
		local isTeamWithFwdDir = footballer.mImpl.isTeamWithFwdDir();
		local pitchHalfLength = footballer.mImpl.getMatch().getPitchHalfLength();
		
		if (isTeamWithFwdDir) {
		
			return -pitchHalfLength + depth;
		
		} 
		
		return pitchHalfLength - depth;
	}
}