
class FootballerBodyProfile extends FootballerBodyProfile_Base {

	static Shot_None = -1;
	static Shot_Low = 0;
	static Shot_High = 1;
	static Shot_Pass = 2;
	static Shot_Extra = 3;
	static ShotTypeCount = 4;

	static ShotNames = [ "Low", "High", "Pass", "Extra"];
	
	idle = null;
	run = null;
	tackle = null;
	fall = null;
	jump = null;
	shoot = null;

	PitchHeight = 0.0;
	
	COCOffset = Vector3();
	BallSweetSpot = Vector3();
	BallPositionEpsilon = 0.05;
	
	BallHeaderHeight = 1.3;
	
	BallFootControlHeight = 0.5;
	static AdaptiveBallFootControlHeight = true;
	BallFootControlDistMin = 0.3;
	BallFootControlDistMax = 1.0;
	
	//will be squared later, init with simple speed setting
	BallPainSpeedSq = 9.0; 

	static BallSweetSpottingMaxDuration = 0.7;
	BallSweetSpottingSpeed = 0.7;
	
	static BallControlSoundName = "dribble";
	static BallControlSoundMag = 1.0;
	static BallControlSoundMinTime = 0.5;
	static BallControlSoundMaxTime = 2.0;
	
	static BallShotSoundName = "kick";
	static BallShotSwitchInfluence = -10.0;
	
	static BallSoftShotSoundName = "body_ball";
	
	static StaminaModel_1 = 0;
	static StaminaModel_2 = 1;
	
	static StaminaModel = 1;
		
	static BoneID_GroupStart_Feet = 10;
	static BoneID_LeftLeg = 10;
	static BoneID_LeftFoot = 11;
	static BoneID_LeftToe = 12;
	static BoneID_RightLeg = 15;
	static BoneID_RightFoot = 16;
	static BoneID_RightToe = 17;
	static BoneID_GroupEnd_Feet = 17;
	
	static BoneID_GroupStart_BallPain = 20;
	static BoneID_GroupStart_ActiveBody = 20;
	static BoneID_GroupStart_ActiveUpperBody = 20;
	static BoneID_Pelvis = 20;
	static BoneID_Spine = 21;
	static BoneID_Spine1 = 22;
	static BoneID_Neck = 23;
	static BoneID_GroupEnd_BallPain = 23;
	static BoneID_GroupEnd_ActiveUpperBody = 23;
	static BoneID_LeftThigh = 24;
	static BoneID_RightThigh = 25;
	static BoneID_GroupEnd_ActiveBody = 25;
	static BoneID_Head = 30;
	
	static ShotLevelCount = 3;
	static DefaultShotLevel = 1;
	
	static ShotPowerCoeffMin = 0.15;
	static ShotPowerCoeffMax = 2.0;
	
