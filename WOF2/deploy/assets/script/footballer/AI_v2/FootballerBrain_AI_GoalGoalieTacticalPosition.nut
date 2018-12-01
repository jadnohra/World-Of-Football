
class FootballerBrain_AI_GoalGoalieTacticalPosition extends FootballerBrain_AI_Goal {

	static State_None = -1;
	static State_Thinking = 0;
	static State_Facing = 1;
	static State_Going = 2;

	constructor() {
	
		mIdealTacticalPos = Vector3();
		
		mManagedGoalGoto = FootballerBrain_AI_MultiUseGoal();
		mManagedGoalFace = FootballerBrain_AI_MultiUseGoal();
	}
	
	function start(brain, footballer, t) {
	
		mState = State_None;
		return doReturn(t, Result_Processing);
	}
	
	function startGoalFace(brain, footballer, t, dt, goal) {
	
		goal.setTargetByDir(footballer, footballer.impl.getTeamFwdDir(), false);
	}
	
	function setupGoalGoto(brain, footballer, t, dt, goal) {
	
		goal.setToleranceMode(goal.Tolerance_JustBefore);
		goal.setStrafe(true);
	}
	
	function startGoalGoto(brain, footballer, t, dt, goal) {
	
		goal.setTarget(footballer, mIdealTacticalPos, false);
	}
	
	function updateTacticalPos(footballer, t) {
	
		if (footballer.impl.isBallOwner())
			return false;
	
		local ret = true;
	
		/*
		if (restart) {
		
			if (footballer.impl.getFormationPos(mIdealTacticalPos)) {
				
				ret = true;
				
			} else {
						
				ret = true;
				mIdealTacticalPos.set(footballer.impl.getPos());
			}
		} 
		*/
		
		local match = footballer.impl.getMatch();
		
		//calc depth
		{
		
			{
				local ball = match.getBall();
				
				mTacticalDepth_Ball = ball.getPos().getEl(::Scene_Fwd);
				
				//add ball vel to estimation
			}
			
			mIdealTacticalDepth = mTacticalDepth_Ball;
		}
		
		//get position at depth
		{
			if (footballer.impl.getFormationPos(mIdealTacticalPos)) {
				
				
			} else {
						
				mIdealTacticalPos.set(footballer.impl.getPos());
			}
			
			mIdealTacticalPos.setEl(::Scene_Fwd, mIdealTacticalDepth);
		}
		
		//use goalie region
		{
		
			local region = footballer.impl.getRegionPenalty();
		
			if (region != null) {
			
				region.clip(match, mIdealTacticalPos, footballer.settings.PitchHeight);
			}
		}

		if (ret) 
			utilFixFootballerGroundPos(footballer, mIdealTacticalPos);
		
		return ret;	
	}
	
	function frameMove(brain, footballer, t, dt) { 
	
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_None: {
				
					mState = State_Thinking;
					repeat = true;
				
				} break;
				
				case State_Thinking: {
				
					brain.body_idle(footballer, t);
					local foundTacticalPos = false;
				
					if (updateTacticalPos(footballer, t)) {
						
						mState = State_Facing;
					}
					
				} break;
			
				case State_Facing: {
				
					if (mManagedGoalFace.update(brain, footballer, t, dt, FootballerBrain_AI_GoalFace, this, null, startGoalFace) != Result_Processing) {
							
						mState = State_Going;
					}
														
				} break;
				
				case State_Going: {
				
					if (updateTacticalPos(footballer, t)) {
					
						if (mManagedGoalGoto.hasGoal())
							mManagedGoalGoto.getGoal().setTarget(footballer, mIdealTacticalPos, true);
					}
				
					if (mManagedGoalGoto.update(brain, footballer, t, dt, FootballerBrain_AI_GoalGoto, this, setupGoalGoto, startGoalGoto) != Result_Processing) {

						//print("reached");	
						brain.body_idle(footballer, t);	
						mState = State_Thinking;
					}
				
				} break;
			}
		}
	
		return doReturn(t, stateToResult(mState)); 
	}
	
	mState = -1;
	
	mIdealTacticalDepth = 0.0;
	mTacticalDepth_Ball = 0.0;
	
	mIdealTacticalPos = null;
	
	mManagedGoalFace = null;
	mManagedGoalGoto = null;
}
