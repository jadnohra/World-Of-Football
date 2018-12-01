
class BodyActionFalling_StartState {
	
	fallDir = null; //optional
}


class BodyAction_Falling extends BodyActionBase {

	function getType(footballer) {
	
		return footballer.Action_Falling;
	}

	constructor() {
	}

	function load(footballer, chunk, conv) {
	
		return true;
	}
	
	function isFinished() { return mIsFinished; }	
	

	function getStartStateFromFallTargetPos(footballer, targetPos) {
	
		local startState = BodyActionFalling_StartState();
		startState.fallDir = Vector3();
	
		targetPos.subtract(footballer.impl.getPos(), startState.fallDir);
		startState.fallDir.normalize();
		
		return startState;
	}
	
	function getStartStateFromHitPos(footballer, hitPos) {
	
		local startState = getStartStateFromFallTargetPos(footballer, hitPos);
		startState.fallDir.negate();
		
		return startState;
	}
	
	function detectFallType(footballer, facing, dir) {
	
		if (dir == null)
			return footballer.fall.Fall_Back;
	
		local type;
	
		local tolerance = 0.0175; //sin(1 degrees)
		local dot = facing.dot(dir);

		if (fabs(1.0 - dot) <= tolerance) {

			type = footballer.fall.Fall_Back;

		} else {

			local discreeteDir = Vector3();

			local step = discretizeDirection(facing, footballer.impl.getMatch().SceneUpAxis(), dir, 4, discreeteDir);

			switch(step) {

				case 0: type = footballer.fall.Fall_Fwd; break;
				case 1: type = footballer.fall.Fall_Right; break;
				case 2: type = footballer.fall.Fall_Back; break;
				case 3: type = footballer.fall.Fall_Left; break;
				default: {
				
					/*
					assrt("Bug: BodyAction_Falling discretizeDirection facing(" + facing.get(0) + "," + facing.get(1) + ","+ facing.get(2) + ") type(" + type.get(0) + "," + type.get(1) + ","+ type.get(2) + ") steps(" + step + ")"); 
					local stepDebug = discretizeDirection(facing, impl.getMatch().SceneUpAxis(), dir, 4, discreeteDir);
					*/
					//assrt("Bug: BodyAction_Falling discretizeDirection");
					type = footballer.fall.Fall_Back; 
					
				} break;
			}
		}
		
		return type;
	}
			
	function start(footballer, t, startState, prevAction) {
	
		mIsFinished = false;
	
		local impl = footballer.impl;
		local fallType = detectFallType(footballer, impl.getFacing(), startState != null ? startState.fallDir : null);
	
		local settings = footballer.fall;
	
		impl.setAnim(settings.AnimNames[fallType], true);
		impl.setAnimLoop(false);
		impl.setAnimSpeed(settings.AnimSpeed);
		
		impl.playSound(settings.SoundName, impl.getPos(), settings.SoundMag);
		
		footballer.setBodyMomentum(null);
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