	Shots = [
	
		//Level 0
		[
	
			//Shot_Low
			FootballerBallShot(true, ::FootballerBallShot_Foot, 15.0, 5.0, 0.0, 0.1, true), 
			//Shot_High
			FootballerBallShot(true, ::FootballerBallShot_Foot, 25.0, 10.0, 0.0, 0.1, true), 
			//Shot_Pass
			FootballerBallShot(true, ::FootballerBallShot_Foot, 10.0, 10.0, 0.0, 0.1, false),
			//Shot_Extra
			FootballerBallShot(true, ::FootballerBallShot_Foot, 16.5, 25.0, 0.0, -0.1, true)
		],
		
		//Level 1
		[
	
			//Shot_Low
			FootballerBallShot(true, ::FootballerBallShot_Foot, 50.0, 5.0, 0.0, 0.1, true), 
			//Shot_High
			FootballerBallShot(true, ::FootballerBallShot_Foot, 50.0, 10.65, 0.0, 0.1, true), 
			//Shot_Pass
			FootballerBallShot(true, ::FootballerBallShot_Foot, 20.0, 5.0, 0.0, 0.1, false),
			//Shot_Extra
			FootballerBallShot(true, ::FootballerBallShot_Foot, 27.0, 30.0, 0.0, 0.05, true)
		],
		
		//Level 2
		[
	
			//Shot_Low
			FootballerBallShot(true, ::FootballerBallShot_Foot, 75.0, 5.0, 0.0, 0.1, true), 
			//Shot_High
			FootballerBallShot(true, ::FootballerBallShot_Foot, 75.0, 10.0, 0.0, 0.1, true), 
			//Shot_Pass
			FootballerBallShot(true, ::FootballerBallShot_Foot, 60.0, 10.0, 0.0, 0.1, false),
			//Shot_Extra
			FootballerBallShot(true, ::FootballerBallShot_Foot, 44.0, 25.0, 0.0, -0.1, true)
		],
		
	];	
			
	
	HeadShots = [
	
		//Level 1
		[
	
			//Shot_Low
			FootballerBallShot(true, ::FootballerBallShot_Header, 13.0, -10.0, 0.0, 0.1, false), 
			//Shot_High
			FootballerBallShot(true, ::FootballerBallShot_Header, 12.0, 5.0, 0.0, 0.1, false), 
			//Shot_Pass
			FootballerBallShot(true, ::FootballerBallShot_Header, 10.0, 5.0, 0.0, 0.1, false),
			//Shot_Extra
			FootballerBallShot(true, ::FootballerBallShot_Header, 12.0, 30.0, 0.0, 0.1, false)
		],
		
		//Level 2
		[
	
			//Shot_Low
			FootballerBallShot(true, ::FootballerBallShot_Header, 13.0, -10.0, 0.0, 0.1, false), 
			//Shot_High
			FootballerBallShot(true, ::FootballerBallShot_Header, 12.0, 5.0, 0.0, 0.1, false), 
			//Shot_Pass
			FootballerBallShot(true, ::FootballerBallShot_Header, 10.0, 5.0, 0.0, 0.1, false),
			//Shot_Extra
			FootballerBallShot(true, ::FootballerBallShot_Header, 12.0, 30.0, 0.0, 0.1, false)
		],
		
		//Level 3
		[
	
			//Shot_Low
			FootballerBallShot(true, ::FootballerBallShot_Header, 13.0, -10.0, 0.0, 0.1, false), 
			//Shot_High
			FootballerBallShot(true, ::FootballerBallShot_Header, 12.0, 5.0, 0.0, 0.1, false), 
			//Shot_Pass
			FootballerBallShot(true, ::FootballerBallShot_Header, 10.0, 5.0, 0.0, 0.1, false),
			//Shot_Extra
			FootballerBallShot(true, ::FootballerBallShot_Header, 12.0, 30.0, 0.0, 0.1, false)
		],
	];	
	
	ZeroShot = FootballerBallShot(true, ::FootballerBallShot_Foot, 0.0, 0.0, 0.0, 1.0, false)
	
	BallAfterTouchSideMag = 28.0; 
	BallAfterTouchUpMag = 9.0; 
	BallAfterTouchDownMag = 11.0; 
	BallAfterTouchRotSyncRatio = 0.15;
	
	static EnableInputDirSmoothing = false;
	static InputDirSmoothingValue = 0.05; //3 degrees
	
	static EnableLimitAfterTouchTime = true;
	static AfterTouchTimeLimit = 0.7;
	static EnableLimitAfterTouchReactionTime = true;
	static AfterTouchTimeReactionTime = 0.2;
	static AfterTouchGiveUpIfIdled = false; //set this to false to enable settigng after touch, idling then apply after touch again, if set to false, the moment after touch is idled it is given up
	
	static EnableShotSwitchControlValues = true;
	
	static EnableFootballerPlayerControl = true;
	
	EnableSprint = true;
	
	mWasAttached = false;
			
	constructor() {
	
		idle = FootballerBodyActionProfile_Idle();
		run = FootballerBodyActionProfile_Run();
		tackle = FootballerBodyActionProfile_Tackle();
		fall = FootballerBodyActionProfile_Fall();
		jump = FootballerBodyActionProfile_Jump();
		shoot = FootballerBodyActionProfile_Shoot();
	
		EnableSprint = (StaminaModel == StaminaModel_1);
	}
	
