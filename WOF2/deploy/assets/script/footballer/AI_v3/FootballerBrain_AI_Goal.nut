class FootballerBrain_AI_Goal {
	
	/*
		the values are negative so that they can be used directly as states as well
	*/
	static Result_None = -1;
	static Result_Processing = -2;
	static Result_Success = -3;
	static Result_Failure = -4;
	
	constructor() {
	
		mLastGoalUpdateTime = -1.0;
	}
	
	function start(brain, footballer, t) { return doReturn(t, Result_Success); }
	function frameMove(brain, footballer, t, dt) { return doReturn(t, Result_Success); }
	
	function update(brain, footballer, t, dt, manager, startSetupFunc) { 
	
		local result = Result_Processing;
	
		if (!isActive(t, dt)) {
			
			if (startSetupFunc != null)
				(startSetupFunc.bindenv(manager))(brain, footballer, t, dt, this);
			
			result = start(brain, footballer, t);
		}
		
		if (result == Result_Processing)
			result = frameMove(brain, footballer, t, dt);
			
		return result;			
	}
	
	function doReturn(t, result) {
	
		mResult = result;
	
		if (result != Result_Processing)
			mLastGoalUpdateTime = -1.0;
		else	
			mLastGoalUpdateTime = t;
			
		return result;	
	}
	
	function getResult() {
	
		return mResult;
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
	
	//this is only the default impl. can be overriden ...
	function stateToResult(state) { return state >= 0 ? Result_Processing : state; }
	
	mLastGoalUpdateTime = -1.0;
	mResult = -1;
}

