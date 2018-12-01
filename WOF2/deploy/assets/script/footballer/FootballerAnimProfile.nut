
class FootballerAnimProfile {

	name = null;
	speed = 1.0;
	loop = false;
	
	function create(_name, _speed, _loop) {
	
		local ret = FootballerAnimProfile();
		
		ret.name = _name;
		ret.speed = _speed;
		ret.loop = _loop;
		
		return ret;
	}
	
	function apply(impl, reset) {
	
		impl.setAnim(name, reset);
		impl.setAnimLoop(loop);
		impl.setAnimSpeed(speed);
	}
}