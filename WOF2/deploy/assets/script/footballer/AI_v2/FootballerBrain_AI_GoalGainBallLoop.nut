class FootballerBrain_AI_GoalGainBallLoop extends FootballerBrain_AI_Goal {

	constructor() {
	
		mGainBallGoal = FootballerBrain_AI_GoalGainBall();
	}

	function start(brain, footballer, t) {
	
		mReached = footballer.impl.isBallOwner();
	}
	
	function frameMove(brain, footballer, t, dt) { 
	
		local repeat = true;
		
		while (repeat) {
		
			repeat = false;
	
			if (mReached) {
			
				if (!footballer.impl.isBallOwner() && mNextStartTime <= t) {
				
					mReached = false;
					repeat = true;
				}
			
			} else {
			
				if (mGainBallGoal.update(brain, footballer, t, dt) >= Result_Success) {
				
					mReached = true; 
					mNextStartTime = t + mOwnershipLossDelay; 
				}
			}
		}
			
		return doReturn(t, Result_Processing); 
	}
	
	mGainBallGoal = null;
	mReached = false;
	
	static mOwnershipLossDelay = 0.25;
	mNextStartTime = 0.0;
}