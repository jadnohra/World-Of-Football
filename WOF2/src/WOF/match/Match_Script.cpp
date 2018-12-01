#include "Match.h"

#include "inc/Entities.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::Match, CMatch);

namespace WOF { namespace match {

const TCHAR* Match::ScriptClassName = L"CMatch";

void Match::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<Match>(ScriptClassName).
		enumInt(Scene_Up, L"Scene_Up").
		enumInt(Scene_Right, L"Scene_Right").
		enumInt(Scene_Forward, L"Scene_Fwd").
		enumInt(Scene2D_Right, L"Scene2D_Right").
		enumInt(Scene2D_Forward, L"Scene2D_Fwd").
		func(&Match::script_getGame, L"getGame").
		func(&Match::script_getScriptMatch, L"getScriptMatch").
		func(&Match::script_resolveScriptRelativePath, L"resolveScriptRelativePath").
		func(&Match::script_executePlugin, L"executePlugin").
		func(&Match::script_SceneAxis, L"SceneAxis").
		func(&Match::script_SceneUpAxis, L"SceneUpAxis").
		func(&Match::script_SceneRightAxis, L"SceneRightAxis").
		func(&Match::script_SceneFwdAxis, L"SceneFwdAxis").
		func(&Match::script_SceneDownAxis, L"SceneDownAxis").
		func(&Match::script_SceneLeftAxis, L"SceneLeftAxis").
		func(&Match::script_SceneBackAxis, L"SceneBackAxis").
		func(&Match::script_getCoordSys, L"getCoordSys").
		func(&Match::script_conv, L"conv").
		func(&Match::script_invConv, L"invConv").
		func(&Match::script_toScene2D, L"toScene2D").
		func(&Match::script_toScene3D, L"toScene3D").
		func(&Match::script_getTweakEnableAI, L"getTweakEnableAI").
		func(&Match::script_setMagicRequestingBallFootballer, L"setMagicRequestingBallFootballer").
		func(&Match::script_getPitchWidth, L"getPitchWidth").
		func(&Match::script_getPitchHalfWidth, L"getPitchHalfWidth").
		func(&Match::script_getPitchLength, L"getPitchLength").
		func(&Match::script_getPitchHalfLength, L"getPitchHalfLength").
		func(&Match::script_setAutoSwitch, L"setAutoSwitch").
		func(&Match::script_setSwitchVeryDynamic, L"setSwitchVeryDynamic").
		func(&Match::script_showScanner, L"showScanner").
		func(&Match::script_getTime, L"getTime").
		func(&Match::script_getFrameInterval, L"getFrameInterval").
		func(&Match::script_getFrameTickLength, L"getFrameTickLength").
		func(&Match::script_getBall, L"getBall").
		func(&Match::script_getTeam, L"getTeam").
		func(&Match::script_getFootballer, L"getFootballer").
		func(&Match::scrit_genBallCommandID, L"genBallCommandID").
		func(&Match::script_setLight, L"setLight").
		func(&Match::script_setAmbient, L"setAmbient").
		func(&Match::script_setShadowing, L"setShadowing").
		func(&Match::script_setShadowDir, L"setShadowDir").
		func(&Match::script_getViewTransform, L"getViewTransform").
		func(&Match::script_freeCam, L"freeCam").
		func(&Match::script_manipulateCam, L"manipulateCam").
		func(&Match::script_queueRenderSphere, L"queueRenderSphere").
		func(&Match::script_getBallShotEstimator, L"getBallShotEstimator").
		func(&Match::script_isEnabledShadowing, L"isEnabledShadowing").
		func(&Match::script_renderSpatialManager, L"renderSpatialManager").
		func(&Match::script_renderBallShotEstimator, L"renderBallShotEstimator").
		func(&Match::script_estimateBallStateAtAAPlane, L"estimateBallStateAtAAPlane").
		func(&Match::script_estimateBallStateAtPlane, L"estimateBallStateAtPlane").
		func(&Match::script_isApproximateBallState, L"isApproximateBallState").
		func(&Match::script_tuneApproximatedBallEstimate, L"tuneApproximatedBallEstimate").
		func(&Match::script_getSpatialBallState, L"getSpatialBallState").
		func(&Match::script_getSpatialPitchModel, L"getSpatialPitchModel").
		func(&Match::script_findFootballerFreeSpace, L"findFootballerFreeSpace").
		func(&Match::script_iterNextRegionFootballer, L"iterNextRegionFootballer");
}

ScriptObject Match::script_getScriptMatch() {

	return mScriptMatch;
}

const TCHAR* Match::script_resolveScriptRelativePath(const TCHAR* path) {
	
	static BufferString scriptPathHolder;

	scriptPathHolder.assign(getScriptPath());

	PathResolver::appendPath(scriptPathHolder, path, true);

	return scriptPathHolder.c_tstr();
}

int Match::script_executePlugin(const TCHAR* plugin, const TCHAR* workingDir, const TCHAR* arg) {

	BufferString pluginDirPath(getPluginPath());
	BufferString pluginPath(pluginDirPath);

	PathResolver::appendPath(pluginPath, plugin, true);

	mLog->log(LOG_INFO, true, true, L"executing: %s", pluginPath.c_tstr());
	if (workingDir)
		mLog->log(LOG_INFO, true, true, L"workingDir: %s", workingDir);

	int exitCode = -1;

	if (!Win::executeProcess(pluginPath.c_tstr(), workingDir, arg, exitCode)) {

		mLog->log(LOG_ERROR, true, true, L"Execution Failed");

		return exitCode;
	}

	return exitCode;
}

bool Match::createScriptMatch() {

	{	
		ScriptFunctionCall<ScriptObject> fct(mScriptEngine, L"createMatch", mStartScript.c_tstr());

		mScriptMatch = (fct)(mScriptEngine->toScriptArg(), scriptArg(this));

		if (!mScriptMatch.IsInstance()) {

			mLoadErrorFlag = true;
			mLog->log(LOG_ERROR, true, true, L"Failed to Create Script-Side Match");

			return false;
		}
	}
	
	return true;
}

bool Match::scriptedLoad() {

	ScriptFunctionCall<bool> fct(mScriptEngine, mScriptMatch, L"load");

	return (fct)();
}

void Match::scriptedPreFrameMove(const Time& t, const Time& dt) {

	ScriptFunctionCall<void> fct(mScriptEngine, mScriptMatch, L"preFrameMove");

	(fct)(t, dt);
}

void Match::scriptedPostFrameMove(const Time& t, const Time& dt) {

	ScriptFunctionCall<void> fct(mScriptEngine, mScriptMatch, L"postFrameMove");

	(fct)(t, dt);
}

void Match::scriptedPreRender() {

	ScriptFunctionCall<void> fct(mScriptEngine, mScriptMatch, L"preRender");

	(fct)();
}

void Match::scriptedPostRender() {

	ScriptFunctionCall<void> fct(mScriptEngine, mScriptMatch, L"postRender");

	(fct)();
}

ScriptObject Match::script_getFootballer(int teamIndex, int number) {

	if (teamIndex < 0 || teamIndex >= 2)
		return ScriptObject();

	Team& team = getTeam((TeamEnum) teamIndex);
	SoftPtr<Footballer> footballer;

	if (team.getFootballer((FootballerNumber) number, footballer.ptrRef())) {

		return (footballer->getScriptObject());
	}

	return ScriptObject();
}

void Match::script_setLight(RenderLight* pLight, int index) {

	setLight(pLight, index);
}

void Match::script_setAmbient(RenderColor* pColor) {

	setAmbient(pColor);
}

void Match::script_setShadowing(bool enable, RenderColor* pColor, bool alphaBlend, bool stencilTest) {

	setShadowing(enable, pColor, alphaBlend, stencilTest);
}

void Match::script_setShadowDir(CtVector3& dir) {

	Plane plane;

	plane.init(Point(0.0f, 1.0f, 0.0f), Vector(0.0f, 1.0f, 0.0f), true);
	mRenderer->setupShadowingPlanar(Vector3(dir.x, dir.y, dir.z), plane);
}

SceneTransform* Match::script_getViewTransform()  {

	return &(getActiveCamera().mTransformWorld);
}

void Match::script_manipulateCam(bool enable) {

	mTweakEnableManipulateCam = enable;
}

void Match::script_freeCam(bool enable) {

	if (enable) {

		SceneCamera& cam = getActiveCamera();
		cam.cancelTrack();

	} else {

		SceneCamera& cam = getActiveCamera();

		float z = 0.0f;
		cam.track(&(mEntityManager.getBall()), &z);
	}
}

bool Match::script_estimateBallStateAtAAPlane(CtVector3& planePos, CtVector3& planeNormal, BallSimulation::TrajSample& result, bool allowPartialSimul) {

	SoftPtr<BallSimulation> simul;

	if (allowPartialSimul) {

		simul = &getFootballerBallInterceptManager().getSimul();

		if (simul->getSimulState() == BallSimulation::Simul_None)
			simul.destroy();

	} else {

		simul = getFootballerBallInterceptManager().getValidSimul();
	}

	if (simul.isValid()) {

		AAPlane plane;
		
		plane.init(planePos, planeNormal, true);

		if (simul->estimateStateAtPlane(plane, true, result, false, NULL, false)) {

			return true;

		} else {

			return false;
		}
	}

	return false;
}

bool Match::script_estimateBallStateAtPlane(CtVector3& planePos, CtVector3& planeNormal, BallSimulation::TrajSample& result, bool allowPartialSimul, bool allowApproximateSimul, bool fallbackCtBallVel, ScriptBool& isFallback) {

	SoftPtr<BallSimulation> simul;
	isFallback.value() = false;
	
	if (allowPartialSimul) {

		simul = &getFootballerBallInterceptManager().getSimul();

		if (simul->getSimulState() == BallSimulation::Simul_None)
			simul.destroy();

	} else {

		simul = getFootballerBallInterceptManager().getValidSimul();
	}

	if (!allowApproximateSimul && simul.isValid()) {

		if (simul->isSimulApproximate())
			simul.destroy();
	}

	if (simul.isValid()) {

		Plane plane;

		plane.init(planePos, planeNormal, true);

		if (simul->estimateStateAtPlane(plane, true, result, false, NULL, false)) {

			return true;

		} else {

			return false;
		}

	} else {

		if (fallbackCtBallVel) {

			isFallback.value() = true;

			Ball& ball = getBall();
			const Vector3& ballPos = ball.getPos();
			Vector3 ballVel = (ball.getVel() != NULL) ? dref(ball.getVel()) : Vector3::kZero;

			if (ball.isOnPitch())
				ballVel.el[Scene_Up] = 0.0f;

			if (ballVel.isZero()) {

				return false;

			} else {

				float u;

				if (intersectPlane(planePos, planeNormal, ballPos, ballVel, u, &result.pos)) {

					result.vel = ballVel;
					result.time = sqrtf(distanceSq(ballPos, result.pos) / ballVel.magSquared());
					result.time += getClock().getTime();
					result.isOnPitch = false;

					return true;

				} else {

					return false;
				}
			}
		}
	}

	return false;
}

bool Match::script_isApproximateBallState() {

	return getFootballerBallInterceptManager().getSimul().isSimulApproximate();
}

bool Match::script_tuneApproximatedBallEstimate(BallSimulation::TrajSample& sample, BallSimulation::TrajSample& tunedSample) {

	tunedSample = sample;

	return getFootballerBallInterceptManager().getSimul().tuneApproximatedEstimate(getBall(), tunedSample);
}

ScriptedFootballer* Match::script_iterNextRegionFootballer(PitchRegionShape* pRegion, int teamFilter, ScriptInt& startIndex) {

	FootballerIndex inOut = startIndex.value();

	Footballer* pRet = mSpatialMatchState.iterNextRegionFootballer(pRegion, (TeamEnum) teamFilter, inOut);

	startIndex.value() = inOut;

	return scriptArg((ScriptedFootballer*) pRet);
}

void Match::script_queueRenderSphere(CtVector3& pos, float radius, RenderColor* pColor) {

	Sphere sphere(pos, radius);

	mRenderer->queueVol(VolumeRef(sphere), NULL, pColor);
}

} }