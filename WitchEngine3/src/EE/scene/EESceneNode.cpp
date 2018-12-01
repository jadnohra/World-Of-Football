#include "EESceneNode.h"
#include "EEScene.h"

#include "EESceneNodeType.h"

namespace EE {

bool SceneNode::nodeTreeEvent(SceneBaseEvent& sceneEvt) {

	return nodeTreeUpdate(sceneEvt);
}

bool SceneNode::nodeTreeRender() {

	SceneEvent sceneEvt;

	sceneEvt.type = SE_Render;
	sceneEvt.pRenderer = &(mNodeScene->renderer());

	if (sceneEvt.pRenderer == NULL) {

		assrt(false);
		return false;
	}

	return nodeTreeEvent(sceneEvt);
}


bool SceneNode::nodeTreeMessage(const SceneEventType& evt) {

	SceneEvent sceneEvt;

	sceneEvt.type = evt;
	
	return nodeTreeEvent(sceneEvt);
}


SceneNode::SceneNode() : mNodeIsDirty(true) {

	mTransformLocal.identity();

	objectId = SceneNodeIndex::Null;
	objectType = SNT_Node;
}

bool SceneNode::nodeInit(Scene& scene, const ObjectType& type, const ObjectIndex& id, const TCHAR* name) {

	if (mNodeScene.isValid()) {

		assrt(false);
		return false;
	}

	mNodeScene = &scene;
	mNodeName.assign(name);

	objectType = type;
	objectId = id;

	return true;
}

bool SceneNode::nodeIsInited() {

	return mNodeScene.isValid();
}

void SceneNode::nodeDestroy(bool _resetTreeNode) {
	
	mNodeScene.destroy();

	if (_resetTreeNode) {

		resetTreeNode();
	}
}

SceneNode::~SceneNode() {
}

const TCHAR* SceneNode::nodeName() {

	return mNodeName.c_tstr();
}

bool SceneNode::nodeIsDirty() {

	return mNodeIsDirty != 0;
}

/*
const Vector3& SceneNode::nodeLocalPosition() {

	return mTransformLocal.getPosition();
}

const Vector3& SceneNode::nodeWorldPosition() {

	return mTransformWorld.getPosition();
}

void SceneNode::nodeLookAt(const Vector3& lookAt) {

	nodeLookAt(lookAt, mNodeScene->mCoordSysAxis[Scene_Up]);
}

void SceneNode::nodeLookAt(const Vector3& lookAt, const Vector3& unitUp) {

	assrt(mNodeIsDirty != 0);

	mTransformLocal.setOrientation(mTransformWorld.row[3], lookAt, unitUp);
}

void SceneNode::nodeOrient(const Vector3& direction) {

	assrt(mNodeIsDirty != 0);

	mTransformLocal.setOrientation(direction, mNodeScene->mCoordSysAxis[Scene_Up]);
}
*/

void SceneNode::nodeProcessSceneEvt(SceneBaseEvent& sceneBaseEvt, const bool& nodeWasDirty) {

	if (sceneBaseEvt.isGameEvt == false) {

		SceneEvent& sceneEvt = (SceneEvent&) sceneBaseEvt;

		if (sceneEvt.type == SE_Render 
				&& mNodeScene->mFlagExtraRenders
				&& mNodeScene->mFlagRenderGhostNodes 
				&& objectType == SNT_NodeGhost ) {

			Renderer& renderer = dref<Renderer>(sceneEvt.pRenderer);
			renderer.draw(mTransformWorld, mNodeScene->mFlagRenderGhostNodeSize);
		}
	}
}


bool SceneNode::nodeMoveTestStart(SceneNodeMoveTester& tester, const Vector3& testPos, bool useCollDetectorTest) {

	mTransformLocal.getPosition(tester.origLocalPos);
	mTransformLocal.setPosition(testPos);

	if (pParent) {

		if (pParent->mNodeIsDirty) {

			mTransformLocal.setPosition(tester.origLocalPos);
			assrt(false);
			return false;
		}

		transformUpdateWorld(pParent->mTransformWorld);

	} else {

		transformUpdateWorld(SceneTransform::kIdentity);
	}

	tester.isTestStart = true;
	tester.isCollTestAssisted = useCollDetectorTest;
	nodeDispatchEvt(true, tester);
	return true;
}

void SceneNode::nodeMoveTestEnd(SceneNodeMoveTester& tester, bool accept) {

	if (accept == false) {

		mTransformLocal.setPosition(tester.origLocalPos);

		if (pParent) {

			transformUpdateWorld(pParent->mTransformWorld);

		} else {

			transformUpdateWorld(SceneTransform::kIdentity);
		}
	}

	tester.isCollTestAccepted = accept;
	tester.isTestStart = false;
	nodeDispatchEvt(true, tester);
}


void SceneNode::nodeMarkDirty() {

	if (mNodeIsDirty) {

		return;
	}

	mNodeIsDirty = true;
	
	//propagate dirty state
	SceneNode* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->nodeMarkDirty();
		pChild = pChild->pNextSibling;
	}
}

