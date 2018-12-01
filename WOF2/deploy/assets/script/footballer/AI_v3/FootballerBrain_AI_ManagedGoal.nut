class FootballerBrain_AI_ManagedGoal {

	function hasGoal() { return false; }
	function getGoal() { return null; }
	function stop(brain) {}

	function update(brain, footballer, t, dt, goalClass, manager, goalSetupFunc, startSetupFunc) { 
	
		return FootballerBrain_AIGoal.Result_None;
	}
}

/*
class FootballerBrain_AI_AutoManagedGoal {

	constructor(managedGoal, goalClass, goalSetupFunc, startSetupFunc) {
	
		mManagedGoal = managedGoal;
		mGoalClass = goalClass;
		mGoalSetupFunc = goalSetupFunc;
		mStartSetupFunc = startSetupFunc;
	}

	function update(brain, footballer, t, dt) { 
	
		return mManagedGoal.update(brain, footballer, t, dt, mGoalClass, mGoalSetupFunc, mStartSetupFunc);
	}
	
	mManagedGoal = null;
	mGoalClass = null;
	mGoalSetupFunc = null;
	mStartSetupFunc = null;
}
*/