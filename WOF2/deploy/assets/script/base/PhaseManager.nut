
class TimedPhase {

	constructor() {}

	constructor(_id, _phaseLength) {
	
		id = _id;
		phaseLength = _phaseLength;
	}

	function phaseStart(object) {}
	
	function phaseIsFinished(object, phaseTime) {
	
		return phaseTime > phaseLength;
	}
	
	id = null;
	phaseLength = null;
}

class InfinitePhase {

	constructor() {}

	constructor(_id) {
	
		id = _id;
	}

	function phaseStart(object) {}
	
	function phaseIsFinished(object, phaseTime) {
	
		return false;
	}
	
	id = null;
}

class ObjectAskPhase {

	constructor() {}

	constructor(_id) {
	
		id = _id;
	}

	function phaseStart(object) {}
	
	function phaseIsFinished(object, phaseTime) {
	
		return object.phaseIsFinished(this, phaseTime);
	}
	
	id = null;
}

class PhaseManager {

	constructor() {
	
		phases = [];
	}

	function addPhase(phase) {
	
		phases.append(phase);
	}
	
	phases = null;
}

class PhaseWalker {

	function start(object, phaseManager, time) {
	
		startTime = time;
		phaseStartTime = time;
		phaseIndex = 0;
		phases = phaseManager.phases;
		
		return phases.len() > 0;
	}
	
	function getCurrPhase() { return phases[phaseIndex]; }
	
	function advance(object, time) {
	
		if (phaseIndex >= phases.len()) 
			return null;
	
		local phaseTime = time - phaseStartTime;
		
		while (phases[phaseIndex].phaseIsFinished(object, phaseTime)) {
		
			phaseIndex += 1;
			
			if (phaseIndex < phases.len()) {
			
				phases[phaseIndex].phaseStart(object);
				phaseStartTime = time;
				phaseTime = 0.0;
			
			} else {
			
				return null;
			}
		}
		
		return phases[phaseIndex];
	}
	
	startTime = null;
	phaseStartTime = null;
	phaseIndex = null;
	phases = null;
}