bool SceneNode::nodeTreeUpdate(SceneBaseEvent& sceneEvt) {

	if (pParent) {

		assrt(false);
		return false;
	}

	if (mNodeIsDirty) {

		nodeDirtyNodeRecurseUpdate(SceneTransform::kIdentity, sceneEvt);

	} else {
		
		nodeUpdateSubtree(sceneEvt, true);
	}

	return true;
}


bool SceneNode::nodeUpdateSubtree(SceneBaseEvent& sceneEvt, bool includeThis) {

	if (mNodeIsDirty) {

		assrt(false);
		return false;
	}

	if (includeThis) {

		nodeDispatchEvt(false, sceneEvt);
	}

	SceneNode* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->nodeRecurseUpdate(sceneEvt);
		pChild = pChild->pNextSibling;
	}

	return true;
}

void SceneNode::nodePropagate(const SceneEventType& evt, bool includeThis) {

	SceneEvent sceneEvt;
	sceneEvt.type = evt;

	nodePropagate(sceneEvt, includeThis);
}

void SceneNode::nodePropagate(SceneBaseEvent& sceneEvt, bool includeThis) {

	if (includeThis) {

		nodeDispatchEvt(false, sceneEvt);
	}

	SceneNode* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->nodeRecursePropagate(sceneEvt);
		pChild = pChild->pNextSibling;
	}
}


void SceneNode::nodeRecursePropagate(SceneBaseEvent& sceneEvt) {

	nodeDispatchEvt(false, sceneEvt);
	
	SceneNode* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->nodeRecursePropagate(sceneEvt);
		pChild = pChild->pNextSibling;
	}
}

void SceneNode::nodeRecurseUpdate(SceneBaseEvent& sceneEvt) {

	if (mNodeIsDirty) {

		if (pParent) {

			nodeDirtyNodeRecurseUpdate(pParent->mTransformWorld, sceneEvt);

		} else {
			
			nodeDirtyNodeRecurseUpdate(SceneTransform::kIdentity, sceneEvt);
		}

	} else {

		nodeDispatchEvt(false, sceneEvt);
		
		SceneNode* pChild = pFirstChild;

		while (pChild != NULL) {

			pChild->nodeRecurseUpdate(sceneEvt);
			pChild = pChild->pNextSibling;
		}
	}
}


void SceneNode::nodeDirtyNodeRecurseUpdate(const SceneTransform& parentWorldTransform, SceneBaseEvent& sceneEvt) {

	transformUpdateWorld(parentWorldTransform);
	mNodeIsDirty = false;

	nodeDispatchEvt(true, sceneEvt);

	SceneNode* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->nodeDirtyNodeRecurseUpdate(mTransformWorld, sceneEvt);
		pChild = pChild->pNextSibling;
	}
}

/*
SceneTransform& SceneNode::nodeGetLocalTransform() {

	assrt (mNodeIsDirty != 0);
	return mTransformLocal;
}
*/

}