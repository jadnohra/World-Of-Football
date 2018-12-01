
class FootballerSettings_Shoot {

	static AnimName = "kick_01";
	static AnimSpeed = 15.0;
	
	static HeaderAnimName = "header_jump_run";
	static HeaderAnimSpeed = 20.0;
	
	static ShootPhase_StartTime = 0.0;
	
	static KickSoundName = null;
	static KickSoundMag = 1.0;
	
	static HeaderSoundName = null;
	static HeaderSoundMag = 1.0;
	
	static LandSoundName = "jump";
	static LandSoundMag = 1.0;
	
	HeaderJumpDistance_Default = 4.0;
	
	static FlyPhase_StartTime = 0.0;
	static FlyPhase_EndTime = 0.5;
	
	static Land_TriggerTime = 0.3;
	
	constructor() {
	}
	
	function load(match, conv, settings) {
	
		HeaderJumpDistance_Default = conv.convUnit(HeaderJumpDistance_Default);
	}

}

