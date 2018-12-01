class FootballerBrain_AI_GoalPool_v3 {

	constructor() {
	
		mPoolTable = {};
	}

	function getPool(classType) {
	
		local pool = classType in mPoolTable ? mPoolTable[classType] : null;
		
		if (pool == null) {
		
			if (!(classType.parent == AI_Goal)) {
			
				return null;
			}
		
			pool = [];
			mPoolTable[classType] <- pool;
		}
		
		return pool;
	}
	
	function get(classType) {
	
		local pool = getPool(classType);
		local retGoal = pool.len() ? pool.pop() : null;
	
		if (retGoal == null) {
		
			retGoal = classType();
			//retGoal.constructor();
		} 
		
		return retGoal;
	}
		
	function put(goal) {
	
		local classType = goal.getclass();
		local pool = getPool(classType);
		
		goal.stop();
		pool.append(goal);
		
		return null;
	}
	
	mPoolTable = null;
}