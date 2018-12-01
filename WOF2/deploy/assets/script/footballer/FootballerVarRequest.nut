
class FootballerVarRequest {

	ID = null;
	isValid = false;
	value = null;
	
	function init(body, requestID) {
	
		if (isValid && ID != null)
			body.setRequestResult(ID, FootballerBody.Request_Blocked);
		
		isValid = true;
		ID = requestID;	
	}
	
	function signal(body, requestResult) {
	
		if (isValid && ID != null)
			body.setRequestResult(ID, requestResult);
	}
	
	function end(body, requestResult) {
	
		if (isValid) {
		
			if (ID != null)
				body.setRequestResult(ID, requestResult);
		
			isValid = false;
		}
	}
}
