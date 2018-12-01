
class FootballerBrain_AI_v2 extends FootballerBrain_AI_BrainBodyInterface {

	static IdleAction_None = -1;
	static IdleAction_Idle = 0;
	static IdleAction_Formation = 1;

	constructor() {
	
		mCurrGoal = FootballerBrain_AI_SingleUseGoal(null);
	}	
	
	function canSwitchOn(footballer, t) {
	
		return !mIsSwitchLocked;
	}
	
	function canSwitchOff(footballer, t) {
	
		return !mIsSwitchLocked;
	}

	function start(footballer, t) {
	
		if (t == 0.0) {
		
			if (!loadAction(footballer, t))
				return false;
		}
	
		body_idle(footballer, t);
	
		return true;
	}
	
	function stop(footballer, t) {
	
		body_idle(footballer, t);
	}
	
	function onSwitchedTeamFormation(footballer, t) {
	}
	
	
	function frameMoveIdle(footballer, t, dt) {
	
		local executedAction = false;
	
		if (!executedAction)
			body_idle(footballer, t);
	
		mLastIdleActionUpdateTime = t;
	}
	
	function signalNotIdle() {
	}
			
	function frameMove(footballer, t, dt) {

		if (mCurrGoal.hasGoal()) {
		
			mCurrGoal.update(this, footballer, t, dt, null, null, null, null);
		} 
		
		if (!mCurrGoal.hasGoal()) {
	
			frameMoveIdle(footballer, t, dt);
		}
		
		if ((mLastIdleActionUpdateTime + 1.5 * dt) < t) {
		
			signalNotIdle();
		}
	}

	function setGoal(footballer, t, goal) {
	
		mCurrGoal.setGoal(goal);
	}
	
	function load(footballer, chunk, conv) {
	
		return true;
	}
	
	function loadAction(footballer, t) {
	
		mIdleActionType = IdleAction_Formation;
	
		local match = footballer.impl.getMatch();
		
		local formationChunk = footballer.impl.getFormationPositionChunk();
		local tryFallbackToCodeJob = true;
		
		if (formationChunk.isValid()) {

			formationChunk.toChild("job");
		
			if (formationChunk.isValid()) {
			
				tryFallbackToCodeJob = false;
			
				switch (formationChunk.get("type")) {
				
					case "Goalie": {
					
						setJobGoalie(footballer, t, true);
						
					} break;
				}
			}
			
		} 
		
		if (tryFallbackToCodeJob) {
		
			local code = footballer.impl.getFormationPositionCode();
							
			switch (code) {
			
				case "goalie": {
			
					setJobGoalie(footballer, t, true);
					
				} break;
			}
		}
	
		return true;
	}
	
	function setJobGoalie(footballer, t, switchLock) {
		
		mIsSwitchLocked = (footballer.settings.EnableFootballerPlayerControl == false);
		
		local job = null;
		
		{
			job = FootballerBrain_AI_JobGoalie();
		}
		
		setGoal(footballer, t, job);
	}
	
	function getTeamAI(footballer) {
	
		return Match.getInstance().getTeamAI(footballer.impl.getTeamEnum());
	}	
	
	function getAIGoalPool(footballer) {
	
		return getTeamAI(footballer).getAIGoalPool();
	}
		
	mCurrGoal = null;
	mIsSwitchLocked = false;
	
	mIdleActionType = -1;
	mIdleActionGoal = null;
	mLastIdleActionUpdateTime = -1.0;
	mIdleActionFormationNextTime = -1.0;
}