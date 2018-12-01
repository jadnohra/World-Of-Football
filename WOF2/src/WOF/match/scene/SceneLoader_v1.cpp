#include "SceneLoader_v1.h"

#include "WE3/data/WEDataSourcePool.h"
#include "WE3/coordSys/WECoordSysConvPool.h"
#include "WE3/helper/load/WECoordSysDataLoadHelper.h"
#include "WE3/helper/load/WEMathDataLoadHelper.h"
#include "WE3/helper/load/WESceneDataLoadHelper.h"
#include "WE3/scene/WESceneBaseLoader_v1.h"
using namespace WE;

#include "../Match.h"

namespace WOF { namespace match {

bool scene_loadTeamScenes(BufferString& tempStr, Match& match, DataChunk& _loopChunk, 
							SceneNodeFactory& nodeFactory, CoordSysConv* pConv, int& errCount) {

	bool success = true;

	for (int i = 0; i < 2; ++i) {

		success = success && match.getTeam((TeamEnum) i).loadScene(match, tempStr, pConv, nodeFactory, errCount);
	}

	return success;
}

bool scene_loadEntity(BufferString& tempStr, Match& match, DataChunk& entChunk, 
					  SceneNodeFactory& nodeFactory, CoordSysConv* pConv, int& errCount) {

	bool loaded = false;

	SoftPtr<SceneNode> node = nodeFactory.createNode(tempStr, match, entChunk, pConv, loaded);

	return node.isValid() || loaded;
}

bool scene_loadEntities(BufferString& tempStr, Match& match, DataChunk& _loopChunk, 
						SceneNodeFactory& nodeFactory, CoordSysConv* pConv, int& errCount) {

	LoadContext& loadContext = match.getLoadContext();
	SoftRef<DataSource> pathResolver = _loopChunk.getSource();
	SoftRef<DataChunk> loopChunk =  loadContext.dataSourcePool().resolveRemoteChunk(_loopChunk, true, pathResolver);

	if (loopChunk.isValid()) {

		SoftRef<DataChunk> child = loopChunk->getFirstChild();

		while (child.isValid()) {

			if (!scene_loadEntity(tempStr, match, child.dref(), nodeFactory, pConv, errCount)) {

				assrt(false);
				return false;
			}

			toNextSibling(child);
		}

	} else {

		return false;
	}

	return true;
}

bool SceneLoader_v1::loadEntities(BufferString& tempStr, Match& match, DataChunk& chunk, 
											SceneNodeFactory& nodeFactory, CoordSysConv* pConv, int& errCount) {

	return scene_loadEntities(tempStr, match, chunk, nodeFactory, pConv, errCount);
}


SceneNode* scene_loadNode(BufferString& tempStr, DataChunk& nodeChunk, SceneNode& parent, Match& match, SceneNodeFactory& nodeFactory, CoordSysConv* pConv, int& errCount, bool& doAbortLoad) {

	doAbortLoad = true;

	bool autoCreate = false;
	SoftPtr<SceneNode> node;
	
	
	if (nodeChunk.getAttribute(L"entity", tempStr) == false) {

		bool loaded;

		node = nodeFactory.createNode(tempStr, match, nodeChunk, pConv, loaded);

	} else {

		node = nodeFactory.getNode(tempStr.c_tstr());
	}

	
	if (node.isNull()) {

		bool isOptional = false;
		nodeChunk.scanAttribute(tempStr, L"optional", isOptional);

		if (isOptional) {

			doAbortLoad = false;

			nodeChunk.getValue(tempStr);
			log(L"creating optional node failed [%s]", tempStr.c_tstr());

		} else {

			nodeChunk.getValue(tempStr);
			log(L"creating node failed [%s]", tempStr.c_tstr());
			assrt(false);
			++errCount;
		}
		
		return NULL;
	}

	if (node->pParent != NULL) {

		++errCount;
		log(L"node reattached [%s]", node->nodeName());
		assrt(false);
		return NULL;
	}

	SceneTransformation sceneTrans;
	
	if (SceneDataLoadHelper::extract(tempStr, nodeChunk, sceneTrans) == false) {

		++errCount;
		log(L"positioning node error [%s]", node->nodeName());
		assrt(false);
	}
	sceneTrans.validate();

	if (pConv) {
	
		sceneTrans.applyConverter(*pConv);
	}

	//node->nodeMarkDirty();
	assrt(node->nodeIsDirty());
	node->mTransformLocal.setTransformation(sceneTrans, match.getBaseCoordSys().rotationLeftHanded);

	if (!node->loadTransform(tempStr, match, &nodeChunk, pConv)) {

		++errCount;
		log(L"transform loading node error [%s]", node->nodeName());
		assrt(false);
	}

	parent.addChild(node.ptr());

	nodeFactory.signalNodeAttached(tempStr, match, nodeChunk, node.ptr(), pConv);

	//bool isNewNode;	

	/*
	if (nodeChunk.getAttribute(L"entity", tempStr)) {

		node = match.mNodes->getNode(tempStr);
	} 

	if (node.isNull()) {

		nodeChunk.getValue(tempStr);

		isNewNode = true;

		if (tempStr.equals(L"_ghost")) {

			MMemNew(node.ptrRef(), SceneNode());

			//if fails node name stays from getValue
			nodeChunk.getAttribute(L"name", tempStr);
			node->nodeInit(match, SNT_NodeGhost, match.genNodeId(), tempStr.c_tstr());

		} else {
		
			MMemNew(node.ptrRef(), SceneNodeLocation());
			
			
			//if fails node name stays from getValue
			nodeChunk.getAttribute(L"name", tempStr);
			node->nodeInit(match, SNT_NodeLocation, match.genNodeId(), tempStr.c_tstr());
		}

	} else {

		isNewNode = false;
	}
	
	SceneTransformation sceneTrans;
	
	if (SceneDataLoadHelper::extract(tempStr, nodeChunk, sceneTrans) == false) {

		assrt(false);
	}
	sceneTrans.validate();

	if (pConv) {
	
		sceneTrans.applyConverter(*pConv);
	}

	//node->nodeMarkDirty();
	assrt(node->nodeIsDirty());
	node->mTransformLocal.setTransformation(sceneTrans, match.mCoordSys.rotationLeftHanded);

	if (isNewNode) {
		
		match.mNodes->putNode(node.ptr());
	}
	*/
	
	return node.ptr();
}

bool scene_recurseLoadNode(BufferString& tempStr, DataChunk& nodeChunk, SceneNode& parent, Match& match, SceneNodeFactory& nodeFactory, CoordSysConv* pConv, int& errCount) {

	bool doAbortLoad = true;

	SoftPtr<SceneNode> thisNode = scene_loadNode(tempStr, nodeChunk, parent, match, nodeFactory, pConv, errCount, doAbortLoad);
	
	if (thisNode.isNull()) {

		return doAbortLoad ? false : true;
	}

	SoftRef<DataChunk> child = nodeChunk.getFirstChild();

	while (child.isValid()) {
		
		if (!scene_recurseLoadNode(tempStr, child, thisNode.dref(), match, nodeFactory, pConv, errCount)) {

			assrt(false);
			return false;
		}

		toNextSibling(child);
	}

	return true;
}

bool scene_loadTransformTree(BufferString& tempStr, Match& match, SceneNodeFactory& nodeFactory, SceneNode& rootNode, DataChunk& treeChunk, CoordSysConv* pConv, int& errCount) {

	SoftRef<DataChunk> child = treeChunk.getFirstChild();

	while (child.isValid()) {

		scene_recurseLoadNode(tempStr, child, rootNode, match, nodeFactory, pConv, errCount);

		toNextSibling(child);
	}

	return true;
}

bool scene_loadPhysics_MaterialPhysics(BufferString& tempStr, DataChunk& matChunk, PhysMaterial* pPhys, const CoordSys& destCoordSys, int& errCount) {

	if (matChunk.scanAttribute(tempStr, L"bouncy", L"%f", &pPhys->nRestCoeff) != 1) {

		pPhys->nRestCoeff = 0.0f;
	}

	if (matChunk.scanAttribute(tempStr, L"rough", L"%f", &pPhys->frictionCoeff) != 1) {

		pPhys->frictionCoeff = 0.0f;
	}

	pPhys->hasFriction = (pPhys->frictionCoeff != 0.0f);

	if (matChunk.scanAttribute(tempStr, L"movePenalty", L"%f", &pPhys->movePenalty) != 1) {

		pPhys->movePenalty = 0.0f;
	}

	if (matChunk.scanAttribute(tempStr, L"moveDrag", L"%f", &pPhys->moveDrag) != 1) {

		pPhys->moveDrag = 0.0f;
	}

	if (matChunk.scanAttribute(tempStr, L"pureRotFrictionCoeff", L"%f", &pPhys->pureRotFrictionCoeff) != 1) {

		pPhys->pureRotFrictionCoeff = 1.3f;
	}

	if (matChunk.scanAttribute(tempStr, L"flexible", L"%f", &pPhys->flexible) != 1) {

		pPhys->flexible = 0.0f;
	}

	pPhys->isFlexible = (pPhys->flexible != 0.0f);

	pPhys->convertUnits(destCoordSys.getFloatUnitsPerMeter());

	return true;
}

bool scene_loadPhysics_Material(BufferString& tempStr, Match& match, DataChunk& matChunk, const CoordSys& destCoordSys, int& errCount) {

	{
		if (matChunk.getAttribute(L"name", tempStr) == false) {

			++errCount;
			log(L"material name required");
			assrt(false);
			return false;
		}

		WorldMaterialManager& manager = match.getWorldMaterialManager();
		WorldMaterialID collMat;

		if (manager.putMaterial(tempStr.hash(), collMat) == false) {

			++errCount;
			log(L"material duplicate [%s]", tempStr.c_tstr());
			assrt(false);
			return false;
		}

		WorldMaterial* pMat = manager.getMaterial(collMat);

#ifdef _DEBUG
		WE_MMemNew(pMat->name.ptrRef(), String(tempStr));		
#endif

		bool isTriggerOnly = false;

		matChunk.scanAttribute(tempStr, L"isTriggerOnly", isTriggerOnly);

		if (isTriggerOnly) {

			pMat->physProperties.destroy();

		} else {

			if (!pMat->physProperties.isValid()) {

				WE_MMemNew(pMat->physProperties.ptrRef(), PhysMaterial());
			}

			scene_loadPhysics_MaterialPhysics(tempStr, matChunk, pMat->physProperties, destCoordSys, errCount);
		}

		if (matChunk.getAttribute(L"sound", tempStr)) {

			SoftRef<AudioBuffer> buff = match.getSoundManager().getAudioBuffers().getBuffer(tempStr.c_tstr());
			pMat->sound = buff.ptr();
		}

		if (matChunk.getAttribute(L"triggerType", tempStr)) {

			if (tempStr.equals(L"GameTrigger")) {

				tempStr.destroy();
				matChunk.getAttribute(L"triggerValue", tempStr);

				pMat->trigger = match.createTrigger(tempStr.c_tstr());

				if (!pMat->trigger.isValid()) {

					++errCount;
					log(L"Invalid TriggerValue [%s]", tempStr.c_tstr());
					assrt(false);
					return false;
				}

			} else {

				++errCount;
				log(L"Invalid TriggerType [%s]", tempStr.c_tstr());
				assrt(false);
				return false;
			}
		}
	}

	return true;
}

bool scene_loadPhysics_CombinedMaterial(BufferString& tempStr, Match& match, DataChunk& matChunk, const CoordSys& destCoordSys, int& errCount) {

	{
		WorldMaterialManager& manager = match.getWorldMaterialManager();
		WorldMaterialID collMat1;
		WorldMaterialID collMat2;

		if (matChunk.getAttribute(L"name1", tempStr) == false) {

			++errCount;
			log(L"combinedMaterial name1 required");
			assrt(false);
			return false;
		}

		if (manager.getMaterialID(tempStr.hash(), collMat1) == false) {

			++errCount;
			log(L"combinedMaterial not found [%s]", tempStr.c_tstr());
			assrt(false);
			return false;
		}

		if (matChunk.getAttribute(L"name2", tempStr) == false) {

			++errCount;
			log(L"combinedMaterial name2 required");
			assrt(false);
			return false;
		}

		if (manager.getMaterialID(tempStr.hash(), collMat2) == false) {

			++errCount;
			log(L"combinedMaterial not found [%s]", tempStr.c_tstr());
			assrt(false);
			return false;
		}
		

		WorldMaterialCombo* pMat = manager.getCombinedMaterial(collMat1, collMat2);
				
		if (pMat->physProperties.isValid()) {

			scene_loadPhysics_MaterialPhysics(tempStr, matChunk, pMat->physProperties, destCoordSys, errCount);
		}

		if (matChunk.getAttribute(L"sound", tempStr)) {

			SoftRef<AudioBuffer> buff = match.getSoundManager().getAudioBuffers().getBuffer(tempStr.c_tstr());
			pMat->setCombinedSound(buff.ptr());
		}

	}

	return true;
}

bool scene_loadPhysics_Materials(BufferString& tempStr, Match& match, DataChunk& loopChunk, const CoordSys& destCoordSys, int& errCount) {

	SoftRef<DataChunk> child = loopChunk.getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"material")) {

			scene_loadPhysics_Material(tempStr, match, child.dref(), destCoordSys, errCount);

		} else if (child->equals(L"combinedMaterial")) {

			scene_loadPhysics_CombinedMaterial(tempStr, match, child.dref(), destCoordSys, errCount);
		} 

