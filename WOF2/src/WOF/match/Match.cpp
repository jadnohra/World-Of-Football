#include "Match.h"

#include "WE3/WEAssert.h"
#include "WE3/WEMacros.h"
#include "WE3/WEAppPath.h"
#include "WE3/filePath/WEPathResolver.h"
#include "WE3/binding/win/includeWindows.h"
#include "WE3/helper/load/WEMathDataLoadHelper.h"
#include "WE3/helper/load/WERenderDataLoadHelper.h"
#include "WE3/input/script/InputManager.h"
#include "WE3/data/WEDataSourceCreator.h"
#include "WE3/defaultImpl/input/Loader.h"
using namespace WE;

#include "inc/Entities.h"
#include "WE3/WEDebugBypass.h"
#include "../WOF.h"

namespace WOF { namespace match {

Match::Match(Game& game, NativeAppInfo& nativeAppInfo, Renderer& renderer, Log& log, Console& console, FramePerformance& perf, ScriptEngine& scriptEngine) 
				: mIsLoaded(false), mIsDestroying(false), mBallCommandCounter(0),
					mCOCSprite(true, false, true, &m3DPitchSpriteDimMap),
					mArrowSprite(true, false, true),
					mBallPitchMarkerSprite(true, false, true, &m3DPitchSpriteDimMap),
					mShapeSprite_Circle(true, true, true), 
					mShapeSprite_Quad(true, true, true) {

	mIsProcessing = false;
	mIsRendering = false;

	mTeamWithFwdSceneDir = Team_A;

	mGame = &game;
	mFirstFrame = true;

	mLoadErrorFlag = false;

	mTweakEnableDebugKeys = game.mTweakEnableDebugKeys;
	
	mDumpInputDevices = false;
	mDumpInputDevicesPath.assign(L"InputDevicesDump.txt");
	

	mNativeAppInfo = &nativeAppInfo;
	mLog = &log;
	mConsole = &console;
	mFramePerformance = &perf;
	mScriptEngine = &scriptEngine;

	{
		ScriptVarTable& varTable = dref(getLoadContext().varTablePtr());
		varTable.setScriptEngine(mScriptEngine);
	}

	mPerfLagCount = 0;

	mIsSingleFrameStepping = false;
#ifdef _DEBUG
		mIsDebugging = false;
#endif

	mPauseIfNotActive = true;
	mRenderIfNotActive = true;

	mFlagExtraRenders = false;
	mFlagRenderBPVolumes = false;
	mFlagRenderVolumes = false;
	mFlagRenderDetailVolumes = false;
	mFlagRenderNPColliders = false;
	mFlagRenderEntityStates = false;
	mFlagRenderSpatialManager = false;
	mFlagRenderBallShotEstimator = false;

	mRenderer = &renderer;
	mRenderer->setRenderState(RS_EnableShadowing, true);
	mRenderer->enableQueuedVols(true);

	Plane plane;

	plane.init(Point(0.0f, 1.0f, 0.0f), Vector(0.0f, 1.0f, 0.0f), true);
	mRenderer->setupShadowingPlanar(Vector3(0.2f, 0.7f, 0.2f), plane);

	mLightEnables.reserve(2);
	
	m3DPitchSpriteDimMap.mapX = Scene_Right;
	m3DPitchSpriteDimMap.mapY = Scene_Forward;
	m3DPitchSpriteDimMap.mapNone = Scene_Up;

	/*
	RenderLight light;

	light.diffuse.set(0.7f);
	light.ambient.set(0.0f);
	light.specular.set(0.55f);

	light.direction.set(-0.3f, -1.0f, 0.3f);
	light.direction.normalize();
	setLight(&light, 0);
	*/

//	mEntityManager.init(*this);
}

Match::~Match() {

	runOnDestroyTests();

	if (mLog.isValid()) {
		
		mLog->log(LOG_INFO, true, true, L"Frame Sleep Count [%d]", mRenderWaitSleepCount);
		mLog->log(LOG_INFO, true, true, L"Sleep Interval [%d] msec.", mRenderSleepInterval);

		mLog->log(LOG_INFO, true, true, L"Perf. Lag Count [%d]", mPerfLagCount);
		mLog->log(LOG_INFO, true, true, L"CPU Lag Count [%d]", mCPULagCount);
		mLog->log(LOG_INFO, true, true, L"GPU Lag Count [%d]", mGPULagCount);
	}

	mIsDestroying = true;

	mSoundManager.destroy();

	destroyBallControllers();

	destroyScene();

	mCollEngine.destroy();

	
	mLoadContext.destroy();
	mRenderer.destroy();
	mLog.destroy();

	mIsDestroying = false;
}

void Match::destroyScene() {

	mEntityManager.destroyNodes();
}


bool Match::createScene() {

	if (mEntityManager.loadScene(*this) == false) {

		return false;
	}

	return true;

	/*
	if (mScene.isValid()) {

		assrt(false);
		return false;
	}

	mCollEngine.init(*this);

	MMemNew(mScene.ptrRef(), Scene());
	Scene& scene = mScene.dref();

	scene.init(mCoordSys, mLoadContext, mCollEngine.getDetector(), mCollEngine.getMaterialManager());


	

#ifdef _DEBUG
	scene.enableExtraRenders(true);
#else
	scene.enableExtraRenders(false);
#endif

	scene.enableExtraRenders(false);

	//scene.enableRenderNPColliders(true);
	scene.enableRenderVolumes(true);
	//scene.enableRenderDetailVolumes(true);
	//scene.enableRenderGhostNodes(true, 100.0f);
	//scene.enableRenderLocationNodes(true, 30.0f);


	mNodeRoot.nodeInit(scene, SNT_Node, scene.genNodeId(), L"root");
	mNodeScene.nodeInit(scene, SNT_Node, scene.genNodeId(), L"scene");

	mNodeRoot.addChild(Match&mNodeCam);
	mNodeRoot.addChild(Match&mNodeScene);

	mNodeCam.setCameraMode(Cam_FPS);

	if (loadSceneFile() == false) {

		assrt(false);
		return false;
	}

	loadManualScene();

	mNodeRoot.nodeTreeMessage(SE_Init);
	mNodeCam.initFromScene();
	initBallControllers();

	*/

	return true;
}

void Match::loadManualScene() {
}

void Match::initBallControllers() {

	/*
	if (mEntityManager.mBall.isValid()) {

		BallControllerFPS* pFPS;
		MMemNew(pFPS, BallControllerFPS());
		pFPS->setVibration(2.5f);
		pFPS->setRotation(2.5f);

		mTestBallControllerFPS.ptrRef() = pFPS;

		BallControllerPhysImpl* pPhys;
		MMemNew(pPhys, BallControllerPhysImpl());
		pPhys->init(*this);

		mTestBallControllerPhys.ptrRef() = pPhys;


		mTestBallControllerFPS->setPosition(mEntityManager.mBall->mTransformLocal.getPosition());
		mTestBallControllerFPS->attach(*this, mEntityManager.mBall.ptr());
	}
	*/
}

void Match::destroyBallControllers() {

	/*
	mTestBallControllerPhys.destroy();
	mTestBallControllerFPS.destroy();
	*/
}



void Match::initInputControllers(Input::InputManager& manager, const TCHAR* inputProfilePath, BufferString& tempStr) {

	if (inputProfilePath) {

		Input::Loader::loadControllerProfiles(manager, inputProfilePath, getLoadContext().dataSourcePool());

		{

			mLog->log(L"\nDetected User Input Controllers Start");
			mLog->log(L"------------------------------------\n");

			BufferString logString;

			for (WE::Input::InputManager::Index i = 0; i < manager.getControllerCount(); ++i) {

				SoftRef<WE::Input::Controller> controller = manager.getController(i);

				logString.assignEx(L"#%u name:'%s' selectName:'%s' type:'%s' productName:'%s' visible:%s used:%s",
									i, controller->getName(), controller->getSelectName(), controller->getType(), controller->getProductName(), controller->isVisible() ? L"yes" : L"no", controller->isMarked() ? L"yes" : L"no");

				mLog->log(logString.c_tstr());
			}

			mLog->log(L"\n");
			mLog->log(L"------------------------------------\n");
		}
	}
}

void Match::initInputManager() {

	{
		BufferString pathStr(getGame().getLocalPath());

		if (!pathStr.isEmpty()) {

			const TCHAR* deviceInfosChunkPath = L"InputDeviceInfos.xml";
		
			PathResolver::appendPath(pathStr, deviceInfosChunkPath, true, true);

			BufferString tempStr;
			DataSourceChunk chunk;
			DataSourcePool& dataSrcPool = getLoadContext().dataSourcePool();

			if (dataSrcPool.getChunk(pathStr.c_tstr(), true, chunk, NULL, true)) {

				Input::Loader::loadDeviceInfos(mInputManager, chunk.chunk(), tempStr);
			}
		}
	}

	HardPtr<Log> dumpLog;

	if (mDumpInputDevices) {

		BufferString pathStr(AppPath::getPath());

		if (!pathStr.isEmpty()) {

			PathResolver::appendPath(pathStr, mDumpInputDevicesPath.c_tstr(), true, true);
		}

		WE_MMemNew(dumpLog.ptrRef(), Log());

		dumpLog->init(LOG_MASK_ALL, L"", pathStr.c_tstr(), LOG_DEBUG, LOG_ENABLED_BIT | LOG_LOG_TOFILE_BIT);
	}

	mInputManager.detectControllers(mNativeAppInfo, 0, dumpLog);


	/*
	Input::InputManager& manager = mInputManager;

	{
		ScriptEngine& scriptEngine = mScriptEngine;

		ScriptFunctionCall<bool> func(scriptEngine, L"initInputControllers");

		bool ok = false;

		try {

			ok = func(scriptArg(manager));

		} catch(SquirrelError& e) {

			assrt(false);
		}

		if (!ok) {

			assrt(false);
			mLoadErrorFlag = true;
		}
	}

	{

		mLog->log(L"\nDetected User Input Controllers Start");
		mLog->log(L"--------------------------\n");

		BufferString logString;

		for (WE::Input::InputManager::Index i = 0; i < manager.getControllerCount(); ++i) {

			SoftRef<WE::Input::Controller> controller = manager.getController(i);

			logString.assignEx(L"#%u name:%s visible:%s deviceName:%s productName:%s",
								i, controller->getName(), controller->isVisible() ? L"yes" : L"no",
								controller->getDeviceName(), controller->getProductName());

			mLog->log(logString.c_tstr());
		}

		mLog->log(L"\n");
		mLog->log(L"------------------------\n");
	}
	*/

	/*
	ScriptEngine& scriptEngine = mScriptEngine;

	if (scriptEngine.createInstance(L"TestController", scriptObject)) {

		ScriptFunctionCall<bool> func2(scriptEngine, scriptObject, L"init2");

		Vector3 temp;

		bool ok2 = func2(&temp);

		ScriptFunctionCall<bool> func(scriptEngine, scriptObject, L"init");

		bool ok = func(scriptArg(&manager));

		if (!ok) {

			scriptObject.Reset();
		}
	}

	on frameMove

	manager.update(getClock().getTime());

	if (!scriptObject.IsNull()) {

		ScriptEngine& scriptEngine = mScriptEngine;

		if (scriptEngine.call(L"frameMove", &scriptObject)) {
				
			bool ok = true;
		}
	}

	*/
}

void Match::onScriptingInited() {

	if (!createScriptMatch()) {

		mLoadErrorFlag = true;
		mLog->log(LOG_ERROR, true, true, L"Failed to Call Main Script");
	}
}

TeamFormation* Match::loadFormation(const TCHAR* name, Team* pTeam) {

	String dirPath(mTeamFormationsPath);
	Win::FileIterator iter;
	BufferString fileName;
	bool isFolder;
	String ext(L".xml");

	BufferString tempStr;

	if (Win::startFileIterate(dirPath.c_tstr(), iter)) {

		DataSourcePool& dataSourcePool = getLoadContext().dataSourcePool();

		do {

			iter.getCurrFileName(fileName, isFolder);

			if (!isFolder && fileName.length() >= 4) {

				if (fileName.findNext(ext, fileName.length() - 4) > 0) {

					fileName.insert(dirPath);

					SoftRef<DataChunk> chunk = dataSourcePool.getChunk(fileName.c_tstr(), true, NULL, true);

					if (chunk.isValid()) {

						SoftRef<DataChunk> child = chunk->getFirstChild();

						while (child.isValid()) {

							if (child->equals(L"formation")
								&& child->getAttribute(L"name", tempStr)
								&& tempStr.equals(name)) {

								SoftPtr<TeamFormation> formation;
								WE_MMemNew(formation.ptrRef(), TeamFormation());

								if (!formation->load(tempStr, *this, child, pTeam)) {

									HardPtr<TeamFormation> destr = formation.ptr();

									return NULL;
								}

								return formation;
							}

							toNextSibling(child);
						}
					}
				}
			}

		} while(Win::nextFileIterate(iter));
	}

	return NULL;
}

void Match::loadGameChunk(BufferString& tempStr, DataChunk& gameChunk, const CoordSys& destCoordSys, CoordSysConv* pConv) {

	SoftRef<DataChunk> chunk = gameChunk.getChild(L"tweaks");

	if (chunk.isValid()) {

		if (chunk->getAttribute(L"inputDeviceProfilePath", tempStr)) {

			if (gameChunk.resolveFilePath(tempStr)) {

				initInputControllers(mInputManager, tempStr.c_tstr(), tempStr);

			} else {

				assrt(false);
			}
		}

		if (chunk->getAttribute(L"teamFormationsPath", mTeamFormationsPath)) {

			if (!gameChunk.resolveFilePath(mTeamFormationsPath)) {

				assrt(false);
			}
		}

		if (chunk->getAttribute(L"teamsPath", mTeamsPath)) {

			if (!gameChunk.resolveFilePath(mTeamsPath)) {

				assrt(false);
			}
		}

		chunk->scanAttribute(tempStr, L"pauseIfNotActive", mPauseIfNotActive);
		chunk->scanAttribute(tempStr, L"renderIfNotActive", mRenderIfNotActive);
		
		chunk->scanAttribute(tempStr, L"showDebugScore", mTweakShowDebugScore);

		chunk->scanAttribute(tempStr, L"vibrateCOC", mTweakVibrateCOC);
		chunk->scanAttribute(tempStr, L"showPitchBallBounceMarker", mTweakShowPitchBallBounceMarker);
		
		if (chunk->scanAttribute(tempStr, L"pitchBallBounceMarkerHeight", L"%f", &mTweakPitchBallBounceMarkerHeight)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakPitchBallBounceMarkerHeight);
			}
		}

