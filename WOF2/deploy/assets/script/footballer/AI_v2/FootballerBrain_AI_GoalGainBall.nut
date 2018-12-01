class FootballerBrain_AI_GoalGainBall extends FootballerBrain_AI_Goal {

	static State_None = -1;
	static State_Estimating = 0;
	static State_Moving = 1;
	static State_Waiting = 2;
	static State_Facing = 3;
	
	constructor() {
	
		mManagedGoalGoto = FootballerBrain_AI_MultiUseGoal();
		mManagedGoalFace = FootballerBrain_AI_MultiUseGoal();
		createEstimate();
	}	
	
	function setChaseInstantaneousBall(chaseInstantaneousBall) {
	
		mChaseInstantaneousBall = chaseInstantaneousBall;
	}
		
	function start(brain, footballer, t) {
		
		mState = State_None;
		mEstimateMode = mEstimate.Analyse_None;
		
		return doReturn(t, Result_Processing);
	}
	
	function createEstimate() {
	
		if (mEstimate == null) {
		
			mEstimate = CBallInterceptEstimate();
			mEstimate.enableAnalysis(true, true);
			mEstimate.enableBallOutOfPlayEstimates(false);
		}
	}
		
	function setupGoalGoto(brain, footballer, t, dt, goal) {
	
		goal.setToleranceMode(goal.Tolerance_JustBefore);
		goal.setStrafe(false);
	}
	
	function startGoalGoto(brain, footballer, t, dt, goal) {
	
		local ballVel = mEstimate.getModeVel(mEstimateMode);
		local ballDir = null;

		if (ballVel != null && !ballVel.isZero()) {
		
			ballDir = Vector3();
			ballDir.set(ballVel);
			footballer.helperFixDir(ballDir);
			
			if (ballDir.isZero())
				ballDir = null;
		}
		
		if (ballDir != null) {
	
			ballDir.mulToSelf(0.5 * footballer.impl.getCOCFwdReach());
			goal.setTargetWithOffset(footballer, mEstimate.getModePos(mEstimateMode), ballDir, false);
			
		} else {
		
			goal.setTargetWithDirectedOffset(footballer, mEstimate.getModePos(mEstimateMode), t, 0.5 * footballer.impl.getCOCFwdReach(), false);
		}
	}
	
	function startGoalFace(brain, footballer, t, dt, goal) {
	
		goal.setTargetByPos(footballer, mEstimate.getModePos(mEstimateMode));
	}
	
	function idleAndReturn(brain, footballer, t, resetEstimateMode) {
	
		brain.body_idle(footballer, t);
					
		if (resetEstimateMode)
			mEstimateMode = mEstimate.Analyse_None;	
			
		mState = State_Estimating;
		return doReturn(t, Result_Processing);
	}
	
	
	function frameMove(brain, footballer, t, dt) { 
	
		local impl = footballer.impl;			
		
		if (impl.isBallOwner()) {
		
			brain.body_idle(footballer, t);
			
			mState = Result_Success;
			return doReturn(t, mState);
		}
		
		{
			local estimResult = mEstimate.Update_Invalidated;
			local prevEstimMode = mEstimateMode;
		
			switch (mEstimateMode) {
			
				case mEstimate.Analyse_None: {
				
					if (t < mNextAnalysisTime) {
					
						return idleAndReturn(brain, footballer, t, false);
					}
				
					mNextAnalysisTime = t + mAnalysisDelay;
				
					local estimResultPath = mEstimate.updatePath(footballer.impl.getMatch(), footballer.impl, t, true, true, true);
					local estimResultColl = mEstimate.updateColl(footballer.impl.getMatch(), footballer.impl, t);
					
					switch (mEstimate.analyseValidity()) {
					
						case mEstimate.Analyse_None: {
						
							return idleAndReturn(brain, footballer, t, false);
						
						} break;
					
						case mEstimate.Analyse_Path: {	

							if (mEstimate.isPathInstantaneous()) {
							
								if (mChaseInstantaneousBall) {
							
									//no analysis delay for instantaneous ball analysis
									mNextAnalysisTime = t;
								
								} else {
							
									return idleAndReturn(brain, footballer, t, false);
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
					
					//because we updated without invalidating, a no change could have resulted
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
					
						return idleAndReturn(brain, footballer, t, false);
					
					} else {
					
						if (prevEstimMode == mEstimate.Analyse_None) {
						
							mState = State_Moving;
						
						} else {
					
							mEstimateMode = mEstimate.Analyse_None;
							return doReturn(t, Result_Processing);
						}
					}
				
				} break;
				
				default: {
				
					return idleAndReturn(brain, footballer, t, true);
				
				} break;
			}
		}
			
	
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_None: {
				
					mState = State_Moving;
					repeat = true;
					
				} break;
				
				case State_Moving: {
				
					local result = mManagedGoalGoto.update(brain, footballer, t, dt, FootballerBrain_AI_GoalGoto, this, setupGoalGoto, startGoalGoto);
					
					switch (result) {
					
						case Result_Success: {
						
							mState = State_Facing;
							repeat = true;
						
						} break;
						
						case Result_Failure: {
						
							mState = Result_Failure;
						
						} break;
					}
				
				} break;
				
				case State_Facing: {
				
					local result = mManagedGoalFace.update(brain, footballer, t, dt, FootballerBrain_AI_GoalFace, this, null, startGoalFace);
					
					switch (result) {
					
						case Result_Success: {
						
							mState = State_Waiting;
							repeat = true;
						
						} break;
						
						case Result_Failure: {
						
							mState = Result_Failure;
							
						} break;
					}
						
				} break;
			}
		}
		
		if (mState == State_Waiting) {
		
			brain.body_idle(footballer, t);
		}
		
		return doReturn(t, stateToResult(mState)); 
	}

	mState = -1;
	mEstimate = null;
	mEstimateMode = -1;
	mManagedGoalGoto = null;
	mManagedGoalFace = null;
	
	static mAnalysisDelay = 0.15;
	mNextAnalysisTime = 0.0;
	
	mChaseInstantaneousBall = true;
}