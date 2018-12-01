#ifndef h_WOF_Match
#define h_WOF_Match

#include "WE3/WitchEngine.h"
#include "WE3/WETime.h"
#include "WE3/WELog.h"
#include "WE3/loadContext/WELoadContextImpl.h"
#include "WE3/native/WENativeAppInfo.h"
#include "WE3/spatialManager/WESpatialManager.h"
#include "WE3/spatialManager/kdtree/WESpatialManager_KDTreeBuilder.h"
#include "WE3/spatialManager/kdtree/WESpatialManager_KDTree.h"
#include "WE3/binding/directx/9/dxut9/includeDXUT9.h"
using namespace WE;

#include "Clock.h"
#include "EntityManager.h"
#include "collision/CollEngine.h"
#include "LoadPropertyManager.h"
#include "WE3/scene/WESceneNodeContainer.h"
#include "WE3/input/include.h"
#include "WE3/coordSys/script/WEScriptCoordSys.h"
using namespace WE;

#include "SoundManager.h"

#include "../script/ScriptEngine.h"

#include "entity/ball/simul/BallSimulation.h"
#include "entity/ball/Ball.h"
#include "entity/footballer/Footballer.h"
#include "EventDispatcher.h"
#include "switchControl/ControlSwitcher.h"
#include "FootballerIteratorImpl.h"
#include "Triggers.h"
#include "spatialQuery/FootballerBallInterceptManager.h"
#include "entity/SceneDynVol.h"
#include "spatialQuery/BallShotEstimator.h"
#include "spatialQuery/SpatialMatchState.h"
#include "spatialQuery/SpatialPitchModel.h"

#include "UI.h"
#include "Scanner.h"

#include "../WOF.h"

namespace WOF { 

	class Game;
}

namespace WOF { namespace match {

	class Ball;

	class Match {
	public:

		inline Game& getGame() { return mGame; }

		inline const NativeAppInfo& getNativeAppInfo() { return mNativeAppInfo.dref(); }

		inline const Log& getLog() { return mLog.dref(); }

		inline const Clock& getClock() { return mClock; }
		
		inline Renderer& getRenderer() { return mRenderer.dref(); };

		inline const CoordSys& getBaseCoordSys() { return mCoordSys; };
		
		inline const FastUnitCoordSys& getCoordSys() { return mGameCoordSys; }

		inline CtVector3& getCoordAxis(const SceneDirectionEnum& dir) { return mCoordSysAxis[dir]; }
		inline CtVector3& getNegCoordAxis(const SceneDirectionEnum& dir) { return mNegCoordSysAxis[dir]; }

		inline CtVector3* getCoordAxis() { return mCoordSysAxis; }

		inline CtVector2& get2DCoordAxis(const SceneDirection2DEnum& dir) { return m2DCoordSysAxis[dir]; }
		inline CtVector2& get2DNegCoordAxis(const SceneDirection2DEnum& dir) { return m2DNegCoordSysAxis[dir]; }

		inline CtVector2* get2DCoordAxis() { return m2DCoordSysAxis; }

		inline LoadPropertyManager* getLoadPropertyManager() { return NULL; }

		inline EntityManager& getEntityManager() { return mEntityManager; }

		inline Team& getTeam(const TeamEnum& team) { return mEntityManager.getTeam(team); }

		inline Ball& getBall() { return mEntityManager.getBall(); }

		inline Ball& getSimulBall() { return mEntityManager.getSimulBall(); }

		inline SceneEntityMesh& getPitch() { return mEntityManager.getPitch(); }

		inline SceneStaticVol& getSimulPitch() { return mEntityManager.getSimulPitch(); }
		
		SceneNodeIndex::Type genNodeId() { return mEntityManager.genNodeId(); }

		inline SceneMeshContainer& getMeshes() { return mEntityManager.getMeshes(); }

		inline SkeletalMeshPropertyManager& getSkelMeshPropManager() { return mEntityManager.getSkelMeshPropManager(); }