		chunk->scanAttribute(tempStr, L"enableManipulateCam", mTweakEnableManipulateCam);

		chunk->scanAttribute(tempStr, L"vibrateAttachedBall", mTweakVibrateAttachedBall);
		chunk->scanAttribute(tempStr, L"rotateAttachedBall", mTweakRotateAttachedBall);

		chunk->scanAttribute(tempStr, L"dribbleVelHorizRatio", L"%f", &mTweakDribbleVelHorizRatio);

		if (chunk->scanAttribute(tempStr, L"dribbleVelVert", L"%f", &mTweakDribbleVelVert)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakDribbleVelVert);
			}
		}

		if (chunk->scanAttribute(tempStr, L"shotLowVelHoriz", L"%f", &mTweakShotLowVelHoriz)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakShotLowVelHoriz);
			}
		}

		if (chunk->scanAttribute(tempStr, L"shotLowVelVert", L"%f", &mTweakShotLowVelVert)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakShotLowVelVert);
			}
		}

		if (MathDataLoadHelper::extract(tempStr, chunk, mTweakShotLowRot, true, true, true, L"shotLowRot")) {

			if (pConv) {
				pConv->toTargetVector(mTweakShotLowRot.el);
			}

			mTweakShotLowRot.mul(k2Pi);
		}

		if (chunk->scanAttribute(tempStr, L"shotHighVelHoriz", L"%f", &mTweakShotHighVelHoriz)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakShotHighVelHoriz);
			}
		}

		if (chunk->scanAttribute(tempStr, L"shotHighVelVert", L"%f", &mTweakShotHighVelVert)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakShotHighVelVert);
			}
		}

		if (MathDataLoadHelper::extract(tempStr, chunk, mTweakShotHighRot, true, true, true, L"shotHighRot")) {

			if (pConv) {
				pConv->toTargetVector(mTweakShotHighRot.el);
			}

			mTweakShotHighRot.mul(k2Pi);
		}

		if (chunk->scanAttribute(tempStr, L"shotExtraVelHoriz", L"%f", &mTweakShotExtraVelHoriz)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakShotExtraVelHoriz);
			}
		}

		if (chunk->scanAttribute(tempStr, L"shotExtraVelVert", L"%f", &mTweakShotExtraVelVert)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakShotExtraVelVert);
			}
		}

		if (MathDataLoadHelper::extract(tempStr, chunk, mTweakShotExtraRot, true, true, true, L"shotExtraRot")) {

			if (pConv) {
				pConv->toTargetVector(mTweakShotExtraRot.el);
			}

			mTweakShotExtraRot.mul(k2Pi);
		}

		if (chunk->scanAttribute(tempStr, L"shotPassVelHoriz", L"%f", &mTweakShotPassVelHoriz)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakShotPassVelHoriz);
			}
		}

		if (chunk->scanAttribute(tempStr, L"shotPassVelVert", L"%f", &mTweakShotPassVelVert)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakShotPassVelVert);
			}
		}

		if (MathDataLoadHelper::extract(tempStr, chunk, mTweakShotPassRot, true, true, true, L"shotPassRot")) {

			if (pConv) {
				pConv->toTargetVector(mTweakShotPassRot.el);
			}

			mTweakShotPassRot.mul(k2Pi);
		}

		if (chunk->scanAttribute(tempStr, L"tackleVelHoriz", L"%f", &mTweakTackleVelHoriz)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakTackleVelHoriz);
			}
		}

		if (chunk->scanAttribute(tempStr, L"tackleVelVert", L"%f", &mTweakTackleVelVert)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakTackleVelVert);
			}
		}

		if (chunk->scanAttribute(tempStr, L"shockAudioScale", L"%f", &mTweakShockAudioScale)) {

			//do we need this?!?!?!?!?!
			//if (pConv) {
			//	pConv->toTargetUnits(mTweakShockAudioScale);
			//}
		}

		/*
		SoftRef<DataChunk> runSpeedChunk = chunk->getChild(L"runSpeed");
		
		if (runSpeedChunk.isValid()) {

			SoftRef<DataChunk> child = runSpeedChunk->getFirstChild();

			while (child.isValid()) {

				FootballerBodyDefs::RunType runType = FootballerBodyDefs::Run_None;

				if (child->equals(L"run")) {

					runType = FootballerBodyDefs::Run_Normal;

				} else if (child->equals(L"strafeFwd")) {

					runType = FootballerBodyDefs::Run_StrafeFwd;

				} else if (child->equals(L"strafeSide")) {

					runType = FootballerBodyDefs::Run_StrafeLeft;

				} else if (child->equals(L"strafeBack")) {

					runType = FootballerBodyDefs::Run_StrafeRight;
				}


				if (runType != FootballerBodyDefs::Run_None) {

					bool repeat = true;

					if (repeat) {

						repeat = false;

						float mTweakRunSpeedTable[FootballerBodyDefs::RunTypeCount][FootballerBodyDefs::RunSpeedTypeCount];

						{
							float* speedType = &mTweakRunSpeedTable[runType][FootballerBodyDefs::RunSpeed_Jog];

							if (chunk->scanAttribute(tempStr, L"jog", L"%f", speedType)) {

								if (pConv) {
									pConv->toTargetUnits(*speedType);
								}
							}
						}

						{
							float* speedType = &mTweakRunSpeedTable[runType][FootballerBodyDefs::RunSpeed_Normal];

							if (chunk->scanAttribute(tempStr, L"normal", L"%f", speedType)) {

								if (pConv) {
									pConv->toTargetUnits(*speedType);
								}
							}
						}

						{
							float* speedType = &mTweakRunSpeedTable[runType][FootballerBodyDefs::RunSpeed_Sprint];

							if (chunk->scanAttribute(tempStr, L"sprint", L"%f", speedType)) {

								if (pConv) {
									pConv->toTargetUnits(*speedType);
								}
							}
						}

						if (runType == FootballerBodyDefs::Run_StrafeLeft) {

							runType = FootballerBodyDefs::Run_StrafeRight;
							repeat = true;
						}
					}
				}

				toNextSibling(child);
			}
						
		}
		*/

	
		
		chunk->scanAttribute(tempStr, L"audioMaxSounds", L"%u", &mTweakAudioMaxSounds);
		chunk->scanAttribute(tempStr, L"audioVolume", L"%f", &mTweakAudioVolume);
		chunk->scanAttribute(tempStr, L"matchAudioAmbientVolume", L"%f", &mTweakAudioAmbientVolume);

		if (chunk->scanAttribute(tempStr, L"matchAudioRefDist", L"%f", &mTweakAudioRefDist)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakAudioRefDist);
			}
		}

		if (chunk->scanAttribute(tempStr, L"matchAudioMaxDist", L"%f", &mTweakAudioMaxDist)) {

			if (pConv) {
				pConv->toTargetUnits(mTweakAudioMaxDist);
			}
		}

		chunk->scanAttribute(tempStr, L"AudioEnableSoundMerge", mTweakAudioEnableSoundMerge);
		chunk->scanAttribute(tempStr, L"audioSoundMergeTime", L"%f", &mTweakAudioSoundMergeTime);
		chunk->scanAttribute(tempStr, L"audioRolloff", L"%f", &mTweakAudioRolloff);

		chunk->scanAttribute(tempStr, L"sharedControlDelay", L"%f", &mTweakSharedControlDelay);
		chunk->scanAttribute(tempStr, L"enableAI", mTweakEnableAI);
		

		chunk->scanAttribute(tempStr, L"gravityCoeff", L"%f", &mTweakGravityCoeff);

		chunk->scanAttribute(tempStr, L"enableAirDrag", mTweakEnableAirDrag);
		chunk->scanAttribute(tempStr, L"enableAirMagnus", mTweakEnableAirMagnus);
		chunk->scanAttribute(tempStr, L"airDragEffectCoeff", L"%f", &mTweakAirDragEffectCoeff);
		
		chunk->scanAttribute(tempStr, L"testBallSimul", mTweakTestBallSimul);

		if (chunk->scanAttribute(tempStr, L"padDeadZone", L"%f", &mTweakPadDeadZone) == false) {

			mTweakPadDeadZone = 0.26f;
		}

		chunk->scanAttribute(tempStr, L"cameraRelativeInput", mTweakCameraRelativeInput);
		
		chunk->scanAttribute(tempStr, L"cameraRelativeInputDiscreetDirections", mTweakCameraRelativeInputDiscreeteDirections);

		chunk->scanAttribute(tempStr, L"tackleSlideTime", L"%f", &mTweakTackleSlideTime);

		chunk->scanAttribute(tempStr, L"tackleSpeedCoeff", L"%f", &mTweakTackleSpeedCoeff);

		chunk->scanAttribute(tempStr, L"tackleConservative", mTweakTackleConservative);

		{

			if (mScriptPath.isEmpty()) {

				if (chunk->getAttribute(L"scriptPath", mScriptPath)) {

					chunk->resolveFilePath(mScriptPath);
				}
			}

			if (!mScriptPath.isEmpty()) {

				if (chunk->getAttribute(L"mainScript", mStartScript)) {

					mScriptEngine->setScriptPath(mScriptPath.c_tstr());

					onScriptingInited();
				}
			}
		}

		chunk->scanAttribute(tempStr, L"showScanner", mTweakShowScanner);
		
	}

	chunk = gameChunk.getChild(L"lighting");

	if (chunk.isValid()) {

		MathDataLoadHelper::extract(tempStr, chunk, mAmbient, true, true, true, L"ambient");
	}

	chunk = gameChunk.getChild(L"arrows");

	if (chunk.isValid()) {

		mArrowSprite.skin().load(tempStr, chunk, getLoadContext().render(), 0.99f, NULL, NULL, L"texture");
		
		chunk->scanAttribute(tempStr, L"size", L"%f", &mTweakArrowSize);
		
		float viewportWidth = mRenderer->getViewportWidth(true);
		mArrowSprite.setSize(mTweakArrowSize * viewportWidth, RenderQuad::Anchor_None, RenderQuad::Anchor_End);
	}

	chunk = gameChunk.getChild(L"shapes");

	if (chunk.isValid()) {

		SoftRef<DataChunk> shapeChunk = chunk->getFirstChild();

		while (shapeChunk.isValid()) {

			if (shapeChunk->equals(L"circle")) {

				mShapeSprite_Circle.load(tempStr, shapeChunk, getLoadContext().render(), 1.0f, NULL, 
												L"color", L"texture", L"blendType", L"texFilter", 
												NULL, L"scale", L"alpha");

			} else if (shapeChunk->equals(L"quad")) {

				mShapeSprite_Quad.load(tempStr, shapeChunk, getLoadContext().render(), 1.0f, NULL, 
												L"color", L"texture", L"blendType", L"texFilter", 
												NULL, L"scale", L"alpha");
			}

			toNextSibling(shapeChunk);
		}
		
	}

	chunk = gameChunk.getChild(L"COC");

	if (chunk.isValid()) {

		if (chunk->scanAttribute(tempStr, L"radius", L"%f", &mCOCSetup.radius)) {

			if (pConv) {
				pConv->toTargetUnits(mCOCSetup.radius);
			}
		}

		if (chunk->scanAttribute(tempStr, L"height", L"%f", &mCOCSetup.height)) {

			if (pConv) {
				pConv->toTargetUnits(mCOCSetup.height);
			}
		}

		if (chunk->scanAttribute(tempStr, L"angle", L"%f", &mCOCSetup.maxAngle)) {

			mCOCSetup.maxAngle = degToRad(mCOCSetup.maxAngle);
		}

		if (chunk->getAttribute(L"texturesPath", mCOCSetup.texturesPath)) {

			chunk->resolveFilePath(mCOCSetup.texturesPath);
		}

		float heightOffset;

		if (chunk->scanAttribute(tempStr, L"heightOffset", L"%f", &heightOffset)) {

			if (pConv) {
				pConv->toTargetUnits(heightOffset);
			}

			mCOCSprite.setOffset3D(heightOffset);
		}

		chunk->scanAttribute(tempStr, L"alpha", L"%f", &mCOCSetup.defaultAlpha);

		mCOCSetup.defaultSkin.load(tempStr, chunk, getLoadContext().render(), mCOCSetup.defaultAlpha, &mCOCSetup.texturesPath);

		{
			SoftRef<DataChunk> skins = chunk->getChild(L"skins");

			if (skins.isValid()) {

				SoftRef<DataChunk> skinChunk = skins->getFirstChild();

				while (skinChunk.isValid()) {

					SoftPtr<COCSkin> skin = mCOCSetup.skins.addNewOne();

					SoftRef<DataChunk> skinCOCChunk = skinChunk->getChild(L"COC");
					SoftRef<DataChunk> skinActiveCOCChunk = skinChunk->getChild(L"activeCOC");

					if (skinCOCChunk.isValid()) {

						WE_MMemNew(skin->COC.ptrRef(), RenderSprite::Skin());

						skin->COC->load(tempStr, skinCOCChunk, getLoadContext().render(), mCOCSetup.defaultAlpha, &mCOCSetup.texturesPath);
					}

					if (skinActiveCOCChunk.isValid()) {

						WE_MMemNew(skin->activeCOC.ptrRef(), RenderSprite::Skin());
						
						skin->activeCOC->load(tempStr, skinActiveCOCChunk, getLoadContext().render(), mCOCSetup.defaultAlpha, &mCOCSetup.texturesPath);
					}

					toNextSibling(skinChunk);
				}
			}
		}

		float renderRadius = mCOCSetup.radius;

		if (chunk->scanAttribute(tempStr, L"renderRadius", L"%f", &renderRadius)) {

			if (pConv) {
				pConv->toTargetUnits(renderRadius);
			}
		}

		{
			SoftPtr<RenderSprite::Skin> defaultSkin;

			if (mCOCSetup.skins.count) {

				defaultSkin = mCOCSetup.skins[0]->COC.ptr();
			}

			mCOCSprite.setSize(renderRadius, RenderQuad::Anchor_None, RenderQuad::Anchor_None, RenderSprite::chooseTexture(defaultSkin.ptr()));
		}
	}

	chunk = gameChunk.getChild(L"ballPitchMarker");

	if (chunk.isValid()) {

		mBallPitchMarkerSprite.load(tempStr, chunk, getLoadContext().render(), 0.99f, NULL, 
									L"color", L"texture", L"blendType", L"texFilter", 
									L"size", L"scale", L"alpha", pConv);
	}

	chunk = gameChunk.getChild(L"pitchInfo");

	if (chunk.isValid()) {

		if (chunk->scanAttribute(tempStr, L"width", L"%f", &mSpatialPitchModel.width)) {

			if (pConv) {
				pConv->toTargetUnits(mSpatialPitchModel.width);
			}
		}

		if (chunk->scanAttribute(tempStr, L"length", L"%f", &mSpatialPitchModel.length)) {

			if (pConv) {
				pConv->toTargetUnits(mSpatialPitchModel.length);
			}
		}

		if (chunk->scanAttribute(tempStr, L"penaltyBoxWidth", L"%f", &mSpatialPitchModel.penaltyBoxWidth)) {

			if (pConv) {
				pConv->toTargetUnits(mSpatialPitchModel.penaltyBoxWidth);
			}
		}

		if (chunk->scanAttribute(tempStr, L"penaltyBoxLength", L"%f", &mSpatialPitchModel.penaltyBoxLength)) {

			if (pConv) {
				pConv->toTargetUnits(mSpatialPitchModel.penaltyBoxLength);
			}
		}

		if (chunk->scanAttribute(tempStr, L"goalBoxWidth", L"%f", &mSpatialPitchModel.goalBoxWidth)) {

			if (pConv) {
				pConv->toTargetUnits(mSpatialPitchModel.goalBoxWidth);
			}
		}

		if (chunk->scanAttribute(tempStr, L"goalBoxLength", L"%f", &mSpatialPitchModel.goalBoxLength)) {

			if (pConv) {
				pConv->toTargetUnits(mSpatialPitchModel.goalBoxLength);
			}
		}

		if (chunk->scanAttribute(tempStr, L"goalHeight", L"%f", &mSpatialPitchModel.goalHeight)) {

			if (pConv) {
				pConv->toTargetUnits(mSpatialPitchModel.goalHeight);
			}
		}

		if (chunk->scanAttribute(tempStr, L"goalWidth", L"%f", &mSpatialPitchModel.goalWidth)) {

			if (pConv) {
				pConv->toTargetUnits(mSpatialPitchModel.goalWidth);
			}
		}

		mSpatialPitchModel.update(*this);

		{
			ScriptVarTable& varTable = dref(getLoadContext().varTablePtr());

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.width);
				varTable.setValue(L"PitchWidth", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.halfWidth);
				varTable.setValue(L"PitchHalfWidth", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.length);
				varTable.setValue(L"PitchLength", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.halfLength);
				varTable.setValue(L"PitchHalfLength", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.penaltyBoxWidth);
				varTable.setValue(L"PenaltyBoxWidth", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.penaltyBoxHalfWidth);
				varTable.setValue(L"PenaltyBoxHalfWidth", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.penaltyBoxLength);
				varTable.setValue(L"PenaltyBoxLength", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.penaltyBoxHalfLength);
				varTable.setValue(L"PenaltyBoxHalfLength", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", -mSpatialPitchModel.halfLength);
				varTable.setValue(L"PenaltyBoxFwdMin", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.penaltyBoxFwdMax);
				varTable.setValue(L"PenaltyBoxFwdMax", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.goalBoxWidth);
				varTable.setValue(L"GoalBoxWidth", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.goalBoxHalfWidth);
				varTable.setValue(L"GoalBoxHalfWidth", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.goalBoxLength);
				varTable.setValue(L"GoalBoxLength", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.goalBoxHalfLength);
				varTable.setValue(L"GoalBoxHalfLength", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", -mSpatialPitchModel.halfLength);
				varTable.setValue(L"GoalBoxFwdMin", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.goalBoxFwdMax);
				varTable.setValue(L"GoalBoxFwdMax", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.goalHeight);
				varTable.setValue(L"GoalHeight", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.goalWidth);
				varTable.setValue(L"GoalWidth", tempStr.c_tstr());
			}

			{
				tempStr.assignEx(L"%f", mSpatialPitchModel.goalHalfWidth);
				varTable.setValue(L"GoalHalfWidth", tempStr.c_tstr());
			}
		}
	}

	chunk = gameChunk.getChild(L"footballerSwitching");

	if (chunk.isValid()) {

		if (chunk->getAttribute(L"mode", tempStr)) {

			if (tempStr.equals(L"auto")) {

				mSwitchControlSetup.mode = FSM_Automatic;

			} else if (tempStr.equals(L"onShoot")) {

				mSwitchControlSetup.mode = FSM_OnShootAction;

			} else if (tempStr.equals(L"manual")) {

				mSwitchControlSetup.mode = FSM_Manual;

			} else {

				assrt(false);
				mSwitchControlSetup.mode = FSM_Automatic;
			}
		}

		//chunk->scanAttribute(tempStr, L"fallbackToBallDist", mSwitchControlSetup.fallbackToBallDist);
		chunk->scanAttribute(tempStr, L"veryDynamic", mSwitchControlSetup.veryDynamic);

		if (chunk->getAttribute(L"technique", tempStr)) {

			if (tempStr.equals(L"nextNumber")) {

				mSwitchControlSetup.technique = FST_NextNumber;

			} else if (tempStr.equals(L"ballDist")) {

				mSwitchControlSetup.technique = FST_BallDist;

			} else if (tempStr.equals(L"shotAnalysis")) {

				mSwitchControlSetup.technique = FST_ShotAnalysis;

			} else {

				assrt(false);
				mSwitchControlSetup.technique = FST_NextNumber;
			}
		}


		if (chunk->scanAttribute(tempStr, L"footballerReachHeight", L"%f", &mSwitchControlSetup.footballerReachHeight)) {

			if (pConv)
				pConv->toTargetUnits(mSwitchControlSetup.footballerReachHeight);
		}
		chunk->scanAttribute(tempStr, L"playerReactionTime", L"%f", &mSwitchControlSetup.playerReactionTime);

		if (chunk->scanAttribute(tempStr, L"pathInterceptDistTolerance", L"%f", &mSwitchControlSetup.pathInterceptDistTolerance)) {

			if (pConv)
				pConv->toTargetUnits(mSwitchControlSetup.pathInterceptDistTolerance);
		}

		chunk->scanAttribute(tempStr, L"playerControlEffectInterval", L"%f", &mSwitchControlSetup.playerControlEffectInterval);
		chunk->scanAttribute(tempStr, L"pathInterceptTimeCompareTolerance", L"%f", &mSwitchControlSetup.pathInterceptTimeCompareTolerance);
		chunk->scanAttribute(tempStr, L"switchInfluenceTimeMultiplier", L"%f", &mSwitchControlSetup.switchInfluenceTimeMultiplier);
		

		if (chunk->scanAttribute(tempStr, L"pathInterceptDistCompareTolerance", L"%f", &mSwitchControlSetup.pathInterceptDistCompareTolerance)) {

			if (pConv)
				pConv->toTargetUnits(mSwitchControlSetup.pathInterceptDistCompareTolerance);
		}

		if (chunk->scanAttribute(tempStr, L"ballDistCompareTolerance", L"%f", &mSwitchControlSetup.ballDistCompareTolerance)) {

			if (pConv)
				pConv->toTargetUnits(mSwitchControlSetup.ballDistCompareTolerance);
		}

		chunk->scanAttribute(tempStr, L"minSwitchInterval", L"%f", &mSwitchControlSetup.minSwitchInterval);
		chunk->scanAttribute(tempStr, L"lazyUpdateInterval", L"%f", &mSwitchControlSetup.lazyUpdateInterval);
	}

	chunk = gameChunk.getChild(L"sceneSubdivision");

	if (chunk.isValid()) {

		mSpatialManagerBuildPhase->loadConfiguration(tempStr, chunk, pConv, true);
	}

		
	chunk = gameChunk.getChild(L"players");

	if (chunk.isValid()) {

		int playerIndex = 0;

		SoftRef<DataChunk> playerChunk = chunk->getFirstChild();

		while (playerChunk.isValid()) {

			if (playerChunk->equals(L"player")) {

				int playerTeam = -1;
				unsigned int instanceIndex = 0;
				String playerID;
				String controller;

				playerChunk->scanAttribute(tempStr, L"team", L"%d", &playerTeam);
				playerChunk->scanAttribute(tempStr, L"instanceIndex", L"%u", &instanceIndex);
				playerChunk->getAttribute(L"ID", playerID);

				playerChunk->getAttribute(L"select", controller);

				if ((playerTeam >= 0) && (playerTeam <= 1)) {

					/*
					getTeam((TeamEnum) playerTeam).addPlayer((TeamEnum) (playerTeam), true, 
												playerIndex,
												0.0f, controller.c_tstr(),
												instanceIndex, &playerID);
					++playerIndex;
					*/
				}
		
			}

			toNextSibling(playerChunk);
		}

	}

	chunk = gameChunk.getChild(L"teams");

	 if (chunk.isValid()) {

		SoftRef<DataChunk> teamChunk = chunk->getFirstChild();

		while (teamChunk.isValid()) {

			int teamID = -1;
			
			if (teamChunk->scanAttribute(tempStr, L"id", L"%d", &teamID)
				&& teamID >= 0 && teamID <= 1) {

				if (!getTeam((TeamEnum) teamID).loadMatchSetup((TeamEnum) teamID, dref(this), tempStr, teamChunk, pConv)) {

					mLoadErrorFlag = true;
				}
			}

			toNextSibling(teamChunk);
		}
	}


	chunk = gameChunk.getChild(L"pathInterceptManager");

	if (chunk.isValid()) {

		mFootballerBallInterceptManager.load(*this, tempStr, chunk, pConv);
	}

	chunk = gameChunk.getChild(L"ballShotEstimator");

	if (chunk.isValid()) {

		mBallShotEstimator->load(*this, tempStr, chunk, pConv);
	}

	chunk = gameChunk.getChild(L"miniPitchScanner");

	if (chunk.isValid()) {

		if (!mScanner.load(*this, tempStr, chunk, pConv)) {

			mLog->log(LOG_ERROR, true, true, L"MiniPitchScanner Load Failed");
			mLoadErrorFlag = true;
		}
	}

	chunk = gameChunk.getChild(L"UI");

	if (chunk.isValid()) {

		mUI.load(*this, tempStr, chunk, pConv);
	}

	chunk = gameChunk.getChild(L"onMatchStart");

	if (chunk.isValid()) {

		if (mMatchStartChunk.isValid()) {

			mLog->log(LOG_ERROR, true, true, L"Multiple onMatchStart tags not allowed");
			mLoadErrorFlag = true;
		}

		mMatchStartChunk = chunk;
	}
}

