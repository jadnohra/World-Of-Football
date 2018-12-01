
class FootballerBrain_AI_Idle extends FootballerBrainBase {

	constructor() {
	}	

	function start(footballer, t) {
	
		footballer.getAction().switchIdle(footballer, t, null);
	
		return true;
	}
}