		inline SceneCamera& getActiveCamera() { return mEntityManager.getActiveCamera(); };

		inline CollEngine& getCollEngine() { return mCollEngine; }
		//inline ICollDetector& getCollDetector() { return mCollEngine.getDetector(); }

		inline WorldMaterialManager& getWorldMaterialManager() { return mCollEngine.getMaterialManager(); }

		inline SpatialManager& getSpatialManager() { return mSpatialManager; }
		inline SpatialManagerBuildPhase& getSpatialManagerBuildPhase() { return mSpatialManagerBuildPhase; }

		inline LoadContext& getLoadContext() { return mLoadContext; }

		inline const bool& getIsDestroying() { return mIsDestroying; }

		inline const bool& getFlagExtraRenders() { return mFlagExtraRenders; }

		inline const bool& getFlagRenderBPVolumes() { return mFlagRenderBPVolumes; }

		inline const bool& getFlagRenderVolumes() { return mFlagRenderVolumes; }

		inline const bool& getFlagRenderDetailVolumes() { return mFlagRenderDetailVolumes; }

		inline const bool& getFlagRenderNPColliders() { return mFlagRenderNPColliders; }

		inline const bool& getFlagRenderEntityStates() { return mFlagRenderEntityStates; }

		inline SoundManager& getSoundManager() { return mSoundManager.dref(); }

		inline Console& getConsole() { return mConsole; }

		inline EventDispatcher& getEventDispatcher() { return mEventDispatcher; }
		
		inline FootballerBallInterceptManager& getFootballerBallInterceptManager() { return mFootballerBallInterceptManager; }
		inline ControlSwitcher& getControlSwitcher() { return mControlSwitcher; }
		inline SpatialMatchState& getSpatialMatchState() { return mSpatialMatchState; }

		inline const PitchInfo& getPitchInfo() { assrt(mSpatialPitchModel.isValid); return mSpatialPitchModel; }
		inline PitchInfo& getPitchInfoRef() { return mSpatialPitchModel; }
		inline SpatialPitchModel& getSpatialPitchModel() { return mSpatialPitchModel; }

		inline FootballerIterator& getActiveFootballerIterator() { return mActiveFootballerIterator; }

		inline const Plane& getTerrainPlane() { return mTerrainPlane; }

		inline const String& getPluginPath() { return mPluginPath; }

		inline const String& getScriptPath() { return mScriptPath; }
		inline const String& getStartScript() { return mStartScript; }
		inline ScriptEngine& getScriptEngine() { return mScriptEngine; }

		inline Input::InputManager& getInputManager() { return mInputManager; }

		inline BallShotEstimator& getBallShotEstimator() { return mBallShotEstimator; }

		Player* findPlayer(const TCHAR* playerID);

		inline unsigned int genBallCommandID() { return mBallCommandCounter++; }

		inline const bool& isEnabledShadowing() { return mEnableShadowing; }
		void setEnabledShadowing(bool enabled) { mEnableShadowing = enabled; }

		inline Scanner& getScanner() { return mScanner; }

		inline RenderSprite& getCOCSprite() { return mCOCSprite; }
		RenderSprite::Skin* getCOCSkin(int index, bool isActive);
		
		inline RenderSprite& getArrowSprite() { return mArrowSprite; }
		inline RenderSprite& getBallPitchMarkerSprite() { return mBallPitchMarkerSprite; }
		
		inline RenderSprite& getShapeSprite_Circle() { return mShapeSprite_Circle; }
		inline RenderSprite& getShapeSprite_Quad() { return mShapeSprite_Quad; }

		inline UI& getUI() { return mUI; }

		inline const TeamEnum& getTeamWithWorldFwdDir() { return mTeamWithFwdSceneDir; }
		inline TeamEnum getTeamWithWorldFwdDir(bool isFwdDir) { return isFwdDir ? mTeamWithFwdSceneDir : opponentTeam(mTeamWithFwdSceneDir); }