void Match::initAudio() {

	if (mSoundManager.isNull()) {

		MMemNew(mSoundManager.ptrRef(), SoundManager());

		if (mSoundManager->create(*this, mGame->getAudioEngine(), mPluginPath.c_tstr())) {

		} else {

			mLog->log(LOG_ERROR, true, true, L"Audio Engine Init. Failed");
		}
	}
}

void Match::processMatchStartChunk() {

	if (mMatchStartChunk.isValid()) {

		BufferString tempStr;
		SoftRef<DataChunk> chunk = mMatchStartChunk->getFirstChild();

		while (chunk.isValid()) {

			if (chunk->equals(L"execute")) {

				if (chunk->getAttribute(L"script", tempStr)) {

					mGame->processConsoleCommand(tempStr.c_tstr());
				}
			}

			toNextSibling(chunk);
		}
	}
}

void Match::loadConfigPerformance() {

	SoftRef<DataSource> source = DataSourceCreator::create(mGame->getConfigPath().c_tstr(), NULL);

	if (source.isValid()) {

		BufferString tempStr;

		SoftRef<DataChunk> root = source->getRoot();
		SoftRef<DataChunk> perfChunk = root->getChild(L"performance");

		if (perfChunk.isValid()) {

			if (perfChunk->getAttribute(L"processorUsage", tempStr)) {

				if (tempStr.equals(L"2") || tempStr.equals(L"aggressive")) {

					mAllowSleep = false;
					mAllowNonZeroSleep = false;

				} else if (tempStr.equals(L"1") || tempStr.equals(L"normal")) {

					mAllowSleep = true;
					mAllowNonZeroSleep = false;

				} else {
					//gentle

					mAllowSleep = true;
					mAllowNonZeroSleep = true;
				}
			}
		}

		SoftRef<DataChunk> tweakChunk = root->getChild(L"tweak");

		if (tweakChunk.isValid()) {

			tweakChunk->scanAttribute(tempStr, L"dumpInputDevices", mDumpInputDevices);
			
			if (tweakChunk->getAttribute(L"dumpInputDevicesPath", tempStr)) {

				mDumpInputDevicesPath.assign(tempStr);
			}
		}
	}
}

