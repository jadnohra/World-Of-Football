class FootballerBodyJumpPhysics {

	constructor() {
	
		velocity = Vector3();
	}

	function reset() {
	
		startTime = 0.0;
		endTime = 0.0;
				
		deceleration = 0.0;
		velocity.zero();
	}
	
	function init(footballer, jumpDir, jumpDistance, _startTime, _endTime) {
			
		startTime = _startTime;
		endTime = _endTime;
				
		local dt = endTime - startTime;
				
		if (jumpDir != null) {
				
			if (jumpDistance == null)
				jumpDistance = 0.0;
				
			deceleration = 0.1;
			jumpDir.mul((jumpDistance - (0.5 * deceleration * dt * dt)) / (dt), velocity);

		} else {
				
			deceleration = 0.0;
			velocity.zero();
		}
	}	
	
	function update(dt, posDiff) {
	
		velocity.subtractNoReverse(deceleration * dt);
		
		velocity.mul(dt, posDiff);
	}
	
	startTime = 0.0;
	endTime = 0.0;
	velocity = null;
	deceleration = 0.0;
}