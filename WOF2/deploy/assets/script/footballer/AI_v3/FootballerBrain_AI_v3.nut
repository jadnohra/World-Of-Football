
class FootballerBrain_AI_v3 extends FootballerBrain_AI_BrainBodyInterface {

	static IdleAction_None = -1;
	static IdleAction_Idle = 0;
	static IdleAction_Formation = 1;

	constructor() {
	
		::FootballerBrain_AI_BrainBodyInterface.constructor();
	
		mCurrGoal = FootballerBrain_AI_SingleUseGoal(null);
	}	
	
	function canSwitchOn(footballer, t) {
	
		return !mIsSwitchLocked;
	}
	
	function canSwitchOff(footballer, t) {
	
		return !mIsSwitchLocked;
	}

	function setSwitchLocked(locked) {
	
		mIsSwitchLocked = locked;
	}	
	
	function start(footballer, t) {
	
		if (t == 0.0) {
		
			if (!loadAction(footballer, t))
				return false;
		}
	
		idle(footballer.mBody, t);
	
		return true;
	}
	
	function stop(footballer, t) {
	
		idle(footballer.mBody, t);
	}
	
	function onSwitchedTeamFormation(footballer, t) {
	}
	
	
	function frameMoveIdle(footballer, t, dt) {
	
		local executedAction = false;
	
		if (!executedAction)
			idle(footballer.mBody, t);
	
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
		
		::FootballerBrain_AI_BrainBodyInterface.frameMove(footballer.mBody, t, dt);
	}

	function setGoal(footballer, t, goal) {
	
		mCurrGoal.setGoal(goal);
	}
	
	function load(footballer, chunk, conv) {
	
		::FootballerBrain_AI_BrainBodyInterface.load(footballer.mBody, chunk, conv);
	
		return true;
	}
	
	function loadAction(footballer, t) {
	
		mIdleActionType = IdleAction_Formation;
	
		local match = footballer.mImpl.getMatch();
		
		local formationChunk = footballer.mImpl.getFormationPositionChunk();
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
		
			local code = footballer.mImpl.getFormationPositionCode();
							
			switch (code) {
			
				case "goalie": {
			
					setJobGoalie(footballer, t, true);
					
				} break;
			}
		}
	
		return true;
	}
	
	function setJobGoalie(footballer, t, switchLock) {
		
		mIsSwitchLocked = (footballer.mBody.mProfile.EnableFootballerPlayerControl == false);
		
		local job = null;
		
		{
			job = FootballerBrain_AI_JobGoalie();
		}
		
		setGoal(footballer, t, job);
	}
	
	function getTeamAI(footballer) {
	
		return Match.getInstance().getTeamAI(footballer.mImpl.getTeamEnum());
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