		toNextSibling(child);
	}

	return true;
}

bool scene_loadPhysics(BufferString& tempStr, Match& match, DataChunk& physChunk, const CoordSys& destCoordSys, int& errCount) {

	SoftRef<DataChunk> child = physChunk.getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"materials")) {

			scene_loadPhysics_Materials(tempStr, match, child.dref(), destCoordSys, errCount);

		} 

		toNextSibling(child);
	}

	return true;
}

bool scene_loadShadowing(BufferString& tempStr, Match& match, DataChunk& chunk, const CoordSys& destCoordSys, int& errCount) {

	Renderer& renderer = match.getRenderer();

	bool enable = false;

	if (chunk.scanAttribute(tempStr, L"enable", enable)) {

		renderer.setRenderState(RS_EnableShadowing, enable);
	}

	match.setEnabledShadowing(enable);

	RenderColor shadowColor(0, 0);
	bool alphaBlend = true;
	bool stencilTest = true;
	
	chunk.scanAttribute(tempStr, L"alphaBlend", alphaBlend);
	chunk.scanAttribute(tempStr, L"stencilTest", stencilTest);
	MathDataLoadHelper::extract(tempStr, chunk, shadowColor, true, true, true, L"color");
	//chunk.scanAttribute(tempStr, L"alpha", L"%f", &shadowColor.alpha);

	renderer.setupShadowingMaterial(shadowColor, alphaBlend, stencilTest);

	return true;
}

