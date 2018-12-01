class FootballerBrain_AI_GoalGotoEmptySpace extends FootballerBrain_AI_Goal {
	
	static State_Thinking = -1;
	static State_Going = 0;
	static State_ReachingBall = 1;
		
	constructor() {	
		
		mEmptyPos = Vector3();	
		
		mGoalGoto = AI_Goal_ReachPos();
	}
		
	function setRegion(region) {
	
		mRegion = region;
	}	
		
	function start( brain, footballer, t) { 
	
		mState = State_Thinking;
	
		return doReturn(t, Result_Processing); 
	}
	
	function frameMove(brain, footballer, t, dt) { 
	
		local repeat = true;
		
		while (repeat) {
	
			repeat = false;
	
			switch (mState) {
			
				case State_Thinking: {
				
					local fbImpl = footballer.impl;
					local runSettings = footballer.settings.run;
					local runSpeed = runSettings.getSpeedValue(runSettings.Run_Normal, runSettings.RunSpeed_Normal)
				
					local safetyTime = Float();
				
					if (fbImpl.getMatch().findFootballerFreeSpace(fbImpl, mSearchDir, true, 
																	fbImpl.getCOCFwdReach() * 1.5, runSpeed, fbImpl.getMatch().conv(5.0), 0.5, 4, mRegion, mEmptyPos, safetyTime)) {
									
						
						mTryKeepBall = fbImpl.isBallOwner();
									
						mGoalGoto.setTarget(footballer, mEmptyPos);
						mState = State_Going;
						repeat = true;
						
					} else {
					
						brain.body_idle(footballer, t);
					}
					
				} break;
				
				case State_Going: {
				
					local result = mGoalGoto.update(teamAI, brain, footballer, t, dt);
						
					if (result != Result_Processing) {
					
						local acceptEnd = true;
					
						if (mTryKeepBall && result == Result_Success) {
						
							mState = State_ReachingBall;
							repeat = true;
							
						} else {
						
							mState = result;
						}
					} 
			
				} break;
				
				case State_ReachingBall: {
				
					if (brain.body_isIdle(footballer)) {
					
						mState = Result_Success;
					
					} else {
				
						//if owner end, otherwize keep running in same dir until owner or somebody else 'steals' the ball command
						if (footballer.impl.isBallOwner() || !footballer.impl.isLastBallOwner()) {
						
							mState = Result_Success;
						} 
					}
				
				} break;
			}
		}
	
		return doReturn(t, stateToResult(mState));	
	}
	
	mState = -1;
	mSearchDir = null;
	mEmptyPos = null;
	mRegion = null;
	mGoalGoto = null;
	mTryKeepBall = false;
}