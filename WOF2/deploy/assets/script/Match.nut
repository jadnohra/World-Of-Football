
function getGame() {

	return ::game;
}

function getMatch() {

	return ::game.getMatch();
}

function getScriptMatch() {

	return ::match;
}


class Match {

	static LightMethod_Default = -1;
	static LightMethod_Fixed = 0;
	static LightMethod_Cam = 1;
	static LightMethod_OppositeCam = 2;

	function constructor() {
	
		mFixedLightDir = Vector3();
		mShadowDir = Vector3();
	}
	
	function init(scriptEng, matchImpl) {

		mScriptEngine = scriptEng;
		impl = matchImpl;
		
		{
			::Scene_Right <- matchImpl.Scene_Right;
			::Scene_Up <- matchImpl.Scene_Up;
			::Scene_Fwd <- matchImpl.Scene_Fwd;
			
			::Scene_RightAxis <- matchImpl.SceneRightAxis();
			::Scene_UpAxis <- matchImpl.SceneUpAxis();
			::Scene_FwdAxis <- matchImpl.SceneFwdAxis();
			
			::Scene_DownAxis <- matchImpl.SceneDownAxis();
			::Scene_LeftAxis <- matchImpl.SceneLeftAxis();
			::Scene_BackAxis <- matchImpl.SceneBackAxis();
			
			::Scene2D_Right <- matchImpl.Scene2D_Right;
			::Scene2D_Fwd <- matchImpl.Scene2D_Fwd;
		}
		
		
		if (!loadScriptFiles(mScriptEngine)) {

			log("Failed to Load Match Script Files");
			return false;
		}
		
		if (!processResources()) {
		
			log("Failed to Process Resources");
			return false;
		}
		
		mSharedFootballerBodyProfile = FootballerBodyProfile();
		
		if (!mSharedFootballerBodyProfile.preLoad(this)) {
		
			log("Failed to preLoad FootballerSettings");
			return false;
		}
		
		{
			mTeamAIs = [];
			
			{
				local teamAI = TeamAI(this, 0, 1);
				mTeamAIs.append(teamAI);
			}
			
			{
				local teamAI = TeamAI(this, 1, 0);
				mTeamAIs.append(teamAI);
			}
			
			mAIGoalPool = FootballerBrain_AI_GoalPool();
		}
		
		
		::match <- this;
		::game <- impl.getGame();
				
		return true;
	}
	
	function getInstance() {
	
		return ::match;
	}
	
	function getTeamAI(teamEnum) {
	
		return mTeamAIs[teamEnum];
	}
	
	function getAIGoalPool() {
	
		return mAIGoalPool;
	}
	
	function loadScriptFiles(mScriptEngine) {

		if (!mScriptEngine.runFolder("base/", false)) 
			return false;
			
		if (!mScriptEngine.runFolder("footballer/base/", false)) 
			return false;	
			
		if (!mScriptEngine.runFolder("footballer/", false)) 
			return false;
			
		if (!mScriptEngine.runFolder("footballer/AI_v3/", false)) 
			return false;
			
		if (!mScriptEngine.runFolder("footballer/depend/", false)) 
			return false;
		
		if (!mScriptEngine.runFile("util")) 
			return false;
		
				
		return true;
	}
	
	function getModul_NoChange() { return "-modulate 100,100,100"; }
	function getModul_RedToGreen() { return "-modulate 100,100,170"; }
	function getModul_RedToBlue() { return "-modulate 100,100,35"; }
	function getModul_RedToYellow() { return "-modulate 100,100,130"; }

	function getModul_RefToP11() { return getModul_NoChange(); }
	function getModul_RefToP12() { return getModul_RedToBlue(); }
	function getModul_RefToP21() { return getModul_RedToYellow(); }
	function getModul_RefToP22() { return getModul_RedToGreen(); }
	
