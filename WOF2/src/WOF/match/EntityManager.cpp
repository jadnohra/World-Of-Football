#include "EntityManager.h"

#include "WE3/helper/load/WEVarTableHelper.h"
#include "WE3/WEAppPath.h"
#include "WE3/filePath/WEPathResolver.h"
#include "WE3/helper/load/WEMathDataLoadHelper.h"
using namespace WE;

#include "entity/ball/Ball.h"
#include "Match.h"

#include "inc/Entities.h"

#include "WE3/WEDebugBypass.h"

namespace WOF { namespace match {

EntityManager::EntityManager() : mNodeIdCounter(0), 
	mSceneIsLoaded(false), mSceneIsLoading(false), mSceneIsPrepared(false) {

	mActiveCamIndex.setNull();
}

EntityManager::~EntityManager() {

	destroyNodes();
}

void EntityManager::destroyNodes() {

	mAllNodes.destroy();
}

/*
void EntityManager::init(Match& match) {

	m = &match;
}
*/

bool EntityManager::loadSceneFile(Match& match) {

	BufferString pathStr(match.getGame().getAssetsPath());

	if (pathStr.isEmpty()) {

		return false;
	}

	const TCHAR* sceneChunkPath = L"WOF.xml/scene";
	
	PathResolver::appendPath(pathStr, sceneChunkPath, true, true);


	BufferString tempStr;
	DataSourceChunk chunk;
	DataSourcePool& dataSrcPool = match.getLoadContext().dataSourcePool();

	{
		ScriptVarTable& varTable = dref(match.getLoadContext().varTablePtr());

		varTable.setValue(L"LocalPath", match.getGame().getLocalPath().c_tstr());
	}

	if (dataSrcPool.getChunk(pathStr.c_tstr(), true, chunk, NULL, true)) {

		chunk->getAttribute(L"type", tempStr);

		if (tempStr.equals(L"scene")) {

			mSceneLoadErrCount = 0;

			if (SceneLoader_v1::load(match, mSceneRoot, chunk.chunk(), *this, this)) {

				//Scene2Loader_v1::load(match, match, match.getSceneRoot(), chunk.chunk());

				return (mSceneLoadErrCount == 0);
			}
		} 
	}

	return false;
}

void EntityManager::loadPluginPath(Match& match, String& pluginPath) {

	pluginPath.destroy();
	BufferString pathStr(AppPath::getPath());

	if (pathStr.isEmpty()) {

		return;
	}

	const TCHAR* sceneChunkPath = L"../assets/WOF.xml/plugins";

	PathResolver::appendPath(pathStr, sceneChunkPath, true, true);

	BufferString tempStr;
	DataSourceChunk chunk;
	DataSourcePool& dataSrcPool = match.getLoadContext().dataSourcePool();

	if (dataSrcPool.getChunk(pathStr.c_tstr(), true, chunk, NULL, true)) {

		if (chunk->getAttribute(L"path", pluginPath)) {

			chunk->resolveFilePath(pluginPath);
		}
	}
}

void EntityManager::loadUnknownChunk(BufferString& tempStr, Match& match, DataChunk& chunk, const CoordSys& destCoordSys, CoordSysConv* pConv) {

	if (chunk.equals(L"game")) {

		SoftRef<DataSource> source = chunk.getSource();

		SoftRef<DataChunk> resolvedChunk = match.getLoadContext().dataSourcePool().resolveRemoteChunk(chunk, true, source);

		match.loadGameChunk(tempStr, resolvedChunk, destCoordSys, pConv);
	}

}

void EntityManager::loadManualSceneNodes(Match& match) {

	if(mBall.isValid()) {
		
		BufferString name;

		name.assign(mBall->nodeName());
		name.append(L"_simul");

		mSimulBall = mBall->clone(name.c_tstr(), Node_SimulBall, false);
		
		if (mSimulBall) {

			mSimulBall->setVisible(false);
			putNode(mSimulBall.ptr());
		}
	}
}

bool EntityManager::loadManualPostPrepareSceneNodes(Match& match) {

	if (mPitch.isValid()) {

		if (mSimulPitch.isValid() 
			|| mPitch->mSkelInst.isValid()
			|| !mPitch->mVolumeColliders.isValid()
			|| !mPitch->mVolumeColliders->staticColl.isValid()
			|| (mPitch->mVolumeColliders->staticColl->volume.type != V_OBB)
			) {

			log(L"Invalid Pitch Entity");
			assrt(false);

			return false;

		} else {

			OBB& obb = mPitch->mVolumeColliders->staticColl->volume.toOBB();

			match.getPitchInfoRef().height = MOBBCenter(obb).el[Scene_Up] + MOBBExtents(obb).el[Scene_Up];
			match.getPitchInfoRef().isValid = true;
			match.getSpatialPitchModel().init(match);
			

			SoftPtr<SceneStaticVol> simulPitch;
			WE_MMemNew(simulPitch.ptrRef(), SceneStaticVol());

			VolumeCollider& pitchVolCollider = mPitch->mVolumeColliders->staticColl;

			WorldMaterialID matID = pitchVolCollider.getMaterial();
			VolumeStruct& pitchVol = pitchVolCollider.volume;
			
			OBB simulPitchVolume = pitchVol.toOBB();
			simulPitchVolume.extents[Scene_Right] = obb.extents[Scene_Right] * 100.0f;
			simulPitchVolume.extents[Scene_Forward] = obb.extents[Scene_Forward] * 100.0f;

			if (simulPitch->create(match, mPitch, L"simulPitch", matID, VolumeRef(simulPitchVolume), false, Node_SimulPitch)) {

				mSimulPitch = simulPitch;
				putNode(mSimulPitch.ptr());

			} else {

				HardPtr<SceneStaticVol> simulPitchDtor = simulPitch.ptr();
				simulPitch.destroy();
			}
		}
	}

	return true;
}

bool EntityManager::loadScene(Match& match) {

	if (mSceneIsLoaded || mSceneIsLoading) {

		assrt(false);
		return false;
	}

	if (!mTeams[Team_A].init(match, Team_A)) {

		assrt(false);
		return false;
	}

	if (!mTeams[Team_B].init(match, Team_B)) {

		assrt(false);
		return false;
	}

	mSceneIsLoading = true;

	
	if (loadSceneFile(match) == false) {

		mSceneIsLoading = false;

		log(L"Errors While Loading  Scene File");
		assrt(false);
		return false;
	}

	loadManualSceneNodes(match);
	prepareScene();
	if (!loadManualPostPrepareSceneNodes(match)) {

		return false;
	}
	
	if (mActiveCam.isNull()) {

		log(L"No Camera Defined");

		assrt(false);
		return false;
	}


	if (mBall.isNull()) {

		log(L"No Ball Defined");

		assrt(false);
		return false;
	}

	if (mPitch.isNull()) {

		log(L"No Pitch Defined");

		assrt(false);
		return false;
	}

	if (mSimulPitch.isNull()) {

		log(L"No Simul Pitch Defined");

		assrt(false);
		return false;
	}


	mControlRadius = match.getCoordSys().convUnit(1.0f);

	mSceneIsLoaded = true;
	mSceneIsLoading = false;

	return true;
}

SceneNodeIndex::Type EntityManager::genNodeId() {

	return mNodeIdCounter++;
}

SceneNode* EntityManager::getNode(const TCHAR* nodeName) {

	SoftPtr<SceneNode> ret;

	ret = mAllNodes.getNode(nodeName);

	return ret.ptr();
}

SceneCamera& EntityManager::getActiveCamera() {

	return mActiveCam.dref();
}

void EntityManager::setActiveCamera(const TCHAR* camName) {

	SoftPtr<SceneCamera> oldActiveCam = mActiveCam;

	mActiveCam = mCameras.getNode(camName);
	mActiveCam->notifySwitchedFrom(oldActiveCam);

	if (mActiveCam.isNull()) {

		mActiveCamIndex.setNull();

	} else {

		if (mCameras.mNodeMap.findIterationIndex(mActiveCam.ptr(), mActiveCamIndex) == false) {

			mActiveCamIndex.setNull();
		}
	}
}

void EntityManager::setActiveCamera(const SceneNodeIndex::Type& index) {

	SoftPtr<SceneCamera> oldActiveCam = mActiveCam;
	mActiveCam.destroy();

	if (mCameras.mNodeMap.searchByIterator(index, mActiveCam.ptrRef())) {

		mActiveCamIndex = index;

	} else {

		mActiveCamIndex.setNull();
	}

	if (mActiveCam.isValid())
		mActiveCam->notifySwitchedFrom(oldActiveCam);
}

void EntityManager::nextCamera() {

	SceneNodeIndex::Type count = mCameras.mNodeMap.getCount();

	if (mActiveCamIndex.isNull()) {

		if (count == 0) {

			return;
		}

		mActiveCamIndex = 0;
	}

	++mActiveCamIndex;

	if (mActiveCamIndex >= count) {

		mActiveCamIndex = 0;
	}

	setActiveCamera(mActiveCamIndex);
}

void EntityManager::prevCamera() {

	SceneNodeIndex::Type count = mCameras.mNodeMap.getCount();

	if (mActiveCamIndex.isNull()) {

		if (count == 0) {

			return;
		}

		mActiveCamIndex = 0;
	}

	if (mActiveCamIndex == 0) {

		mActiveCamIndex = count - 1;

	} else {

		--mActiveCamIndex;
	}

	setActiveCamera(mActiveCamIndex);
}

void EntityManager::prepareScene(SceneNode* pNode) {

	switch (pNode->objectType) {
		case Node_Node:
			break;
		case Node_Camera:
			SceneCamera::camFromNode(pNode)->init_prepareScene();
			break;
		case Node_Goal:
		case Node_GoalProxy:
		case Node_Pitch:
		case Node_EntMesh:
		case Node_EntMeshNoScriptedCharacterColl:
			SceneEntityMesh::entMeshFromNode(pNode)->init_prepareScene();
			break;
		case Node_Ball:
		case Node_SimulBall:
			Ball::ballFromNode(pNode)->init_prepareScene();
			break;
		default:
			assrt(false);
			break;
	}
}

void EntityManager::prepareScene() {

	if (mSceneIsPrepared) {

		assrt(false);
		return;
	}

	mSceneIsPrepared = true;

	mSceneRoot.rootNodeTreeClean();

	SceneNodeIndex::Type i;
	
	/*
	SceneNode* pNode;
	i = mNodes.mNodeMap.iterator();
	while(mNodes.mNodeMap.next(i, pNode)) {

		prepareScene(pNode);
	}
	*/

	{
		SceneEntityMesh* pEntMesh;
		i = mEntMeshes[0].mNodeMap.iterator();
		while(mEntMeshes[0].mNodeMap.next(i, pEntMesh)) {

			pEntMesh->init_prepareScene();
		}
	}

	{
		SceneEntityMesh* pEntMesh;
		i = mEntMeshes[1].mNodeMap.iterator();
		while(mEntMeshes[1].mNodeMap.next(i, pEntMesh)) {

			pEntMesh->init_prepareScene();
		}
	}

	{
		SceneScriptedCharacter* pEntMesh;
		i = mScriptedCharacters.mNodeMap.iterator();
		while(mScriptedCharacters.mNodeMap.next(i, pEntMesh)) {

			pEntMesh->init_prepareScene();
		}
	}

	if (mBall.isValid()) {

		mBall->init_prepareScene();
	}

	if (mSimulBall.isValid()) {

		mSimulBall->init_prepareScene();
	}

	mTeams[0].init_prepareScene();
	mTeams[1].init_prepareScene();

	SceneCamera* pCam;
	i = mCameras.mNodeMap.iterator();
	while(mCameras.mNodeMap.next(i, pCam)) {

		pCam->init_prepareScene();
	}
}

/*
bool EntityManager::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	bool processed = false;

	if (mBall.isValid()) {

		processed = processed || mBall->handleMessage(uMsg, wParam, lParam);
	}

	processed = processed || mTeams[0].handleMessage(uMsg, wParam, lParam);
	processed = processed || mTeams[1].handleMessage(uMsg, wParam, lParam);

	if (mActiveCam.isValid()) {

		processed = processed || mActiveCam->handleMessage(uMsg, wParam, lParam);
	}

	return processed;
}
*/

Ball& EntityManager::getBall() {

	return mBall.dref();
}

Ball& EntityManager::getSimulBall() {

	return mSimulBall.dref();
}

SceneEntityMesh& EntityManager::getPitch() {

	return mPitch.dref();
}

SceneStaticVol& EntityManager::getSimulPitch() {

	return mSimulPitch.dref();
}

void EntityManager::updateBallPosession(Match& match, const Clock& time) {

	getBall().updateFootballerOwnerships(match, time);
}

void EntityManager::frameMove(Match& match, const Clock& time) {

	if (mActiveCam.isValid()) {

		//DIRTY HACK!!!!
		if (mActiveCam->getTrackedNode() == NULL) {

			mActiveCam->frameMove(time.getTime(), time.getFrameMoveTime());
			mActiveCam->clean();
		}
	} 

	/*
		The order of frameMove's is very sensitive since some parts
		of code inside frameMove's depend on the current order ...

		maybe there is a way to do this better (more robust), maybe not ...
		probably by using frameID's but that would complicate the code,
		its a choice that has to be made
	*/

	updateBallPosession(match, time);
	
	match.getFootballerBallInterceptManager().frameMove(match, time.getTime());
	//match.getBallSimulManager().frameMove(match, time.mTime);
	match.getSpatialMatchState().frameMove(match, time);
	match.getControlSwitcher().frameMove(match, time.getTime());
	
	mTeams[0].frameMovePlayers(time.mTime);
	mTeams[1].frameMovePlayers(time.mTime);
		
	{

		TickCount ticks = time.getFrameMoveTickCount();

		if (ticks == 0 && time.getTime() == 0.0f) {

			ticks = 1;
		}

		if (ticks > 1) {

			//if we look at the way we are advancing time in Match.cpp
			//we see that ticks can only be 1.
			// if it is > 1 it is porbably due to floating point
			//imprecision in WETime.cpp
			//this needs to be fixed, anyway with the new 
			// time managment in Match.cpp the whole WETime class
			//has to be refactored and a MatchClock class should
			//replace WOFMatchTime

			log(L"TODO: time frameCount bugfix %f", time.getTime());
			ticks = 1;
		}

		for (TickCount tick = 0; tick < ticks; ++tick) {

			const Time& t = time.getTime();
			const Time& dt = time.getFrameMoveTickLength();

			match.scriptedPreFrameMove(t, dt);
			
			mTeams[0].frameMove(time);
			mTeams[1].frameMove(time);

			SceneNodeIndex::Type i = mScriptedCharacters.mNodeMap.iterator();
			SceneScriptedCharacter* pNode;

			while(mScriptedCharacters.mNodeMap.next(i, pNode)) {

				pNode->frameMove(t, dt);
			}

			match.scriptedPostFrameMove(t, dt);
		}
	}

	if (mBall.isValid()) {

		mBall->frameMove(time);
	}

	if (mActiveCam.isValid()) {

		//DIRTY HACK!!!!
		//P.S The dirty hack also continues inside the camera.clean()
		//where only the local position is tracked!
		if (mActiveCam->getTrackedNode() != NULL) {

			mActiveCam->frameMove(time.getTime(), time.getFrameMoveTime());
			//mActiveCam->clean();
		}

		match.getSoundManager().setListenerTransform(mActiveCam->mTransformWorld);
	} 

	/*
	static bool isFirstTime = true;

	if (isFirstTime) {

		isFirstTime = false;

		mFlagTest = true;
		mTestLimit = -1;
	}

	setDebugBypass(&getStaticDebugBypass());
	DebugBypass* pBypass = getDebugBypass();
	pBypass->currTri = 0;

	if (mFlagTest) {

		mFlagTest = false;
		++mTestLimit;

		mTestLimit = 22;
		pBypass->currTri = 119;


		SceneEntityMesh* pEntMesh;
		SceneNodeIndex::Type i = mEntMeshes[0].mNodeMap.iterator();
		
		int index = 0;


		while(mEntMeshes[0].mNodeMap.next(i, pEntMesh)) {

			//if (index == 7) 
			{

				WorldPolygonalCollider*& pCollider = mKDTreeColliders.addOne();

				WE_MMemNew(pCollider, WorldPolygonalCollider(NULL));
				pCollider->createFromMesh(dref(pEntMesh->mMeshInst.getMesh()), match.getWorldMaterialManager(), &pEntMesh->mTransformWorld, true);
				
				mKDTreeBuilder.add(*pCollider);
			}

			++index;
		}

		mKDTreeBuilder.process(match.mSubdivSettings);
		mKDTree.destroy();
		mKDTreeBuilder.build(mKDTree);

		mKDTreeFootballer = mTeams[0].getFootballerAt(0);

		if (mKDTreeFootballer.isValid()) {

			//mKDTreeFootballerVolume.volume().init(mKDTreeFootballer->getTestVolume());
			//mKDTree.add(mKDTreeFootballerVolume);
		}
	}

	if (mKDTreeFootballer.isValid()) {

		//mKDTreeFootballerVolume.volume().init(mKDTreeFootballer->getTestVolume());
		//mKDTree.update(mKDTreeFootballerVolume);
	}
	*/
}

void EntityManager::cleanScene() {

	mSceneRoot.rootNodeTreeClean();
}

void EntityManager::render(Match& match, Renderer& renderer, bool flagRenderExtras, RenderPassEnum pass) {

	mActiveCam->setMatrices(renderer, true, pass);

	{
		SceneNodeIndex::Type i = mEntMeshes[0].mNodeMap.iterator();
		SceneEntityMesh* pNode;

		while(mEntMeshes[0].mNodeMap.next(i, pNode)) {

			pNode->render(renderer, true, flagRenderExtras, pass);
		}
	}

	mTeams[0].render(renderer, true, flagRenderExtras, pass);
	mTeams[1].render(renderer, true, flagRenderExtras, pass);

	if (mBall.isValid()) {

		mBall->render(renderer, true, flagRenderExtras, pass);
	}

	if (mSimulBall.isValid()) {

		mSimulBall->render(renderer, true, flagRenderExtras, pass);
	}

	{
		SceneNodeIndex::Type i = mEntMeshes[1].mNodeMap.iterator();
		SceneEntityMesh* pNode;

		while(mEntMeshes[1].mNodeMap.next(i, pNode)) {

			pNode->render(renderer, true, flagRenderExtras, pass);
		}
	}

	{
		SceneNodeIndex::Type i = mScriptedCharacters.mNodeMap.iterator();
		SceneScriptedCharacter* pNode;

		while(mScriptedCharacters.mNodeMap.next(i, pNode)) {

			pNode->render(renderer, true, flagRenderExtras, pass);
		}
	}

	

	/*
	if (pass == RP_Normal) {

		//mKDTreeBuilder.render(renderer, true, &RenderColor::kWhite, false, NULL, false, &RenderColor::kYellow);
		mKDTree.render(renderer, false, &RenderColor::kWhite, false, NULL, true, &RenderColor::kYellow);
	}
	*/
}

void EntityManager::putNode(SceneNode* pNode) {

	if (mAllNodes.putNode(pNode->nodeName(), pNode) == false) {

		++mSceneLoadErrCount;
		log(L"node duplicate [%s]", pNode->nodeName());
		assrt(false);
	}
}

SceneNode* EntityManager::createEntNode(BufferString& tempStr, Match& match, DataChunk& chunk) {

	SoftPtr<SceneNode> ret;
	bool initSuccess = true;

	MMemNew(ret.ptrRef(), SceneNode());
	
	chunk.getValue(tempStr);

	initSuccess = initSuccess &&  ret->nodeInit(match, Node_Node, genNodeId(), tempStr.c_tstr());

	if (initSuccess) {

		putNode(ret.ptr());
		
	} else {

		ret.destroy();
		assrt(false);
	}

	return ret.ptr();
}

SceneCamera* EntityManager::createCamera(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv) {

	SoftPtr<SceneCamera> ret;

	MMemNew(ret.ptrRef(), SceneCamera());

	if (ret->init_create(match, tempStr, chunk, pConv)) {

		putNode(ret.ptr());

	} else {

		ret.destroy();
		assrt(false);
	}

	return ret.ptr();
}

SceneEntityMesh* EntityManager::createEntMesh(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType) {

	SoftPtr<SceneEntityMesh> ret;
	
	MMemNew(ret.ptrRef(), SceneEntityMesh());
	
	if (ret->init_create(match, tempStr, chunk, pConv, objType)) {

		putNode(ret.ptr());

	} else {

		ret.destroy();
		assrt(false);
	}
		
	return ret.ptr();
}

SceneScriptedCharacter* EntityManager::createEntMeshScripted(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType) {

	SoftPtr<SceneScriptedCharacter> ret;
	
	MMemNew(ret.ptrRef(), SceneScriptedCharacter());
	
	if (ret->init_create(match, tempStr, chunk, pConv, objType)) {

		putNode(ret.ptr());

	} else {

		ret.destroy();
		assrt(false);
	}
		
	return ret.ptr();
}


Ball* EntityManager::createBall(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv) {

	SoftPtr<Ball> ret;
	
	MMemNew(ret.ptrRef(), Ball());
	
	if (ret->init_create(match, tempStr, chunk, pConv)) {

		putNode(ret.ptr());

	} else {

		ret.destroy();
		assrt(false);
	}

	return ret.ptr();
}

SceneNode* EntityManager::createLight(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv) {

	RenderLight light;

	light.diffuse.set(0.7f);
	light.ambient.set(0.0f);
	light.specular.set(0.55f);

	light.direction.set(-0.3f, -1.0f, 0.3f);
	
	light.type = RL_Directional;

	if (chunk.getAttribute(L"lightType", tempStr)) {

		if (tempStr.equals(L"point")) {

			light.type = RL_Point;
		}
	}

	MathDataLoadHelper::extract(tempStr, chunk, light.diffuse, true, true, true, L"diffuse");
	MathDataLoadHelper::extract(tempStr, chunk, light.ambient, true, true, true, L"ambient");
	MathDataLoadHelper::extract(tempStr, chunk, light.specular, true, true, true, L"specular");
	MathDataLoadHelper::extract(tempStr, chunk, light.direction, true, true, true, L"direction");
	MathDataLoadHelper::extract(tempStr, chunk, light.position, true, true, true, L"position");

	if (pConv) 
		pConv->toTargetVector(light.direction.el);

	light.direction.normalize();

	if (pConv) 
		pConv->toTargetPoint(light.position.el);

	Vector3 lookAt;

	if (MathDataLoadHelper::extract(tempStr, chunk, lookAt, true, true, true, L"lookAt")) {

		if (pConv) 
			pConv->toTargetPoint(lookAt.el);

		lookAt.subtract(light.position, light.direction);
		light.direction.normalize();
	}

	light.range = FLT_MAX;
	chunk.scanAttribute(tempStr, L"range", L"%f", &light.range);

	if (pConv && light.range != FLT_MAX) 
		pConv->toTargetUnits(light.range);

	light.attenuation = 1.0f;
	chunk.scanAttribute(tempStr, L"attenuation", L"%f", &light.attenuation);

	if (pConv) 
		pConv->fromTargetUnits(light.attenuation);

	int lightIndex = 0;
	chunk.scanAttribute(tempStr, L"lightIndex", L"%d", &lightIndex);

	match.setLight(&light, lightIndex);

	return NULL;
}

Footballer* EntityManager::createFootballer(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv) {

	SoftPtr<Footballer> ret;
	
	MMemNew(ret.ptrRef(), Footballer());

	if (ret->init_create(match, tempStr, chunk, pConv, Node_Footballer)) {

		putNode(ret.ptr());

	} else {

		ret.destroy();
		assrt(false);
	}

	return ret.ptr();
}

void EntityManager::signalNodeAttached(BufferString& tempStr, Match& match, DataChunk& chunk, SceneNode* pNode, CoordSysConv* pConv) {

	switch (pNode->objectType) {
		case Node_Node:
			mNodes.putNode(pNode->nodeName(), pNode);
			break;
		case Node_Camera:
			{
				SceneCamera* pCam = SceneCamera::camFromNode(pNode);

				pCam->init_nodeAttached(tempStr, chunk, pConv);
				mCameras.putNode(pCam->nodeName(), pCam);

				if (mCameras.mNodeMap.getCount() == 1) {

					SceneNodeIndex idx = 0;

					setActiveCamera(idx);
				}
			}
			break;
		case Node_GoalProxy:
		case Node_Goal:
		case Node_Pitch:
		case Node_EntMesh:
		case Node_EntMeshNoScriptedCharacterColl:
			{
				SceneEntityMesh* pEnt = SceneEntityMesh::entMeshFromNode(pNode);
				
				pEnt->init_nodeAttached(tempStr, chunk, pConv);

				int order = 0;

				chunk.scanAttribute(tempStr, L"renderOrder", L"%d", &order);

				if (order == -1)
					order = order;

				order = tcap(order, -1, 0);

				mEntMeshes[order + 1].putNode(pEnt->nodeName(), pEnt);
			}
			break;
		case Node_ScriptedCharacter:
			{
				SceneScriptedCharacter* pEnt = SceneScriptedCharacter::entMeshScriptedFromNode(pNode);
				
				pEnt->init_nodeAttached(tempStr, chunk, pConv);

				mScriptedCharacters.putNode(pEnt->nodeName(), pEnt);
				//mEntMeshes[0].putNode(pEnt->nodeName(), pEnt);
			}
			break;
		case Node_Ball:
			{
				Ball* pBall = Ball::ballFromNode(pNode);

				pBall->init_nodeAttached(tempStr, chunk, pConv);

				if (mBall.isNull()) {

					mBall = pBall;
				}
			}
			break;
		case Node_Footballer:
			{
				Footballer* pFootballer = Footballer::footballerFromNode(pNode);

				pFootballer->init_nodeAttached(tempStr, chunk, pConv);

				mTeams[pFootballer->mTeam].addFootballer(pFootballer);
			}
			break;	
	}

}

SceneNode* EntityManager::createNode(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv, bool& success) {

	DataSourceTranslationContext transCtx;
	transCtx.start(&chunk, true);

	{
		if (!chunk.getAttribute(L"name", tempStr)) {

			chunk.getValue(tempStr);
		}

		SoftPtr<SceneNode> node = getNode(tempStr.c_tstr());

		if (node.isValid()) {

			++mSceneLoadErrCount;
			log(L"node duplicate [%s]", tempStr.c_tstr());

			assrt(false);
			return NULL;
		}
	}

	SceneNodeType nodeType = Node_None;
	bool catchPitch = false;
	bool isActiveCOC = false;

	if (chunk.getAttribute(L"gameType", tempStr) == false) {
	
		if (chunk.getAttribute(L"type", tempStr)) {

			if (tempStr.equals(L"mesh")) {
				
				nodeType = Node_EntMesh;

			} else if (tempStr.equals(L"scriptedCharacter")) {
				
				nodeType = Node_ScriptedCharacter;

			} else if (tempStr.equals(L"camera")) {
				
				nodeType = Node_Camera;

			} else if (tempStr.equals(L"light")) {

				nodeType = Node_Light;
			} 

		} else {

			nodeType = Node_Node;
		}

	} else {

		if (tempStr.equals(L"footballer")) {
	
			nodeType = Node_Footballer;

		} else if (tempStr.equals(L"ball")) {

			nodeType = Node_Ball;

		} else if (tempStr.equals(L"goal")) {

			nodeType = Node_Goal;
			//nodeType = Node_EntMesh;

		} else if (tempStr.equals(L"goalProxy")) {

			nodeType = Node_GoalProxy;
			//nodeType = Node_EntMesh;

		} else if (tempStr.equals(L"pitch")) {

			catchPitch = true;

			nodeType = Node_Pitch;
			//nodeType = Node_EntMesh;

		} else if (tempStr.equals(L"noScriptedCharacterColl")) {

			nodeType = Node_EntMeshNoScriptedCharacterColl;
			//nodeType = Node_EntMesh;

		} 
	}

	
	SoftPtr<SceneNode> ret;
	SoftPtr<SceneEntityMesh> retEntMesh;
	
	switch (nodeType) {
	case Node_Node:
		ret = createEntNode(tempStr, match, chunk);
		break;
	case Node_Camera:
		ret = createCamera(tempStr, match, chunk, pConv);
		break;	
	case Node_EntMesh:
	case Node_EntMeshNoScriptedCharacterColl:
		ret = createEntMesh(tempStr, match, chunk, pConv, nodeType);
		break;	
	case Node_ScriptedCharacter:
		ret = createEntMeshScripted(tempStr, match, chunk, pConv, Node_ScriptedCharacter);
		break;	
	case Node_Footballer:
		{

			SoftRef<DataChunk> gameChunk = chunk.getChild(L"game");

			if (gameChunk.isValid()) {

				unsigned int teamNumber = 0;

				if (gameChunk->scanAttribute(tempStr, L"team", L"%u", &teamNumber)) {

					Team& team = match.getTeam((TeamEnum) teamNumber);

					if (team.getActiveFootballerStartIndexLeft() != 0) {

						team.decreaseActiveFootballerStartIndexLeft();
						success = true;

					} else {

						if (team.getActiveFootballerCountLeft() > 0) {

							ret = createFootballer(tempStr, match, chunk, pConv);

							if (ret.isValid())
								team.decreaseActiveFootballerCountLeft();

						} else {

							success = true;
						}
					}
				} 
			}
		}
		break;	
	case Node_Ball:
		ret = createBall(tempStr, match, chunk, pConv);
		break;	
	case Node_Goal:
		return createEntMesh(tempStr, match, chunk, pConv, Node_Goal);
		break;		
	case Node_GoalProxy:
		return createEntMesh(tempStr, match, chunk, pConv, Node_GoalProxy);
		break;		
	case Node_Pitch: 
		catchPitch = true;
		retEntMesh = createEntMesh(tempStr, match, chunk, pConv, Node_Pitch);
		ret = retEntMesh;
		break;		
	case Node_Light: {
		
			success = true;
			return createLight(tempStr, match, chunk, pConv);

		} break;		
	}
	
	if (catchPitch) {

		if (mPitch.isValid()) {

			log(L"Duplicate Pitches detected!");
			assrt(false);

			success = false;
		}

		mPitch = retEntMesh;
	}
	
	return ret.ptr();
}


/*
bool EntityManager::getGameType(BufferString& tempStr, Scene& scene, DataChunk& chunk, ObjectType& baseType, ObjectType& gameType, CollMask& collMask) {
	
	if (chunk.getAttribute(L"gameType", tempStr) == false) {
	
		CollEngine::initCollMaskWorld(collMask);

		return false;
	}

	if (tempStr.equals(L"ball")) {

		if (mBall.isValid()) {

			return false;
		} 

		baseType = SNT_EntMesh;
		gameType = Node_Ball;
	
		CollEngine::initCollMaskBall(collMask);

		return true;

	} else if (tempStr.equals(L"footballer")) {

		baseType = SNT_EntMesh;
		gameType = Node_Footballer;

		CollEngine::initCollMaskFootballer(collMask);

		return true;

	} else if (tempStr.equals(L"goal")) {

		baseType = SNT_EntMesh;
		gameType = Node_Goal;

		CollEngine::initCollMaskGoal(collMask);

		return true;

	} else if (tempStr.equals(L"pitch")) {

		baseType = SNT_EntMesh;
		gameType = Node_Pitch;

		CollEngine::initCollMaskPitch(collMask);

		return true;
	}

	assrt(false);
	return false;
}


SceneNode* EntityManager::create(BufferString& tempStr, Scene& scene, DataChunk& chunk, const ObjectType& baseType, const ObjectType& gameType) {

	SceneNode* pRet = NULL;

	switch(gameType) {
		
		case Node_Ball:
			{
				if (mBall.isNull()) {

					MMemNew(mBall.ptrRef(), Ball());
					pRet = mBall.ptr();
				} 
			}
			break;
		case Node_Footballer:
			{
				MMemNew(pRet, Footballer());
			}
			break;
		case Node_Goal:
			{
				MMemNew(pRet, SceneEntityMesh());
			}
			break;
		case Node_Pitch:
			{
				MMemNew(pRet, SceneEntityMesh());
			}
			break;
		default:
			break;
	}

	return pRet;
}


bool EntityManager::init(BufferString& tempStr, Scene& scene, DataChunk& chunk, SceneNode* pNode) {

	
	switch (pNode->objectType) {
		case Node_Ball:
			{
				Ball* pBall = (Ball*) pNode;
				if (pBall != mBall.ptr()) {

					return false;
				}

				if (mBall->bindNode(m.dref()) == false) {

					mBall.destroy();
					return false;
				}
		
				return true;
			}
			break;
		case Node_Footballer:
			{

				Footballer* pFootballer = (Footballer*) pNode;

				SoftRef<DataChunk> gameChunk = chunk.getChild(L"game");
				if (gameChunk.isNull()) {

					return false;
				}

				if (gameChunk->getAttribute(L"team", tempStr) == false) {

					return false;
				}

				TeamEnum team;
				FootballerNumber number;

				if (tempStr.equals(L"1")) {
					
					team = Team_A;

				} else {

					team = Team_B;
				}

				assrt(sizeof(FootballerNumber) == sizeof(int));
				if (gameChunk->scanAttribute(tempStr, L"number", L"%d", &number) == false) {

					return false;
				}

				if (mTeams[team].hasFootballer(number)) {

					return false;
				}

				if (pFootballer->bindNode(m.dref(), team, number, mSkelMeshProps) == false) {

					return false;
				}

				mTeams[team].addFootballer(pFootballer);
				return true;
			}
			break;
		case Node_Goal:
			{
				return true;
			}
			break;
		case Node_Pitch:
			{
				return true;
			}
			break;
	}

	assrt(false);
	return false;
}


void EntityManager::destroy(BufferString& tempStr, Scene& scene, DataChunk& chunk, SceneNode* pNode) {

	switch (pNode->objectType) {
		case Node_Ball:
			{
				if (mBall.ptr() == (Ball*) pNode) {

					mBall.destroy();

				} else {
					
					MMemDelete(pNode);
				}
			}
			break;
		case Node_Footballer:
			{
				Footballer* pFootballer = (Footballer*) pNode;

				if (pFootballer->isInited()) {
					mTeams[pFootballer->mTeam].removeFootballer(pFootballer->mNumber);
				}
				MMemDelete(pFootballer);
			}
			break;
		default:
			{
				MMemDelete(pNode);
			}
			break;
	}
}
*/

} }