	function preLoad(match) {
	
		local coordSys = match.impl.getCoordSys();
	
		BallSweetSpottingSpeed = coordSys.convUnit(BallSweetSpottingSpeed);
		
		COCOffset.zero();
		COCOffset.setEl(::Scene_Fwd, 0.15);
		coordSys.convVector3Units(COCOffset);
		
		BallHeaderHeight = coordSys.convUnit(BallHeaderHeight);
		
		BallFootControlHeight = coordSys.convUnit(BallFootControlHeight);
		BallFootControlDistMin = coordSys.convUnit(BallFootControlDistMin);
		BallFootControlDistMax = coordSys.convUnit(BallFootControlDistMax);
		
		BallPainSpeedSq = coordSys.convUnit(BallPainSpeedSq);
		BallPainSpeedSq *= BallPainSpeedSq;
		
		BallPositionEpsilon = coordSys.convUnit(BallPositionEpsilon);
		
		foreach (level in Shots) {
		
			foreach (shot in level) {
			
				shot.speed = coordSys.convUnit(shot.speed);		
			}
		}
		
		foreach (level in HeadShots) {
		
			foreach (shot in level) {
			
				if (shot != null)
					shot.speed = coordSys.convUnit(shot.speed);		
			}
		}
		
		BallAfterTouchSideMag = coordSys.convUnit(BallAfterTouchSideMag);
		BallAfterTouchUpMag = coordSys.convUnit(BallAfterTouchUpMag);
		BallAfterTouchDownMag = coordSys.convUnit(BallAfterTouchDownMag);
		
		{
			if (!idle.preLoad(match, this))
				return false;
				
			if (!run.preLoad(match, this))
				return false;
				
			if (!tackle.preLoad(match, this))
				return false;
				
			if (!fall.preLoad(match, this))
				return false;
				
			if (!jump.preLoad(match, this))
				return false;
			
			if (!shoot.preLoad(match, this))
				return false;
		}
			
		return true;
	}
	
	function load(match) {
	
		local coordSys = match.impl.getCoordSys();
	
		coordSys.setRUF(0.1, coordSys.invConvUnit(match.impl.getBall().getRadius()), 0.45, BallSweetSpot);
		
		{
			if (!idle.load(match, this))
				return false;
				
			if (!run.load(match, this))
				return false;
				
			if (!tackle.load(match, this))
				return false;
				
			if (!fall.load(match, this))
				return false;
				
			if (!jump.load(match, this))
				return false;
			
			if (!shoot.load(match, this))
				return false;
		}
		
		return true;
	}
	
	function onFirstAttached(body, bodyProfile) {
	
		PitchHeight = body.mImpl.getPos().getEl(::Scene_Up);
	
		if (!jump.onFirstAttached(body, this))
			return false;
			
		return true;	
	}
	
	function onAttached(body, bodyProfile) {
	
		if (!mWasAttached) {
		
			mWasAttached = true;
			return onFirstAttached(body, this);
		}
		
		return true;
	}
	
	function getShot(level, shotType) {
	
		if (level < 0) 
			return null;
	
		if (shotType < 0) 
			return null;
			
		return Shots[level][shotType];		
	}
	
	function getHeadShot(level, shotType) {
	
		if (level < 0) 
			return null;
	
		if (shotType < 0) 
			return null;
			
		return HeadShots[level][shotType];		
	}
	
	function addTriggerVolumesFeet(impl, leftTriggerID, rightTriggerID) {
	
		local match = impl.getMatch();
		local position = Vector3();
		local radius = 0.0;
	
		{
			radius = match.conv(0.3);
			position.zero();
			position.setEl(::Scene_Up, match.conv(0.2));
		
			impl.addTriggerVolume(leftTriggerID, "l_foot", position, radius);
			
			radius = match.conv(0.42);
			position.setEl(::Scene_Up, match.conv(-0.2));
			position.setEl(::Scene_Fwd, match.conv(-0.12));
			impl.addTriggerVolume(rightTriggerID, "r_foot", position, radius);
		}
	}
	
