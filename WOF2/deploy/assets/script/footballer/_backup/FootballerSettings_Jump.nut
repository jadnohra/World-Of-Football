
class FootballerSettings_Jump {

	static AnimSpeed = 8.0;
	static AnimNames = ["k_jump_up", "k_jump_up_left", "k_jump_up_right"];
	
	JumpDistance_Default = 2.0;
	
	static Jump_None = -1;
	static Jump_Up = 0;
	static Jump_Left = 1;
	static Jump_Right = 2;
	static JumpTypeCount = 3;
	
	static Phase_None = -1;
	static Phase_Prepare = 0;
	static Phase_Fly = 1;
	static Phase_StandUp = 2;
	
	static FlyPhase_StartTime = 0.1;
	static FlyPhase_EndTime = 0.4;
	
	static SoundName = "jump";
	static SoundMag = 1.0;
	
	function load(match, conv, settings) {
	
		JumpDistance_Default = conv.convUnit(JumpDistance_Default);
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