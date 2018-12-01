#include "SceneScriptedCharacter.h"

#include "../../Match.h"
#include "../camera/SceneCamera.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::SceneScriptedCharacter, CCharacter);

namespace WOF { namespace match {

const TCHAR* SceneScriptedCharacter::ScriptClassName = L"CCharacter";


bool SceneScriptedCharacter::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType) {

	return init_create(this, match, tempStr, chunk, pConv, objType);
	/*
	if (!SceneEntityMesh::init_create(match, tempStr, chunk, pConv, objType)) {

		return false;
	}

	if (chunk.getAttribute(L"scriptClass", tempStr)) {

		ScriptEngine& scriptEngine = mNodeMatch->getScriptEngine();

		if (scriptEngine.createInstance(tempStr.c_tstr(), mScriptObject)) {

			ScriptFunctionCall<bool> func(scriptEngine, mScriptObject, L"load");

			if (!func(this, scriptArg(chunk), scriptArg(pConv))) {

				assrt(false);
				return false;
			}
	
		} else {

			assrt(false);
			return false;
		}

	} else {

		assrt(false);
		return false;
	}

	return true;
	*/
}

void SceneScriptedCharacter::init_prepareScene() {

	SceneEntityMesh::init_prepareScene();

	scriptedPrepareScene();
}

bool SceneScriptedCharacter::isActiveCamVisible() {

	/*

	in frameMove...

	Frustum* pFrustum = mNodeMatch->getActiveCamera().getFrustum();
	
	bool wasVis = mActiveCamVisible;

	if (pFrustum) {

		mActiveCamVisible = intersect(mCollider.mVolumeWorld, *pFrustum);
	
	} else {

		mActiveCamVisible = false;
	}
	*/

	return false;
}

SceneScriptedCharacter::SceneScriptedCharacter(Match& match) {

	mNodeMatch = &match;
}

SceneScriptedCharacter::~SceneScriptedCharacter() {

	{
		ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"destructor");

		(func)();
	}
}

void SceneScriptedCharacter::sanityCheck() {

	ScriptEngine& scriptEngine = mNodeMatch->getScriptEngine();

	declareInVM(scriptEngine.getVM());

	SquirrelObject instance;
	const TCHAR* TestClass = L"TestEntityMeshConroller";

	if (scriptEngine.createInstance(TestClass, instance, TestClass)) {

		ScriptFunctionCall<void> fct(scriptEngine, instance, L"test1");

		(fct)(this);
	}
}

void SceneScriptedCharacter::frameMove(const Time& time, const Time& dt) {

	ScriptEngine& scriptEngine = mNodeMatch->getScriptEngine();

	ScriptFunctionCall<void> func(scriptEngine, mScriptObject, L"frameMove");

	func(time, dt);

	scriptClean();
}

void SceneScriptedCharacter::script_orientLookAt(Vector3* lookAt) { orientLookAt(dref(lookAt), mNodeMatch->getCoordAxis(Scene_Up)); }
void SceneScriptedCharacter::script_orientDirection(Vector3* direction) { orientDirection(dref(direction), mNodeMatch->getCoordAxis(Scene_Up)); }
bool SceneScriptedCharacter::script_isOrientDirection(CtVector3* direction) { return isOrientDirection(dref(direction), mNodeMatch->getCoordAxis(Scene_Up)); }

void SceneScriptedCharacter::script_camRelativizeInPlace(Vector3* direction, bool constrainUp, bool discretize) {

	mNodeMatch->getActiveCamera().relativiseDirection(Vector3(dref(direction)), *direction, constrainUp, discretize);
}

AnimationIndex::Type SceneScriptedCharacter::script_getCurrAnimationIndex() {

	return mSkelInst->getAnimation();
}

AnimationIndex::Type SceneScriptedCharacter::script_getAnimationIndex(const TCHAR* name) {

	AnimationIndex::Type index = -1;

	mSkelInst->getAnimationIndex(name, index);

	return index;
}

bool SceneScriptedCharacter::script_setAnimationByIndex(AnimationIndex::Type index, bool reset) {

	if (index != -1) {

		if (reset || mSkelInst->getAnimation() != index) {

			mSkelInst->setAnimation(index, 1.0f);
		}

		return true;
	}

	assrt(false);
	return false;
}

bool SceneScriptedCharacter::script_setAnimation(const TCHAR* name, bool reset) {

	AnimationIndex::Type index;

	if (mSkelInst->getAnimationIndex(name, index)) {

		if (reset || mSkelInst->getAnimation() != index) {

			mSkelInst->setAnimation(index, 1.0f);
		}

		return true;
	}

	assrt(false);
	return false;
}


bool SceneScriptedCharacter::script_addBlendAnimation(const TCHAR* name, float weight) {

	AnimationIndex::Type index;

	if (mSkelInst->getAnimationIndex(name, index)) {

		mSkelInst->addBlendAnimation(index, weight);

		return true;
	}

	assrt(false);
	return false;
}

