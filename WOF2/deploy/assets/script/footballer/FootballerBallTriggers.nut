
class FootballerBallTriggerBase {

	function setTriggerID(ID) {
	
		mTriggerID = ID;
		return this;
	}

	function frameMove(action, footballer, t) {}
	
	function onBallSelfCollision(action, footballer, ball, boneID) { return true; }	
	function onBallPassiveSelfCollision(action, footballer, ball, boneID, isInFront) { return true; }	
	function onBallFootballerCollision(action, footballer, ball, collider, boneID) { return true; }	
	function onBallObstacleCollision(action, footballer, ball, object) { return true; }	
	function onBallPitchCollision(action, footballer, ball) { return true; }	
	
	function onBallCommandAccepted(action, footballer, command) { return true; }	
	function onBallCommandRejected(action, footballer, ball, commandID) { return true; }	
	function onBallCommand(action, footballer, command) { return true; }	
	function onBallCommandConflictFeedback(action, footballer, command, result) { return true; }
	
	function onBallTriggerVolume(action, footballer, ball, volumeID) { return true; }
	
	mTriggerID = -1;
};

class FootballerBallTrigger_TriggerVolumeAny extends FootballerBallTriggerBase {

	function onBallTriggerVolume(action, footballer, ball, volumeID) { 
	
		return action.onBallTrigger(mTriggerID, footballer, ball);
	}
};

class FootballerBallTrigger_ActiveUpperBodyColl extends FootballerBallTriggerBase {

	function onBallSelfCollision(action, footballer, ball, boneID) {
	
		if (footballer.mBody.mProfile.boneIsActiveUpperBody(boneID)) 
			return action.onBallTrigger(mTriggerID, footballer, ball);
				
		return true;		
	}
};

class FootballerBallTrigger_ActiveBodyColl extends FootballerBallTriggerBase {

	function onBallSelfCollision(action, footballer, ball, boneID) {
	
		if (footballer.mBody.mProfile.boneIsActiveBody(boneID)) 
			return action.onBallTrigger(mTriggerID, footballer, ball);
				
		return true;		
	}
};

class FootballerBallTrigger_FootColl extends FootballerBallTriggerBase {

	function onBallSelfCollision(action, footballer, ball, boneID) {
	
		if (footballer.mBody.mProfile.boneIsFeet(boneID))
			return action.onBallTrigger(mTriggerID, footballer, ball);
			
		return true;	
	}
};

class FootballerBallTrigger_ActiveColl extends FootballerBallTriggerBase {

	function onBallSelfCollision(action, footballer, ball, boneID) {
	
		if (footballer.mBody.mProfile.boneIsActiveBody(boneID) || footballer.mBody.mProfile.boneIsFeet(boneID))
			return action.onBallTrigger(mTriggerID, footballer, ball);
		
		return true;	
	}
};


class FootballerBallTrigger_Pitch extends FootballerBallTriggerBase {

	function onBallPitchCollision(action, footballer, ball) {
	
		return action.onBallTrigger(mTriggerID, footballer, ball);
	}
	
	function frameMove(action, footballer, t) {
	
		local ball = footballer.impl.getBall();
		
		if (ball.isOnPitch()) 
			return onBallPitchCollision(action, footballer, ball);
			
		return true;	
	}
};

class FootballerBallTrigger_PitchRadius extends FootballerBallTriggerBase {

	function setRadius(radius) {
	
		mRadiusSq = radius * radius;
	}

	function onBallPitchCollision(action, footballer, ball) {
	
		if (distSq(footballer.impl.getPos(), ball.getPos()) <= mRadiusSq) 
			return action.onBallTrigger(mTriggerID, footballer, ball);
			
		return true;	
	}
	
	function frameMove(action, footballer, t) {
	
		local ball = footballer.impl.getBall();
		
		if (ball.isOnPitch()) 
			return onBallPitchCollision(action, footballer, ball);
			
		return true;	
	}
	
	mRadiusSq = 0.0;
};

class FootballerBallTrigger_FootControl extends FootballerBallTriggerBase {

	function setRadius(radius) {
	
		mRadiusSq = radius * radius;
	}

	function onBallPitchCollision(action, footballer, ball) {
	
		if (footballer.mBody.ballIsFootControllableEx(ball))
			return action.onBallTrigger(mTriggerID, footballer, ball);
			
		return true;	
	}
	
	function frameMove(action, footballer, t) {
	
		if (footballer.mBody.ballIsFootControllable()) 
			return action.onBallTrigger(mTriggerID, footballer, ball);
			
		return true;	
	}
	
	mRadiusSq = 0.0;
};

class FootballerBallTrigger_SweetSpotApproach extends FootballerBallTriggerBase {

	//P.S also if limited to foot height watch for leaving foot height range
};

/*
body: relatively static: use only on collision (also use feet if possible)
	feet: relatively dynamic: not only on collision but on approriate range:
			problem with simple range: do action while ball is far away although ball is coming closer ... smart trigger
			
*/

/*
class FootballerBallHandler_BodyCollTrigger {

	static State_None = -1;
	static State_WaitingForBallOwnership = 0;
	static State_BallOwner = 1;
	static State_WaitingToTriggerStabilize = 2;
	static State_WaitingForBallPitchTouch = 3;
	
	
	
	handle all possibilities: 
		body: always stiff 
		feet: stiff?
		feet range: smart?
			
	
	function start(action, footballer) {
	}
	
	function stop(action) {
	}
	
	function frameMove(action, footballer, t) {
	
		if (footballer.impl.isBallOwner()) {
		
			
		}
	}

	function onBallSelfCollision(footballer, ball, boneID) {
	
		mAction.handleBall_trigger(mTriggerID, footballer, ball);
	}
	
	function onBallPitchCollision(footballer) {
	
		if (mUseFeetControlRadius) {
	
			mAction.handleBall_trigger(mTriggerID, footballer, null);
		}
	}

	mTriggerID = -1;
	mUseFeetControlRadius = false;
	mFeetControlRadius = 0.0;
	mFeetControlHeight = 0.0;
	mUseFeetSmartControl = false;
};
*/