		inline Footballer* getMagicRequestingBallFootballer() { return mMagicRequestingBallFootballer; }
		inline void setMagicRequestingBallFootballer(Footballer* pFootballer) { mMagicRequestingBallFootballer = pFootballer; }

	public:

		void toScene2D(const CtVector3& v3D, Vector2& v2D);
		void toScene3D(const CtVector2& v2D, Vector3& v3D, const float& height);
				
	public:

		void setLight(RenderLight* pLight, int index = 0);
		void setAmbient(RenderColor* pColor);
		void setShadowing(bool enable, RenderColor* pColor = NULL, bool alphaBlend = true, bool stencilTest = true);
		
	public:

		float mTweakDribbleVelHorizRatio;
		float mTweakDribbleVelVert;
		float mTweakShotLowVelHoriz;
		float mTweakShotLowVelVert;
		Vector3 mTweakShotLowRot;
		float mTweakShotHighVelHoriz;
		float mTweakShotHighVelVert;
		Vector3 mTweakShotHighRot;
		float mTweakShotExtraVelHoriz;
		float mTweakShotExtraVelVert;
		Vector3 mTweakShotExtraRot;
		float mTweakShotPassVelHoriz;
		float mTweakShotPassVelVert;
		Vector3 mTweakShotPassRot;
		float mTweakTackleVelHoriz;
		float mTweakTackleVelVert;
		float mTweakSharedControlDelay;
		float mTweakShockAudioScale;
		float mTweakAudioVolume;
		unsigned int mTweakAudioMaxSounds;
		bool mTweakEnableAI;
		float mTweakAudioAmbientVolume;
		float mTweakAudioRefDist;
		float mTweakAudioMaxDist;
		bool mTweakAudioEnableSoundMerge;
		float mTweakAudioSoundMergeTime;
		float mTweakAudioRolloff;

		bool mTweakEnableDebugKeys;

		//float mTweakRunSpeedTable[FootballerBodyDefs::RunTypeCount][FootballerBodyDefs::RunSpeedTypeCount];

		float mTweakPadDeadZone;

		bool mTweakCameraRelativeInput;
		bool mTweakCameraRelativeInputDiscreeteDirections;

		float mTweakGravityCoeff;

		bool mTweakEnableAirDrag;
		bool mTweakEnableAirMagnus;
		float mTweakAirDragEffectCoeff;

		bool mTweakTestBallSimul;

		float mTweakArrowSize;

		float mTweakTackleSlideTime;
		float mTweakTackleSpeedCoeff;

		bool mTweakTackleConservative;

		bool mTweakVibrateAttachedBall;
		bool mTweakRotateAttachedBall;

		bool mTweakVibrateCOC;

		bool mTweakShowPitchBallBounceMarker;
		float mTweakPitchBallBounceMarkerHeight;

		bool mTweakShowScanner;

		bool mTweakShowDebugScore;

		bool mTweakEnableManipulateCam;

		bool mFlagRenderSpatialManager;
		bool mFlagRenderBallShotEstimator;

		bool mDumpInputDevices;
		String mDumpInputDevicesPath;

		RenderColor mAmbient;
		bool mEnableShadowing;

		SpatialPitchModel mSpatialPitchModel;

		SwitchControlSetup mSwitchControlSetup;
		COCSetup mCOCSetup;

		String mPluginPath;
		String mScriptPath;
		String mStartScript;
		String mTeamFormationsPath;
		String mTeamsPath;

	public:

		MaterialTrigger* createTrigger(const TCHAR* triggerName);

		void executeMaterial(WorldMaterial& material, const Vector3& point, const float& shockMag);
		void executeMaterial(WorldMaterialCombo& material, const Vector3& point, const float& shockMag);

	public:

		Match() { assrt(false); }
		Match(Game& game, NativeAppInfo& nativeAppInfo, Renderer& renderer, Log& log, Console& console, FramePerformance& perf, ScriptEngine& scriptEngine);
		~Match();

		bool load();
		bool isLoaded();
		
		void loadGameChunk(BufferString& tempStr, DataChunk& chunk, const CoordSys& destCoordSys, CoordSysConv* pConv);