bool Match::load() {

	if (mIsLoaded) {

		assrt(false);
		return false;
	}

	srand(0);

	mLog->log(LOG_INFO, true, true, L"Loading ");

	runPreLoadTests();

	mCoordSys.set(mRenderer->getCoordSys());
	mCoordSys.setFloatUnitsPerMeter(100.0f);

	if (mGameCoordSys.init(mCoordSys, 1.0f) == false) {

		return false;
	}

	mGameCoordSys.getUnitVector(CSD_Right, mCoordSysAxis[Scene_Right].el);
	mGameCoordSys.getUnitVector(CSD_Up, mCoordSysAxis[Scene_Up].el);
	mGameCoordSys.getUnitVector(CSD_Forward, mCoordSysAxis[Scene_Forward].el);

	mCoordSysAxis[Scene_Right].negate(mNegCoordSysAxis[Scene_Right]);
	mCoordSysAxis[Scene_Up].negate(mNegCoordSysAxis[Scene_Up]);
	mCoordSysAxis[Scene_Forward].negate(mNegCoordSysAxis[Scene_Forward]);

	m2DCoordSysAxis[Scene2D_Right].el[Scene2D_Right] = 1.0f;
	m2DCoordSysAxis[Scene2D_Right].el[Scene2D_Forward] = 0.0f;

	m2DCoordSysAxis[Scene2D_Forward].el[Scene2D_Right] = 0.0f;
	m2DCoordSysAxis[Scene2D_Forward].el[Scene2D_Forward] = 1.0f;

	m2DCoordSysAxis[Scene2D_Right].negate(m2DNegCoordSysAxis[Scene2D_Right]);
	m2DCoordSysAxis[Scene2D_Forward].negate(m2DNegCoordSysAxis[Scene2D_Forward]);

	mTerrainPlane.init(Vector3::kZero, getCoordAxis(Scene_Up), true);

	WE_MMemNew(mBallShotEstimator.ptrRef(), BallShotEstimator(*this));

	mAmbient.set(0.0f, 0.0f, 0.0f);

	{
		mTweakDribbleVelHorizRatio = 1.3f;
		mTweakDribbleVelVert = mGameCoordSys.convUnit(1.0f);
		mTweakShotLowVelHoriz = mGameCoordSys.convUnit(40.0f);
		mTweakShotLowVelVert = mGameCoordSys.convUnit(2.0f);
		mTweakShotLowRot.zero();
		mTweakShotHighVelHoriz = mGameCoordSys.convUnit(30.0f);
		mTweakShotHighVelVert = mGameCoordSys.convUnit(10.0f);
		mTweakShotHighRot.zero();
		mTweakShotExtraVelHoriz = mGameCoordSys.convUnit(20.0f);
		mTweakShotExtraVelVert = mGameCoordSys.convUnit(20.0f);
		mTweakShotExtraRot.zero();
		mTweakShotPassVelHoriz = mGameCoordSys.convUnit(10.0f);
		mTweakShotPassVelVert = mGameCoordSys.convUnit(1.5f);
		mTweakShotPassRot.zero();
		mTweakTackleVelHoriz = mGameCoordSys.convUnit(6.0f);
		mTweakTackleVelVert = mGameCoordSys.convUnit(1.0f);
		mCOCSetup.radius = mGameCoordSys.convUnit(1.0f);
		mCOCSetup.height = mGameCoordSys.convUnit(1.0f);
		mCOCSetup.maxAngle = kPiOver2;
		mCOCSetup.defaultAlpha = 0.99f;
		mTweakShockAudioScale = mGameCoordSys.convUnit(1.0f);
		mTweakAudioVolume = 1.0f;
		mTweakAudioMaxSounds = 6;
		mTweakAudioEnableSoundMerge = true;
		mTweakAudioSoundMergeTime = 0.2f;
		mTweakAudioRolloff = 1.0f;
		mTweakSharedControlDelay = 2.0f;
		mTweakAudioAmbientVolume = 1.0f;
		mTweakAudioRefDist = 1.0f;
		mTweakAudioMaxDist = 100.0f;
		mTweakVibrateAttachedBall = true;
		mTweakRotateAttachedBall = true;
		mTweakVibrateCOC = false;
		mTweakShowPitchBallBounceMarker = true;
		mTweakPitchBallBounceMarkerHeight = mGameCoordSys.convUnit(1.5f);
		mTweakShowDebugScore = false;
		mTweakEnableManipulateCam = false;

		/*
		mTweakRunSpeedTable[FootballerBodyDefs::Run_Normal][FootballerBodyDefs::RunSpeed_Jog] = mGameCoordSys.convUnit(3.7f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_Normal][FootballerBodyDefs::RunSpeed_Normal] = mGameCoordSys.convUnit(6.5f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_Normal][FootballerBodyDefs::RunSpeed_Sprint] = mGameCoordSys.convUnit(8.5f);

		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeFwd][FootballerBodyDefs::RunSpeed_Jog] = mGameCoordSys.convUnit(5.0f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeFwd][FootballerBodyDefs::RunSpeed_Normal] = mGameCoordSys.convUnit(5.0f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeFwd][FootballerBodyDefs::RunSpeed_Sprint] = mGameCoordSys.convUnit(5.0f);

		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeLeft][FootballerBodyDefs::RunSpeed_Jog] = mGameCoordSys.convUnit(4.5f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeLeft][FootballerBodyDefs::RunSpeed_Normal] = mGameCoordSys.convUnit(4.5f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeLeft][FootballerBodyDefs::RunSpeed_Sprint] = mGameCoordSys.convUnit(4.5f);

		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeRight][FootballerBodyDefs::RunSpeed_Jog] = mGameCoordSys.convUnit(4.5f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeRight][FootballerBodyDefs::RunSpeed_Normal] = mGameCoordSys.convUnit(4.5f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeRight][FootballerBodyDefs::RunSpeed_Sprint] = mGameCoordSys.convUnit(4.5f);

		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeBack][FootballerBodyDefs::RunSpeed_Jog] = mGameCoordSys.convUnit(4.0f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeBack][FootballerBodyDefs::RunSpeed_Normal] = mGameCoordSys.convUnit(4.0f);
		mTweakRunSpeedTable[FootballerBodyDefs::Run_StrafeBack][FootballerBodyDefs::RunSpeed_Sprint] = mGameCoordSys.convUnit(4.0f);
		*/
	
		mTweakEnableAI = false;
		mTweakEnableAirDrag = true;
		mTweakEnableAirMagnus = false;
		mTweakAirDragEffectCoeff = 0.75f;

		mTweakTestBallSimul = false;

		mTweakArrowSize = 0.01f;

		mTweakCameraRelativeInput = true;
		mTweakCameraRelativeInputDiscreeteDirections = false;

		mTweakTackleSlideTime = 2.0f;
		mTweakTackleSpeedCoeff = 1.0f;
		mTweakTackleConservative = true;

		mTweakShowScanner = true;

		mEnableShadowing = false;

		mSwitchControlSetup.mode = FSM_Automatic;
		//mSwitchControlSetup.fallbackToBallDist = true;
		mSwitchControlSetup.veryDynamic = false;
		mSwitchControlSetup.technique = FST_NextNumber;
		mSwitchControlSetup.footballerReachHeight = mGameCoordSys.convUnit(1.5f);

		mSpatialPitchModel.width = mGameCoordSys.convUnit(80.0f);
		mSpatialPitchModel.length = mGameCoordSys.convUnit(120.0f);
		mSpatialPitchModel.penaltyBoxWidth = mGameCoordSys.convUnit(40.23);
		mSpatialPitchModel.penaltyBoxLength = mGameCoordSys.convUnit(16.46);
		mSpatialPitchModel.update(*this);

		mSwitchControlSetup.playerReactionTime = 0.5f;
		mSwitchControlSetup.pathInterceptDistTolerance = mGameCoordSys.convUnit(0.5f);
		mSwitchControlSetup.playerControlEffectInterval = 2.0f;
		mSwitchControlSetup.switchInfluenceTimeMultiplier = 1.0f;
		mSwitchControlSetup.pathInterceptTimeCompareTolerance = 0.5f;
		mSwitchControlSetup.pathInterceptDistCompareTolerance = mGameCoordSys.convUnit(1.5f);
		mSwitchControlSetup.ballDistCompareTolerance = mGameCoordSys.convUnit(2.0f);
		mSwitchControlSetup.minSwitchInterval = 2.0f;
		mSwitchControlSetup.lazyUpdateInterval = 4.0f;
	}

	mEntityManager.loadPluginPath(*this, mPluginPath);
	mLog->log(LOG_INFO, true, true, L"Plugin Path is [%s]", mPluginPath.c_tstr());

	mClockMaxElapsedLimit = 1.0f / 25.0f;
	mClockCompensation = 0.0f;
	mClock.init(30.0f, 4);

	mSimulIntervalsPerRenderFrame = 1;
	mUnsimulatedIntervalCount = mSimulIntervalsPerRenderFrame;

	mAllowSleep = true;
	mAllowNonZeroSleep = true;

	mNextRenderTime = mClock.mTime.tickLength;
	mSimClock = -1.0f;
	mSuccessivePerLagCount = 0;
	mMaxSuccessivePerLagCount = 10;
	mRenderWaitSleepCount = 0;
	mRenderSleepsLeft = 0;
	mRenderSleepInterval = 0;
	mRenderSleepTimeAvgSampleCount = 10;
	mRenderSleepTimeCumul = 0.0f;
	mRenderSleepTimeAvgSamplesLeft = 0;
	mRenderSleepTime = 0.0f;
	mFrameIsWaitingForRendering = false;
	mCPULagCount = 0;
	mGPULagCount = 0;
	mRenderSleepSubtractCount = 1;
	mLastFrameWasRenderFrame = false;

	mFrameStartTime = 0.0f; 
	mFrameProcessEndTime = 0.0f;
	mFrameEndTime = 0.0f;


	loadConfigPerformance();

	initAudio();

	mLoadContext.render().setRenderer(mRenderer.ptr());
	
	WE_MMemNew(mSpatialManager.ptrRef(), SpatialManager_KDTree());
	WE_MMemNew(mSpatialManagerBuildPhase.ptrRef(), SpatialManagerBuildPhase());
	mSpatialManagerBuildPhase->init(mSpatialManager, true);
	
	mCollEngine.init(*this, mSpatialManager, mSpatialManagerBuildPhase);
	mFootballerBallInterceptManager.init(*this);

	mEventDispatcher.init(*this);
	
	initInputManager();

	mLoadErrorFlag = false;

	if (createScene() == false) {

		mLog->log(LOG_INFO, true, true, L"Load Failed");

		return false;
	}

	if (mLoadErrorFlag) {

		mLog->log(LOG_INFO, true, true, L"Error Flag Set While Loading");

		return false;
	}


	if (!mSpatialManagerBuildPhase->finalize()) {

		mLog->log(LOG_INFO, true, true, L" Building Spatial Scene Failed");

		assrt(false);
		return false;
	}

	mSpatialManagerBuildPhase.destroy();
	mCollEngine.setBuildPhase(mSpatialManagerBuildPhase);
	
	mSoundManager->init(mTweakAudioMaxSounds, mTweakShockAudioScale);
	mSoundManager->loadAudio();
	
	mActiveFootballerIterator.init(*this);
	mControlSwitcher.init(*this);
	mSpatialMatchState.init(*this);
	mEventDispatcher.onActiveFootballersChange();
	getBall().setIsInPlay(true);
	
	getTeam(Team_A).prepare(dref(this));
	getTeam(Team_B).prepare(dref(this));

	if (mGame->hasWOFDebugger())
		mPauseIfNotActive = false;

	/*
	mTestBall = mEntityManager.mBall.ptr();
	mTestBallAttached = false;
	mTestBallLoadingShot = false;
	*/
	

	mPerfLagCount = -1;

	mClock.start();

	if (!scriptedLoad()) {

		return false;
	}
	
	mLog->log(LOG_INFO, true, true, L" Loaded");

	processMatchStartChunk();

	runOnLoadTests();

#ifndef _DEBUG
	bool enableDebugKeys = mTweakEnableDebugKeys;

	mTweakEnableDebugKeys = true;
	processWndMessage(WM_KEYDOWN, VK_F11, 0);
	processWndMessage(WM_KEYUP, VK_F11, 0);

	//processWndMessage(WM_KEYDOWN, 'M', 0);
	//processWndMessage(WM_KEYUP, 'M', 0);

	processWndMessage(WM_KEYDOWN, 'B', 0);
	processWndMessage(WM_KEYUP, 'B', 0);

	processWndMessage(WM_KEYDOWN, 'G', 0);
	processWndMessage(WM_KEYUP, 'G', 0);

	mTweakEnableDebugKeys = enableDebugKeys;
#endif

	mClock.start();

	return true;
}

