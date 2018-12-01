
class FootballerSettings_Run {

	SpeedTable = 
	[ 
		//Run_Normal
		[6.0, 8.5, 11.0]
		//Run_Dribble
		,[6.0, 7.15, 8.0]
		//Run_StrafeFwd
		,[4.5, 5.0, 5.5]
		//Run_StrafeLeft
		,[4.5, 5.0, 5.5]
		//Run_StrafeRight
		,[4.5, 5.0, 5.5]
		//Run_StrafeBack
		,[3.5, 4.0, 4.5]
	];
	
	StaminaLossSpeedLimit = 8.6;
	
	function getStaminaGain(footballer, runType, runSpeedType) {
	
		switch (footballer.settings.StaminaModel) {

			case footballer.settings.StaminaModel_1: {
	
				local speed = getSafeSpeedValue(runType, runSpeedType);
				
				if (speed >= StaminaLossSpeedLimit)
					return speed * -0.007;
				
				return 0.0;
				
			} break;
			
			default: {
			
				local speed = getSafeSpeedValue(runType, runSpeedType);
				
				return speed * -0.007;
				
			} break;
		}
	}
	
	static AnimNames = ["run", "run", "run", "strafe_left", "strafe_right", "walk"];
	static AnimSpeed = 3.0;
		
	static Run_None = -1;
	static Run_Normal = 0;
	static Run_Dribble = 1;
	static Run_StrafeFwd = 2;
	static Run_StrafeLeft = 3;
	static Run_StrafeRight = 4;
	static Run_StrafeBack = 5;
	static RunTypeCount = 6;
	
	static RunSpeed_None = -1;
	static RunSpeed_Jog = 0;
	static RunSpeed_Normal = 1;
	static RunSpeed_Sprint = 2;
	static RunSpeedTypeCount = 3;
	
	static ShotVelocity = 0;
	static ShotAngle = 1;
	static ShotHorizRatio = 1;
	
	static Shot_None = -1;
	static Shot_Dribble = 0;
	static Shot_Stabilize = 1;
	static ShotTypeCount = 2;
	
	static DribbleShotSwitchInfluence = 30.0; //this should be set to the time needed to get to ball again when dribbling
	
	static RunDirChangeEpsilon = sinf(degToRad(3.0));
	static RunDirUnderSampleCount = 2;
	
	static EnableStrafeCorrectDribble = true;
	StrafeCorrectDribbleSpeed = 0.3;
	
	static StrictDribbleControl = true;
	
	static EnableEasyLockDribble = false;
	
	static ImmediateShots = true;
	
	
	Shots = [
		
		//Shot_Dribble
		//FootballerBallShot(true, ::FootballerBallShot_Foot, 6.25, 5.0, 0.0, 1.0, false) //P.S: footballer current speed will be added to this
		//FootballerBallShot(true, ::FootballerBallShot_Foot, 6.25, 10.0, 0.0, 0.1, false) //P.S: footballer current speed will be added to this
		//FootballerBallShot(true, ::FootballerBallShot_Foot, 4.5, 20.0, 0.0, 0.1, false) //P.S: footballer current speed will be added to this
		FootballerBallShot(true, ::FootballerBallShot_Foot, 5.5, 20.0, 0.0, 0.1, false) //P.S: footballer current speed will be added to this
		//Shot_Stabilize
		FootballerBallShot(true, ::FootballerBallShot_Body, 3.0, -2.0, 0.0, -0.1, false) //P.S: footballer current speed will be added to this
		
	];
		
	constructor() {
	}
	
	function load(match, conv, settings) {
	
		//print(RunDirChangeEpsilon);
	
		if (!settings.EnableSprint) {
		
			foreach (speeds in SpeedTable)
				speeds[RunSpeed_Sprint] = speeds[RunSpeed_Normal];
		}
	
		foreach (speeds in SpeedTable)
			foreach (i, speed in speeds)
				speeds[i] = conv.convUnit(speeds[i]);
				
		foreach (shot in Shots) {
		
			shot.speed = conv.convUnit(shot.speed);		
		}
		
		StaminaLossSpeedLimit = conv.convUnit(StaminaLossSpeedLimit);		
		
		StrafeCorrectDribbleSpeed = conv.convUnit(StrafeCorrectDribbleSpeed);
	}
	
	function getSpeedValue(runType, speedType) { 
	
		return SpeedTable[runType][speedType];
	}
	
	function getSafeSpeedValue(runType, speedType) { 
	
		if (runType < 0 || speedType < 0)
			return 0.0;
	
		return SpeedTable[runType][speedType];
	}
	
	function speedToAnimSpeed(footballer, speed) { 
	
		return AnimSpeed * footballer.impl.getMatch().getCoordSys().invConvUnit(speed); 
	}
	
	function isStrafing(runType) {
	
		return (runType != Run_Normal && runType != Run_Dribble);
	}
	
	function isStrafingHard(runType) {
	
		return ((runType != Run_Normal) && (runType != Run_Dribble) && (runType != Run_StrafeFwd));
	}
	
}

