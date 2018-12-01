
class FootballerBodyAction_Jumping extends FootballerBodyAction_Base {

	function getType() { return Footballer.Action_Jumping; }

	constructor() {
	
		mPhysics = FootballerBodyJumpPhysics();
		mPhysicsPhase = PhaseWalker();
	}

	function load(body, chunk, conv) {
	
		return true;
	}
	
	function detectJumpType(body, refDir, dir) {
	
		local settings = body.mProfile.jump;
		
		if (dir == null || dir.isZero())
			return settings.Jump_Up;
	
		if (refDir == null || refDir.isZero())
			refDir = body.mImpl.getFacing();
	
		local type;
	
		local tolerance = 0.0175; //sin(1 degrees)
		local dot = refDir.dot(dir);

		if (fabs(1.0 - dot) <= tolerance) {

			type = settings.Jump_Up;

		} else {

			local discreeteDir = Vector3();
			local divCount = 200;
			local divCountMiddle = divCount / 2.0;
			local focusDivCount = 10;

			local step = discretizeDirection(refDir, body.mImpl.getMatch().SceneUpAxis(), dir, divCount, discreeteDir);
			step = step.tofloat();
		
			if (fabs(step) <= focusDivCount) {
			
				type = settings.Jump_Up; 
				
			} else if (fabs(divCountMiddle - step) <= focusDivCount) {
			
				//actually jump back
				type = settings.Jump_Up; 
				
			} else {
			
				if (step < divCountMiddle) {
				
					type = settings.Jump_Right; 
				} else {
				
					type = settings.Jump_Left; 
				}
			}
		}
		
		return type;
	}
	
	function start(body, t, startState, prevAction) {
		
		mInitedStart = false;
		
		body.setBodyMomentum(null, false);
		
		return true;
	}
	
	function setStartJump(body, t, optionalJumpDir, optionalJumpDistance, optionalJumpType, optionalJumpHeight) {
	
		if ((body.mAction == this) && (t == body.mActionStartTime)) {
		
			local settings = body.mProfile.jump;
			
			local jumpType = optionalJumpType != null ? optionalJumpType : settings.Jump_Up;
			
			mPhysicsPhase.start(this, settings.jumpProfile.PhysicsPhases, t);
			
			switch (jumpType) {
				
				case settings.Jump_Left:
				case settings.Jump_Right: {
				
					mPhysics.init(body, settings.jumpProfile, optionalJumpDir, optionalJumpDistance, optionalJumpHeight);
														
				} break;
				
				default: {
				
					mPhysics.reset();
					
				} break;
			}
						
			
			local impl = body.mImpl;
			
			impl.setAnim(settings.AnimNames[jumpType], true);
			impl.setAnimLoop(false);
			impl.setAnimSpeed(settings.AnimSpeed);
			
			impl.playSound(settings.SoundName, impl.getPos(), settings.SoundMag);
		}
	}
	
	function stop(body, t) {
	
		mInitedStart = false;
		
		::FootballerBodyAction_Base.stop(body, t);
	}
	
	function handleActionEnd(body) {
	
		if (body.mImpl.isAnimFinished()) {
		
			return false;
		}
		
		return true;
	}
	
	function frameMove(body, t, dt) {
			
		if (!mInitedStart) {
		
			setStartJump(body, t, null, null, null, null);
		}
			
		local impl = body.mImpl;	
				
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		local enableGhostMode = false;
		
		{
			local physPhase = mPhysicsPhase.advance(this, t);
			local settings = body.mProfile.jump;
		
			if (physPhase == FootballerJumpProfile.PhysPhase_JumpDown)
				enableGhostMode = true;
		
			mPhysics.update(body, dt, settings.jumpProfile, physPhase.id, true);
		}
						
		{
			if (impl.nodeIsDirty()) {

				impl.collUpdate();
				body.resolveCollisions(body, t, enableGhostMode);
			}
		}
		
		return handleActionEnd(body);
	}
	
	
	mInitedStart = false;
	
	mIsFinished = true;
	mPhysics = null;
	mPhysicsPhase = null;
	mVelocity = null;
}