void Match::resetFrameRate(int FPS) {

	//needs some more code in disreete time classes
	/*
	mClockCompensation = 0.0f;
	mClock.init((float) FPS, mClock.mBallPhysTime.ticksPerSec);

	mSimulIntervalsPerRenderFrame = 1;
	mUnsimulatedIntervalCount = mSimulIntervalsPerRenderFrame;

	mSuccessivePerLagCount = 0;
	mMaxSuccessivePerLagCount = 10;
	mRenderWaitSleepCount = 0;
	mRenderSleepsLeft = 0;
	mRenderSleepInterval = 0;
	mRenderSleepTimeAvgSampleCount = 10;
	mRenderSleepTimeCumul = 0.0f;
	mRenderSleepTimeAvgSamplesLeft = 0;
	mRenderSleepTime = 0.0f;
	mCPULagCount = 0;
	mGPULagCount = 0;
	*/
}

static Vector3 replayPos(true);
static Vector3 replayDir(true);
static Vector3 replayV(true);
static Vector3 replayW(true);


bool Match::processWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	if (mIsProcessing || mIsRendering)
		assrt(false);

	//String::debug(L"+processWndMessage %u\n", timeGetTime());

	bool processed = false;
	bool queue = false;

	switch (uMsg) {
		
		case WM_KEYDOWN:
		{
			queue = true;

			switch (wParam) {
				case VK_F2:
					if (mTweakEnableDebugKeys) {
						processed = true;
						mFlagExtraRenders = !mFlagExtraRenders;
					}
					break;
				case VK_F3:
					if (mTweakEnableDebugKeys) {
						processed = true;
						mFlagRenderBPVolumes = !mFlagRenderBPVolumes;
					}
					break;
				case VK_F4:
					if (mTweakEnableDebugKeys) {
						processed = true;
						mFlagRenderVolumes = !mFlagRenderVolumes;
					}
					break;
				case VK_F5:
					if (mTweakEnableDebugKeys) {
						processed = true;
						mFlagRenderDetailVolumes = !mFlagRenderDetailVolumes;
					}
					break;
				case VK_F6:
					if (mTweakEnableDebugKeys) {
						processed = true;
						mFlagRenderNPColliders = !mFlagRenderNPColliders;
					}
					break;	
				case VK_F7:
					if (mTweakEnableDebugKeys) {
						processed = true;
						mFlagRenderEntityStates = !mFlagRenderEntityStates;
					}
					break;
				case VK_F9:
					processed = true;
						mTweakEnableAI = !mTweakEnableAI;
					break;
				case VK_F11: {

					processed = true;

					static bool enabled = false;

					enabled = !enabled;

					mRenderer->setTextureFilterOverride(enabled, RTS_Min, RTF_Anisotropic, 0);
					mRenderer->setTextureFilterOverride(enabled, RTS_Mip, RTF_Anisotropic, 0);
						
				} break;
				case 'R':
				processed = true;
					{
						String lastCmd(getConsole().getLastLine());

						if (!lastCmd.isEmpty()) {

							//getConsole().print(lastCmd.c_tstr(), false);
							getGame().processConsoleCommand(lastCmd.c_tstr());
						}
					}
					break;
				case 'C':
				processed = true;
					{
						mEntityManager.nextCamera();
					}
					break;
				case 'V':
					if (mTweakEnableDebugKeys) {

					processed = true;
					{
						SceneCamera& cam = getActiveCamera();

						cam.cancelTrack();
					}
					}
					break;
				case 'M':
					if (mTweakEnableDebugKeys) {

					processed = true;
					{
						SceneCamera& cam = getActiveCamera();
					
						SoftPtr<SceneNode> trackedNode = cam.getTrackedNode();

						if (trackedNode.isValid() && trackedNode->objectType != Node_Footballer) {

							cam.cancelTrack();
						}

						for (int ti = 0; ti < 2; ++ti) {

							Team& team = getTeam((TeamEnum) ti);

							for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

								Player& player = team.getPlayer(pi);
								SoftPtr<SceneNode> playerFootballer = (SceneNode*) (player.getAttachedFootballer());

								if (playerFootballer.isValid()) {

									if (cam.getTrackedPlayer() == NULL) {

										cam.track(&player);
										break;
									}

									if (cam.getTrackedPlayer() == &player) {

										cam.cancelTrack();
									}
								} 
							}
						}
						
						if (cam.getTrackedPlayer() == NULL) {

							for (int ti = 0; ti < 2; ++ti) {

								Team& team = getTeam((TeamEnum) ti);

								for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

									Player& player = team.getPlayer(pi);
									SoftPtr<SceneNode> playerFootballer = (SceneNode*) (player.getAttachedFootballer());

									if (playerFootballer.isValid()) {

										cam.track(&player);
										break;
									} 
								}

								if (cam.getTrackedPlayer() != NULL) 
									break;
							}
						}

						if (trackedNode.isValid()) {

							SoftPtr<SceneNode> newTrackedNode = cam.getTrackedNode();

							if (!newTrackedNode.isValid()) {

								cam.track(trackedNode);
							}
						}
					}
					}
					break;
				case 'B':
					if (mTweakEnableDebugKeys) {
					processed = true;
					{
						SceneCamera& cam = getActiveCamera();

						float z = 0.0f;
						
						if (GetAsyncKeyState(VK_SHIFT) < 0) {

							cam.track(&(mEntityManager.getBall()), NULL);

						} else {

							cam.track(&(mEntityManager.getBall()), &z);
						}
					}
					}
					break;
				/*
				case 'T':
				processed = true;
				{
					//mEntityManager.mFlagTest = true;
				}
				break;
				case 'Y':
				processed = true;
				{
					//mEntityManager.mFlagTest = true;
					//mEntityManager.mTestLimit -= 2;
				}
				break;
				case 'U':
				processed = true;
				{
					//setDebugBypass(&getStaticDebugBypass());

					//DebugBypass* pBypass = getDebugBypass();

					//++pBypass->renderTri;
				}
				break;
				case 'I':
				processed = true;
				{
					//DebugBypass* pBypass = getDebugBypass();

					//--pBypass->renderTri;
				}
				break;
				*/
#ifdef _DEBUG	
				case 'Z':
				if (mTweakEnableDebugKeys) {
				processed = true;
				{
					if (!mIsDebugging) {

						DebugBreak();
						mIsDebugging = !mIsDebugging;

					} else {

						mIsDebugging = !mIsDebugging;
					}
				}
				}
				break;
				case 'X':
				if (mTweakEnableDebugKeys) {
				processed = true;
				{
					if (!mIsDebugging) {

						DebugBreak();
						mIsDebugging = !mIsDebugging;

						mIsSingleFrameStepping = true;
						mDoSingleFrameStep = false;

					} else {

						mIsDebugging = !mIsDebugging;
						mIsSingleFrameStepping = false;
					}
				}
				}
				break;
#endif
				
				case 'S':
				if (mTweakEnableDebugKeys) {
				processed = true;
				{
					mIsSingleFrameStepping = !mIsSingleFrameStepping;
					mDoSingleFrameStep = false;

					mSingleFrameAllowCamMove = !(GetAsyncKeyState(VK_SHIFT) < 0);
				}
				}
				break;
				case VK_SPACE:
				if (mTweakEnableDebugKeys) {
				processed = true;
				{

					if (mIsSingleFrameStepping) {

						mDoSingleFrameStep = true;
					}
				};
				}
				break;
			}
		}
		break;

		case WM_KEYUP:
		case WM_MOUSEMOVE:
		case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_LBUTTONDBLCLK:
		case WM_RBUTTONUP: 
        case WM_MBUTTONUP: 
        case WM_LBUTTONUP:   
		case WM_MOUSEWHEEL: 
			queue = true;
			break;

	}

	if (!processed && queue) {

		processed = processed || getBall().processWndMessage(uMsg, wParam, lParam);
		processed = processed || getActiveCamera().processWndMessage(uMsg, wParam, lParam);
	}

	//String::debug(L"-processWndMessage %u\n", timeGetTime());

	return processed;
}


