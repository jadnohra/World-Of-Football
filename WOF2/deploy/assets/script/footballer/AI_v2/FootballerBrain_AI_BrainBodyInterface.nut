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
			
			return true;
		}
		
		return false;
	}

	function body_updateSetIsStrafing(footballer, t, isStrafing) {
	
		local actionRunning = footballer.getActionRunning();
	
		if (actionRunning.getIsStrafing() == isStrafing)
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
	
	function body_updateStrafeRunDir(footballer, t, dir) {
		
		if (t - mRunDirUpdateTime > mReflexTimeDir) {
		
			local actionRunning = footballer.getActionRunning();
		
			if (footballer.getActionType() == footballer.Action_Running) {
		
				mRunDirUpdateTime = t;
			
				local currRunDir = actionRunning.getRunDir();
				
				if (!currRunDir.equals(dir)) {
				
					actionRunning.setIsStrafing(true);
					actionRunning.setRunDir(footballer, dir);
				}
				
			} else {
			
				actionRunning.setIsStrafing(true);
				actionRunning.setRunDir(footballer, dir);
			
				footballer.getAction().switchRunning(footballer, t, null);
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