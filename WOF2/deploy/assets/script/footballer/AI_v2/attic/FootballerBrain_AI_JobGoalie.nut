class FootballerBrain_AI_JobGoalie extends FootballerBrain_AI_Goal {

	static State_Thinking = 0;
	static State_TriggeringJump = 1;
	static State_Jumping = 2;
	static State_Idle = 3;
	
	constructor() {
	
		mGoalJump = FootballerBrain_AI_GoalGoalieJump();
	}

	function start( brain, footballer, t) { 
	
		mState = State_Thinking;

		brain.body_idle(footballer, t);		
		
		return doReturn(t, Result_Processing); 
	}
	
			
	function frameMove(brain, footballer, t, dt) { 
	
		local repeat = true;
		local ballState = footballer.impl.getMatch().getSpatialBallState();
		
		while (repeat) {
		
			repeat = false;
	
			switch (mState) {
			
				case State_Thinking: {
				
					local doTriggerJump = false;
				
					switch (ballState.finalGetState()) {
					
						/*
						case ballState.FinalBall_Goalie: {
						
							if (ballState.finalGetPitchInTeam() == footballer.impl.getTeamEnum()) {
							
								doTriggerJump = true;
							}
							
						} break;
						
						case ballState.FinalBall_Corner: {
						
							if (ballState.finalGetPitchInTeam() != footballer.impl.getTeamEnum()) {
							
								doTriggerJump = true;
							}
							
						} break;
						*/
						
						case ballState.FinalBall_Goal: {
						
							if (ballState.finalGetPitchInTeam() == footballer.impl.getTeamEnum()) {
							
								doTriggerJump = true;
							}
							
						} break;
						
						case ballState.FinalBall_InPlay:
						case ballState.FinalBall_ThrowIn:
						case ballState.FinalBall_Goalie:
						case ballState.FinalBall_Corner: {
						
							mState = State_Idle;
							
						} break;
						
						default: {
						
							mState = State_Idle;
						}
					}
					
					if (doTriggerJump) {
					
						mState = State_TriggeringJump;
						repeat = true;
					} 
					
					if (mState == State_Thinking) {
					
						brain.body_idle(footballer, t);
						
					} else {
					
						mBallStateID = ballState.finalGetID();
					}
				
				} break;
				
				case State_TriggeringJump: {
				
					if (mBallStateID == ballState.finalGetID()) {
					
						if (mGoalJump.start(brain, footballer, t) == Result_Processing) {
							
							mState = State_Jumping;
							repeat = true;
							
						} else {
						
							mState = State_Idle;
							repeat = true;
						}
						
					} else {
					
						mState = State_Thinking;
						repeat = true;
					}
				
				} break;

			
				case State_Jumping: {
				
					if (mGoalJump.frameMove(brain, footballer, t, dt) != Result_Processing) {
					
						mState = State_Idle;
						repeat = true;
					}
				
				} break;
			
				case State_Idle: {
				
					if (mBallStateID != ballState.finalGetID()) {
					
						mState = State_Thinking;
						repeat = true;
						
					} else {
					
						brain.frameMoveIdle(footballer, t, dt);
					}
				
				} break;
			}
					
		}
		
		return doReturn(t, Result_Processing); 
	}
	
	mGoalJump = null;
	mState = -1;
	mBallStateID = -1;
}