void Match::updateSleepTimeAvg(DWORD dt) {

	if (mRenderSleepTimeAvgSamplesLeft) {

		--mRenderSleepTimeAvgSamplesLeft;
		mRenderSleepTimeCumul += ((float) dt) / 1000.0f;

	} else {

		mRenderSleepTimeAvgSamplesLeft = mRenderSleepTimeAvgSampleCount;
		mRenderSleepTime = mRenderSleepTimeCumul / (float) mRenderSleepTimeAvgSampleCount;
		mRenderSleepTimeCumul = 0.0f;

		if (mRenderSleepTime == 0.0f && mAllowNonZeroSleep && mAllowSleep) {

			//setting sleep interval to anythign else than zero greatly increases 
			//sleep variance
			mRenderSleepInterval += 1;
		}
	}
}

void Match::sleep() {

	DWORD dt = timeGetTime();

	if (mAllowSleep) {

		Sleep(mRenderSleepInterval);

	} else {

		DWORD t = timeGetTime();

		while (t - timeGetTime() < (DWORD) mRenderSleepInterval) {

			//CPU load loop, better Sleep(0) really...
		}
	}
	
	dt = timeGetTime() - dt;

	updateSleepTimeAvg(dt);
}

void Match::tuneSleepCount(int currSleepCount, bool inc, float dt) {

	static int mAdaptiveStepCountSuccessiveThreshold = 5;
	static int mRenderSleepSteps = 10;
	static bool mLastSleepCountTuneWasInc = false;
	static int mSuccessiveSameDirTuneCount = 0;

	int prevRenderWaitSleepCount = currSleepCount;

	dt = fabs(dt);

	if (inc) {

		if (dt && mRenderSleepTime) {

			unsigned int diff = (unsigned int) (dt / mRenderSleepTime);
			mRenderWaitSleepCount = currSleepCount + diff;

		} else {

			mRenderWaitSleepCount = currSleepCount + mRenderSleepSteps;
		}

	} else {

		if (dt && mRenderSleepTime) {

			unsigned int diff = (unsigned int) (dt / mRenderSleepTime);
			mRenderWaitSleepCount = currSleepCount - diff;

		} else {

			mRenderWaitSleepCount = currSleepCount - mRenderSleepSteps;
		}
		
		if (mRenderWaitSleepCount < 0)
			mRenderWaitSleepCount = 0;
	}

	/*
	if (inc == mLastSleepCountTuneWasInc) {

		mRenderSleepSteps *= 2;

	} else {

		mRenderSleepSteps /= 2;

		if (mRenderSleepSteps == 0)
			mRenderSleepSteps = 1;
	}
	*/

	if (mLastSleepCountTuneWasInc == inc) {
		
		if (mSuccessiveSameDirTuneCount < mAdaptiveStepCountSuccessiveThreshold) {

			++mSuccessiveSameDirTuneCount;
		}

	} else {

		mRenderSleepSteps /= 2;

		if (mRenderSleepSteps == 0)
			mRenderSleepSteps = 1;
		
		mSuccessiveSameDirTuneCount = 0;
	}

	if (mSuccessiveSameDirTuneCount == mAdaptiveStepCountSuccessiveThreshold) {

		mRenderSleepSteps *= 2;
	}

	mLastSleepCountTuneWasInc = inc;

	//mLog->log(LOG_INFO, true, true, L"Tuned %d -> %d", prevRenderWaitSleepCount, mRenderWaitSleepCount);
}