void SceneScriptedCharacter::script_setAnimationWeight(float weight) { mSkelInst->setAnimationWeight(weight); }
void SceneScriptedCharacter::script_setAnimationSpeed(float speed) { mSkelInst->setSpeedFactor(speed); }
void SceneScriptedCharacter::script_setAnimationLooping(bool looping) { mSkelInst->setLoop(looping); }
bool SceneScriptedCharacter::script_isAnimationFinished() { return mSkelInst->reachedAnimationEnd(); }
void SceneScriptedCharacter::script_addAnimationTime(float dt) { return mSkelInst->addTime(dt); }

void SceneScriptedCharacter::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<SceneScriptedCharacter>(ScriptClassName).
		MAppendSQClassDefMatchSceneScriptedCharacter_ScriptFuncs(SceneScriptedCharacter);
}

bool SceneScriptedCharacter::scriptClean() {

	if (clean()) {

		if (mCollProcessing.isValid()) {

			mCollProcessing->collRegistry.update(mNodeMatch->getCollEngine(), mVolumeColliders->dynamicColl, false);

			SoftPtr<SceneDynVol> proxyVol = getProxyVol();

			if (proxyVol.isValid()) {

				mCollProcessing->collProxyRegistry.update(mNodeMatch->getCollEngine(), proxyVol->getCollider(), true);
			}
		}

		return true;
	}

	return false;
}

bool SceneScriptedCharacter::scriptedIsInAIMode() {

	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"isInAIMode");

	return (func)();
}

bool SceneScriptedCharacter::scriptedHasAIMode() {

	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"hasAIMode");

	return (func)();
}

bool SceneScriptedCharacter::scriptedHasPlayerMode() {

	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"hasPlayerMode");

	return (func)();
}

bool SceneScriptedCharacter::scriptedActivatePlayerMode(Input::Controller* pController) { 
	
	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"activatePlayerMode");

	return (func)(scriptArg(dref(pController)), mNodeMatch->getClock().getTime());
}

bool SceneScriptedCharacter::scriptedActivateAIMode() {

	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"activateAIMode");

	return (func)(mNodeMatch->getClock().getTime());
}

bool SceneScriptedCharacter::scriptedCanActivateAIMode() {

	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"canActivateAIMode");

	return (func)(mNodeMatch->getClock().getTime());
}

bool SceneScriptedCharacter::scriptedCanActivatePlayerMode() {

	ScriptFunctionCall<bool> func(mNodeMatch->getScriptEngine(), mScriptObject, L"canActivatePlayerMode");

	return (func)(mNodeMatch->getClock().getTime());
}

int SceneScriptedCharacter::scriptedGetTypeInt() {

	ScriptFunctionCall<int> func(mNodeMatch->getScriptEngine(), mScriptObject, L"getTypeInt");

	return (func)();
}

const TCHAR* SceneScriptedCharacter::scriptedGetType() {

	ScriptFunctionCall<const TCHAR*> func(mNodeMatch->getScriptEngine(), mScriptObject, L"getType");

	return (func)();
}

void SceneScriptedCharacter::scriptedPrepareScene() {

	ScriptFunctionCall<void> func(mNodeMatch->getScriptEngine(), mScriptObject, L"prepareScene");

	(func)();
}

void SceneScriptedCharacter::script_playSound(const TCHAR* name, CtVector3* pPos, float mag) {

	mNodeMatch->getSoundManager().playSound(name, dref(pPos), mag, true);	
}

void SceneScriptedCharacter::script_playSound2(const TCHAR* name, CtVector3* pPos, float mag, float pitch) {

	mNodeMatch->getSoundManager().playSound(name, dref(pPos), mag, true, pitch);	
}

bool SceneScriptedCharacter::script_createPositionProxy(float horizeRadiusCoeff) {

	if (mCollProcessing.isValid() && mSkelInst.isValid()) {

		scriptClean();

		AAB refVol = mSkelInst->dirtyGetLocalDynamicAAB();
		
		float newRadius = 0.25f * (refVol.size(Scene_Right) + refVol.size(Scene_Forward));
		newRadius *= horizeRadiusCoeff;
		Vector3 center;
		refVol.center(center);

		refVol.min.el[Scene_Right] = center.el[Scene_Right] - newRadius;
		refVol.max.el[Scene_Right] = center.el[Scene_Right] + newRadius;
		refVol.min.el[Scene_Forward] = center.el[Scene_Forward] - newRadius;
		refVol.max.el[Scene_Forward] = center.el[Scene_Forward] + newRadius;
		
		const Vector3& offset = mTransformWorld.getPosition();
	
		SoftPtr<SceneDynVol> proxy = createProxyVol(VolumeRef(refVol), Node_ScripedCharacterPositionProxy);

		return proxy.isValid();
	}

	assrt(false);
	return false;
}

} }