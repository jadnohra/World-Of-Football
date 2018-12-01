
class FootballerBodyActionProfile_Shoot extends FootballerBodyProfile_Base {

	static Anim_None = -1;
	static Anim_Kick = 0;
	static Anim_Header = 1;
	
	static AnimProfiles = 
	[
		//Anim_Kick
		FootballerAnimProfile.create("kick_01", 15.0, false),
		
		//Anim_Header
		FootballerAnimProfile.create("header_jump_run", 20.0, false),
	];
	
	static ShootPhase_StartTime = 0.0;
	
	static KickSoundName = null;
	static KickSoundMag = 1.0;
	
	static HeaderSoundName = null;
	static HeaderSoundMag = 1.0;
	
	static LandSoundName = "jump";
	static LandSoundMag = 0.7;
	
	HeaderJumpDistance_Default = 4.0;
	
	jumpProfile = null;
	
	constructor() {
	}
	
	function preLoad(match, settings) {
	
		local coordSys = match.impl.getCoordSys();
	
		HeaderJumpDistance_Default = coordSys.convUnit(HeaderJumpDistance_Default);
		
		jumpProfile = FootballerJumpProfile();
		jumpProfile.init(0.0, 0.42, 0.42, 0.42, 0.5, null);
		
		return true;
	}

}

