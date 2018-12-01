
class FootballerBodyAction_Falling extends FootballerBodyAction_Base {

	function getType() { return Footballer.Action_Falling; }
	function createStartState() { return BodyAction_Falling_StartState(); }
	
	constructor() {
	}

	function load(body, chunk, conv) {
	
		return true;
	}
	
	function detectFallType(body, facing, dir, needsFixing) {
		
		local settings = body.mProfile.fall;
	
		if (dir == null)
			return settings.Fall_Back;
	
		if (needsFixing) {
		
			local fixedDir = Vector3();
			fixedDir.set(dir);
			
			body.mFootballer.helperFixDir(fixedDir);
			dir = fixedDir;
		}
	
		local type;
	
		local tolerance = 0.0175; //sin(1 degrees)
		local dot = facing.dot(dir);

		if (fabs(1.0 - dot) <= tolerance) {

			type = settings.Fall_Back;

		} else {

			local discreeteDir = Vector3();

			local step = discretizeDirection(facing, body.mImpl.getMatch().SceneUpAxis(), dir, 4, discreeteDir);

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
	
	function startFall(body, t, fallDir, needsFixing) {
	
		mStartInited = true;
	
		local impl = body.mImpl;
		local fallType = detectFallType(body, impl.getFacing(), fallDir, needsFixing);
	
		local settings = body.mProfile.fall;
	
		impl.setAnim(settings.AnimNames[fallType], true);
		impl.setAnimLoop(false);
		impl.setAnimSpeed(settings.AnimSpeed);
		
		impl.playSound(settings.SoundName, impl.getPos(), settings.SoundMag);
	}
			
	function createFallDirByTargetPos(body, targetPos) {
	
		local fallDir = Vector3();
	
		targetPos.subtract(body.mImpl.getPos(), fallDir);
		body.mFootballer.helperFixDir(fallDir);
		
		return fallDir;
	}
	
	function createFallDirByHitPos(body, hitPos) {
		
		local fallDir = setFallDirByTargetPos(body, hitPos);
		fallDir.negate();
		
		return fallDir;
	}
			
	function start(body, t) {
	
		mStartInited = false;
		body.setMomentum(null, false);
		
		return true;
	}
	
	function setStartFallDir(body, t, fallDir, needsFixing) {
	
		if ((body.mAction == this) && (t == body.mActionStartTime)) {
	
			startFall(body, t, fallDir, needsFixing);
		}
	}
	
	function stop(body, t) {
	
		mStartInited = false;
	
		::FootballerBodyAction_Base.stop(body, t);
	}
	
	function handleActionEnd(body) {
	
		if (body.mImpl.isAnimFinished()) {
		
			return false;
		}
		
		return true;
	}
	
	function frameMove(body, t, dt) {
		
		if (!mStartInited) {
		
			startFall(body, null, false);
		}
		
		local impl = body.mImpl;		
		
		impl.nodeMarkDirty();
			
		{		
			impl.addAnimTime(dt);		
		}
		
		{
			if (impl.nodeIsDirty()) {

				body.resolveCollisions(t, true);
			}
		}
		
		return handleActionEnd(body);	
	}

	mStartInited = false;
}