	function addTriggerVolumeHead(impl, triggerID) {
	
		local match = impl.getMatch();
		local position = Vector3();
		local radius = 0.0;
	
		{
			radius = match.conv(0.5);
			position.zero();
			position.setEl(::Scene_Up, match.conv(0.2));
			position.setEl(::Scene_Right, match.conv(-0.25));
					
			impl.addTriggerVolume(triggerID, "head", position, radius);
		}
	}
	
	function addTriggerVolumes(impl) {
	
		/*
		local match = impl.getMatch();
		local position = Vector3();
		local radius = 0.0;
	
		{
			radius = match.conv(0.25);
			position.zero();
			
			position.setEl(::Scene_Up, match.conv(0.3));
			position.setEl(::Scene_Right, match.conv(-0.2));
			impl.addTriggerVolume(footballer.TriggerVol_Head, "head", position, radius);
		}
		
		*/
			
		/*
		
		{
			radius = match.conv(0.25);
			position.setEl(::Scene_Fwd, match.conv(0.3));
			position.setEl(::Scene_Up, match.conv(0.25));
		
			position.setEl(::Scene_Right, match.conv(-0.3));
			impl.addBallControlVolume(footballer.BallControlVol_LeftFoot, position, radius);
		
		
			position.setEl(::Scene_Right, match.conv(0.3));
			impl.addBallControlVolume(footballer.BallControlVol_RightFoot, position, radius);
		}
		
		
		{
			radius = match.conv(0.07);
			position.setEl(::Scene_Fwd, match.conv(0.3));
			position.setEl(::Scene_Up, match.conv(0.7));
		
			position.setEl(::Scene_Right, match.conv(-0.2));
			impl.addBallControlVolume(footballer.BallControlVol_LeftKnee, position, radius);
		
		
			position.setEl(::Scene_Right, match.conv(0.2));
			impl.addBallControlVolume(footballer.BallControlVol_RightKnee, position, radius);
		}
		
		{
			radius = match.conv(0.013);
			position.setEl(::Scene_Fwd, match.conv(0.2));
			position.setEl(::Scene_Up, match.conv(1.1));
		
			position.setEl(::Scene_Right, match.conv(-0.1));
			impl.addBallControlVolume(footballer.BallControlVol_LeftChest, position, radius);
		
		
			position.setEl(::Scene_Right, match.conv(0.1));
			impl.addBallControlVolume(footballer.BallControlVol_RightChest, position, radius);
		}
		
		{
			radius = match.conv(0.05);
			position.setEl(::Scene_Fwd, match.conv(0.1));
			position.setEl(::Scene_Up, match.conv(1.45));
			position.setEl(::Scene_Right, match.conv(0.0));
		
			impl.addBallControlVolume(footballer.BallControlVol_Head, position, radius);
		}
		
		{
			radius = match.conv(0.05);
			position.setEl(::Scene_Fwd, match.conv(0.1));
			position.setEl(::Scene_Up, match.conv(0.85));
			position.setEl(::Scene_Right, match.conv(0.0));
		
			impl.addBallControlVolume(footballer.BallControlVol_Belly, position, radius);
		}
		*/
		
		return true;
	}
	
	function boneIsFeet(boneID) { return boneID >= BoneID_GroupStart_Feet && boneID <= BoneID_GroupEnd_Feet; }
	function boneIsBallPain(boneID) { return boneID >= BoneID_GroupStart_BallPain && boneID <= BoneID_GroupEnd_BallPain; }
	function boneIsActiveBody(boneID) { return boneID >= BoneID_GroupStart_ActiveBody && boneID <= BoneID_GroupEnd_ActiveBody; }
	function boneIsActiveUpperBody(boneID) { return boneID >= BoneID_GroupStart_ActiveUpperBody && boneID <= BoneID_GroupEnd_ActiveUpperBody; }
	
	function tapCountToShotLevel(tapCount, shotType, isHeader) {
	
		local level;
	
		if (tapCount != null) {
		
			level = tapCount - 1;
			
			if (level < 0)
				level = 0;
				
			if (level >= ShotLevelCount)
				level = ShotLevelCount - 1;
					
			
		} else {
		
			level = DefaultShotLevel;
		}
		
		if (shotType == null || shotType < 0)
			shotType = 0;
			
		return level;
	}	
}

