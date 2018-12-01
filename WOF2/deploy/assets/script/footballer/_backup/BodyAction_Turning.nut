
class BodyActionTurning_StartState {

	bodyFacing = null; //optional
	
	function init(dir) {
	
		if (dir != null && !dir.isZero()) {
		
			bodyFacing = Vector3();
			bodyFacing.set(dir);
		}
	}
}

class BodyAction_Turning extends BodyActionBase_BallTriggers {

	function getType(footballer) {
	
		return footballer.Action_Turning;
	}

	constructor() {
	
		mRequestBodyFacing = FootballerVarRequest();
		mRequestBodyFacing.value = Vector3();
		
		mRequestBallShot = FootballerVarRequest();
		mRequestBallShot.value = null;
		
		mRequestBallShotVelScale = 1.0;
		
		mBallShot = FootballerBallShot(false, ::FootballerBallShot_None, 0.0, 0.0, 0.0, 0.0, false);
		
		ballControlOnConstructor();
	}

	function load(footballer, chunk, conv) {

		return true;
	}
	
	function isFinished() { return true; }	
	function getStaminaGain(footballer) { return 0.0; }
	
	function switchTurning(footballer, t, startState) {
	
		return true;
	}
	
	function cancelSetBodyFacing() {

		mRequestBodyFacing.isValid = false;
	}

	function setBodyFacing(footballer, val) {

		if (footballer.impl.getFacing().equals(val) || val.isZero())
			return;
				
		mHasRequests = true;	
		
		local request = mRequestBodyFacing;
			
		request.value.set(val);
		request.isValid = true;
	}
	
	function hasBallShot() {
	
		return mBallShot.isValid;
	}
	
	function cancelBallShot() {
	
		return mBallShot.isValid = false;
	}
	
	function setBallShot(shot, shotVelScale, copyShot, queue) {
	
		if (shot == null || shot.isValid == false) {

			mRequestBallShot.isValid = false;
			mRequestBallShot.value = null;
			return;		
		}
		
		mRequestBallShot.isValid = true;
		
		if (copyShot) {
		
			mRequestBallShot.value = FootballerBallShot(false, ::FootballerBallShot_None, 0.0, 0.0, 0.0, 0.0, false);
			mRequestBallShot.value.set(shot);
			
		} else {
		
			mRequestBallShot.value = shot;
		}
		
		mRequestBallShotVelScale = shotVelScale;
	}
	
	function start(footballer, t, startState, prevAction) {
		
		setAnimIdle(footballer);	
		
		if (startState != null && startState.bodyFacing != null) {
		
			setBodyFacing(footballer, startState.bodyFacing);
		}
		
		ballControlOnStart(footballer, t, startState);
		
		footballer.setBodyMomentum(null);
		return true;
	}
	
	function stop(footballer, t) {
	
		ballControlOnStop(footballer, t);
	}
	
	function setAnimIdle(footballer) {
	
		local impl = footballer.impl;
	
		impl.setAnim(footballer.idle.AnimName, false);
		impl.setAnimLoop(true);
		impl.setAnimSpeed(footballer.idle.AnimSpeed);
	}
	
	function acceptBallShotRequest(footballer) {
	
		if (mRequestBallShot.isValid) {

			//print("accepeted");		
	
			mBallShot.set(mRequestBallShot.value);
			mBallShotVelScale = mRequestBallShotVelScale;
			mRequestBallShot.isValid = false;
			mRequestBallShot.value = null;
		}
	}
	
	function handleRequests(footballer, t) {

		if (mRequestBallShot.isValid) {
		
			acceptBallShotRequest(footballer);
		}
	
		if (mHasRequests) {

			mHasRequests = false;

			if (mRequestBodyFacing.isValid) {

				footballer.impl.setFacing(mRequestBodyFacing.value);
				mRequestBodyFacing.isValid = false;
			}
		} 
		
		return true;
	}
		
	function handleInterBodySignals(footballer, t, dt) {

		return true;
	}		
		
	function frameMove(footballer, t, dt) {

		if (!handleInterBodySignals(footballer, t, dt))
			return;
			
		local dirChanged = mRequestBodyFacing.isValid;
			
		handleRequests(footballer, t);	
			
		local impl = footballer.impl;

		impl.nodeMarkDirty();

		{	
			impl.addAnimTime(dt);
		}
		
		handleBallControlState(footballer, t, dirChanged);
	}

	mHasRequests = false;
	
	mRequestBodyFacing = null;
	mRequestBallShot = null;
	mRequestBallShotVelScale = null;
	
	mBallShot = null;
	mBallShotVelScale = null;
		
	
	/*******************
		Ball Handling
	********************/

	
	function ballControlOnConstructor() {
	}
	
	function ballControlOnStart(footballer, t, startState) {
	}
	
	function ballControlOnStop(footballer, t) {
	}
	
	function doBallActionSweetSpotControl(footballer) {
	
		local impl = footballer.impl;
		local match = impl.getMatch();
	
		impl.setBallControlImmediateTeleport(match.genBallCommandID(), footballer.getSkillBallProtection(), 
												true, footballer.settings.BallSweetSpot, true, true);
			
		//we need to wait for sound to finish!			
		//impl.playSound(footballer.settings.BallControlSoundName, match.getBall().getPos(), footballer.settings.BallControlSoundMag);
	}
	
	function handleBallControlState(footballer, t, dirChanged) {
	
		if (footballer.ballIsFootControlHeight()) {
					
			doBallActionSweetSpotControl(footballer);
		}
	}
	
};