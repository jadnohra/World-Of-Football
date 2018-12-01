
class FootballerBrain_AI_Random extends FootballerBrainBase {

	constructor() {
	}	
	
	function start(footballer, t) {
	
		mNextUpdateTime = t;
	
		return true;
	}
	
	function stop(footballer, t) {
	}

	function frameMove(footballer, t, dt) {
	
		if (mStartPos == null) {
		
			mStartPos = Vector3();
			mStartPos.set(footballer.impl.getPos());
		}
	
		if (footballer.impl.getMatch().getTweakEnableAI()) {
	
			local forceRunning = false;
	
			if (t < mNextUpdateTime) {
			
				if (footballer.getActionType() != footballer.Action_Running && footballer.impl.isBallOwner()) {
				
					forceRunning = true;
					mNextUpdateTime = t;
				}
			}
	
			if (t >= mNextUpdateTime) {
			
				mNextUpdateTime = t + randf(2.0, 8.0);
			
				local newState = rand(0.0, 2.0);

				if (forceRunning)
					newState = 1;
				
				switch (newState) {
				
					case 0: {
					
						footballer.getAction().switchIdle(footballer, t, null);
					
					} break;
					
					default: {
					
						local startState = BodyActionRunning_StartState();
						
						local randomDir = rand(0.0, 2.0) == 0 ? true : false;
						
						if (randomDir) {
						
							startState.runDir = Vector3();
							
							startState.runDir.set3(randf(-1.0, 1.0), 0.0, randf(-1.0, 1.0));
							
						} else {
						
							startState.runDir = Vector3();
							
							mStartPos.subtract(footballer.impl.getPos(), startState.runDir);
						}
						
						while (startState.runDir.isZero()) 
							startState.runDir.set3(randf(-1.0, 1.0), 0.0, randf(-1.0, 1.0));
							
				
						startState.runDir.normalize();
						
						{
						
							local pos = footballer.impl.getPos();
							local match = footballer.impl.getMatch();
						
							if (fabs(pos.get(::Scene_Right)) > match.getPitchHalfWidth()) {
						
								if (pos.get(::Scene_Right) > 0.0) {
								
									if (startState.runDir.get(::Scene_Right) > 0.0) {
									
										startState.runDir.setEl(::Scene_Right, -startState.runDir.get(::Scene_Right));
									}
								
								} else {
								
									if (startState.runDir.get(::Scene_Right) < 0.0) {
									
										startState.runDir.setEl(::Scene_Right, -startState.runDir.get(::Scene_Right));
									}
								}
								
								
							} else if (fabs(pos.get(::Scene_Fwd)) > match.getPitchHalfLength()) {
							
								if (pos.get(::Scene_Fwd) > 0.0) {
								
									if (startState.runDir.get(::Scene_Fwd) > 0.0) {
									
										startState.runDir.setEl(::Scene_Fwd, -startState.runDir.get(::Scene_Fwd));
									}
								
								} else {
								
									if (startState.runDir.get(::Scene_Fwd) < 0.0) {
									
										startState.runDir.setEl(::Scene_Fwd, -startState.runDir.get(::Scene_Fwd));
									}
								}
								
							}
						}
						
						startState.bodyFacing = startState.runDir;
						
						startState.runSpeedType = footballer.settings.run.RunSpeed_Normal;
					
						footballer.getAction().switchRunning(footballer, t, startState);
					
					} break;
				}
				
			} else {
			
				if (footballer.getActionType() == footballer.Action_Running) {
			
					local pos = footballer.impl.getPos();
					local match = footballer.impl.getMatch();
					
					if (fabs(pos.get(::Scene_Right)) > match.getPitchHalfWidth()) {
					
						footballer.getAction().switchIdle(footballer, t, null);
						
					} else if (fabs(pos.get(::Scene_Fwd)) > match.getPitchHalfLength()) {
					
						footballer.getAction().switchIdle(footballer, t, null);
					}
				}
			}
						
			
		} else {
		
			footballer.getAction().switchIdle(footballer, t, null);
		}
	}
	
	mNextUpdateTime = 0.0;
	mStartPos = null;
}