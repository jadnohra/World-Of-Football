
class FootballerSettings_Idle {

	static StaminaGain = 10.0;
	
	static HighBallFootAction_None = -1;
	static HighBallFootAction_Stabilize = 0;
	static HighBallFootAction_SmoothControl = 1;
	
	static HighBallFootAction = 0;

	static AnimName = "idle";
	static AnimSpeed = 4.0;
	
	static HitAnim_None = -1;
	static HitAnim_Front = 0;
	static HitAnim_Back = 1;
	static HitAnim_Left = 2;
	static HitAnim_Right = 3;
	
	static HitAnimSpeed = 10.0;
	static HitAnimNames = ["hit_front", "hit_back", "hit_left", "hit_right"];
	
	static AlternateAnim_None = -1;
	static AlternateAnim_SweetSpotLeft = 0;
	static AlternateAnim_SweetSpotRight = 1;
	
	static AlternateAnimSpeeds = [30.0, 30.0];
	static AlternateAnimNames = ["sweet_spot_left", "sweet_spot_right"];
	
	function load(match, conv, settings) {
	}
}
