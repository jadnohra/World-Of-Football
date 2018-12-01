
class FootballerSettings_Tackle {

	static AnimName = "tackle";
	static AnimSpeed = 9.0;
	
	static PhysPhase_None = -1;
	static PhysPhase_SlideStart = 0;
	static PhysPhase_SlideMiddle = 1;
	static PhysPhase_SlideEnd = 2;
	static PhysPhase_StandUp = 3;
	static PhysPhase_Ended = 4;
	
	static TacklePhase_None = -1;
	static TacklePhase_Tackle = 0;
	static TacklePhase_Ended = 1;
	
	static SlideDecelerationTime = 0.33;

	static SoundName = "tackle";
	static SoundMag = 1.0;
	
	static ShotSoundName = "kick";
	static ShotSoundMag = 1.0;
	
	static ShotSwitchInfluence = 5.0;
	
	static TacklePowerRandomAmount = 0.15;
	
	PhysicsPhases = PhaseManager();
	TacklePhases = PhaseManager();
	
	shot = FootballerBallShot(true, ::FootballerBallShot_Foot, 15.0, 5.0, 0.0, 0.3, false);
	
	constructor() {
	
		PhysicsPhases.addPhase(TimedPhase(PhysPhase_SlideStart, 0.2));
		PhysicsPhases.addPhase(TimedPhase(PhysPhase_SlideMiddle, 0.07));
		PhysicsPhases.addPhase(TimedPhase(PhysPhase_SlideEnd, SlideDecelerationTime));
		PhysicsPhases.addPhase(InfinitePhase(PhysPhase_StandUp));
		
		TacklePhases.addPhase(TimedPhase(TacklePhase_None, 0.1));
		TacklePhases.addPhase(TimedPhase(TacklePhase_Tackle, 0.5));
		TacklePhases.addPhase(InfinitePhase(TacklePhase_Ended));
	}
	
	function load(match, conv, settings) {
	
		shot.speed = conv.convUnit(shot.speed);	
	}
}

class FootballerSettings_Tackle_SlideParams {

	constructor() {
	
		startVelocity = Vector3();
		velocity = Vector3();
	}
	
	function init(footballer, dir, speed) {
	
		local settings = footballer.tackle;
	
		if (speed > 0.0) {
		
			dir.mul(speed, startVelocity);
			deceleration = startVelocity.mag() / settings.SlideDecelerationTime;
					
		} else {
		
			startVelocity.zero();
			deceleration = 0.0;
		}
		
		velocity.set(startVelocity);
	}

	velocity = null;
	startVelocity = null;
	deceleration = 0.0;
}

