
class Footballer {
	
	static TypeInt_Footballer = 0;
	static Type_Footballer = "Footballer";
	
	static Action_None = -1;
	static Action_Idle = 0;
	static Action_Running = 1;
	static Action_Shooting = 2;
	static Action_Heading = 3;
	static Action_Tackling = 4;
	static Action_Falling = 5;
	static Action_Jumping = 6;
	static Action_Turning = 7;
	static Action_Injured = 8;
	static Action_Transition = 9;
	static ActionTypeCount = 10;

	static BallConflictResult_None = -1;
	static BallConflictResult_Lost = 0;
	static BallConflictResult_Tie = 1;
	static BallConflictResult_Won = 2;
	
	function constructor() {
	}

	function destructor() {
	}	
	
	function getTypeInt() { return TypeInt_Footballer; }
	function getType() { return Type_Footballer; }
	function getDefaultRunSpeed() { return mBody.getDefaultRunSpeed(); }
	
	function hasPlayerMode() { return mBrainPlayer != null; }
	function hasAIMode() { return mBrainAI != null; }
	
	function isInAIMode() { return mIsModeAI; }
	function getBrainAI() { return mBrainAI; }
	
	function canActivatePlayerMode(t) {
		
		if (mIsModeAI) 
			return canSwitchBrains(mBrainPlayer, mBrainAI, t);
		
		return true;
	}
	
	function canActivateAIMode(t) {
	
		if (!mIsModeAI) 
			return canSwitchBrains(mBrainAI, mBrainPlayer, t);
		
		return true;
	}
	
	function isSwitchLocked() { return mBody.isSwitchLocked(this); }
	
	function canSwitchBrains(onBrain, offBrain, t) {
				
		return (!mBody.isSwitchLocked(this)) && ((onBrain != null && onBrain.canSwitchOn(this, t)) && (offBrain == null || offBrain.canSwitchOff(this, t)));		
	}
	
	function canSwitchBrains(onBrain, offBrain, t) {
				
		return (!mBody.isSwitchLocked(this)) && ((onBrain != null && onBrain.canSwitchOn(this, t)) && (offBrain == null || offBrain.canSwitchOff(this, t)));		
	}
	
	function activatePlayerMode(controller, t) {
	
		if (mIsModeAI) {
		
			if (controller == null || !controller.isValid())
				return false;
		
			mBrain.stop(this, t);
			
			if (mBrainPlayer.start(this, t, controller)) {
			
				mBrain = mBrainPlayer;
				mIsModeAI = false;			
				
			} else {
			
				assrt("Failed Footballer Brain Player Start");
			}
		} 
		
		return mIsModeAI == false;
	}
	
	function activateAIMode(t) {
	
		if (!mIsModeAI) {
		
			mBrain.stop(this, t);
			
			if (mBrainAI.start(this, t)) {
			
				mBrain = mBrainAI;
				mIsModeAI = true;			
				
			} else {
			
				assrt("Failed Footballer Brain AI Start");
			}
		} 
		
		return mIsModeAI == true;
	}
	
	function load(cimpl, chunk, conv) {

		mImpl = cimpl;
		mBody = FootballerBody();
		mBrainAI = FootballerBrain_AI();
		mBrainPlayer = FootballerBrain_Player();
	
		if (!mBody.load(this, chunk, conv)) {
			
			assrt("Failed: Footballer Body Load");
			return false;
		}
	
		if (!mBrainPlayer.load(this, chunk, conv)) {
			
			assrt("Failed: Footballer BrainPlayer Load");
			return false;
		}
		
		if (!mBrainAI.load(this, chunk, conv)) {
			
			assrt("Failed: Footballer BrainAI Load");
			return false;
		}
			
		loadAttributes(chunk, conv);	
			
		return true;
	}
	
	function prepareScene() {
	
		if (!mBody.prepareScene(this)) {
		
			assrt("Failed: Footballer Body PrepareScene");
			return false;
		}
		
		if (!mBody.start(this, 0.0)) {
		
			assrt("Failed: Footballer Body Start");
			return false;
		}
		
		if (!mBrainAI.start(this, 0.0)) {
		
			assrt("Failed: Footballer Brain AI Start");
			return false;
		}
		
		mBrain = mBrainAI;
		mIsModeAI == true;
		
		return true;
	}
	