		bool processWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void frameMove(double fTime, float fElapsedTime, bool& isRenderFrame, bool isActive);
		void render();

		void resetFrameRate(int FPS);

		void renderText(ID3DXSprite* pSprite, ID3DXFont* pFont, ID3DXFont* pBigFont);

		void scriptedPreFrameMove(const Time& t, const Time& dt);
		void scriptedPostFrameMove(const Time& t, const Time& dt);

	public:

		TeamFormation* loadFormation(const TCHAR* name, Team* pTeam);

	public:

		void onActiveFootballersChange();

		void onGoalTrigger(TeamEnum team);

	protected:

		void doFrameMove();

		void initAudio();

		bool createScene();
		void destroyScene();

		void loadManualScene();

		void initBallControllers();
		void destroyBallControllers();

		void runPreLoadTests();
		void runOnLoadTests();
		void runOnFrameMoveTests();
		void runOnRenderTests();
		void runOnDestroyTests();

		void onScriptingInited();
		void initInputManager();
		void initInputControllers(Input::InputManager& manager, const TCHAR* inputProfilePath, BufferString& tempStr);

		void loadConfigPerformance();

		void processMatchStartChunk();

	protected:

		enum FrameState {

			FS_WaitingToStartSimul = 0,
			FS_Simulating,
			FS_WaitingToStartRender,
		};

		bool mAllowSleep;
		bool mAllowNonZeroSleep;

		float mNextRenderTime;
		float mSimStartTime;
		float mSimStartTimeOffset;
		float mSimClock;
		bool mSimIsPaused;
		float mSimPauseStartTime;
		float mSimPauseedNextRenderTime;
		unsigned int mSuccessivePerLagCount;
		unsigned int mMaxSuccessivePerLagCount;
		int mRenderWaitSleepCount;
		int mRenderSleepsLeft;
		int mRenderSleepInterval;
		int mRenderSleepTimeAvgSampleCount;
		float mRenderSleepTimeCumul;
		int mRenderSleepTimeAvgSamplesLeft;
		float mRenderSleepTime;
		unsigned int mRenderSleepSubtractCount;
		bool mFrameIsWaitingForRendering;
		unsigned int mCPULagCount;
		unsigned int mGPULagCount;
		bool mLastFrameWasRenderFrame;
		int mSimulIntervalsPerRenderFrame;
		int mUnsimulatedIntervalCount;

		FrameState mFrameState;
		int mWaitForSimulSleepCount;
		int mWaitForStartRenderSleepCount;

		float mFrameStartTime;
		float mFrameProcessStartTime;
		float mFrameProcessEndTime;
		float mFrameRenderStartTime;
		float mFrameEndTime;

		bool mIsProcessing;
		bool mIsRendering;

		void tuneSleepCount(int currSleepCount, bool inc, float dt);
		void sleep();
		void updateSleepTimeAvg(DWORD dt);

	protected:

		bool mIsLoaded;
		SoftPtr<Game> mGame;
		SoftPtr<NativeAppInfo> mNativeAppInfo;
		SoftPtr<Log> mLog;
		SoftPtr<Console> mConsole;
		SoftPtr<FramePerformance> mFramePerformance;
		SoftPtr<ScriptEngine> mScriptEngine;
		ScriptObject mScriptMatch;
		
		bool mLoadErrorFlag;

		bool mIsDestroying;

		bool mPauseIfNotActive;
		bool mRenderIfNotActive;

		SoftPtr<Renderer> mRenderer;
		Clock mClock;	

		bool mFirstFrame;
		float mClockMaxElapsedLimit;
		float mClockCompensation;
		float mStartTimeCompensation;

		HardPtr<SoundManager> mSoundManager;

		Input::InputManager mInputManager;
		
		CoordSys mCoordSys;
		FastUnitCoordSys mGameCoordSys;
		Vector3 mCoordSysAxis[3];
		Vector3 mNegCoordSysAxis[3];

		Vector2 m2DCoordSysAxis[2];
		Vector2 m2DNegCoordSysAxis[2];

