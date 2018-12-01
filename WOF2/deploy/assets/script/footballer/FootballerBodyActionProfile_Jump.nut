
class FootballerBodyActionProfile_Jump extends FootballerBodyProfile_Base {

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
		
		jumpUpProfile = FootballerJumpProfile();
		jumpUpProfile.init(0.1, 0.2, 0.35, 0.45, 0.75, 0.9); //needs tuning
		
		return true;
	}
	
	
	function onFirstAttached(body, settings) {
	
		local transform = CSceneTransform();
		
		/*
			we can do better than this, we could find the FlyPhase_MaxHeightTime by searching for time with max 
			hand height, something for later	
		*/
	
		{	
			local boneName = "r_hand";
			local animName = AnimNames[Jump_Right];
				
			if (body.mImpl.extractBoneMeshLocalTransformAt(boneName, animName, jumpProfile.ApogeeTime * AnimSpeed, transform)) {
			
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
				
			if (body.mImpl.extractBoneMeshLocalTransformAt(boneName, animName, 0.5 * AnimSpeed, transform)) {
			
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