	function frameMove(t, dt) {
		
		mBody.preFrameMove(this, t, dt);
		mBrain.frameMove(this, t, dt);
		mBody.frameMove(this, t, dt);
	}
		
	/***************************
		Member Variables
	****************************/	
	
	mImpl = null;	
	mIsModeAI = true;
	
	mBody = null;
	mBrain = null;
	mBrainAI = null;
	mBrainPlayer = null;
		
	
	/***************************
		Notifications
	****************************/
	
	function onLostBall(ball) { mBody.mAction.onLostBall(this, ball); }
	function onAcquiredBall(ball, isSolo) { mBody.mAction.onAcquiredBall(this, ball, isSolo); }
		
	function onBallSelfCollision(ball, boneID) { mBody.mAction.onBallSelfCollision(this, ball, boneID); }	
	function onBallPassiveSelfCollision(ball, boneID, isInFront) { mBody.mAction.onBallPassiveSelfCollision(this, ball, boneID, isInFront); }	
	function onBallFootballerCollision(ball, footballer, boneID) { mBody.mAction.onBallFootballerCollision(this, ball, footballer, boneID); }	
	function onBallObstacleCollision(ball, object) { mBody.mAction.onBallObstacleCollision(this, ball, object); }	
	function onBallPitchCollision(ball) { mBody.mAction.onBallPitchCollision(this, ball); }	
		
	function onBallCommandAccepted(command) { mBody.mAction.onBallCommandAccepted(this, command); }	
	function onBallCommandRejected(ball, commandID) { mBody.mAction.onBallCommandRejected(this, ball, commandID); }	
	function onBallCommand(command) { mBody.mAction.onBallCommand(this, command); }	
	function onBallCommandConflictFeedback(command, result) { mBody.mAction.onBallCommandConflictFeedback(this, command, result); }	
		
	function onBallAcquiredAfterTouch(ball) { mBody.mAction.onBallAcquiredAfterTouch(this, ball); }
	function onBallLostAfterTouch(ball) { mBody.mAction.onBallLostAfterTouch(this, ball); }	
		
	function onBallTriggerVolume(ball, volumeID) { mBody.mAction.onBallTriggerVolume(this, ball, volumeID); }
	
	function onSwitchedTeamFormation(t) { mBrain.onSwitchedTeamFormation(this, t); }
		
	
	/***************************
		Attributes
	****************************/

	static StaticAttributeAIReactionMinTime = 0.0;
	static StaticAttributeAIReactionMaxTime = 0.0;

	AttributeAIReactionMinTime = 0.0;
	AttributeAIReactionMaxTime = 0.0;
	
	function loadAttributes(chunk, conv) {

		local attrChunk = null;
		
		if (chunk.isValid()) 
			attrChunk = chunk.getChild("attributes");
					
		if (attrChunk.isValid()) {
		
			AttributeAIReactionMinTime = attrChunk.getf("AIReactionMinTime", AttributeAIReactionMinTime);
			AttributeAIReactionMaxTime = attrChunk.getf("AIReactionMaxTime", AttributeAIReactionMaxTime);
		}
	}
	
	function getAIReactionTime() { 
	
		return lerpf(AttributeAIReactionMaxTime, AttributeAIReactionMinTime,  mBody.mCurrStamina);
	}
	
	

	/***************************
		Helpers
	****************************/
	
	static HelperDirEpsilonSq = 0.05 * 0.05;
	
	function helperFixDir(dir) {
	
		dir.setEl(::Scene_Up, 0.0);
		
		if (dir.magSq() <= HelperDirEpsilonSq)
			dir.zero();
		else
			dir.normalize();
	}
	
	function helperFixVel(vel) {
	
		vel.setEl(::Scene_Up, 0.0);
	}
	
	function helperFixFootballerGroundPos(pos) {
	
		pos.setEl(::Scene_Up, mBody.mProfile.PitchHeight);
	}
}