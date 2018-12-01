
class footballerBallHandler_Stabilize {

	static Stabilized_None = -1;
	static Stabilized_TouchedPitch = 0;
	static Stabilized_TouchedFeet = 1;

	static State_None = -1;
	static State_WaitingForBallOwnership = 0;
	static State_BallOwner = 1;
	static State_WaitingToTriggerStabilize = 2;
	static State_WaitingForBallPitchTouch = 3;
	
	function start(action, footballer) {
	}
	
	function stop() {
	}

	
	function onBallPassiveSelfCollision(footballer, ball, boneID, isInFront) {
	
		//if handled and ball handing interrupted stop must be called!
		mAction.handleBall_onHit(footballer, ball, boneID);
	}	
	
	
	function onBallSelfCollision(footballer, ball, boneID) {
	
		switch (mState) {
		
			case State_None: 
			case State_WaitingForBallOwnership: {
			
				if (!mAction.handleBall_handleHit(footballer, ball, boneID)) {
			
					doBallActionStabilize(footballer);
				}
			
			} break;
			
			case State_BallOwner: 
			case State_WaitingToTriggerStabilize: {
			
				if (!mAction.handleBall_handleHit(footballer, ball, boneID)) {
			
					mState = State_TriggerStabilizeBall; 
				}
				
			} break;
			
			case State_WaitingForBallPitchTouch: {
			
				if (footballer.settings.boneIsFeet(boneID)) {
				
					mAction.handleBall_onStabilized(Stabilized_TouchedFeet); 
				}
			
			} break;
		}
	}
	
	mAction = null;
	
};