class FootballerBodyJumpPhysics {

	static HorizVel_Up = 0;
	static HorizVel_Float = 1;
	static HorizVel_Down = 2;
	
	static Horiz_Decel = 0.1;

	constructor() {
	
		mHorizDir = Vector3();
		mHorizVels = [0.0, 0.0, 0.0];
	}

	function reset() {
	
		mHorizDir.zero();
		mHeightVelocity = null;
	}
	
	function init(body, jumpProfile, jumpDir, jumpDistance, jumpHeight) {
			
		_init(body, jumpProfile, jumpDir, jumpDistance, 
				jumpProfile.ApogeeDistFactor == null ? null : jumpDistance * jumpProfile.ApogeeDistFactor, 
				jumpProfile.FloatDistFactor == null ? null : jumpDistance * jumpProfile.FloatDistFactor, 
				jumpHeight, jumpProfile.FlyUpTime, jumpProfile.EndTime, jumpProfile.ApogeeTime);
	}	
	
	function _init(body, jumpProfile, jumpDir, jumpDistance, distanceAtApogee, distanceAfterFloat, jumpHeight, startTime, endTime, apogeeTime) {
			
		mJumpStartHeight = body.mImpl.getPos().getEl(::Scene_Up);	
					
		local dt = endTime - startTime;
				
		if (jumpDir != null) {
		
			mHorizDir.set(jumpDir);
			mHorizDir.setEl(::Scene_Up, 0.0);
			mHorizDir.normalize();
				
			FootballerBodyJumpPhysics.calcHorizVelocities(jumpProfile, jumpDistance, distanceAtApogee, distanceAfterFloat, mHorizVels);
			
		} else {
				
			mHorizDir.zero();
		}
		
		if (jumpHeight != null && jumpHeight != 0.0) {
		
			mHeightVelocity = 0.0;
			
			local maxHeightDt = apogeeTime - startTime;
			
			mHeightVelocity = jumpHeight / maxHeightDt;
									
		} else {
		
			mHeightVelocity = null;
		}
	}	
	
	function calcHorizVelocities(jumpProfile, jumpDistance, distanceAtApogee, distanceAfterFloat, outVels) {
	
		if (jumpDistance == null)
				jumpDistance = 0.0;
				
		if (distanceAtApogee == null || distanceAtApogee <= 0.0)
			distanceAtApogee = 0.0;
			
		if (distanceAfterFloat == null || distanceAfterFloat <= 0.0)
			distanceAfterFloat = distanceAtApogee;
			
		local dt = jumpProfile.ApogeeTime - jumpProfile.FlyUpTime;	
		outVels[HorizVel_Up] = (distanceAtApogee - (0.5 * -Horiz_Decel * dt * dt)) / (dt);
		
		dt = jumpProfile.FlyDownTime - jumpProfile.ApogeeTime;
		outVels[HorizVel_Float] = ((distanceAfterFloat - distanceAtApogee) - (0.5 * -Horiz_Decel * dt * dt)) / (dt);
		
		dt = jumpProfile.EndTime - jumpProfile.FlyDownTime;
		outVels[HorizVel_Down] = ((jumpDistance - distanceAfterFloat) - (0.5 * -Horiz_Decel * dt * dt)) / (dt);
	}
	
	/*
	function calcTotalDistForApogeeDist(jumpProfile, apogeeDist) {
	
		local horizVels = [0.0, 0.0, 0.0];
	
		FootballerBodyJumpPhysics.calcHorizVelocities(jumpProfile, jumpDistance, apogeeDist, 
							jumpProfile.ApogeeDistFactor == null ? null : jumpDistance * jumpProfile.ApogeeDistFactor, 
							jumpProfile.FloatDistFactor == null ? null : jumpDistance * jumpProfile.FloatDistFactor, horizVels);
				
		
		local dt = jumpProfile.ApogeeTime - jumpProfile.FlyUpTime;
		local d0 = (0.5 * -Horiz_Decel * dt * dt) + (horizVels[HorizVel_Up] * dt);
		
		dt = jumpProfile.FlyDownTime - jumpProfile.ApogeeTime;
		local d1 = (0.5 * -Horiz_Decel * dt * dt) + (horizVels[HorizVel_Float] * dt);
		
		dt = jumpProfile.EndTime - jumpProfile.FlyDownTime;
		local d2 = (0.5 * -Horiz_Decel * dt * dt) + (horizVels[HorizVel_Down] * dt);
		
		return d0 + d1 + d2;
	}
	*/
	
	function updateApplyVelocity(body, dt, velIndex, setBodyMomentum) {
	
		mHorizVels[velIndex] -= Horiz_Decel * dt;
		
		if (mHorizVels[velIndex] < 0.0)
			mHorizVels[velIndex] = 0.0;
	
		body.mImpl.shiftPosByVel(mHorizDir, dt * mHorizVels[velIndex]);
		
		if (setBodyMomentum) {
		
			mHorizDir.mul(dt * mHorizVels[velIndex], body.getMomentum());
			body.onUpdatedMomentum(null);
		}
	}
	
	function updateJumpPhase(body, dt, isJumpUpPhase, setBodyMomentum) {
	
		updateApplyVelocity(body, dt, isJumpUpPhase ? HorizVel_Up : HorizVel_Down, setBodyMomentum);
										
		if (mHeightVelocity != null) {
					
			local pos = body.mImpl.pos();
			
			if (isJumpUpPhase) {
					
				pos.setEl(::Scene_Up, pos.getEl(::Scene_Up) + mHeightVelocity * dt);
				
			} else {
			
				pos.setEl(::Scene_Up, pos.getEl(::Scene_Up) - mHeightVelocity * dt);
			}
		}
	}	
	
	function updateFloatPhase(body, dt, setBodyMomentum) {
	
		updateApplyVelocity(body, dt, HorizVel_Float, setBodyMomentum);
	}
	
	function updateEnded(body, dt, setBodyMomentum) {
	
		body.mImpl.pos().setEl(::Scene_Up, mJumpStartHeight);	
		
		if (setBodyMomentum) {
		
			body.setMomentum(null, null);
		}
	}
	
	
	function update(body, dt, jumpProfile, phaseID, setBodyMomentum) {	
	
		switch (phaseID) {
		
			case FootballerJumpProfile.PhysPhase_JumpUp: {
			
				updateJumpPhase(body, dt, true, setBodyMomentum);
			
			} break;
			
			case FootballerJumpProfile.PhysPhase_JumpFloat: {
			
				updateFloatPhase(body, dt, setBodyMomentum);
			
			} break;
			
			case FootballerJumpProfile.PhysPhase_JumpDown: {
			
				updateJumpPhase(body, dt, false, setBodyMomentum);
			
			} break;
			
			case FootballerJumpProfile.PhysPhase_Ended: {
			
				updateEnded(body, dt, setBodyMomentum);
			
			} break;
		}
	}
	
	mJumpStartHeight = 0.0;
	
	mHorizDir = null;
	mHorizVels = null;
	mHeightVelocity = null;
}