		LoadContextImpl mLoadContext;
		
		CollEngine mCollEngine;
		HardPtr<SpatialManager> mSpatialManager;
		HardPtr<SpatialManagerBuildPhase> mSpatialManagerBuildPhase;
		
		HardPtr<BallShotEstimator> mBallShotEstimator;

		Plane mTerrainPlane;

		typedef WETL::CountedArray<bool, true, unsigned int, WETL::ResizeExact> LightEnables;

		LightEnables mLightEnables;

		EntityManager mEntityManager;
		EventDispatcher mEventDispatcher;
		FootballerBallInterceptManager mFootballerBallInterceptManager;
		ControlSwitcher mControlSwitcher;
		SpatialMatchState mSpatialMatchState;

		FootballerIterator_ActiveFotballers mActiveFootballerIterator;

		bool mFlagExtraRenders;
		bool mFlagRenderBPVolumes;
		bool mFlagRenderVolumes;
		bool mFlagRenderDetailVolumes;
		bool mFlagRenderNPColliders;
		bool mFlagRenderEntityStates;

		int mPerfLagCount;

		unsigned int mBallCommandCounter;

		Scanner mScanner;

		DimMap_2_3 m3DPitchSpriteDimMap;
		RenderSprite mCOCSprite;

		RenderSprite mArrowSprite;
		RenderSprite mBallPitchMarkerSprite;

		RenderSprite mShapeSprite_Circle;
		RenderSprite mShapeSprite_Quad;

		UI mUI;

		TeamEnum mTeamWithFwdSceneDir;

		HardRef<DataChunk> mMatchStartChunk;
	
		SoftPtr<Footballer> mMagicRequestingBallFootballer;

	protected:

			bool mIsSingleFrameStepping;
			bool mDoSingleFrameStep;
			bool mSingleFrameAllowCamMove;
			#ifdef _DEBUG
				bool mIsDebugging;
			#endif

	protected:

		bool createScriptMatch();
		bool scriptedLoad();
		
		void scriptedPreRender();
		void scriptedPostRender();

	public:

		/*
		inline int script_SceneUp() { return Scene_Up; }
		inline int script_SceneRight() { return Scene_Right; }
		inline int script_SceneFwd() { return Scene_Forward; }
		*/

		ScriptObject script_getGame() { return scriptArg(mGame); }

		const TCHAR* script_resolveScriptRelativePath(const TCHAR* path);
		int script_executePlugin(const TCHAR* plugin, const TCHAR* workingDir, const TCHAR* arg);

		ScriptObject script_getScriptMatch();

		CtVector3* script_SceneAxis(int dir) { return &mCoordSysAxis[dir]; }

		CtVector3* script_SceneUpAxis() { return &mCoordSysAxis[Scene_Up]; }
		CtVector3* script_SceneRightAxis() { return &mCoordSysAxis[Scene_Right]; }
		CtVector3* script_SceneFwdAxis() { return &mCoordSysAxis[Scene_Forward]; }

		CtVector3* script_SceneDownAxis() { return &mNegCoordSysAxis[Scene_Up]; }
		CtVector3* script_SceneLeftAxis() { return &mNegCoordSysAxis[Scene_Right]; }
		CtVector3* script_SceneBackAxis() { return &mNegCoordSysAxis[Scene_Forward]; }

		inline void script_toScene2D(CtVector3& v3D, Vector2& v2D) { return toScene2D(v3D, v2D); }
		inline void script_toScene3D(CtVector2& v2D, Vector3& v3D, float height) { return toScene3D(v2D, v3D, height); }

		ScriptCoordSys script_getCoordSys() { return scriptArg(mGameCoordSys); }

		bool script_getTweakEnableAI() { return mTweakEnableAI; }

		inline float script_conv(float meters) { return getCoordSys().convUnit(meters); }
		inline float script_invConv(float val) { return getCoordSys().invConvUnit(val); }

