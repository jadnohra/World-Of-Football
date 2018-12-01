
class FootballerBodyAction_Turning extends FootballerBodyAction_BaseBallTriggers {

	function getType() { return Footballer.Action_Turning; }

	constructor() {
	
		mRequestBodyFacing = FootballerVarRequest();
		mRequestBodyFacing.value = Vector3();
		
		ballControlOnConstructor();
	}

	function load(body, chunk, conv) {

		return true;
	}
	
	function getStaminaGain(body) { return 0.0; }
	function isSwitchLocked(body) { return true; }
	
	function requestBodyFacing(body, requestID, bodyFacing, needsFixing) {

		if (body.mImpl.getFacing().equals(bodyFacing))
			return ::FootballerBody.Request_Executed;
		
		local request = mRequestBodyFacing;
			
		request.init(body, requestID);
		request.value.set(bodyFacing);
		
		if (needsFixing)
			body.helperFixDir(request.value);
			
		return ::FootballerBody.Request_Executing;
	}
	
	
	function start(body, t) {
		
		setAnim(body, null);	
					
		ballControlOnStart(body, t);
		
		body.setMomentum(null, false);
		return true;
	}
	
	function stop(body, t) {
		
		cancelRequests(body, t);
		ballControlOnStop(body, t);
		
		::FootballerBodyAction_Base.stop(body, t);
	}
	
	function setAnim(body, animIndex) {
	
		if (animIndex == null)
			animIndex = FootballerBodyActionProfile_Idle.Anim_Idle;
	
		body.mProfile.idle.AnimProfiles[animIndex].apply(body.mImpl, false);
	}
	
	function cancelRequests(body, t) {
	
		mRequestBodyFacing.end(body, FootballerBody.Request_Blocked);
	}
	
	function handleRequests(body, t) {

		if (mRequestBodyFacing.isValid) {

			body.mImpl.setFacing(mRequestBodyFacing.value);
			mRequestBodyFacing.end(body, FootballerBody.Request_Executed);
		}
		
		return true;
	}
		
	function handleInterBodySignals(body, t, dt) {

		return true;
	}		
	
		
	function frameMove(body, t, dt) {

		handleInterBodySignals(body, t, dt);
			
		local dirChanged = mRequestBodyFacing.isValid;
			
		handleRequests(body, t);	
			
		local impl = body.mImpl;

		impl.nodeMarkDirty();

		{	
			impl.addAnimTime(dt);
		}
		
		handleBallControlState(body, t, dirChanged);
		
		return false;
	}
	
	mRequestBodyFacing = null;
		
	/*******************
		Ball Handling
	********************/
	
	function ballControlOnConstructor() {
	}
	
	function ballControlOnStart(body, t) {
	}
	
	function ballControlOnStop(body, t) {
	}
	
	function doBallActionSweetSpotControl(body) {
	
		local impl = body.mImpl;
		local match = impl.getMatch();
	
		impl.setBallControlImmediateTeleport(match.genBallCommandID(), body.getSkillBallProtection(), 
												true, body.mProfile.BallSweetSpot, true, true);
			
		//we need to wait for sound to finish!			
		//impl.playSound(body.settings.BallControlSoundName, match.getBall().getPos(), body.settings.BallControlSoundMag);
	}
	
	function handleBallControlState(body, t, dirChanged) {
	
		if (body.ballIsFootControlHeight()) {
					
			doBallActionSweetSpotControl(body);
		}
	}
	
};