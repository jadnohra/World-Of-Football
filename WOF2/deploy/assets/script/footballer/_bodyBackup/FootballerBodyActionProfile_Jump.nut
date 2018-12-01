
class FootballerBodyProfile_Jump extends FootballerBodyProfile_Base {

	static AnimSpeed = 8.0;
	static AnimNames = ["k_jump_up", "k_jump_up_left", "k_jump_up_right"];
	
	JumpDistance_Default = 3.5;
	JumpDistance_MaxHeight = 1.3;
	
	static Jump_None = -1;
	static Jump_Up = 0;
	static Jump_Left = 1;
	static Jump_Right = 2;
	static JumpTypeCount = 3;

	jumpProfile = null;	
	jumpUpProfile = null;
	
	FlyPhase_HandPosAtApogee = null;
	JumpUp_HandPosAtApogee = null;
	
	static SoundName = "jump";
	static SoundMag = 1.0;
	
	PhysicsPhases = PhaseManager();
	
	function preLoad(match, settings) {
	
		local coordSys = match.impl.getCoordSys();
	
		JumpDistance_Default = coordSys.convUnit(JumpDistance_Default);
		JumpDistance_MaxHeight = coordSys.convUnit(JumpDistance_MaxHeight);
		
		jumpProfile = FootballerJumpProfile();
		jumpProfile.init(0.1, 0.2, 0.35, 0.45, 0.75, 0.9);
		
		return true;
	}
	
	
	function onFirstAttached(footballer, settings) {
	
		local transform = CSceneTransform();
		
		/*
			we can do better than this, we could find the FlyPhase_MaxHeightTime by searching for time with max 
			hand height, something for later	
		*/
	
		{	
			local boneName = "r_hand";
			local animName = AnimNames[Jump_Right];
				
			if (footballer.impl.extractBoneMeshLocalTransformAt(boneName, animName, jumpProfile.ApogeeTime * AnimSpeed, transform)) {
			
				FlyPhase_HandPosAtApogee = Vector3();
			
				FlyPhase_HandPosAtApogee.set(transform.getPos());
			
			} else {
			
				assrt("Footballer mesh missing: " + animName + ", " + boneName);
				return false;
			}
		}
		
		{	
			local boneName = "r_hand";
			local animName = AnimNames[Jump_Up];
				
			if (footballer.impl.extractBoneMeshLocalTransformAt(boneName, animName, 0.5 * AnimSpeed, transform)) {
			
				JumpUp_HandPosAtApogee = Vector3();
			
				JumpUp_HandPosAtApogee.set(transform.getPos());
			
			} else {
			
				assrt("Footballer mesh missing: " + animName + ", " + boneName);
				return false;
			}
		}
			
		return true;	
	}
}

/*
class FootballerSettings_JumpParams {

	constructor() {
	
		flyVelocity = Vector3();
	}

	function init(footballer, jumpType, jumpDir, jumpDistance) {
	
		local settings = footballer.jump;
	
		switch(jumpType) {
		
			case settings.Jump_Left:
			case settings.Jump_Right: {

				flyStartTime = settings.FlyPhase_StartTime;
				flyEndTime = settings.FlyPhase_EndTime;
				
				local dt = flyEndTime - flyStartTime;
				
				if (jumpDir != null) {
				
					if (jumpDistance == null)
						jumpDistance = 0.0;
				
					flyDeceleration = 0.1;
					jumpDir.mul((jumpDistance - (0.5 * flyDeceleration * dt * dt)) / (dt), flyVelocity);
					
				} else {
				
					flyDeceleration = 0.0;
					flyVelocity.zero();
				}
							
			} break;
			
			default: {
			
				flyStartTime = settings.FlyPhase_StartTime;
				flyEndTime = settings.FlyPhase_EndTime;
				
				flyDeceleration = 0.0;
				flyVelocity.zero();
								
			} break;
		}
	}	
	
	flyStartTime = 0.0;
	flyEndTime = 0.0;
	flyVelocity = null;
	flyDeceleration = 0.0;
}
*/