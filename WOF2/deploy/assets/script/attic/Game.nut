
class Game {

	function init(scriptEngine) {
	
		impl = scriptEngine.getGame();
		impl.getVersion();
		
		return true;
	}
	
	function getMatch() { return impl.getMatch(); }
	
	function onMatchLoaded(match) {
	
		local estimator = match.getBallShotEstimator();
		
		return true;
	}
	
	function loadMatchLights(match) {
	
		ambientColor = CRenderColor();
		ambientColor.set(1.0, 1.0, 1.0, 1.0);
	
		enableShadowing = true;
	
		shadowColor = CRenderColor();
		shadowColor.set(0.75, 0.75, 0.75, 1.0);
		
	
		lightKey = CRenderLight();
		
		lightKey.setType(lightKey.Directional);
		lightKey.diffuse().set(1.0, 1.0, 1.0, 1.0);
		lightKey.ambient().set(0.9, 0.9, 0.9, 1.0);
		lightKey.specular().set(0.6, 0.6, 0.6, 1.0);
		lightKey.direction().set3(0.0, -1.0, 0.0);
		lightKey.direction().normalize();
		lightKey.position().set3(0.0, match.conv(20.0), match.conv(-50.0));
		lightKey.attenuation = match.invConv(0.0001);
		lightKey.range = match.conv(1000.0);
		
		lightBack = CRenderLight();
		
		lightBack.setType(lightBack.Directional);
		lightBack.diffuse().set(1.0, 1.0, 1.0, 1.0);
		lightBack.ambient().set(0.4, 0.4, 0.4, 1.0);
		lightBack.specular().set(1.0, 1.0, 1.0, 1.0);
		lightBack.direction().set3(0.4, 0.25, 0.4);
		lightBack.direction().normalize();
		lightKey.attenuation = match.invConv(0.0001);
		lightBack.range = match.conv(1000.0);
		
		lightFill = CRenderLight();
		
		lightFill.setType(lightFill.Point);
		lightFill.diffuse().set(0.005, 0.005, 0.005, 1.0);
		lightFill.ambient().set(0.0, 0.0, 0.0, 1.0);
		lightFill.specular().set(0.0, 0.0, 0.0, 1.0);
		lightFill.direction().set3(0.0, -1.0, 0.0);
		lightFill.direction().normalize();
		lightFill.position().set3(0.0, match.conv(20.0), 0.0);
		lightKey.attenuation = match.invConv(0.0001);
		lightFill.range = match.conv(1000.0);
		
		/*
		lightKey = CRenderLight();
		
		lightKey.setType(lightKey.Directional);
		lightKey.diffuse().set(1.0, 1.0, 1.0, 1.0);
		lightKey.ambient().set(0.0, 0.0, 0.0, 1.0);
		lightKey.specular().set(0.0, 0.0, 0.0, 1.0);
		lightKey.direction().set3(0.0, 1.0, -0.2);
		lightKey.direction().normalize();
		
		lightFill = CRenderLight();
		
		lightFill.setType(lightFill.Point);
		lightFill.diffuse().set(0.7, 0.7, 0.7, 1.0);
		lightFill.ambient().set(0.0, 0.0, 0.0, 1.0);
		lightFill.specular().set(0.0, 0.0, 0.0, 1.0);
		lightFill.direction().set3(0.4, 0.25, 0.4);
		lightFill.direction().normalize();
		lightFill.attenuation = 0.000001;
		lightFill.range = 100000;
		
		lightBack = CRenderLight();
		
		lightBack.setType(lightBack.Point);
		lightBack.diffuse().set(1.0, 1.0, 1.0, 1.0);
		lightBack.ambient().set(0.0, 0.0, 0.0, 1.0);
		lightBack.specular().set(0.2, 0.2, 0.2, 1.0);
		lightBack.direction().set3(0.0, -1.0, 0.0);
		lightBack.direction().normalize();
		lightBack.position().set3(0.0, 2000.0, -50000.0);
		lightBack.attenuation = 0.000001;
		lightBack.range = 100000;
		*/
	
		return true;
	}
	
	function setMatchLights(match) {
	
		/*
		{
			local viewDir = match.getViewTransform().getDir(match.Scene_Fwd);
			
			lightBack.direction().set3(-viewDir.get(0) * 1.5, viewDir.get(1) * 2.0, -viewDir.get(2));
			lightBack.direction().normalize();
		}
		*/
		
		match.setShadowing(enableShadowing, shadowColor, true, true);
		match.setAmbient(ambientColor);
				
		{
			local viewDir = match.getViewTransform().getDir(match.Scene_Fwd);
		
			lightKey.position().set(match.getViewTransform().getPos());
			lightKey.direction().set(viewDir);
			
			lightBack.direction().set3(-viewDir.get(0), viewDir.get(1) * 2.0, -viewDir.get(2));
			lightBack.direction().normalize();
		}
			
		match.setLight(lightKey, 0);
		//match.setLight(lightFill, 1);
		//match.setLight(lightBack, 1);
		
				
		return true;
	}
	
	impl = null;
	
	ambientColor = null;
	
	enableShadowing = true;
	shadowColor = null;
	
	lightKey = null;
	lightFill = null;
	lightBack = null;
}