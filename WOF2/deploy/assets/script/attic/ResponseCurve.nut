
class UnitResponseCurve {

	curvature = 1.0; 
	deadZone = 0.0;
	saturation = 1.0;
	min = 0.0;
	max = 1.0;

	constructor(_curvature, _deadZone, _saturation, _min, _max) {
	
		curvature = _curvature; 
		deadZone = _deadZone;
		saturation = _saturation;
		min = _min;
		max = _max;
		
		hasCurvature = curvature != 1.0;
	}
	
	function setCurvature(_curvature) {
	
		curvature = _curvature; 
		hasCurvature = curvature != 1.0;
	}
	
	function fix(value) {
	
		if (value < 0.0) {
		
			value = 0.0;
		
		} else if (value > 1.0) {
		
			value = 1.0;
		}
	
		return value;
	}
	
	function fixPosNeg(value) {
	
		if (value < -1.0) {
		
			value = 0.0;
		
		} else if (value > 1.0) {
		
			value = 1.0;
		}
	
		return value;
	}
	
	function map(value) {
	
		if (hasCurvature) {
		
			value = powf(value, curvature);
		}
		
		if (value <= deadZone || value <= min)
			return min;
			
		if (value >= saturation || value >= max)
			return max;
			
		return value;	
	}
	
	function mapPosNeg(value) {
	
		local neg;
		
		if (value > 0.0) {
		
			neg = false;	
		
		} else {
		
			value = -value;
			neg = true;	
		}
	
		if (hasCurvature) {
		
			value = powf(value, curvature);
		}
		
		if (value <= deadZone || value <= min)
			return neg ? -min : min;
			
		if (value >= saturation || value >= max)
			return neg ? -max : max;
			
		return neg ? -value : value;	
	}
	
	function fixAndMap(value) {
	
		return map(fix(value));
	}
	
	function fixAndMapPosNeg(value) {
	
		return mapPosNeg(fixPosNeg(value));
	}
	
	
	
	hasCurvature = false;
}

class DefaultUnitResponseCurve extends UnitResponseCurve {

	constructor() {}
}

class ResponseCurve extends UnitResponseCurve  {

	maxAmplitude = 1.0;
		
	constructor(_maxAmplitude) {
	
		maxAmplitude = _maxAmplitude;
	}	
	
	constructor(_curvature, _deadZone, _saturation, _min, _max, _maxAmplitude) {
	
		::UnitResponseCurve.constructor(_curvature, _deadZone, _saturation, _min, _max);
	
		maxAmplitude = _maxAmplitude;	
	}
	
	function setMaxAmplitude(_maxAmplitude) {
	
		maxAmplitude = _maxAmplitude;
	}	
	
	function fixAndMap(value) {
	
		local amplitude = fabs(value);
		local unitValue = value / maxAmplitude;
			
		return maxAmplitude * ::UnitResponseCurve.fixAndMap(unitValue);
	}
	
	function fixAndMapPosNeg(value) {
	
		local amplitude = fabs(value);
		local unitValue = value / maxAmplitude;
			
		return maxAmplitude * ::UnitResponseCurve.fixAndMapPosNeg(unitValue);
	}
}

class DefaultResponseCurve extends ResponseCurve {

	constructor() {}
}

