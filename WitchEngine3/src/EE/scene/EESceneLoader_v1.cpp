#include "EESceneLoader_v1.h"

#include "WE3/data/WEDataSourcePool.h"
#include "WE3/coordSys/WECoordSysConvPool.h"
#include "WE3/helper/load/WECoordSysDataLoadHelper.h"
#include "WE3/helper/load/WEMathDataLoadHelper.h"
#include "WE3/helper/load/WESceneDataLoadHelper.h"

#include "EESceneNodeType.h"
#include "EESceneNode.h"
#include "node/EESceneNodeLocation.h"

#include "WE3/scene/WESceneBaseLoader_v1.h"
using namespace WE;


namespace EE {

bool scene_loadEntity(BufferString& tempStr, Scene& scene, DataChunk& entChunk, 
					  ISceneNodeFactory& nodeFactory, ISceneGameNodeFactory* pGameFactory) {

	SoftPtr<SceneNode> entity = nodeFactory.create(tempStr, scene, entChunk, pGameFactory);

	if (entity.isNull()) {

		return false;
	}

	return scene.mNodes->putNode(entity.ptr());
}

void scene_loadCamera(BufferString& tempStr, Scene& scene, DataChunk& camChunk, SceneNodeCamera* pCam, CoordSysConv* pConv) {

	pCam->nodeInit(scene, SNT_NodeCamera, scene.genNodeId(), L"camera");

	if (camChunk.scanAttribute(tempStr, L"fov", L"%f", &pCam->mFOV) == false) {

		pCam->mFOV = 45.0f;
	}
	pCam->mFOV = degToRad(pCam->mFOV);

	bool rangeLoaded = false;

	if (camChunk.getAttribute(L"range", tempStr)) {

		if (_stscanf(tempStr.c_tstr(), L"%f, %f", &pCam->mRangeMin, &pCam->mRangeMax) == 2) {

			rangeLoaded = true;
		}
	}

	if (rangeLoaded == false) {

		pCam->mRangeMin = 0.01f;
		pCam->mRangeMax = 1000.0f;
	}

	if (camChunk.scanAttribute(tempStr, L"velocity", L"%f", &pCam->mVelocity) == false) {

		pCam->mVelocity = 1.0f;
	}

	if (camChunk.scanAttribute(tempStr, L"rotation", L"%f", &pCam->mRotation) == false) {

		pCam->mRotation = 1.0f;
	}

	MathDataLoadHelper::extract(tempStr, camChunk, pCam->mEye, true, true, false, L"eye");
	MathDataLoadHelper::extract(tempStr, camChunk, pCam->mLookAt, true, true, false, L"lookAt");

	if (pConv) {

		pConv->toTargetUnits(pCam->mRangeMin);
		pConv->toTargetUnits(pCam->mRangeMax);
		pConv->toTargetUnits(pCam->mVelocity);

		pConv->toTargetPoint(pCam->mEye.el);
		pConv->toTargetPoint(pCam->mLookAt.el);
	}
}

bool scene_loadEntities(BufferString& tempStr, Scene& scene, DataChunk& loopChunk, 
						ISceneNodeFactory& nodeFactory, ISceneGameNodeFactory* pGameFactory, CoordSysConv* pConv, SceneNodeCamera* pCam) {


	SoftRef<DataChunk> child = loopChunk.getFirstChild();

	while (child.isValid()) {

		if (pCam && child->getValue(tempStr) && tempStr.equals(L"camera")) {

			scene_loadCamera(tempStr, scene, child.dref(), pCam, pConv);

		} else {

			scene_loadEntity(tempStr, scene, child.dref(), nodeFactory, pGameFactory);
		}

		toNextSibling(child);
	}

	return true;
}



SceneNode* scene_loadNode(BufferString& tempStr, DataChunk& nodeChunk, SceneNode& parent, Scene& scene, CoordSysConv* pConv) {

	SoftPtr<SceneNode> node;
	bool isNewNode;	
	
	if (nodeChunk.getAttribute(L"entity", tempStr)) {

		node = scene.mNodes->getNode(tempStr);
	} 

	if (node.isNull()) {

		nodeChunk.getValue(tempStr);

		isNewNode = true;

		if (tempStr.equals(L"_ghost")) {

			MMemNew(node.ptrRef(), SceneNode());

			//if fails node name stays from getValue
			nodeChunk.getAttribute(L"name", tempStr);
			node->nodeInit(scene, SNT_NodeGhost, scene.genNodeId(), tempStr.c_tstr());

		} else {
		
			MMemNew(node.ptrRef(), SceneNodeLocation());
			
			
			//if fails node name stays from getValue
			nodeChunk.getAttribute(L"name", tempStr);
			node->nodeInit(scene, SNT_NodeLocation, scene.genNodeId(), tempStr.c_tstr());
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
	node->mTransformLocal.setTransformation(sceneTrans, scene.mCoordSys.rotationLeftHanded);

	if (isNewNode) {
		
		scene.mNodes->putNode(node.ptr());
	}
	
	return node.ptr();
}

bool scene_recurseLoadNode(BufferString& tempStr, DataChunk& nodeChunk, SceneNode& parent, Scene& scene, CoordSysConv* pConv) {

	SoftPtr<SceneNode> thisNode = scene_loadNode(tempStr, nodeChunk, parent, scene, pConv);
	
	if (thisNode.isNull()) {

		return false;
	}

	parent.addChild(thisNode.ptr());
	
	SoftRef<DataChunk> child = nodeChunk.getFirstChild();

	while (child.isValid()) {
		
		scene_recurseLoadNode(tempStr, child, thisNode.dref(), scene, pConv);

		toNextSibling(child);
	}

	return true;
}

bool scene_loadTransformTree(BufferString& tempStr, Scene& scene, SceneNode& rootNode, DataChunk& treeChunk, CoordSysConv* pConv) {

	SoftRef<DataChunk> child = treeChunk.getFirstChild();

	while (child.isValid()) {

		scene_recurseLoadNode(tempStr, child, rootNode, scene, pConv);

		toNextSibling(child);
	}

	return true;
}

bool scene_loadPhysics_MaterialPhysics(BufferString& tempStr, DataChunk& matChunk, PhysMaterial* pPhys, CoordSys& destCoordSys) {

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

	if (matChunk.scanAttribute(tempStr, L"flexible", L"%f", &pPhys->flexible) != 1) {

		pPhys->flexible = 0.0f;
	}

	pPhys->isFlexible = (pPhys->flexible != 0.0f);

	pPhys->convertUnits(destCoordSys.getFloatUnitsPerMeter());

	return true;
}

bool scene_loadPhysics_Material(BufferString& tempStr, Scene& scene, DataChunk& matChunk, CoordSys& destCoordSys) {

	if (matChunk.getAttribute(L"name", tempStr) == false) {

		assrt(false);
		return false;
	}

	CollMaterialManager& manager = scene.mCollMatManager.dref();
	CollMaterial collMat;

	if (manager.putCollMaterial(tempStr.hash(), collMat) == false) {

		assrt(false);
		return false;
	}

	PhysMaterial* pPhys = manager.getPhysics(collMat);
	scene_loadPhysics_MaterialPhysics(tempStr, matChunk, pPhys, destCoordSys);


	return true;
}

bool scene_loadPhysics_CombinedMaterial(BufferString& tempStr, Scene& scene, DataChunk& matChunk, CoordSys& destCoordSys) {

	CollMaterialManager& manager = scene.mCollMatManager.dref();
	CollMaterial collMat1;
	CollMaterial collMat2;

	if (matChunk.getAttribute(L"name1", tempStr) == false) {

		assrt(false);
		return false;
	}

	if (manager.getCollMaterial(tempStr.hash(), collMat1) == false) {

		assrt(false);
		return false;
	}

	if (matChunk.getAttribute(L"name2", tempStr) == false) {

		assrt(false);
		return false;
	}

	if (manager.getCollMaterial(tempStr.hash(), collMat2) == false) {

		assrt(false);
		return false;
	}
	

	PhysMaterial* pPhys = manager.getCombinedPhysics(collMat1, collMat2);
	scene_loadPhysics_MaterialPhysics(tempStr, matChunk, pPhys, destCoordSys);

	return true;
}

bool scene_loadPhysics_Materials(BufferString& tempStr, Scene& scene, DataChunk& loopChunk, CoordSys& destCoordSys) {

	if (scene.mCollMatManager.isNull()) {

		warn(L"scene has no Physics Material Manager");
		return false;
	}

	SoftRef<DataChunk> child = loopChunk.getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"material")) {

			scene_loadPhysics_Material(tempStr, scene, child.dref(), destCoordSys);

		} else if (child->equals(L"combinedMaterial")) {

			scene_loadPhysics_CombinedMaterial(tempStr, scene, child.dref(), destCoordSys);
		} 

		toNextSibling(child);
	}