		inline float script_getPitchWidth() { return mSpatialPitchModel.width; }
		inline float script_getPitchLength() { return mSpatialPitchModel.length; }
		inline float script_getPitchHalfWidth() { return mSpatialPitchModel.halfWidth; }
		inline float script_getPitchHalfLength() { return mSpatialPitchModel.halfLength; }

		inline void script_setAutoSwitch(bool val) { mSwitchControlSetup.mode = val ? FSM_Automatic : FSM_Manual; }
		inline void script_setSwitchVeryDynamic(bool val) { mSwitchControlSetup.veryDynamic = val; }
		
		inline float script_getTime() { return getClock().getTime(); }
		inline float script_getFrameInterval() { return getClock().getFrameMoveTime(); }
		inline float script_getFrameTickLength() { return getClock().getFrameMoveTickLength(); }
		inline Ball* script_getBall() { return scriptArg(getBall()); }

		ScriptObject script_getFootballer(int team, int number);
		Team* script_getTeam(int team) { return &(getTeam((TeamEnum) team)); }

		inline unsigned int scrit_genBallCommandID() { return genBallCommandID(); }

		void script_setAmbient(RenderColor* pColor);
		void script_setLight(RenderLight* pLight, int index);
		void script_setShadowing(bool enable, RenderColor* pColor, bool alphaBlend, bool stencilTest);
		void script_setShadowDir(CtVector3& dir);

		SceneTransform* script_getViewTransform();

		inline BallShotEstimator* script_getBallShotEstimator() { return mBallShotEstimator; }

		void script_freeCam(bool enable);
		void script_manipulateCam(bool enable);

		inline bool script_isEnabledShadowing() { return isEnabledShadowing(); }
		inline void script_renderSpatialManager(bool enable) { mFlagRenderSpatialManager = enable; }
		inline void script_renderBallShotEstimator(bool enable) { mFlagRenderBallShotEstimator = enable; }

		inline void script_showScanner(bool show) { mTweakShowScanner = show; }

		bool script_estimateBallStateAtAAPlane(CtVector3& planePos, CtVector3& planeNormal, BallSimulation::TrajSample& result, bool allowPartialSimul);
		bool script_estimateBallStateAtPlane(CtVector3& planePos, CtVector3& planeNormal, BallSimulation::TrajSample& result, bool allowPartialSimul, bool allowApproximateSimul, bool fallbackCtBallVel, ScriptBool& isFallback);

		bool script_isApproximateBallState();	
		bool script_tuneApproximatedBallEstimate(BallSimulation::TrajSample& sample, BallSimulation::TrajSample& tunedSample);

		inline SpatialMatchState::BallState* script_getSpatialBallState() { return &(mSpatialMatchState.getBallState()); }

		inline SpatialPitchModel* script_getSpatialPitchModel() { return &mSpatialPitchModel; }


		inline bool script_findFootballerFreeSpace(ScriptedFootballer& footballer, CtVector3* pSearchDir, bool tryCurrPos, 
													float footballerRadius, float footballerSpeed, float searchDistance, float minSafetyTime, int maxIterCount, PitchRegionShape* pRegion, Vector3& outPos, ScriptFloat& outSafetyTime) {

			return mSpatialMatchState.findFootballerFreeSpace((Footballer&) footballer, pSearchDir, tryCurrPos, 
																footballerRadius, footballerSpeed, searchDistance, minSafetyTime, maxIterCount, pRegion, outPos, outSafetyTime.value());
		}

		ScriptedFootballer* script_iterNextRegionFootballer(PitchRegionShape* pRegion, int teamFilter, ScriptInt& startIndex);

		inline void script_setMagicRequestingBallFootballer(ScriptedFootballer* pFootballer) {

			if (mTweakEnableDebugKeys)
				mMagicRequestingBallFootballer = (Footballer*) pFootballer;
		}

		void script_queueRenderSphere(CtVector3& pos, float radius, RenderColor* pColor);

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline Match* scriptArg(Match& match) { return &match; }
	inline Match* scriptArg(Match* match) { return match; }

} }

#endif