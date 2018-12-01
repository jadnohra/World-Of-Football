class Footballer extends EntityMeshBase {

	function destructor() {
	}

	function load(impl, chunk, conv) {

		local v1 = impl.getMatch().SceneUpAxis();
		local v2 = Vector3();
		local v3 = Vector3();
		
		v2.zero();
		v2.x = 1.0;
		
		v1.mul(v2, v3);
		
		return true;
	}

	
	function frameMove(impl, t, dt) {

		
	}

	
	
}