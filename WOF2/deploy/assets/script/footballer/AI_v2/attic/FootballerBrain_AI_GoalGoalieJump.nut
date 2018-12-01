
class FootballerBrain_AI_GoalGoalieJump extends FootballerBrain_AI_Goal {

	static State_None = -1;
	static State_EstimatingTarget = 0;
	static State_ChoosingJump = 1;
	static State_ImprovingJumpPosition = 2;
	static State_WaitingToTriggerJump = 3;
	static State_TriggeringJump = 4;
	static State_Jumping = 5;

	constructor() {
	
		mEstimateTarget = CBallTrajSample();
		mTunedEstimateTarget = CBallTrajSample();
				
		mManagedGoalGoto = FootballerBrain_AI_MultiUseGoal();
		mJumpVect = Vector3();
	
		mJumpStartState = BodyActionJumping_StartState();
		mJumpStartState.jumpDir = Vector3();
	}
		
	function start(brain, footballer, t) {
	
		mState = State_None;
		return doReturn(t, Result_Processing);
	}
	
	function updateApproximateTarget(footballer, match, ball) {
	
		if (ball.hasAfterTouchPosDiff()) {
		
			match.tuneApproximatedBallEstimate(mEstimateTarget, mTunedEstimateTarget);
		
		} else {
		
			mTunedEstimateTarget.set(mEstimateTarget);
		}
		
		mTunedEstimateTarget.getPos().subtract(footballer.impl.getPos(), mJumpVect);
		mJumpVect.setEl(match.Scene_Up, 0.0);
	}
	
	function chooseJump(brain, footballer, t, dt) {
	
		local match = footballer.impl.getMatch();
		local jumpSettings = footballer.settings.jump;
		local targetTime = mTunedEstimateTarget.getTime();
		local ball = match.getBall();
	
		mJumpStartTime = t;
		
		updateApproximateTarget(footballer, match, ball);
		
		local apogeeDistance = mJumpVect.mag();
		local jumpDistance = 0.0;
		
		if (apogeeDistance <= 0.0) {
		
			mJumpStartState.jumpType = FootballerSettings_Jump.Jump_None;
			return true;
		}
		
		mJumpVect.div(apogeeDistance, mJumpStartState.jumpDir);
		mJumpStartState.jumpType = footballer.getActionJumping().detectJumpType(footballer, null, mJumpStartState.jumpDir);			
		
		{
			local handPosAtApogee = jumpSettings.FlyPhase_HandPosAtApogee;
		
			mJumpStartState.jumpHeight = mTunedEstimateTarget.getPos().getEl(match.Scene_Up) - handPosAtApogee.getEl(match.Scene_Up);
				
			if (mJumpStartState.jumpHeight < 0.0) {
			
				mJumpStartState.jumpHeight = 0.0;
			}
			
			local cappedJumpHeight = minf(mJumpStartState.jumpHeight, jumpSettings.JumpDistance_MaxHeight);
			mJumpStartState.jumpHeight = cappedJumpHeight;
			
			jumpDistance = jumpSettings.jumpProfile.calcTotalDistForApogeeDist(apogeeDistance);
		}
		
		switch (mJumpStartState.jumpType) {
		
			case FootballerSettings_Jump.Jump_Left:
			case FootballerSettings_Jump.Jump_Right: {
			
				local cappedJumpedDistance = minf(jumpDistance, jumpSettings.JumpDistance_Default);
				mJumpStartTime = (targetTime - jumpSettings.jumpProfile.ApogeeTime) - (match.getFrameInterval() * 1.5); 
				mJumpStartState.jumpDistance = cappedJumpedDistance;
			
				local reactionTime = mJumpStartTime - t;
				local minReactionTime = footballer.getAIReactionTime();
				
				if (reactionTime < minReactionTime) {
				
					mJumpStartTime = t + minReactionTime;
				}
			
			} break;
		}
		
		return true;
	}
	
	function setupGoalGoto(brain, footballer, t, dt, goal) {
	
		goal.setToleranceMode(goal.Tolerance_JustBefore);
		goal.setStrafe(true);
	}
	
	function startGoalGoto(brain, footballer, t, dt, goal) {
	
		goal.setTarget(footballer, mTunedEstimateTarget.getPos(), false);
	}
	
