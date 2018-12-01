class FootballerBrain_AI_GoalFireShot extends FootballerBrain_AI_Goal {

	static State_None = -1;
	static State_WaitingToShoot = 0;

	constructor() {
	}

	function setShotStartState(shotStartState) {
	
		//footballer.getActionShooting().createStartState(footballer, footballer.ballIsHeaderHeight(), shotType, shot, false, shotSpeedScale);
		mShootStartState = shotStartState; 
	}
	
	function start(brain, footballer, t) { 
	
		mState = State_WaitingToShoot;
		
		return doReturn(t, Result_Processing);
	}
		
	function frameMove(brain, footballer, t, dt) { 
		
		local impl = footballer.impl;
		
		switch (footballer.getActionType()) {
		
			case footballer.Action_Running: {
			
				footballer.getActionRunning().setActiveBallShot(mShootStartState.ballShot, mShootStartState.shotSpeedScale, false, false);
				mState = State_Succeeded;
			
			} break;
			
			default: {
				
				if (footballer.getAction().switchShooting(footballer, t, mShootStartState)) {
				
					mState = Result_Success;
				}
			
			} break;
		}
		
		return doReturn(t, stateToResult(mState));	
	}
	
	mState = null;
	mShootStartState = null;
}