
class FootballerBrain_AI_Idle extends FootballerBrainBase {

	constructor() {
	}	

	function start(footballer, t) {
	
		return true;
	}
	
	function frameMove(footballer, t, dt) {
	
		footballer.mBody.requestZeroVelocity(null, t, dt);
	
		return true;
	}
}