bool SceneLoader_v1::loadTracks(BufferString& tempStr, SoundManager& soundMan, LoadContext& loadContext, DataChunk& _loopChunk, int &errCount) {

	SoftRef<DataSource> pathResolver = _loopChunk.getSource();
	SoftRef<DataChunk> loopChunk =  loadContext.dataSourcePool().resolveRemoteChunk(_loopChunk, true, pathResolver);

	if (loopChunk.isValid()) {

		SoftRef<DataChunk> child = loopChunk->getFirstChild();
		BufferString path;

		while (child.isValid()) {

			if (child->getAttribute(L"remote", path)) {
				
				float volume = 1.0f;

				child->scanAttribute(tempStr, L"volume", L"%f", &volume);

				child->resolveFilePath(path);
				child->getValue(tempStr);
				
				soundMan.addTrack(tempStr.c_tstr(), path.c_tstr(), volume);
			}

			toNextSibling(child);
		}
	}

	return true;
}

bool SceneLoader_v1::load(Match& match, SceneNode& rootNode, DataChunk& startChunk, SceneNodeFactory& nodeFactory, SceneLoaderClient* pClient) {

	int errCount = 0;

	LoadContext& loadContext = match.getLoadContext();
	BufferString tempStr;

	SoftRef<DataChunk> sceneChunk = loadContext.dataSourcePool().resolveRemoteChunk(startChunk, true, NULL);

	if (sceneChunk.isNull()) {

		return true;
	}

	CoordSys fileSceneCoordSys;
	SoftRef<CoordSysConv> fileToSceneConv;
	const CoordSys& loadDestCoordSys = match.getBaseCoordSys();

	if (SceneBaseLoader_v1::loadCoordSys(tempStr, loadContext, loadDestCoordSys, sceneChunk, 
											fileSceneCoordSys, fileToSceneConv, errCount) == false) {

		assrt(false);
		return false;
	}

	
	SoftRef<DataChunk> child = sceneChunk->getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"physics")) {

			scene_loadPhysics(tempStr, match, child.dref(), loadDestCoordSys, errCount);

		}else if (child->equals(L"shadowing")) {

			scene_loadShadowing(tempStr, match, child.dref(), loadDestCoordSys, errCount);

		} else if (child->equals(L"debug")) {

			SceneBaseLoader_v1::loadDebug(tempStr, child.dref(), errCount);

		} else if (child->equals(L"meshes")) {

			SceneBaseLoader_v1::loadMeshes(tempStr, match.getMeshes(), loadContext, child.dref(), loadDestCoordSys, fileToSceneConv.ptr(), errCount);

		} else if (child->equals(L"sounds")) {

			if (match.getSoundManager().isEnabled()) {

				SceneBaseLoader_v1::loadSounds(tempStr, match.getSoundManager().getAudioBuffers(), loadContext, child.dref(), fileToSceneConv.ptr(), errCount);
			}

		} else if (child->equals(L"tracks")) {

			loadTracks(tempStr, match.getSoundManager(), loadContext, child.dref(), errCount);

		} else {

			if (pClient) {

				pClient->loadUnknownChunk(tempStr, match, child.dref(), loadDestCoordSys, fileToSceneConv.ptr());
			}
		}
		

		toNextSibling(child);
	}


	child = sceneChunk->getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"entities")) {

			if (!scene_loadEntities(tempStr, match, child.dref(), nodeFactory, fileToSceneConv.ptr(), errCount)) {

				assrt(false);
				return false;
			}

		} else if (child->equals(L"transformTree")) {

			scene_loadTransformTree(tempStr, match, nodeFactory, rootNode, child.dref(), fileToSceneConv.ptr(), errCount);

		} else if (child->equals(L"loadTeamScenes")) {

			if (!scene_loadTeamScenes(tempStr, match, child.dref(), nodeFactory, fileToSceneConv.ptr(), errCount)) {

				assrt(false);
				return false;
			}
		}

		toNextSibling(child);
	}
	
	
	return (errCount == 0);
}


} }