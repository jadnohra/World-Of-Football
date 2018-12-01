
class BodyActionBase {
	
	function getStaminaGain(footballer) { return 0.0; }
	function isSwitchLocked(footballer) { return false; }
	
	function switchIdle(footballer, t, startState) {

		if (!isFinished())
			return false;
	
		stop(footballer, t);
		footballer.setAndStartAction(footballer.getActionIdle(), t, startState);
		
		return true;
	}
	
	function switchTurning(footballer, t, startState) {

		if (!isFinished())
			return false;
	
		stop(footballer, t);
		footballer.setAndStartAction(footballer.getActionTurning(), t, startState);
		
		return true;
	}
	
	function switchRunning(footballer, t, startState) {

		if (!isFinished())
			return false;
	
		stop(footballer, t);
		footballer.setAndStartAction(footballer.getActionRunning(), t, startState);
		
		return true;
	}
	
	function switchTackling(footballer, t, startState) {
	
		if (!isFinished())
			return false;
	
		stop(footballer, t);
		footballer.setAndStartAction(footballer.getActionTackling(), t, startState);
		
		return true;
	}
	
	function switchTackled(footballer, t, tackler) {
	
		if (!isFinished())
			return false;
	
		local actionFalling = footballer.getActionFalling();
	
		stop(footballer, t);
		footballer.setAndStartAction(actionFalling, t, actionFalling.createStartState().initFromFallTargetPos(footballer, tackler.impl.getPos()));
		
		return true;
	}
	
	function switchPainHitDir(footballer, t, hitDir, needsFixing) {
	
		if (!isFinished())
			return false;
	
		stop(footballer, t);
		footballer.setAndStartAction(actionFalling, t, footballer.getActionFalling().createStartState().initFromDir(footballer, hitDir, needsFixing));
		
		return true;
	}
	
	function switchJumping(footballer, t, startState) {
	
		if (!isFinished())
			return false;
	
		stop(footballer, t);
		footballer.setAndStartAction(footballer.getActionJumping(), t, startState);
		
		return true;
	}
	
	function switchShooting(footballer, t, startState) {
	
		if (!isFinished())	
			return false;
	
		stop(footballer, t);
		footballer.setAndStartAction(footballer.getActionShooting(), t, startState);
		
		return true;
	}
	
	
	function onLostBall(footballer, ball) {}
	function onAcquiredBall(footballer, ball, isSolo) {}
		
	function onBallSelfCollision(footballer, ball, boneID) {}	
	function onBallPassiveSelfCollision(footballer, ball, boneID, isInFront) {}	
	function onBallFootballerCollision(footballer, ball, collider, boneID) {}	
	function onBallObstacleCollision(footballer, ball, object) {}	
	function onBallPitchCollision(footballer, ball) {}	
		
	function onBallCommandAccepted(footballer, command) {}	
	function onBallCommandRejected(footballer, ball, commandID) {}	
	function onBallCommand(footballer, command) {}	
	function onBallCommandConflictFeedback(footballer, command, result) {}
		
	function onBallAcquiredAfterTouch(footballer, ball) {}
	function onBallLostAfterTouch(footballer, ball) {}	
	
	function onBallTriggerVolume(footballer, volumeID) {}
}

class BodyActionBase_BallTriggers extends BodyActionBase {

	function onBallSelfCollision(footballer, ball, boneID) { foreach (trigger in mBallTriggers) if (!trigger.onBallSelfCollision(this, footballer, ball, boneID)) break; }	
	function onBallPassiveSelfCollision(footballer, ball, boneID, isInFront) { foreach (trigger in mBallTriggers) if (!trigger.onBallPassiveSelfCollision(this, footballer, ball, boneID, isInFront)) break; }	
	function onBallFootballerCollision(footballer, ball, collider, boneID) { foreach (trigger in mBallTriggers) if (!trigger.onBallFootballerCollision(this, footballer, ball, collider, boneID)) break; }	
	function onBallObstacleCollision(footballer, ball, object) { foreach (trigger in mBallTriggers) if (!trigger.onBallObstacleCollision(this, footballer, ball, object)) break; }	
	function onBallPitchCollision(footballer, ball) { foreach (trigger in mBallTriggers) if (!trigger.onBallPitchCollision(this, footballer, ball)) break; }	

	function onBallCommandAccepted(footballer, command) { foreach (trigger in mBallTriggers) if (!trigger.onBallCommandAccepted(this, footballer, command)) break; }	
	function onBallCommandRejected(footballer, ball, commandID) {  foreach (trigger in mBallTriggers) if (!trigger.onBallCommandRejected(this, footballer, ball, commandID)) break; }	
	function onBallCommand(footballer, command) {  foreach (trigger in mBallTriggers) if (!trigger.onBallCommand(this, footballer, command)) break; }	
	function onBallCommandConflictFeedback(footballer, command, result) {  foreach (trigger in mBallTriggers) if (!trigger.onBallCommandConflictFeedback(this, footballer, command, result)) break; }
	
	function onBallTriggerVolume(footballer, ball, volumeID) { foreach (trigger in mBallTriggers) if (!trigger.onBallTriggerVolume(this, footballer, ball, volumeID)) break; }
	
	mBallTriggers = [];
}