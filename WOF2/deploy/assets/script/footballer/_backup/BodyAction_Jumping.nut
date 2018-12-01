
class BodyActionJumping_StartState {

	jumpDir = null; //optional
	jumpDistance = null;
	jumpType = null; //optional
}

class BodyAction_Jumping extends BodyActionBase {

	function getType(footballer) {
	
		return footballer.Action_Jumping;
	}

	constructor() {
	
		mPhysics = FootballerBodyJumpPhysics();
	}

	function load(footballer, chunk, conv) {
	
		return true;
	}
	
	function isFinished() { return mIsFinished; }	
	
		
	function detectJumpType(footballer, refDir, dir) {
	
		if (dir == null || dir.isZero())
			return footballer.jump.Jump_Up;
	
		if (refDir == null || refDir.isZero())
			refDir = footballer.impl.getFacing();
	
		local type;
	
		local tolerance = 0.0175; //sin(1 degrees)
		local dot = refDir.dot(dir);

		if (fabs(1.0 - dot) <= tolerance) {

			type = footballer.jump.Jump_Up;

		} else {

			local discreeteDir = Vector3();

			local step = discretizeDirection(refDir, footballer.impl.getMatch().SceneUpAxis(), dir, 200, discreeteDir);

			//allow left/right jumps even if its a bit to the back, makes control a bit friendlier
			local leftRightTolerance = 3;
			
			if (step < 0) {
			
				//assrt("Bug: BodyAction_Jumping discretizeDirection");
				type = footballer.jump.Jump_Up; 
			
			} else if (step <= 50 + leftRightTolerance) {
			
				type = footballer.jump.Jump_Right; 
				
			} else if (step < 150 - leftRightTolerance) {
			
				//actually jump back
				type = footballer.jump.Jump_Up;  
				
			} else if (step < 200) {
			
				type = footballer.jump.Jump_Left; 
				
			} else {
			
				//assrt("Bug: BodyAction_Jumping discretizeDirection");
				type = footballer.jump.Jump_Up;
			}
		}
		
		return type;
	}
	
	function getStartStateFromJumpDirs(footballer, refDir, dir, distance) {
	
		local startState = BodyActionJumping_StartState();
		
		startState.jumpDir = dir;
		startState.jumpDistance = distance;
		startState.jumpType = detectJumpType(footballer, refDir, dir);
		
		return startState;
	}

	function start(footballer, t, startState, prevAction) {
		
		mIsFinished = false;
		
		local jumpType = startState != null ? startState.jumpType : footballer.jump.Jump_Up;
		
		if (startState != null) {
		
			local settings = footballer.jump;
		
			switch (startState.jumpType) {
			
				case settings.Jump_Left:
				case settings.Jump_Right: {
				
					mPhysics.init(footballer, startState.jumpDir, startState.jumpDistance, settings.FlyPhase_StartTime, settings.FlyPhase_EndTime);
				
				} break;
				
				default: {
				
					mPhysics.reset();
				} break;
			}
		
		} else {
		
			mPhysics.reset();
		}
		
		local impl = footballer.impl;
		local settings = footballer.jump;
		
		impl.setAnim(settings.AnimNames[jumpType], true);
		impl.setAnimLoop(false);
		impl.setAnimSpeed(settings.AnimSpeed);
		
		impl.playSound(settings.SoundName, impl.getPos(), settings.SoundMag);
		
		mActionStartTime = t;
		
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
		
		local actionTime = t - mActionStartTime;
		
		if (actionTime > mPhysics.startTime && actionTime < mPhysics.endTime) {
		
			local posDiff = Vector3();
		
			mPhysics.update(dt, posDiff);
			
			impl.shiftPos(posDiff);
		} 
		
		
		{
			if (impl.nodeIsDirty()) {

				impl.collUpdate();
				footballer.resolveCollisions(footballer, t, false);
			}
		}
		
		handleActionEnd(footballer);
	}
	
	
	mIsFinished = true;
	mPhysics = null;
	mVelocity = null;
	mActionStartTime = 0.0;
}