
class FootballerBodyActionProfile_Idle extends FootballerBodyProfile_Base {

	static StaminaGain = 10.0;
	
	static HighBallFootAction_None = -1;
	static HighBallFootAction_Stabilize = 0;
	static HighBallFootAction_SmoothControl = 1;
	
	static HighBallFootAction = 0;
	
	static Anim_None = -1;
	static Anim_Idle = 0;
	static Anim_SweetSpotLeft = 1;
	static Anim_SweetSpotRight = 2;
	static Anim_HitFront = 3;
	static Anim_HitBack = 4;
	static Anim_HitLeft = 5;
	static Anim_HitRight = 6;
	
	
	static AnimProfiles = 
	[
		//Anim_Idle
		FootballerAnimProfile.create("idle", 4.0, true),
		
		//Anim_SweetSpotLeft
		FootballerAnimProfile.create("sweet_spot_left", 30.0, false),
		//Anim_SweetSpotRight
		FootballerAnimProfile.create("sweet_spot_right", 30.0, false),
		
		//Anim_HitFront
		FootballerAnimProfile.create("hit_front", 10.0, false),
		//Anim_HitBack
		FootballerAnimProfile.create("hit_back", 10.0, false),
		//Anim_HitLeft
		FootballerAnimProfile.create("hit_left", 10.0, false),
		//Anim_HitRight
		FootballerAnimProfile.create("hit_right", 10.0, false),
	];
		
}
