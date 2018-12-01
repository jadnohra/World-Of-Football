
class BodyAction_Jumping_StartState {

	jumpDir = null; //optional
	jumpDistance = null;
	jumpType = null; //optional
	jumpHeight = null; //optional
	
	function initFromJumpDirs(footballer, refDir, dir, distance) {
	
		if (dir != null) {
		
			this.jumpDir = Vector3();
			this.jumpDir.set(dir);
		}
		
		this.jumpDistance = distance;
		this.jumpType = detectJumpType(footballer, refDir, dir);
		
		return startState;
	}
}

class FootballerBodyAction_Jumping extends FootballerBodyAction_Base {

	function getType() { return Footballer.Action_Jumping; }
	function createStartState() { return BodyAction_Jumping_StartState(); }

	constructor() {
	
		mPhysics = FootballerBodyJumpPhysics();
		mPhysicsPhase = PhaseWalker();
	}

	function load(footballer, chunk, conv) {
	
		return true;
	}
	
	function isFinished() { return mIsFinished; }	
	
		
	function detectJumpType(footballer, refDir, dir) {
	
		local settings = footballer.settings.jump;
		
		if (dir == null || dir.isZero())
			return settings.Jump_Up;
	
		if (refDir == null || refDir.isZero())
			refDir = footballer.impl.getFacing();
	
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

			local step = discretizeDirection(refDir, footballer.impl.getMatch().SceneUpAxis(), dir, divCount, discreeteDir);
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
	
	

	function start(footballer, t, startState, prevAction) {
		
		local settings = footballer.settings.jump;
		
		mIsFinished = false;
				
		local jumpType = startState != null ? startState.jumpType : settings.Jump_Up;
		
		mPhysicsPhase.start(this, settings.jumpProfile.PhysicsPhases, t);
		
		if (startState != null) {
		
			switch (startState.jumpType) {
			
				case settings.Jump_Left:
				case settings.Jump_Right: {
				
					mPhysics.init(footballer, settings.jumpProfile, startState.jumpDir, startState.jumpDistance, startState.jumpHeight);
														
				} break;
				
				default: {
				
					mPhysics.reset();
					
				} break;
			}
		
		} else {
		
			mPhysics.reset();
		}
		
		local impl = footballer.impl;
		
		impl.setAnim(settings.AnimNames[jumpType], true);
		impl.setAnimLoop(false);
		impl.setAnimSpeed(settings.AnimSpeed);
		
		impl.playSound(settings.SoundName, impl.getPos(), settings.SoundMag);
		
		footballer.setBodyMomentum(null, false);
		return true;
	}
	
	function stop(footballer, t) {
	
		mIsFinished = true;
	}
	
	function handleActionEnd(footballer) {
	
		if (footballer.impl.isAnimFinished()) {
		
			mIsFinished = true;
		}
	}
	
	function frameMove(footballer, t, dt) {
			
		if (mIsFinished) {
			
			switchIdle(footballer, t, null);
			return;
		}	
			
		local impl = footballer.impl;	
				
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		local enableGhostMode = false;
		
		{
			local physPhase = mPhysicsPhase.advance(this, t);
			local settings = footballer.settings.jump;
		
			if (physPhase == FootballerJumpProfile.PhysPhase_JumpDown)
				enableGhostMode = true;
		
			mPhysics.update(footballer, dt, settings.jumpProfile, physPhase.id, true);
		}
						
		{
			if (impl.nodeIsDirty()) {

				impl.collUpdate();
				footballer.resolveCollisions(footballer, t, enableGhostMode);
			}
		}
		
		handleActionEnd(footballer);
	}
	
	
	mIsFinished = true;
	mPhysics = null;
	mPhysicsPhase = null;
	mVelocity = null;
}