void Match::frameMove(double _fTime, float _fElapsedTime, bool& isRenderFrame, bool isActive) {

	//String::debug(L"+frameMove %u %f %f\n", timeGetTime(), _fTime, _fElapsedTime);

	mIsProcessing = true;

	/*
		using elapsed time is causing strange problems 
		where elapsed time doesnt correspond to really elapsed time!!
	*/

	isRenderFrame = false;

	float singleStepPenalty = 0.0f;

	if (mSimClock < 0.0f) {

		mSimStartTime = ((float) timeGetTime()) / 1000.0f;
		mSimClock = 0.0f;
		mSimStartTimeOffset = 0.0f;

		mFrameStartTime = ((float) timeGetTime()) / 1000.0f;
		
		mFrameState = FS_WaitingToStartSimul;
		mWaitForSimulSleepCount = 0;
		mWaitForStartRenderSleepCount = 0;

		mSoundManager->setGain(mTweakAudioVolume);
		mSoundManager->setAmbientGain(mTweakAudioAmbientVolume);
		mSoundManager->playAmbient();


		mRenderer->setWasReset(true);

	} else {

		/*
		float currTime = (((float) timeGetTime()) / 1000.0f) - mSimStartTime;
		float elapsedTime = currTime - mSimClock;

		if (isActive) {

			if (mSimIsPaused) {
			
				mSimIsPaused = false;
				mSimStartTime += currTime - mSimPauseStartTime;
			} 

		} else {

			if (mPauseIfNotActive && !mSimIsPaused) {

				mSimPauseStartTime = currTime;
				mSimIsPaused = true;
				mSimPauseedNextRenderTime = mSimPauseStartTime;
			}
		}

		if (mSimIsPaused) {
		
			if (mRenderIfNotActive) {

				if (currTime - mSimPauseedNextRenderTime > 0.0f) {

					isRenderFrame = true;
					mSimPauseedNextRenderTime = currTime + 1.0f;
				}
			}

			mIsProcessing = false;
			return;

		} else {

			//mSimClock += _fElapsedTime;
			mSimClock += elapsedTime;
			mSimClock += mSimStartTimeOffset;
		}
		*/

		float currTime = (((float) timeGetTime()) / 1000.0f) - mSimStartTime;
		float elapsedTime = currTime - mSimClock;
		float elpasedTimeCorrection = 0.0f;

#ifdef _DEBUG
		if (mIsDebugging) {

			isActive = true;

			elpasedTimeCorrection = mClock.mTime.tickLength - elapsedTime;
		}
#endif

		if (mIsSingleFrameStepping) {

			isActive = true;

			if (mDoSingleFrameStep) {

				elpasedTimeCorrection = mClock.mTime.tickLength - elapsedTime;
				mDoSingleFrameStep = false;

			} else {

				elpasedTimeCorrection = -elapsedTime;
				singleStepPenalty = elapsedTime;
			}
		}

		if (isActive) {

			if (mSimIsPaused) {
			
				mSimIsPaused = false;
			} 

		} else {

			if (mPauseIfNotActive && !mSimIsPaused) {

				mSimPauseStartTime = currTime;
				mSimIsPaused = true;
				mSimPauseedNextRenderTime = mSimPauseStartTime;
			}
		}

		if (mSimIsPaused) {
		
			elpasedTimeCorrection = -elapsedTime;

			if (mRenderIfNotActive) {

				if (currTime - mSimPauseedNextRenderTime > 0.0f) {

					isRenderFrame = true;
					mSimPauseedNextRenderTime = currTime + 1.0f;
				}
			}
		}

		if (elpasedTimeCorrection != 0.0f) {

			elapsedTime += elpasedTimeCorrection;
			mSimStartTime -= elpasedTimeCorrection;
		}

		mSimClock += elapsedTime;

		if (mSimIsPaused) {

			mIsProcessing = false;

			//String::debug(L"-frameMove1 %u\n", timeGetTime());
			return;
		}
	}

	if (mLastFrameWasRenderFrame) {

		float overshootTime = mSimClock - mNextRenderTime;
		float thisTargetRenderTime = mNextRenderTime;

		//mLog->log(LOG_INFO, true, true, L"overhsoot %f", overshootTime);

		if (overshootTime <= 0.0f) {

			mNextRenderTime += mClock.mTime.tickLength;

			if (overshootTime != 0.0f) {

				tuneSleepCount(mWaitForStartRenderSleepCount, true, fabs(overshootTime));
			}

		} else {

			tuneSleepCount(mWaitForStartRenderSleepCount, false, fabs(overshootTime));

			float overhsootFrames = overshootTime / mClock.mTime.tickLength;
			int addFrameCount = ((int) (overhsootFrames)) + 1;

			mNextRenderTime += (float) (addFrameCount) * mClock.mTime.tickLength;

			if (addFrameCount > 1) {

				if (mWaitForSimulSleepCount == 0 && mWaitForStartRenderSleepCount == 0) {

					++mPerfLagCount;
					++mSuccessivePerLagCount;

					if (mRenderSleepInterval) {

						mRenderSleepTimeAvgSamplesLeft = mRenderSleepTimeAvgSampleCount;
						mRenderSleepTime = 0.0f;
						mRenderSleepTimeCumul = 0.0f;

						--mRenderSleepInterval;
					}

					if (mSuccessivePerLagCount >= mMaxSuccessivePerLagCount) {

						//mLog->log(LOG_INFO, true, true, L"Serious Performance Lag at time %f", mSimClock);
						///DebugBreak();

						mSuccessivePerLagCount = 0;
						//mRenderSleepInterval = 0;
					}
				}
			}
		}

		mFrameEndTime = ((float) timeGetTime()) / 1000.0f;
		float frameTime = mFrameEndTime - mFrameStartTime;

		/*
		mLog->log(LOG_INFO, true, true, L"* %f, %f -> %f, wt %f(%d), mt %f, wt2 %f(%d), rt %f", 
					mSimClock, thisTargetRenderTime, mNextRenderTime, 
					mFrameProcessStartTime - mFrameStartTime, mWaitForSimulSleepCount, 
					mFrameProcessEndTime - mFrameProcessStartTime,
					mFrameRenderStartTime - mFrameProcessEndTime, mWaitForStartRenderSleepCount, 
					mFrameEndTime - mFrameRenderStartTime);
		*/
					

		/*
		if (frameTime > mClock.mTime.tickLength) {

			if (mWaitForSimulSleepCount == 0 && mWaitForStartRenderSleepCount == 0) {

				++mPerfLagCount;
				++mSuccessivePerLagCount;

				if (mSuccessivePerLagCount >= mMaxSuccessivePerLagCount) {

					mLog->log(LOG_INFO, true, true, L"Serious Performance Lag at time %f", mSimClock);
					///DebugBreak();

					mSuccessivePerLagCount = 0;
					//mRenderSleepInterval = 0;
				}
			}

			tuneSleepCount(false, mClock.mTime.tickLength - frameTime);
		}
		*/

		mFramePerformance->sleepCount = mWaitForSimulSleepCount + mWaitForStartRenderSleepCount;
		mFramePerformance->sleepInterval = mRenderSleepInterval;
		mFramePerformance->lagCount = mPerfLagCount;
		mFramePerformance->frameMoveTime = mFrameProcessEndTime - mFrameProcessStartTime;
		mFramePerformance->renderTime = mFrameEndTime - mFrameRenderStartTime;



		mWaitForSimulSleepCount = 0;
		mWaitForStartRenderSleepCount = 0;

		mFrameStartTime = ((float) timeGetTime()) / 1000.0f;
	}

	float timeToRenderSignal = mNextRenderTime - mSimClock;
	float timeToSimul = timeToRenderSignal - mClock.mTime.tickLength;

	static float lastSleepTimeToRenderSignal = -100.0f;
	static int lastSleepIndex = -1;

	bool lastFrameWasRenderFrame = mLastFrameWasRenderFrame;
	mLastFrameWasRenderFrame = false;

	mSoundManager->startFrame(mSimClock);

	bool reevalState = true;
	bool didSimul = false;

	while (reevalState) {

		reevalState = false;

		switch (mFrameState) {

			case FS_WaitingToStartSimul: {

				if (timeToSimul <= 0.0f) {

					reevalState = true;
					mFrameState = FS_Simulating;

					//mLog->log(LOG_INFO, true, true, L"End WaitForSimul (%d waits) %f", mWaitForSimulSleepCount, timeToSimul);

				} else {

					sleep();
					++mWaitForSimulSleepCount;
				}

			} break;

			case FS_Simulating: {

				didSimul = true;

				mFrameProcessStartTime = ((float) timeGetTime()) / 1000.0f;

				//mLog->log(LOG_INFO, true, true, L"%f - Move x %d", mFrameProcessStartTime, mUnsimulatedIntervalCount);

				for (int i = 0; i < mUnsimulatedIntervalCount; ++i) {

					doFrameMove();
				}

				mFrameProcessEndTime = ((float) timeGetTime()) / 1000.0f;

				//reevalState = true;
				mFrameState = FS_WaitingToStartRender;
				mRenderSleepsLeft = mRenderWaitSleepCount;

			} break;

			case FS_WaitingToStartRender: {

				if (timeToRenderSignal <= 0.0f) {

					//mLog->log(LOG_INFO, true, true, L"Render Signal2 (%d waits) %f [%d - %f]", mWaitForStartRenderSleepCount, timeToRenderSignal, lastSleepIndex, lastSleepTimeToRenderSignal);

					isRenderFrame = true;

				} else {

					if (mRenderSleepsLeft > 0) {

						sleep();
						++mWaitForStartRenderSleepCount;
						--mRenderSleepsLeft;

						lastSleepTimeToRenderSignal = timeToRenderSignal;
						lastSleepIndex = mWaitForStartRenderSleepCount;

					} else {

						//mLog->log(LOG_INFO, true, true, L"Render Signal1 (%d waits) %f", mWaitForStartRenderSleepCount, timeToRenderSignal);
						isRenderFrame = true;
					}
				}

			} break;
		}
	}

	if (isRenderFrame) {

		mFrameState = FS_WaitingToStartSimul;

		mFrameRenderStartTime = ((float) timeGetTime()) / 1000.0f;
		mLastFrameWasRenderFrame = true;
		mUnsimulatedIntervalCount = mSimulIntervalsPerRenderFrame;
	}

	if (isRenderFrame && lastFrameWasRenderFrame) {

		log(L"double render");
	}

	if (mIsSingleFrameStepping && mSingleFrameAllowCamMove && !isRenderFrame) {

		sleep();
		isRenderFrame = true;
		//String::debug(L"+isRender\n");

		SceneCamera& cam = getActiveCamera();

		cam.frameMove(getClock().getTime(), singleStepPenalty);
		cam.clean();
	}

	mSoundManager->endFrame();

	mIsProcessing = false;

	//String::debug(L"-frameMove %u\n", timeGetTime());
}