	return true;
}

bool scene_loadPhysics(BufferString& tempStr, Scene& scene, DataChunk& physChunk, CoordSys& destCoordSys) {

	SoftRef<DataChunk> child = physChunk.getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"materials")) {

			scene_loadPhysics_Materials(tempStr, scene, child.dref(), destCoordSys);

		} 

		toNextSibling(child);
	}

	return true;
}

bool SceneLoader_v1::load(Scene& scene, SceneNode& rootNode, DataChunk& startChunk, 
						  ISceneNodeFactory& nodeFactory, ISceneGameNodeFactory* pGameFactory, SceneNodeCamera* pCam) {

	LoadContext& loadContext = scene.mLoadContext;
	BufferString tempStr;

	SoftRef<DataChunk> sceneChunk = loadContext.dataSourcePool().resolveRemoteChunk(startChunk, true, NULL);

	if (sceneChunk.isNull()) {

		return true;
	}

	CoordSys fileSceneCoordSys;
	SoftRef<CoordSysConv> fileToSceneConv;
	CoordSys loadDestCoordSys;

	if (SceneBaseLoader_v1::loadCoordSys(tempStr, scene, sceneChunk, 
			fileSceneCoordSys, fileToSceneConv, loadDestCoordSys) == false) {

		assrt(false);
		return false;
	}

	
	SoftRef<DataChunk> child = sceneChunk->getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"physics")) {

			scene_loadPhysics(tempStr, scene, child.dref(), loadDestCoordSys);

		} else if (child->equals(L"debug")) {

			SceneBaseLoader_v1::loadDebug(tempStr, scene, child.dref());

		} else if (child->equals(L"meshes")) {

			SceneBaseLoader_v1::loadMeshes(tempStr, scene, child.dref(), loadDestCoordSys);
		}
		

		toNextSibling(child);
	}


	child = sceneChunk->getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"entities")) {

			scene_loadEntities(tempStr, scene, child.dref(), nodeFactory, pGameFactory, fileToSceneConv.ptr(), pCam);

		} else if (child->equals(L"transformTree")) {

			scene_loadTransformTree(tempStr, scene, rootNode, child.dref(), fileToSceneConv.ptr());

		}

		toNextSibling(child);
	}
	
	
	return true;
}


}