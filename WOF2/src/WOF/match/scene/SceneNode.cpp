#include "SceneNode.h"

#include "../Match.h"
#include "../DataTypes.h"

namespace WOF { namespace match {

int SceneNode::getSceneNodeClassID() {

	return NodeClassID_Node;
}

SceneNode::SceneNode() : mNodeLocalTransformIsDirty(true), mNodeWorldTransformChangedFlag(false) {

	mTransformLocal.identity();

	objectId = SceneNodeIndex::Null;
	objectType = Node_Node;
}

WorldMaterialID SceneNode::getTriMaterial(const ObjectBinding* pBinding) { 
	
	WorldMaterialID ID; 
	WorldMaterialManager::invalidateMaterialID(ID); 
	return ID; 
}

bool SceneNode::nodeInit(Match& match, const ObjectType& type, const ObjectIndex& id, const TCHAR* name) {

	if (mNodeMatch.isValid()) {

		assrt(false);
		return false;
	}

	mNodeMatch = &match;
	mNodeName.assign(name);

	objectType = type;
	objectId = id;

	return true;
}

bool SceneNode::nodeIsInited() {

	return mNodeMatch.isValid();
}

void SceneNode::nodeDestroy(bool _resetTreeNode) {
	
	mNodeMatch.destroy();

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

	return mNodeLocalTransformIsDirty != 0;
}

void SceneNode::nodeMarkDirty() {

	if (mNodeLocalTransformIsDirty) {

		return;
	}

	mNodeLocalTransformIsDirty = true;
	
	//propagate dirty state
	SceneNode* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->nodeMarkDirty();
		pChild = pChild->pNextSibling;
	}
}

bool SceneNode::rootNodeTreeClean() {

	if (pParent) {

		assrt(false);
		return false;
	}

	if (mNodeLocalTransformIsDirty) {

		nodeDirtyNodeRecurseClean(SceneTransform::kIdentity);

	} else {
		
		nodeCleanSubtree(true);
	}

	return true;
}


bool SceneNode::nodeCleanSubtree(bool includeThis) {

	if (mNodeLocalTransformIsDirty) {

		assrt(false);
		return false;
	}

	if (includeThis) {

		nodeDispatchEvt(false);
	}

	SceneNode* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->nodeRecurseClean();
		pChild = pChild->pNextSibling;
	}

	return true;
}

void SceneNode::nodeRecurseClean() {

	if (mNodeLocalTransformIsDirty) {

		if (pParent) {

			nodeDirtyNodeRecurseClean(pParent->mTransformWorld);

		} else {
			
			nodeDirtyNodeRecurseClean(SceneTransform::kIdentity);
		}

	} else {

		nodeDispatchEvt(false);
		
		SceneNode* pChild = pFirstChild;

		while (pChild != NULL) {

			pChild->nodeRecurseClean();
			pChild = pChild->pNextSibling;
		}
	}
}


void SceneNode::nodeDirtyNodeRecurseClean(const SceneTransform& parentWorldTransform) {

	transformUpdateWorld(parentWorldTransform);
	mNodeLocalTransformIsDirty = false;

	nodeDispatchEvt(true);

	SceneNode* pChild = pFirstChild;

	while (pChild != NULL) {

		pChild->nodeDirtyNodeRecurseClean(mTransformWorld);
		pChild = pChild->pNextSibling;
	}
}

} }