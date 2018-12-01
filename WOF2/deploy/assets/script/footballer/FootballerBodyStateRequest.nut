
/*
class FootballerBodyStateRequest {

	static Type_None = -1;
	static Type_BodyFacing = 0;

	type = -1;
	ID = null;
}

class FootballerBodyStateRequest_BodyFacing extends FootballerBodyStateRequest {

	bodyFacing = null;
	
	function constructor(body, _ID, _bodyFacing, needsFixing) {
	
		type = Type_BodyFacing;
		ID = _ID;
		
		bodyFacing = Vector3();
		bodyFacing.set(_bodyFacing);
		
		if (needsFixing)
			body.mFootballer.helperFixDir(bodyFacing);
	}
	
	function check(body, _ID, _bodyFacing, needsFixing) {
	
		if (needsFixing && _bodyFacing.isZero())
			return Request_Invalid;
	
		if (body.mFootballer.impl.getFacing().equals(_bodyFacing))
			return Request_Redundant;
			
		return Request_Accepted;	
	}
	
	function execute(body, t, dt) {
	
		switch (body.mAction.getType()) {
		
			case Footballer.Action_Idle: {
			
				if (body.setActionTurning(ID, t, bodyFacing, false) {
				
					return FootballerBody.Request_Accepted;
				}
			
			} break;
			
			case Footballer.Action_Turning: {
			
				mActionTurning.setBodyFacing(bodyFacing, needsFixing);
			
			
			} break;
		}
		
		return FootballerBody.Request_Blocked;
	}
}
*/