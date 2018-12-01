
class FootballerBrainBase {
	
	//function getCurrHeadShotType(footballer) { return footballer.settings.Shot_None; }
	
	function load(footballer, chunk, conv) {
	
		return true;
	}	
	
	function canSwitchOn(footballer, t) {
	
		return true;
	}
	
	function canSwitchOff(footballer, t) {
	
		return true;
	}
	
	function start(footballer, t) {
	
		return true;
	}
	
	function stop(footballer, t) {
	}

	function frameMove(footballer, t, dt) {
	}
	
	function onSwitchedTeamFormation(footballer, t) {
	}
}