	function generateTeamKitTextures() {
	
		//convert r_01.png -gravity northwest -fill yellow 
			//-font Tahoma 
			//-pointsize 16 -annotate +163+10 BIANCO 
			//-pointsize 40 -annotate +182+26 2 
			//-pointsize 24 -annotate +28+130 2 
			//out.png
	}
	
	function processResources() {

		local processCOCs = false;
		local processActiveCOCs = false;
		local processArrows = false;
		local processPitch = false;
		local processBoards = false;
		
		/*
			those should be organized into 'effects' thats take structures as parameters
			and input output files should also be more organized in lists with more support
			from file system scanning functions
		*/
		
		if (processCOCs)
		{
		
			local workingDirPath = impl.resolveScriptRelativePath("../mesh/coc/");
			local srcFilePath = "coc_red_circle.*";
			
			local ret = 0;
			
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP11() + " " + "coc_p11.png"))
				return false;						
			
			/*	
			local waterMark = impl.resolveScriptRelativePath("../tex/wmark_image.*");
			
			if (0 != impl.executePlugin("convert.exe", srcFilePath + " " + "-modulate 100,100,120" + " " + dirPath + "temp.png"))
				return false;							
				
			if (0 != impl.executePlugin("composite.exe", "-dissolve 20% -gravity SouthEast " + waterMark + " " + dirPath + "temp.png" + " " + dirPath + "coc_p11.png"))
				return false;						
			*/	
										
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP12() + " " + "coc_p12.png"))
				return false;											
								
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP21() + " " + "coc_p21.png"))
				return false;											
								
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath  + " " + getModul_RefToP22() + " " + "coc_p22.png"))
				return false;							
		}
		
		if (processActiveCOCs)
		{
		
			local workingDirPath = impl.resolveScriptRelativePath("../mesh/coc/");
			local srcFilePath = "coc_red_circle_active.*";
			
			local ret = 0;
			
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP11() + " " + "coc_active_p11.png"))
				return false;						
			
			/*	
			local waterMark = impl.resolveScriptRelativePath("../tex/wmark_image.*");
			
			if (0 != impl.executePlugin("convert.exe", srcFilePath + " " + "-modulate 100,100,120" + " " + dirPath + "temp.png"))
				return false;							
				
			if (0 != impl.executePlugin("composite.exe", "-dissolve 20% -gravity SouthEast " + waterMark + " " + dirPath + "temp.png" + " " + dirPath + "coc_p11.png"))
				return false;						
			*/	
										
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP12() + " " + "coc_active_p12.png"))
				return false;											
								
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP21() + " " + "coc_active_p21.png"))
				return false;											
								
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath  + " " + getModul_RefToP22() + " " + "coc_active_p22.png"))
				return false;							
		}
		
		
		
		if (processArrows)
		{
			local workingDirPath = impl.resolveScriptRelativePath("../mesh/arrow/");
			local srcFilePath = "coc_red_arrow.*";
			
			local ret = 0;
			
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP11() + " " + "arrow_p11.png"))
				return false;						
								
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP12() + " " + "arrow_p12.png"))
				return false;											
								
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP21() + " " + "arrow_p21.png"))
				return false;											
								
			if (0 != impl.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP22() + " " + "arrow_p22.png"))
				return false;							
		}
		
			
		if (processPitch)
		{
			local modulatePitch = false;
			
			local workingDirPath = impl.resolveScriptRelativePath("../mesh/pitch/");
			local modulStr = "";
			
			if (modulatePitch) {
			
				modulStr = "-modulate 100,150,110";
				//modulStr = "-modulate 100,130,150";
				//modulStr = "-charcoal 5";
			} 
					
			local ret = 0;
			
			if (0 != impl.executePlugin("convert.exe", workingDirPath, "ref_grass_center.*" + " " + modulStr + " " + "grass_center.png"))
				return false;						
				
			if (0 != impl.executePlugin("convert.exe", workingDirPath, "ref_grass_corner_2.*" + " " + modulStr + " " + "grass_corner_2.png"))
				return false;							
			
			if (0 != impl.executePlugin("convert.exe", workingDirPath, "ref_grass_generic.*" + " " + modulStr + " " + "grass_generic.png"))
				return false;									
			
			if (0 != impl.executePlugin("convert.exe", workingDirPath, "ref_grass_patt1.*" + " " + modulStr + " " + "grass_patt1.png"))
				return false;											
				
			if (0 != impl.executePlugin("convert.exe", workingDirPath, "ref_grass_penalty.*" + " " + modulStr + " " + "grass_penalty.png"))
				return false;												
								
		} 
		
			
		if (processBoards)
		{
		
			local workingDirPath = impl.resolveScriptRelativePath("../mesh/boards/");
			local fontDirPath = impl.resolveScriptRelativePath("../font/");
				
			local genBoardCount = 0;
			
			local size = "1024x256";
			local backColor = "xc:lightblue";
			local pointSize = "210";
			local gravity = "center";
			local font = "FontCenter.com/Vademecum.ttf";
			local fillColor = "red";
			local stroke = "black";
			local strokeWidth = "1";
			local drawCmd = "\"text 0,0 'W.O.F Rulz'\"";
			
			local sizes = [size, size, size, size, size, size, size, size, size, size];
			local backColors = [backColor, "xc:black", "xc:yellow", "xc:lightgreen", "xc:orange", "xc:white", "xc:brown", "xc:pink", "xc:pink", "xc:white"];
			local pointSizes = [pointSize, "170", "220", pointSize, "180", "160", "200", pointSize, "200", pointSize];
			local gravities = [gravity, gravity, gravity, gravity, gravity, gravity, gravity, gravity, gravity, gravity];
			local fonts = [font, font, font, "FontCenter.com/cuomotype.ttf", font, "FontCenter.com/cuomotype.ttf", font, font, "FontCenter.com/electroharmonix.ttf", "FontCenter.com/electroharmonix.ttf"];
			local fillColors = [fillColor, "blue", "blue", "red", "black", fillColor, fillColor, "white", fillColor, "green"];
			local strokes = [stroke, stroke, stroke, "white", stroke, "blue", stroke, "green", stroke, "red"];
			local strokeWidths = [strokeWidth, strokeWidth, "2", "4", "0", strokeWidth, strokeWidth, "3", strokeWidth, "2"];
			local drawCmds = [drawCmd, "\"text 0,0 'ImageMagick'\"", "\"text -40,-30 'Constant'\"", "\"text 0,20 'ProGrEsS'\"", 
								"\"text 0,0 'Kimera'\"", "\"text 0,0 'Name-In-Game'\"", "\"text 0,-10 'Release'\"", 
								"\"text 0,0 'Game'\"", "\"text 0,0 'Old Skool'\"", "\"text 0,-10 'ArCade'\""];
			
			local j = 1;
			for (local i = 0; i < genBoardCount; i+=1) {
			
				local cmdStr = " -size " + sizes[i]
							+ " " + backColors[i]
							+ " -pointsize " + pointSizes[i]
							+ " -gravity " + gravities[i]
							+ " -font " + fontDirPath + fonts[i]
							+ " -fill " + fillColors[i]
							+ " -stroke " + strokes[i]
							+ " -strokewidth " + strokeWidths[i]
							+ " -draw " + drawCmds[i];
				
				local j1 = j / 10;
				local j2 = j % 10;
							
				if (0 != impl.executePlugin("convert.exe", workingDirPath, cmdStr + " " + "board_" + j1 + j2 + ".png" ))
					return false;
					
				j += 1;	
			}
			
			for (local i = genBoardCount; i < 10; i+=1) {
			
				local j1 = j / 10;
				local j2 = j % 10;
			
				if (0 != impl.executePlugin("convert.exe", workingDirPath, "ref_board_" + j1 + j2 + ".png" + " " + "board_" + j1 + j2 + ".png" ))
					return false;
			
				j += 1;
			}
		
		}
		
		return true;	
	}
	
	function getSharedFootballerBodyProfile() {
	
		return mSharedFootballerBodyProfile;
	}
	
	function load() {
	
		if (!loadLights()) {
		
			log("Failed to Load Script Lights");
			return false;
		}
		
		setupLighting(1.0, 0.65, Match.LightMethod_Fixed, 0.9, 1.0, 0.6);
		setFixedLightDirRUF(-0.05, -0.7, -0.2, true);
		
		if (!mSharedFootballerBodyProfile.load(this)) {
		
			log("Failed to load FootballerSettings");
			return false;
		}
		
		loadShotEstimator();
	
		return true;
	}
	
	function preFrameMove(t, dt) {
	
		mTeamAIs[0].frameMove(this, t, dt);
		mTeamAIs[1].frameMove(this, t, dt);
	}
	
	function postFrameMove(t, dt) {
	}
	
	function preRender() {
	
		setLights();
	}
	
	function postRender() {
	}
	
	function addShotEstimatorRecord(estimator, groupName, autoCreateGroup, shot, shotCoeff) {
	
		local shotSpeed = shot.speed * shotCoeff;
		local ball = impl.getBall();
	
		local shotV = Vector3();
		local shotW = Vector3();
		
		shot.getVelocity(shotSpeed, ball, impl.SceneFwdAxis(), shotSpeed, null, shotV, shotW, 0.0, 0.0, 1.0, true);
		
		local command = CBallCommand();
		command._setVel(ball, impl.genBallCommandID(), 0.0, shotV, shotW, 0.0, false);
	
		estimator.addRecord(groupName, command, autoCreateGroup);
	}
	
	function addShotEstimatorGroup(estimator, groupName, shot, interSampleCount) {
	
		local settings = mSharedFootballerBodyProfile;
	
		addShotEstimatorRecord(estimator, groupName, true, shot, settings.ShotPowerCoeffMin);
		
		local shotCoeffInc = (settings.ShotPowerCoeffMax - settings.ShotPowerCoeffMin) / (interSampleCount + 1);
		local shotCoeff = settings.ShotPowerCoeffMin + shotCoeffInc;
		
		for (local i = 0; i < interSampleCount; i += 1) { 
		
			addShotEstimatorRecord(estimator, groupName, true, shot, shotCoeff);
			
			shotCoeff += shotCoeffInc;
		}
		
		addShotEstimatorRecord(estimator, groupName, true, shot, settings.ShotPowerCoeffMax);
	}
	
	function loadShotEstimator() {

		local estimator = impl.getBallShotEstimator();
		local settings = mSharedFootballerBodyProfile;
		
		for (local i = 0; i < settings.ShotTypeCount; i += 1) { 
		
			addShotEstimatorGroup(estimator, settings.ShotNames[i], settings.getShot(settings.DefaultShotLevel, i), 4);
		}
	}
	
	function setupLighting(globAmbientScale, shadowScale, method, ambientScale, diffuseScale, specularScale) {
	
		mAmbientColor.set(globAmbientScale, globAmbientScale, globAmbientScale, 1.0);
	
		mLightMethod = method;
		mLightKey.ambient().set(ambientScale, ambientScale, ambientScale, 1.0);
		mLightKey.diffuse().set(diffuseScale, diffuseScale, diffuseScale, 1.0);
		mLightKey.specular().set(specularScale, specularScale, specularScale, 1.0);
		
		mShadowColor.set(shadowScale, shadowScale, shadowScale, 1.0);
	}
	
	function colorizeShadow(r, g, b) {
	
		local color = mShadowColor;
		local scale = color.getR();
	
		color.set(scale * r, scale * g, scale * b, 1.0);
	}
	
	function colorizeGlobAmbient(r, g, b) {
	
		local color = mAmbientColor;
		local scale = color.getR();
	
		color.set(scale * r, scale * g, scale * b, 1.0);
	}
	
	function colorizeAmbient(r, g, b) {
	
		local color = mLightKey.ambient();
		local scale = color.getR();
	
		color.set(scale * r, scale * g, scale * b, 1.0);
	}
	
	function colorizeDiffuse(r, g, b) {
	
		local color = mLightKey.diffuse();
		local scale = color.getR();
	
		color.set(scale * r, scale * g, scale * b, 1.0);
	}
	
	function colorizeSpecular(r, g, b) {
	
		local color = mLightKey.specular();
		local scale = color.getR();
	
		color.set(scale * r, scale * g, scale * b, 1.0);
	}
	
	function setFixedLightDirRUF(right, up, forward, linkShadowDir) {
	
		sceneSetRUF(mFixedLightDir, right.tofloat(), up.tofloat(), forward.tofloat());
		mFixedLightDir.normalize();
		
		if (linkShadowDir) {
		
			setShadowDirRUF(-right, up > 0.0 ? up : -up, -forward);
		}
	}
	
	function setShadowDirRUF(right, up, forward) {
	
		sceneSetRUF(mShadowDir, right.tofloat(), up.tofloat(), forward.tofloat());
		
		impl.setShadowDir(mShadowDir);
	}
	
	function loadLights() {
	
		mAmbientColor = CRenderColor();
		mAmbientColor.set(1.0, 1.0, 1.0, 1.0);
	
		mEnableShadowing = impl.isEnabledShadowing();
	
		local shadowIntensity = 0.4;
	
		mShadowColor = CRenderColor();
		mShadowColor.set(1.0 - shadowIntensity, 1.0 - shadowIntensity, 1.0 - shadowIntensity, 1.0);
		
	
		mLightKey = CRenderLight();
		
		mLightKey.setType(mLightKey.Directional);
		mLightKey.diffuse().set(1.0, 1.0, 1.0, 1.0);
		mLightKey.ambient().set(0.9, 0.9, 0.9, 1.0);
		mLightKey.specular().set(0.6, 0.6, 0.6, 1.0);
		mLightKey.direction().set3(0.0, -1.0, 0.0);
		mLightKey.direction().normalize();
		mLightKey.position().set3(0.0, impl.conv(20.0), impl.conv(-50.0));
		mLightKey.attenuation = impl.invConv(0.0001);
		mLightKey.range = impl.conv(1000.0);
		
		mLightBack = CRenderLight();
		
		mLightBack.setType(mLightBack.Directional);
		mLightBack.diffuse().set(1.0, 1.0, 1.0, 1.0);
		mLightBack.ambient().set(0.4, 0.4, 0.4, 1.0);
		mLightBack.specular().set(1.0, 1.0, 1.0, 1.0);
		mLightBack.direction().set3(0.4, 0.25, 0.4);
		mLightBack.direction().normalize();
		mLightKey.attenuation = impl.invConv(0.0001);
		mLightBack.range = impl.conv(1000.0);
		
		mLightFill = CRenderLight();
		
		mLightFill.setType(mLightFill.Point);
		mLightFill.diffuse().set(0.005, 0.005, 0.005, 1.0);
		mLightFill.ambient().set(0.0, 0.0, 0.0, 1.0);
		mLightFill.specular().set(0.0, 0.0, 0.0, 1.0);
		mLightFill.direction().set3(0.0, -1.0, 0.0);
		mLightFill.direction().normalize();
		mLightFill.position().set3(0.0, impl.conv(20.0), 0.0);
		mLightKey.attenuation = impl.invConv(0.0001);
		mLightFill.range = impl.conv(1000.0);
		
		/*
		mLightKey = CRenderLight();
		
		mLightKey.setType(mLightKey.Directional);
		mLightKey.diffuse().set(1.0, 1.0, 1.0, 1.0);
		mLightKey.ambient().set(0.0, 0.0, 0.0, 1.0);
		mLightKey.specular().set(0.0, 0.0, 0.0, 1.0);
		mLightKey.direction().set3(0.0, 1.0, -0.2);
		mLightKey.direction().normalize();
		
		mLightFill = CRenderLight();
		
		mLightFill.setType(mLightFill.Point);
		mLightFill.diffuse().set(0.7, 0.7, 0.7, 1.0);
		mLightFill.ambient().set(0.0, 0.0, 0.0, 1.0);
		mLightFill.specular().set(0.0, 0.0, 0.0, 1.0);
		mLightFill.direction().set3(0.4, 0.25, 0.4);
		mLightFill.direction().normalize();
		mLightFill.attenuation = 0.000001;
		mLightFill.range = 100000;
		
		mLightBack = CRenderLight();
		
		mLightBack.setType(mLightBack.Point);
		mLightBack.diffuse().set(1.0, 1.0, 1.0, 1.0);
		mLightBack.ambient().set(0.0, 0.0, 0.0, 1.0);
		mLightBack.specular().set(0.2, 0.2, 0.2, 1.0);
		mLightBack.direction().set3(0.0, -1.0, 0.0);
		mLightBack.direction().normalize();
		mLightBack.position().set3(0.0, 2000.0, -50000.0);
		mLightBack.attenuation = 0.000001;
		mLightBack.range = 100000;
		*/
		
		return true;
	}
	
	function setLights() {
		
		/*
		{
			local viewDir = impl.getViewTransform().getDir(impl.Scene_Fwd);
			
			mLightBack.direction().set3(-viewDir.get(0) * 1.5, viewDir.get(1) * 2.0, -viewDir.get(2));
			mLightBack.direction().normalize();
		}
		*/
		
		impl.setShadowing(mEnableShadowing, mShadowColor, true, true);
		impl.setAmbient(mAmbientColor);

		switch (mLightMethod) {
		
			case LightMethod_Cam: {
			
				local viewDir = impl.getViewTransform().getDir(impl.Scene_Fwd);
		
				mLightKey.position().set(impl.getViewTransform().getPos());
				mLightKey.direction().set(viewDir);
					
			} break;
		
			case LightMethod_OppositeCam: {
			
				local viewDir = impl.getViewTransform().getDir(impl.Scene_Fwd);
				local viewPos = impl.getViewTransform().getPos();
		
				mLightKey.position().setEl(impl.Scene_Right, -viewPos.getEl(impl.Scene_Right));
				mLightKey.position().setEl(impl.Scene_Fwd, -viewPos.getEl(impl.Scene_Fwd));
				mLightKey.position().setEl(impl.Scene_Up, viewPos.getEl(impl.Scene_Up));
			
				mLightKey.direction().setEl(impl.Scene_Right, -viewDir.getEl(impl.Scene_Right));
				mLightKey.direction().setEl(impl.Scene_Fwd, -viewDir.getEl(impl.Scene_Fwd));
				mLightKey.direction().setEl(impl.Scene_Up, viewDir.getEl(impl.Scene_Up));
		
			} break;
			
			default: {
			
				mLightKey.direction().set(mFixedLightDir);
			}
		}
	
		/*
		{	
			//mLightKey.direction().setEl(impl.Scene_Up, mLightKey.direction().getEl(impl.Scene_Up) * 1.5);
			//mLightKey.direction().normalize();
			
			mLightBack.direction().set3(-viewDir.get(0), viewDir.get(1) * 2.0, -viewDir.get(2));
			mLightBack.direction().normalize();
		}
		*/
			
		impl.setLight(mLightKey, 0);
		//impl.setLight(mLightFill, 1);
		//impl.setLight(mLightBack, 1);
	}
	
	
	mScriptEngine = null;
	impl = null;

	mSharedFootballerBodyProfile = null;
	mTeamAIs = null;
	mAIGoalPool = null;
	
	
	mLightMethod = -1;
	mFixedLightDir = null;
	mShadowDir = null;
	
	mAmbientColor = null;
	mEnableShadowing = true;
	mShadowColor = null;
	mLightKey = null;
	mLightFill = null;
	mLightBack = null;
}