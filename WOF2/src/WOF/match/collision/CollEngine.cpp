#include "CollEngine.h"

#include "../Match.h"

namespace WOF { namespace match {

void CollEngine::destroy() {

	mMatch.destroy();

	mMatManager.destroy();
	mQueryObject.destroy();
}

void CollEngine::init(Match& match, SpatialManager& spatialManager, SpatialManagerBuildPhase* pBuildPhase) {

	mMatch = &match;
	mSpatialManager = &spatialManager;
	mSpatialManagerBuildPhase = pBuildPhase;
	mQueryObject = mSpatialManager->createSpatialObjectGroup();

	WE_MMemNew(mMatManager.ptrRef(), WorldMaterialManager());

	mCollRecordPool.tris.init(32);
	mCollRecordPool.vols.init(32);

	mCollMaskMatrix.matrix.create(_Node_Count, _Node_Count);

	for (int i = 0; i < mCollMaskMatrix.matrix.rows; ++i) {

		for (int j = 0; j < mCollMaskMatrix.matrix.cols; ++j) {

			mCollMaskMatrix.matrix.el(i, j) = false;
		}
	}

	

	//Ball
	mCollMaskMatrix.matrix.el(Node_Ball, Node_EntMesh) = true;
	mCollMaskMatrix.matrix.el(Node_EntMesh, Node_Ball) = true;

	mCollMaskMatrix.matrix.el(Node_Ball, Node_Footballer) = true;
	mCollMaskMatrix.matrix.el(Node_Footballer, Node_Ball) = true;

	mCollMaskMatrix.matrix.el(Node_Ball, Node_Goal) = true;
	mCollMaskMatrix.matrix.el(Node_Goal, Node_Ball) = true;

	mCollMaskMatrix.matrix.el(Node_Ball, Node_Pitch) = true;
	mCollMaskMatrix.matrix.el(Node_Pitch, Node_Ball) = true;

	mCollMaskMatrix.matrix.el(Node_Ball, Node_EntMeshNoScriptedCharacterColl) = true;
	mCollMaskMatrix.matrix.el(Node_EntMeshNoScriptedCharacterColl, Node_Ball) = true;
	

	//SimulBall
	mCollMaskMatrix.matrix.el(Node_SimulBall, Node_EntMesh) = true;
	mCollMaskMatrix.matrix.el(Node_EntMesh, Node_SimulBall) = true;

	//mCollMaskMatrix.matrix.el(Node_SimulBall, Node_Footballer) = true;
	//mCollMaskMatrix.matrix.el(Node_Footballer, Node_SimulBall) = true;

	mCollMaskMatrix.matrix.el(Node_SimulBall, Node_Goal) = true;
	mCollMaskMatrix.matrix.el(Node_Goal, Node_SimulBall) = true;

	mCollMaskMatrix.matrix.el(Node_SimulBall, Node_Pitch) = true;
	mCollMaskMatrix.matrix.el(Node_Pitch, Node_SimulBall) = true;

	mCollMaskMatrix.matrix.el(Node_SimulBall, Node_EntMeshNoScriptedCharacterColl) = true;
	mCollMaskMatrix.matrix.el(Node_EntMeshNoScriptedCharacterColl, Node_SimulBall) = true;

	//Footballer
	mCollMaskMatrix.matrix.el(Node_Footballer, Node_EntMesh) = true;
	mCollMaskMatrix.matrix.el(Node_EntMesh, Node_Footballer) = true;

	mCollMaskMatrix.matrix.el(Node_Footballer, Node_Footballer) = true;
	mCollMaskMatrix.matrix.el(Node_Footballer, Node_Footballer) = true;

	mCollMaskMatrix.matrix.el(Node_Footballer, Node_Goal) = true;
	mCollMaskMatrix.matrix.el(Node_Goal, Node_Footballer) = true;

	mCollMaskMatrix.matrix.el(Node_Footballer, Node_Ball) = true;
	mCollMaskMatrix.matrix.el(Node_Ball, Node_Footballer) = true;

	//ScriptedCharacter
	mCollMaskMatrix.matrix.el(Node_ScriptedCharacter, Node_EntMesh) = true;
	mCollMaskMatrix.matrix.el(Node_EntMesh, Node_ScriptedCharacter) = true;

	mCollMaskMatrix.matrix.el(Node_ScriptedCharacter, Node_ScriptedCharacter) = true;
	mCollMaskMatrix.matrix.el(Node_ScriptedCharacter, Node_ScriptedCharacter) = true;

	mCollMaskMatrix.matrix.el(Node_ScriptedCharacter, Node_Footballer) = true;
	mCollMaskMatrix.matrix.el(Node_Footballer, Node_ScriptedCharacter) = true;

	mCollMaskMatrix.matrix.el(Node_ScriptedCharacter, Node_Goal) = true;
	mCollMaskMatrix.matrix.el(Node_Goal, Node_ScriptedCharacter) = true;

	mCollMaskMatrix.matrix.el(Node_ScriptedCharacter, Node_Ball) = true;
	mCollMaskMatrix.matrix.el(Node_Ball, Node_ScriptedCharacter) = true;

	//Node_ScripedCharacterPositionProxy
	mCollMaskMatrix.matrix.el(Node_ScripedCharacterPositionProxy, Node_ScripedCharacterPositionProxy) = true;
	mCollMaskMatrix.matrix.el(Node_ScripedCharacterPositionProxy, Node_ScripedCharacterPositionProxy) = true;

	mCollMaskMatrix.matrix.el(Node_ScripedCharacterPositionProxy, Node_EntMesh) = true;
	mCollMaskMatrix.matrix.el(Node_EntMesh, Node_ScripedCharacterPositionProxy) = true;

	mCollMaskMatrix.matrix.el(Node_ScripedCharacterPositionProxy, Node_ScriptedCharacter) = true;
	mCollMaskMatrix.matrix.el(Node_ScriptedCharacter, Node_ScripedCharacterPositionProxy) = true;

	mCollMaskMatrix.matrix.el(Node_ScripedCharacterPositionProxy, Node_GoalProxy) = true;
	mCollMaskMatrix.matrix.el(Node_GoalProxy, Node_ScripedCharacterPositionProxy) = true;
}

void CollEngine::setBuildPhase(SpatialManagerBuildPhase* pBuildPhase) {

	mSpatialManagerBuildPhase = pBuildPhase;
}

/*
void CollEngine::render(Match& match, Object& thisNode, PolygonalCollider& collider, Renderer& renderer, 
						const RenderColor* pColor) {
	
	collider.render(renderer, pColor);
}
*/

void CollEngine::init(Match& match, Object& object, PolygonalCollider& collider) {

	//collider.bindObject(&object);
	//collider.init();
}


bool CollEngine::add(Match& match, Object& object, PolygonalCollider& collider, Mesh& mesh, const Matrix4x3Base* pTransform, bool simplifiyIfEnabled, bool* pResultIsEmpty) {

	if (!mSpatialManagerBuildPhase.isValid()) {

		assrt(false);
		return false;
	}

	if (!collider.createFromMesh(&object, mesh, match.getWorldMaterialManager(), pTransform, simplifiyIfEnabled, pResultIsEmpty)) {

		assrt(false);
		return false;
	}

	collider.add(match.getSpatialManagerBuildPhase());

	return true;
}

void CollEngine::destroy(Match& match, Object& object, PolygonalCollider& collider) {

	collider.remove(match.getSpatialManager());
}

/*
void CollEngine::render(Match& match, Object& thisNode, DynamicVolumeCollider& collider, Renderer& renderer, 
						const bool& renderCollData, const bool& renderWorldVolume, 
						const RenderColor* pColorCollData, const RenderColor* pColorVolume) {
	
	if (renderWorldVolume) {

		renderer.drawVol(collider.spatialVolume(), &Matrix43::kIdentity, pColorVolume);
	}
}
*/

void CollEngine::init(Match& match, Object& object, DynamicVolumeCollider& collider, const WorldMaterialID& materialID) {

	collider.bindObject(&object);
	collider.init(materialID);
}


bool CollEngine::add(Match& match, Object& object, DynamicVolumeCollider& collider, VolumeStruct& volume) {
	
	if (!mSpatialManagerBuildPhase.isValid()) {

		assrt(false);
		return false;
	}

	match.getSpatialManagerBuildPhase().addDynamicVolume(volume, &collider);
	
	return true;
}

Volume& CollEngine::updateStart(Match& match, Object& object, DynamicVolumeCollider& collider) {

	return dref(match.getSpatialManager().updateStartDynamicVolume(&collider));
}

void CollEngine::updateEnd(Match& match, Object& object, DynamicVolumeCollider& collider, Volume& volume) {

	match.getSpatialManager().updateEndDynamicVolume(&volume, &collider);
}

void CollEngine::destroy(Match& match, Object& object, DynamicVolumeCollider& collider) {

	match.getSpatialManager().removeDynamicVolume(&collider);
}


/*
void CollEngine::render(Match& match, Object& thisNode, VolumeCollider& collider, Renderer& renderer, 
						const bool& renderCollData, const bool& renderWorldVolume, 
						const RenderColor* pColorCollData, const RenderColor* pColorVolume) {
	
	if (renderWorldVolume) {

		renderer.drawVol(collider.spatialVolume(), &Matrix43::kIdentity, pColorVolume);
	}
}
*/

void CollEngine::init(Match& match, Object& object, VolumeCollider& collider, const WorldMaterialID& materialID) {

	collider.bindObject(&object);
	collider.init(materialID);
}


bool CollEngine::add(Match& match, Object& object, VolumeCollider& collider, VolumeStruct& volume) {
	
	if (!mSpatialManagerBuildPhase.isValid()) {

		assrt(false);
		return false;
	}

	match.getSpatialManagerBuildPhase().add(volume, &collider);

	return true;
}


void CollEngine::destroy(Match& match, Object& object, VolumeCollider& collider) {

	match.getSpatialManager().removeVolume(&collider);
}


} }