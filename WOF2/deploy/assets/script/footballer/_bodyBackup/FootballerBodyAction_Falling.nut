
class BodyAction_Falling_StartState {
	
	fallDir = null; //optional
	
	function initFromDir(footballer, fallDir, needsFixing) {
	
		this.fallDir = Vector3();
		this.fallDir.set(fallDir);
		
		if (needsFixing)
			footballer.helperFixDir(this.fallDir);
	}
	
	function initFromFallTargetPos(footballer, targetPos) {
	
		this.fallDir = Vector3();
	
		targetPos.subtract(footballer.impl.getPos(), this.fallDir);
		footballer.helperFixDir(this.fallDir);
	}
	
	function initFromHitPos(footballer, hitPos) {
	
		initFromFallTargetPos(footballer, hitPos);
		this.fallDir.negate();
	}
}


class BodyAction_Falling extends BodyActionBase {

	function getType() { return Footballer.Action_Falling; }
	function createStartState() { return BodyAction_Falling_StartState(); }
	
	constructor() {
	}

	function load(footballer, chunk, conv) {
	
		return true;
	}
	
	function isFinished() { return mIsFinished; }	
	
	function detectFallType(footballer, facing, dir) {
	
		local settings = footballer.settings.fall;
	
		if (dir == null)
			return settings.Fall_Back;
	
		local type;
	
		local tolerance = 0.0175; //sin(1 degrees)
		local dot = facing.dot(dir);

		if (fabs(1.0 - dot) <= tolerance) {

			type = settings.Fall_Back;

		} else {

			local discreeteDir = Vector3();

			local step = discretizeDirection(facing, footballer.impl.getMatch().SceneUpAxis(), dir, 4, discreeteDir);

			switch(step) {

				case 0: type = settings.Fall_Fwd; break;
				case 1: type = settings.Fall_Right; break;
				case 2: type = settings.Fall_Back; break;
				case 3: type = settings.Fall_Left; break;
				default: {
				
					type = settings.Fall_Back; 
					
				} break;
			}
		}
		
		return type;
	}
			
	function start(footballer, t, startState, prevAction) {
	
		mIsFinished = false;
	
		local impl = footballer.impl;
		local fallType = detectFallType(footballer, impl.getFacing(), startState != null ? startState.fallDir : null);
	
		local settings = footballer.settings.fall;
	
		impl.setAnim(settings.AnimNames[fallType], true);
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
		
		{
			if (impl.nodeIsDirty()) {

				footballer.resolveCollisions(footballer, t, true);
			}
		}
		
		handleActionEnd(footballer);	
	}
	
	mIsFinished = true;
}