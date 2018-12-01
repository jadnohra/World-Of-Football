
class FootballerJumpProfile {

	static PhysPhase_None = -1;
	static PhysPhase_Prepare = 0;
	static PhysPhase_JumpUp = 1;
	static PhysPhase_JumpFloat = 2;
	static PhysPhase_JumpDown = 3;
	static PhysPhase_Ended = 4;

	FlyUpTime = 0.0;
	ApogeeTime = 0.0;
	FlyDownTime = 0.0;
	EndTime = 0.0;
	
	ApogeeDistFactor = 0.0;
	FloatDistFactor = 0.0;
	
	PhysicsPhases = null;
	
	constructor() {
	
		PhysicsPhases = PhaseManager();
	}
	
	function init(jumpStartTime, jumpFloatTime, jumpDownTime, jumpEndTime, apogeeDistFactor, floatDistFactor) {
	
		{
			FlyUpTime = jumpStartTime;
			ApogeeTime = jumpFloatTime;
			FlyDownTime = jumpDownTime;
			EndTime = jumpEndTime;
			
			local dt = jumpStartTime - 0.0;
			if (dt > 0.0)
				PhysicsPhases.addPhase(TimedPhase(PhysPhase_Prepare, dt));
			
			dt = jumpFloatTime - jumpStartTime;
			if (dt > 0.0)
				PhysicsPhases.addPhase(TimedPhase(PhysPhase_JumpUp, dt));
			
			dt = jumpDownTime - jumpFloatTime;
			if (dt > 0.0)
				PhysicsPhases.addPhase(TimedPhase(PhysPhase_JumpFloat, dt));

			dt = jumpEndTime - jumpDownTime;
			if (dt > 0.0)
				PhysicsPhases.addPhase(TimedPhase(PhysPhase_JumpDown, dt));
			
			PhysicsPhases.addPhase(InfinitePhase(PhysPhase_Ended));
		}
		
		ApogeeDistFactor = apogeeDistFactor;
		FloatDistFactor = floatDistFactor;
	}
	
	function calcTotalDistForApogeeDist(apogeeDist) {	
	
		if (ApogeeDistFactor != null && ApogeeDistFactor > 0.0)
			return apogeeDist / ApogeeDistFactor;
			
		if (FloatDistFactor != null && FloatDistFactor > 0.0)
			return apogeeDist / FloatDistFactor;	
			
		return apogeeDist;
	}
}