	function frameMove(brain, footballer, t, dt) { 
	
		local match = footballer.impl.getMatch();
		local ballState = match.getSpatialBallState();
	
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_None: {
				
					mState = State_EstimatingTarget;
					repeat = true;
				
				} break;
				
				case State_EstimatingTarget: {
				
					brain.body_idle(footballer, t);
				
					if (match.estimateBallStateAtPlane(footballer.impl.getPos(), footballer.impl.getFacing(), mEstimateTarget, true)) {
					
						updateApproximateTarget(footballer, match, match.getBall());
						
						mEstimateID = ballState.finalGetID();
						mState = State_ChoosingJump;
						repeat = true;
					}
				
				} break;
			
				case State_ChoosingJump: {
				
					brain.body_idle(footballer, t);
				
					if (ballState.finalGetID() == mEstimateID) {
					
						if (chooseJump(brain, footballer, t, dt)) {
						
							mNeedReevaluateJump = false;
							mState = State_ImprovingJumpPosition;
							repeat = true;
						}
					
					} else {
					
						mState = State_EstimatingTarget;
						repeat = true;
					}
									
				} break;
				
				case State_ImprovingJumpPosition: {
				
					if (ballState.finalGetID() == mEstimateID) {
					
						local timeLeft = mJumpStartTime - t;
					
						if (timeLeft > 0) {
						
							local ball = match.getBall();
						
							if (ball.hasAfterTouchPosDiff()) {
							
								updateApproximateTarget(footballer, match, ball);
								
								if (mManagedGoalGoto.hasGoal())
									mManagedGoalGoto.getGoal().setTarget(footballer, mTunedEstimateTarget.getPos(), true);
							}
							
							if (mManagedGoalGoto.update(brain, footballer, t, dt, FootballerBrain_AI_GoalGoto, this, setupGoalGoto, startGoalGoto) == Result_Processing) {
							
								mNeedReevaluateJump = true;
							}
							
						} else {
						
							mManagedGoalGoto.stop(brain);
						}
						
						if (!mManagedGoalGoto.hasGoal()) {
						
							mState = State_WaitingToTriggerJump;
							repeat = true;
						}
					
					} else {
					
						mState = State_EstimatingTarget;
						repeat = true;
					}
				
				} break;
				
				case State_WaitingToTriggerJump: {
				
					if (ballState.finalGetID() == mEstimateID) {
					
						local timeLeft = mJumpStartTime - t;
					
						if (timeLeft <= 0) {
						
							local ball = match.getBall();
						
							if (mNeedReevaluateJump || ball.hasAfterTouchPosDiff()) {
							
								updateApproximateTarget(footballer, match, ball);
								chooseJump(brain, footballer, t, dt);
							}
														
							mState = State_TriggeringJump;
							repeat = true;
							
						} else {
						
							brain.body_idle(footballer, t);
						}
					
					} else {
					
						mState = State_EstimatingTarget;
						repeat = true;
					}
				
				} break;
			
				case State_TriggeringJump: {
				
					if (footballer.getActionType() == footballer.Action_Jumping) {
					
						mState = State_Jumping;
						repeat = true;
					
					} else {
				
						footballer.getAction().switchJumping(footballer, t, mJumpStartState);
					}
				
				} break;
				
				case State_Jumping: {
				
					if (brain.body_isIdle(footballer))
						mState = Result_Success;
				
				} break;
				
				default: {
				
					brain.body_idle(footballer, t);
				
				} break;
			}
		}
	
		return doReturn(t, stateToResult(mState)); 
	}
	
	mState = -1;
	
	mEstimateTarget = null;
	mTunedEstimateTarget = null;
	mEstimateID = 0;
	
	mJumpStartTime = 0.0;
	mJumpVect = null;
	mManagedGoalGoto = null;
	mJumpStartState = null;
	mNeedReevaluateJump = false;
}

/*
class FootballerBrain_AI_GoalGoalieJump extends FootballerBrain_AI_Goal {

	static State_None = -1;
	static State_TriggeringJump = 0;
	static State_Jumping = 1;

	constructor() {
	
		mJumpStartState = BodyActionJumping_StartState();
		mJumpStartState.jumpDir = Vector3();
	}
		
	function start(brain, footballer, t) {
	
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
	
		mState = Result_Failure;
		mJumpStartTime = t;
		
		local mJumpVect = Vector3();
		
		targetPos.subtract(pos, mJumpVect);
		mJumpVect.setEl(match.Scene_Up, 0.0);
		
		local apogeeDistance = mJumpVect.mag();
		local jumpDistance = 0.0;
		
		if (apogeeDistance <= 0.0)
			return doReturn(t, mState);
		
		mJumpVect.div(apogeeDistance, mJumpStartState.jumpDir);
		mJumpStartState.jumpType = footballer.getActionJumping().detectJumpType(footballer, null, mJumpStartState.jumpDir);			
		
		{
			local handPosAtApogee = jumpSettings.FlyPhase_HandPosAtApogee;
		
			mJumpStartState.jumpHeight = targetPos.getEl(match.Scene_Up) - handPosAtApogee.getEl(match.Scene_Up);
				
			if (mJumpStartState.jumpHeight < 0.0) {
			
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
			
				return doReturn(t, mState);
			
			} break;
		}
		
		mState = State_TriggeringJump;
		return doReturn(t, Result_Processing);
	}
	
	function frameMove(brain, footballer, t, dt) { 
	
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_TriggeringJump: {
				
					if (footballer.getActionType() == footballer.Action_Jumping) {
					
						mState = State_Jumping;
						repeat = true;
					
					} else {
				
						if (t >= mJumpStartTime) {
						
							footballer.getAction().switchJumping(footballer, t, mJumpStartState);
							
						} else {
						
							
						}
					}
				
				} break;
				
				case State_Jumping: {
				
					if (brain.body_isIdle(footballer))
						mState = Result_Success;
				
				} break;
				
				default: {
				
					brain.body_idle(footballer, t);
				
				} break;
			}
		}
	
		return doReturn(t, stateToResult(mState)); 
	}
	
	mJumpStartTime = 0.0;
	mState = -1;
	mJumpStartState = null;
}
*/