void Match::doFrameMove() {

	//String::debug(L"+doFrameMove %u\n", timeGetTime());

	mClock.add(mSimClock, mClock.mTime.tickLength);

	mClock.accept();

	mInputManager.update(getClock().getTime());
	mEntityManager.frameMove(*this, mClock);

	mClock.consume();

	runOnFrameMoveTests();

	//String::debug(L"-doFrameMove %u\n", timeGetTime());
}


void Match::setLight(RenderLight* pLight, int index) {

	while (index + 1 > (int) mLightEnables.count) {

		mLightEnables.addOne();
	}

	if (pLight) {

		mRenderer->setLight(pLight, index);
		mLightEnables[index] = true;

	} else {

		mLightEnables[index] = false;
	}
}

void Match::setAmbient(RenderColor* pColor) {

	RenderStateValue ambient;

	mRenderer->toRenderStateValue(dref(pColor), ambient);
	mRenderer->setRenderState(RS_AmbientColor, ambient);
}

void Match::setShadowing(bool enable, RenderColor* pColor, bool alphaBlend, bool stencilTest) {

	mRenderer->setRenderState(RS_EnableShadowing, enable);

	if (enable)
		mRenderer->setupShadowingMaterial(dref(pColor), alphaBlend, stencilTest);
}

void Match::render() {

	if (mRenderer->wasReset()) {

		float viewportWidth = mRenderer->getViewportWidth(true);
		mArrowSprite.setSize(mTweakArrowSize * viewportWidth, RenderQuad::Anchor_None, RenderQuad::Anchor_End);
	}
	
	//mLog->log(LOG_INFO, true, true, L"%f - render", ((float) timeGetTime()) / 1000.0f);

	//String::debug(L"+render %u\n", timeGetTime());

	{
		setAmbient(&mAmbient);
	}


	mIsRendering = true;

	scriptedPreRender();
	
	for (LightEnables::Index i = 0; i < mLightEnables.count; ++i) {

		mRenderer->enableLight(mLightEnables[i], i);
	}

	RenderPassEnum pass = RP_Start;

	while ((pass = mRenderer->nextPass(pass)) != RP_End) {

		mEntityManager.render(*this, mRenderer, mFlagExtraRenders, pass);
	}

	//we render those here becuase we want to render the COC's AFTER shadows!
	getTeam(Team_A).renderPlayers(mRenderer, true, mFlagExtraRenders, RP_Normal);
	getTeam(Team_B).renderPlayers(mRenderer, true, mFlagExtraRenders, RP_Normal);

	//mBallSimulManager.render(*this, mRenderer, mFlagExtraRenders, RP_Normal);
	mFootballerBallInterceptManager.render(mRenderer, mFlagExtraRenders, RP_Normal);
	mControlSwitcher.render(*this, mRenderer, mFlagExtraRenders, RP_Normal);

	mRenderer->drawQueuedVols();

	if (mFlagRenderSpatialManager)
		mSpatialManager->render(mRenderer);

	if (mFlagRenderBallShotEstimator)
		getBallShotEstimator().render(mRenderer);

	//mSpatialManager->render(mRenderer, true, &RenderColor::kWhite, false, NULL, true, &RenderColor::kYellow);

	getActiveCamera().render(mRenderer, true, mFlagExtraRenders);

	mScanner.render(dref(this), mRenderer);
	
	runOnRenderTests();

	/*
	{

		RenderColor col(1.0f, 0.0f, 0.0f, 0.5f);
		mRenderer->draw2DQuad(5, 5, 200, 100, &col);
	}
	*/

	scriptedPostRender();

	/*
	//bad we shoudlnt need this
	mRenderer->enableLight(true, 0);

	Scene2NodeMesh* pNode;
	SceneNodeIndex::Type i = mObjectRegistry.meshNodes.mNodeMap.iterator();
	while(mObjectRegistry.meshNodes.mNodeMap.next(i, pNode)) {

		pNode->renderableAddToList(mTestRenderList);
	}

	mTestRenderList.render(mRenderer, true);
	*/

	//mNodeRoot.nodeTreeRender();
	//mTestBallControllerPhys->debugRender(mRenderer.dref(), false);

	mIsRendering = false;

	//String::debug(L"-render %u\n", timeGetTime());
}	

void Match::renderText(ID3DXSprite* pSprite, ID3DXFont* pFont, ID3DXFont* pBigFont) {

	RenderViewportInfo viewport;

	if (!mRenderer->getViewportInfo(viewport)) 
		return;

	if (mTweakShowDebugScore) {
		
		unsigned int receivedGoals[2];
			
		receivedGoals[Team_A] = getTeam(Team_A).getRecievedGoalCount();
		receivedGoals[Team_B] = getTeam(Team_B).getRecievedGoalCount();
		
		{

			TEXTMETRIC textMetricsHolder;
			SoftPtr<TEXTMETRIC> textMetrics;
			
			if (pBigFont->GetTextMetrics(&textMetricsHolder)) {

				textMetrics = &textMetricsHolder;
			}

			CDXUTTextHelper texter(pBigFont, pSprite, 20);
			texter.Begin();

			texter.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			String scoreText;

			scoreText.assignEx(L"%u : %u", receivedGoals[Team_A], receivedGoals[Team_B]);

			float centerX = (float) (viewport.width / 2);
			float centerY = (float) (viewport.height / 2);

			float offsetX = 0.0f;
			float offsetY = 0.0f;

			if (textMetrics.isValid()) {

				offsetX = -0.5f * ((float)(textMetrics->tmAveCharWidth * scoreText.length()));
				offsetY = 15.0f; //tmHeight
			}

			texter.SetInsertionPos((int) (centerX + offsetX), (int) (offsetY));
			texter.DrawTextLine(scoreText.c_tstr());

			texter.End();
		}
	}

	mUI.render(mRenderer, pSprite, pFont, pBigFont);

}

void Match::onActiveFootballersChange() {
}

Player* Match::findPlayer(const TCHAR* playerID) {

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = getTeam((TeamEnum) ti);

		for (PlayerIndex pi = 0; pi < team.getPlayerCount(); ++pi) {

			Player& player = team.getPlayer(pi);

			if (player.getID().equals(playerID))
				return &player;
		}
	}

	return NULL;
}

void Match::executeMaterial(WorldMaterial& material, const Vector3& point, const float& shockMag) {

	if (shockMag > 0.0f && material.sound.isValid()) {

		mSoundManager->playSound(material.sound, point, shockMag, true);
	}

	if (material.trigger.isValid()) {

		material.trigger->execute(&point);
	}
}

MaterialTrigger* Match::createTrigger(const TCHAR* triggerName) {

	if (tcharCompare(triggerName, L"GoalA") == 0) {

		SoftPtr<MatchTrigger_Goal> trigger;

		WE_MMemNew(trigger.ptrRef(), MatchTrigger_Goal());
		trigger->init(*this, Team_A);
		
		return trigger;

	} else if (tcharCompare(triggerName, L"GoalB") == 0) {

		SoftPtr<MatchTrigger_Goal> trigger;

		WE_MMemNew(trigger.ptrRef(), MatchTrigger_Goal());
		trigger->init(*this, Team_B);
		
		return trigger;
	}

	return NULL;
}

void Match::onGoalTrigger(TeamEnum team) {

	if (team == Team_Invalid) {

		assrt(false);
		return;
	}

	getTeam(team).onGoal();

	{
		String logStr;

		if (team == Team_A) {

			mLog->log(L"TeamB Score at %.3f : %u", getClock().getTime(), getTeam(team).getRecievedGoalCount());

		} else {

			mLog->log(L"TeamA Score at %.3f : %u", getClock().getTime(), getTeam(team).getRecievedGoalCount());
		}
	}

	mSoundManager->playGoal();
}

void Match::executeMaterial(WorldMaterialCombo& material, const Vector3& point, const float& shockMag) {

	if (shockMag > 0.0f) {

		if (material.useCombinedSound) {

			if (material.combinedSound.isValid()) {
			
				mSoundManager->playSound(material.combinedSound, point, shockMag, true);
			}

		} else {

			if (material.mats[0]->sound.isValid()) {

				mSoundManager->playSound(material.mats[0]->sound, point, shockMag, true);
			}

			if (material.mats[1]->sound.isValid()) {

				mSoundManager->playSound(material.mats[1]->sound, point, shockMag, true);
			}
		}
	}

	if (material.mats[0]->trigger.isValid()) {

		material.mats[0]->trigger->execute(&point);
	}

	if (material.mats[1]->trigger.isValid()) {

		material.mats[1]->trigger->execute(&point);
	}
}

RenderSprite::Skin* Match::getCOCSkin(int index, bool isActive) {

	if (!mCOCSetup.skins.count)
		return NULL;

	if (index >= (int) mCOCSetup.skins.count)
		index = mCOCSetup.skins.count - 1;

	COCSkin& skin = dref(mCOCSetup.skins[index]);

	if (isActive && !skin.activeCOC.isValid()) {

		isActive = false;
	}

	return isActive ? skin.activeCOC : skin.COC;
}

void Match::toScene2D(const CtVector3& v3D, Vector2& v2D) {

	v2D.el[Scene2D_Right] = v3D.el[Scene_Right];
	v2D.el[Scene2D_Forward] = v3D.el[Scene_Forward];
}

void Match::toScene3D(const CtVector2& v2D, Vector3& v3D, const float& height) {

	v3D.el[Scene_Right] = v2D.el[Scene2D_Right];
	v3D.el[Scene_Forward] = v2D.el[Scene2D_Forward];
	v3D.el[Scene_Up] = height;
}

} }