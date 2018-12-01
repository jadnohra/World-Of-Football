class FootballerBrain_AI_SingleUseGoal extends FootballerBrain_AI_ManagedGoal {

	constructor(goal) {
	
		mGoal = goal;
	}

	function setGoal(goal) {
	
		if (mGoal != null)
			mGoal.stop();
		
		mGoal = goal;
	}	
	
	function hasGoal() { return mGoal != null; }
	function getGoal() { return mGoal; }
	function stop(brain) { setGoal(null); }
	
	function update(brain, footballer, t, dt, goalClass, manager, goalSetupFunc, startSetupFunc) {
	
		if (mGoal != null) {
		
			local result = mGoal.update(brain, footballer, t, dt, manager, startSetupFunc);
				
			if (result != FootballerBrain_AI_Goal.Result_Processing) {
			
				mGoal = null;
			}
			
			return result;
		}
		
		return FootballerBrain_AI_Goal.Result_None;
	}
	
	mGoal = null;
}