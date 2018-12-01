#include "SceneEntityMesh.h"

#include "WE3/helper/load/WEVarTableHelper.h"
#include "WE3/helper/load/WEMathDataLoadHelper.h"
using namespace WE;

#include "../Match.h"
#include "../DataTypes.h"

namespace WOF { namespace match {

int SceneEntityMesh::getSceneNodeClassID() {

	return NodeClassID_EntMesh;
}

SceneEntityMesh::SceneEntityMesh() {

	mEnableShadowing = false;
	mIsVisible = true;
}

SceneEntityMesh::~SceneEntityMesh() {

	destroyProxyVol();

	if (mSkelInst.isValid()) {

		if (mVolumeColliders.isValid()) {

			mNodeMatch->getCollEngine().destroy(mNodeMatch, *this, mVolumeColliders->dynamicColl);
			mVolumeColliders->dynamicColl.destroy();
		}

	} else {

		if (mVolumeColliders.isValid()) {

			mNodeMatch->getCollEngine().destroy(mNodeMatch, *this, mVolumeColliders->staticColl.dref());
			mVolumeColliders->staticColl.destroy();
		}
	}
}

void SceneEntityMesh::_init_shadowing(bool enable) {

	mEnableShadowing = enable;
}

bool SceneEntityMesh::_init_mesh(Mesh& mesh) {

	if (mMeshInst.init(mesh, mNodeMatch->getLoadContext().render()) == false) {

		return false;
	}

	if (mesh.hasSkeleton()) {

		MMemNew(mSkelInst.ptrRef(), SkeletonInstance());

		mSkelInst->bind(&mesh.getSkeleton());
		mSkelInst->useWorldMatrix(&mTransformWorld);
	}

	return true;
}

bool SceneEntityMesh::_init_nonSkeletalVolumeLocal() {

	SoftPtr<Mesh> mesh = mMeshInst.getMesh();
	AAB vol(true);

	mesh->getGeometry().extractVolume(vol, true);

	{
		SoftRef<CoordSysConv> conv = mNodeMatch->getLoadContext().convPool().getConverterFor(mesh->getGeometry().getUnifiedCoordSys(), mNodeMatch->getBaseCoordSys(), false);

		if (conv.isValid()) {

			conv->toTargetVolume(vol);
		}
	}

	mNonSkeletalVolumeLocal.init(&vol);

	return true;
}


bool SceneEntityMesh::_init_collider(const WorldMaterialID& collMaterial) {

	SoftPtr<Mesh> mesh = mMeshInst.getMesh();

	if (mesh.isNull() || mVolumeColliders.isValid()) {

		return false;
	}
	
	if (mSkelInst.isNull()) {
		
		if (_init_nonSkeletalVolumeLocal() == false) {

			return false;
		}
	}

	CollEngine& collEngine = mNodeMatch->getCollEngine();

	if (mSkelInst.isValid()) {

		MMemNew(mVolumeColliders.ptrRef(), ColliderContainer());
		MMemNew(mVolumeColliders->dynamicColl.ptrRef(), DynamicVolumeCollider());

		collEngine.init(mNodeMatch, *this, mVolumeColliders->dynamicColl, collMaterial);

		return mVolumeColliders.isValid();

	} 

	MMemNew(mVolumeColliders.ptrRef(), ColliderContainer());
	MMemNew(mVolumeColliders->staticColl.ptrRef(), VolumeCollider());

	collEngine.init(mNodeMatch, *this, mVolumeColliders->staticColl, collMaterial);
	
	return mVolumeColliders.isValid();
}


bool SceneEntityMesh::_init_NPGeomCollider() {

	SoftPtr<Mesh> mesh = mMeshInst.getMesh();

	if (mesh.isNull() || mMeshCollider.isValid() || mSkelInst.isValid()) {

		return false;
	}

	CollEngine& collEngine = mNodeMatch->getCollEngine();
	MMemNew(mMeshCollider.ptrRef(), MeshCollider());

	collEngine.init(mNodeMatch, *this, mMeshCollider);

	return mMeshCollider.isValid();
}

bool SceneEntityMesh::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType) {

	bool initSuccess = true;
	
	if (!chunk.getAttribute(L"name", tempStr)) {

		chunk.getValue(tempStr);
	}

	initSuccess = initSuccess &&  nodeInit(match, objType, match.genNodeId(), tempStr.c_tstr());

	initSuccess = initSuccess &&  chunk.getAttribute(L"source", tempStr);

	SoftRef<Mesh> srcMesh = match.getMeshes().getMesh(tempStr.c_tstr());

	initSuccess = initSuccess &&  srcMesh.isValid();
	
	if (initSuccess == false) {

		return false;
	}

	int attrCount;
	ScriptVarTable* pVarTable = match.getLoadContext().varTablePtr();

	bool coll = false;
	chunk.scanAttribute(tempStr, L"coll", coll);

	bool collNP = false;
	chunk.scanAttribute(tempStr, L"collNP", collNP);

	
	WorldMaterialID collMaterial;
	WorldMaterialManager::invalidateMaterialID(collMaterial);
	if (chunk.getAttribute(L"collMaterial", tempStr)) {

		match.getWorldMaterialManager().getMaterialID(tempStr.hash(), collMaterial);
	} 


	VarTableHelper::setUnsetParams(tempStr, pVarTable, chunk, attrCount, true);
	initSuccess = initSuccess && _init_mesh(srcMesh.dref());
	if (coll) {
		initSuccess = initSuccess && _init_collider(collMaterial);
	}
	if (collNP) {
		initSuccess = initSuccess && _init_NPGeomCollider();
	}
	VarTableHelper::setUnsetParams(tempStr, pVarTable, chunk, attrCount, false);

	bool enableShadowing = false;
	chunk.scanAttribute(tempStr, L"shadowing", enableShadowing);
	_init_shadowing(enableShadowing);

	chunk.scanAttribute(tempStr, L"visible", mIsVisible);

	return initSuccess;
}

bool SceneEntityMesh::init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool success = true;
	bool boneAttach = false;

	String boneName;

	if (chunk.getAttribute(L"boneAttach", boneName)) {

		mBoneAttachment.destroy();

		SoftPtr<SceneEntityMesh> entity;
		SoftPtr<BoneInstance> boneInst;

		SoftPtr<SceneNode> node = pParent;

		if (node.isValid() && node->getSceneNodeClassID() == NodeClassID_EntMesh) {

			entity = entMeshFromNode(node);
		}
		
		if (entity.isValid() && entity->mSkelInst.isValid()) {

			SoftPtr<SkeletonInstance> skelInst = entity->mSkelInst;
			
			boneInst = skelInst->findBoneInstanceByName(boneName.c_tstr());
		}

		if (boneInst.isValid()) {

			WE_MMemNew(mBoneAttachment.ptrRef(), BoneAttachment());
			
			mBoneAttachment->boneInst = boneInst;

		} else {
			
			assrt(false);
			success = false;
		}
	}

	return success;
}

void SceneEntityMesh::init_prepareScene() {

	mNodeLocalTransformIsDirty = false;
	mNodeWorldTransformChangedFlag = false;

	//assrt(pParent->mNodeLocalTransformIsDirty == false);
	//transformUpdateWorld(pParent->mTransformWorld);

	if (mSkelInst.isValid()) {

		mSkelInst->signalExternalWorldMatrixChanged();
		mSkelInst->update();

		if (mVolumeColliders.isValid()) {

			CollEngine& collEngine = mNodeMatch->getCollEngine();

			transform(mSkelInst->dirtyGetLocalDynamicAAB(), mTransformWorld, mVolumeColliders->dynamicColl->volume);
			collEngine.add(mNodeMatch, *this, mVolumeColliders->dynamicColl, mVolumeColliders->dynamicColl->volume);
		}

		
	} else {

		if (mVolumeColliders.isValid()) {

			CollEngine& collEngine = mNodeMatch->getCollEngine();

			transform(mNonSkeletalVolumeLocal, mTransformWorld, mVolumeColliders->staticColl->volume);
			collEngine.add(mNodeMatch, *this, mVolumeColliders->staticColl, mVolumeColliders->staticColl->volume);
			//mCollider->addToDetector(*this, mNodeMatch->getCollEngine());
		}

		if (mMeshCollider.isValid()) {

			CollEngine& collEngine = mNodeMatch->getCollEngine();
			
			collEngine.add(mNodeMatch, *this, mMeshCollider, *mMeshInst.getMesh(), &mTransformWorld);
			//mNPGeomCollider->setNewWorldTransform(*mMeshInst.getMesh(), mTransformWorld);
		}
	}
}

bool SceneEntityMesh::hasProxyVol() {

	return mProxyVol.isValid();
}

SceneDynVol* SceneEntityMesh::getProxyVol() {

	return mProxyVol;
}

SceneDynVol* SceneEntityMesh::createProxyVol(VolumeStruct& refVolume, const ObjectType& objType) {

	if (hasProxyVol())
		return NULL;

	if (!mSkelInst.isValid())
		return NULL;

	WE_MMemNew(mProxyVol.ptrRef(), SceneDynVol());

	String name(nodeName());
	name.append(L"_proxyVol");

	WorldMaterialID collMaterial;
	mNodeMatch->getCollEngine().getMaterialManager().invalidateMaterialID(collMaterial);

	if (!mProxyVol->create(mNodeMatch, this, name.c_tstr(), collMaterial, mTransformWorld.getPosition(), refVolume, objType)) {

		mProxyVol.destroy();
	}

	return mProxyVol;
}

void SceneEntityMesh::destroyProxyVol() {

	mProxyVol.destroy();
}


bool SceneEntityMesh::clean() {

	bool doUpdate = false;
	bool isBoneAttached = mBoneAttachment.isValid();
	
	if (mBoneAttachment.isValid()) {

		transformUpdateWorld(mBoneAttachment->boneInst->worldTransform);
		
		doUpdate = true;

	} else if (mNodeLocalTransformIsDirty) {

		doUpdate = true;

		if (pParent) {

			assrt(pParent->mNodeLocalTransformIsDirty == false);
			transformUpdateWorld(pParent->mTransformWorld);

		} else {

			mTransformWorld = mTransformLocal;
		}

	} else if (mNodeWorldTransformChangedFlag) {

		doUpdate = true;
	}

	if (doUpdate && mProxyVol.isValid()) {

		mProxyVol->update(mTransformWorld.getPosition());
	}

	if (mSkelInst.isValid()) {

		if (doUpdate || mSkelInst->needsUpdateAnim()) {

			mSkelInst->signalExternalWorldMatrixChanged();
			mSkelInst->update();

			if (mVolumeColliders.isValid()) {

				CollEngine& collEngine = mNodeMatch->getCollEngine();

				{
					Volume& newVol = collEngine.updateStart(mNodeMatch, *this, mVolumeColliders->dynamicColl);
					
					transform(mSkelInst->dirtyGetLocalDynamicAAB(), mTransformWorld, newVol);
					collEngine.updateEnd(mNodeMatch, *this, mVolumeColliders->dynamicColl, newVol);
				}
			}
		}
		
	} else {

		if (doUpdate) {

			//mMeshInst.nonSkeletalSignalExtrernalWorldMatrixChanged();

			if (mVolumeColliders.isValid() || mMeshCollider.isValid()) {

				assrt(false);
			}

			/*
			
			if (mVolumeColliders.isValid()) {

				CollEngine& collEngine = mNodeMatch->getCollEngine();

				transform(mNonSkeletalVolumeLocal, mTransformWorld, mVolumeColliders->spatialVolume());
				collEngine.update(mNodeMatch, *this, mVolumeColliders);
			}

			if (mMeshGeomCollider.isValid()) {

				//mMeshCollider->setNewWorldTransform(*mMeshInst.getMesh(), mTransformWorld);
			}
			*/

		} else {

		}
	}
	
	if (doUpdate) {
			
		mNodeLocalTransformIsDirty = false;
		mNodeWorldTransformChangedFlag = false;
	}

	return doUpdate;
}

void SceneEntityMesh::render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass) {

	switch(pass) {
		case RP_Normal:
			break;
		case RP_Shadowing:
			if (mEnableShadowing == false) {

				return;
			}
			break;
		default:
			return;
	}

	if (cleanIfNeeded) {
		
		clean();
	}

	if (mIsVisible) {

		if (mSkelInst.isValid()) {

			//mSkelInst->update();
			mMeshInst.render(renderer, mSkelInst.dref(), pass);

		} else {

			mMeshInst.render(renderer, mTransformWorld, pass);
		}

	} else {

		mIsVisible = mIsVisible;
	}

	if (flagExtraRenders && (pass == RP_Normal)) {

		if (mProxyVol.isValid())
			mProxyVol->render(renderer, cleanIfNeeded, flagExtraRenders, pass);

		if (mSkelInst.isValid()) {

			if (mVolumeColliders.isValid()) {

				renderer.drawVol(mVolumeColliders->dynamicColl->volume, &Matrix4x3Base::kIdentity, &RenderColor::kGreen);
			}

		} else {

			if (mVolumeColliders.isValid()) {

				CollEngine& collEngine = mNodeMatch->getCollEngine();

				/*
				collEngine.render(mNodeMatch, *this, mVolumeColliders, renderer, 
									mNodeMatch->getFlagRenderBPVolumes(), mNodeMatch->getFlagRenderVolumes(), 
									&RenderColor::kGreen, &RenderColor::kWhite);
									*/
				renderer.drawVol(mVolumeColliders->staticColl->volume, &Matrix4x3Base::kIdentity, &RenderColor::kGreen);
			}
	  
			if (mNodeMatch->getFlagRenderNPColliders() && mMeshCollider.isValid()) {

				CollEngine& collEngine = mNodeMatch->getCollEngine();

				//collEngine.render(mNodeMatch, *this, mMeshCollider, renderer, &RenderColor::kYellow);
			}
		}

		if (mNodeMatch->getFlagRenderDetailVolumes() && mSkelInst.isValid()) {
			
			mSkelInst->renderBoundingBoxes(renderer, &RenderColor::kBlue, &RenderColor::kRed);
		}
		
	}
	
}

WorldMaterialID SceneEntityMesh::getTriMaterial(const ObjectBinding* pBinding) {

	return mMeshCollider->getMaterial(pBinding);
}

void SceneEntityMesh::addSweptBallContacts(
						WorldMaterialManager& matManager, 
						const WorldMaterialID& sphereMat, const float& radius, 
						const Vector3& pos, const Vector3& sweepVector, 
						SweptIntersectionArray& intersections, bool executeMaterials) {

	//WorldMaterialID collMat;
	
	if (mVolumeColliders.isValid()) {

		if (mSkelInst.isValid()) {

			/*
			for (BoneIndex::Type i = 0; i < mSkelInst->binding.boneInstances.size; i++) {

				if (mSkelInst->binding.boneInstances.el[i].localBox.isEmpty() == false) {

					CollContact::CollIteratorOBB iter;

					
					if (CollContact::collIterStart(iter, radius, pos, sweepVector, mSkelInst->binding.boneInstances.el[i].worldBox)) {

						SweptIntersection* pInter = &(intersections.makeSpaceForOne());

						while (CollContact::collIterNext_sweptContact(iter, *pInter)) {

							mSkelInst->binding.boneInstances.el[i].markedColl = true;

							pInter->recheckGeometry = (pInter->sweptFraction == 0.0f);
							pInter->pMaterial = matManager.getCombinedMat(sphereMat, mCollider->mMaterial);
							pInter->contactHasVelocity = false;
							pInter->staticObjType = objectType;
							pInter->staticObj = this;

							intersections.addOneReserved();
							pInter = &(intersections.makeSpaceForOne());
						}

						CollContact::collIterEnd(iter);
					}
					
				}
			}
			*/
		
		} else {

			VolumeCollider& collider = mVolumeColliders->staticColl;

			collider.addSweptBallContacts(this,matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);
		}
	} 
}

bool SceneEntityMesh::extractBoneMeshLocalTransformAt(const TCHAR* boneName, const TCHAR* animName, Time time, SceneTransform& result) {

	if (mSkelInst.isValid() && mSkelInst->hasValidSkeleton()) {

		Skeleton& skel = dref(mSkelInst->getSkeleton());
		BoneIndex::Type boneIndex;
		AnimationIndex::Type animIndex;

		SoftPtr<Bone> bone = skel.findBoneByGameName(boneName);
		
		if (bone.isValid()) {

			boneIndex = bone->paletteIndex;

		} else {

			return false;
		}

		if (!skel.getAnimationIndex(animName, animIndex))
			return false;

		return extractBoneMeshLocalTransformAt(boneIndex, animIndex, time, result);
	}

	return false;
}

bool SceneEntityMesh::extractBoneMeshLocalTransformAt(BoneIndex::Type boneIndex, AnimationIndex::Type animIndex, Time time, SceneTransform& result) {

	if (mSkelInst.isValid() && mSkelInst->hasValidSkeleton()) {

		Skeleton& skel = dref(mSkelInst->getSkeleton());

		SoftPtr<Bone> bone = skel.getBoneByBlendIndex(boneIndex);

		if (bone.isValid()) {

			skel.setState(animIndex, time *	mSkelInst->getSpeedFactor());
			skel.updateRecurse(NULL);

			result = bone->mTransformWorld;

			return true;
		}
	}

	return false;
}

/*
void SceneEntityMesh::addSweptBallContacts(
						WorldMaterialManager& matManager, 
						const WorldMaterialID& sphereMat, const float& radius, 
						const Vector3& pos, const Vector3& sweepVector, 
						SweptIntersectionArray& intersections) {


	WorldMaterialID collMat;
	
	if (mNPGeomCollider.isValid()) {

		NPGeometryCollider& NPColl = mNPGeomCollider->mNPCollider;

		NPGeometryCollider::CollIterator iter;

		if (NPColl.collIterStart(iter)) {

			SweptIntersection* pInter = &(intersections.makeSpaceForOne());
			
			while (NPColl.collIterNext_sweptContact(iter, radius, pos, sweepVector, *pInter, collMat)) {

				pInter->recheckGeometry = (pInter->sweptFraction == 0.0f);
				pInter->pMaterial = matManager.getCombinedMat(sphereMat, collMat);
				pInter->contactHasVelocity = false;
				pInter->staticObjType = objectType;
				pInter->staticObj = this;

				intersections.addOneReserved();
				pInter = &(intersections.makeSpaceForOne());
			}
			

			NPColl.collIterEnd(iter);
		}

	} else {

		if (mSkelInst.isValid()) {

			for (BoneIndex::Type i = 0; i < mSkelInst->binding.boneInstances.size; i++) {

				if (mSkelInst->binding.boneInstances.el[i].localBox.isEmpty() == false) {

					CollContact::CollIteratorOBB iter;

					
					if (CollContact::collIterStart(iter, radius, pos, sweepVector, mSkelInst->binding.boneInstances.el[i].worldBox)) {

						SweptIntersection* pInter = &(intersections.makeSpaceForOne());

						while (CollContact::collIterNext_sweptContact(iter, *pInter)) {

							mSkelInst->binding.boneInstances.el[i].markedColl = true;

							pInter->recheckGeometry = (pInter->sweptFraction == 0.0f);
							pInter->pMaterial = matManager.getCombinedMat(sphereMat, mCollider->mMaterial);
							pInter->contactHasVelocity = false;
							pInter->staticObjType = objectType;
							pInter->staticObj = this;

							intersections.addOneReserved();
							pInter = &(intersections.makeSpaceForOne());
						}

						CollContact::collIterEnd(iter);
					}
					
				}
			}
		

		} else {

			switch (mCollider->mVolumeWorld.type) {
				case V_OBB:
					{
						CollContact::CollIteratorOBB iter;

						
						if (CollContact::collIterStart(iter, radius, pos, sweepVector, mCollider->mVolumeWorld.toOBB())) {

							SweptIntersection* pInter = &(intersections.makeSpaceForOne());

							while (CollContact::collIterNext_sweptContact(iter, *pInter)) {

								pInter->recheckGeometry = (pInter->sweptFraction == 0.0f);
								pInter->pMaterial = matManager.getCombinedMat(sphereMat, mCollider->mMaterial);
								pInter->contactHasVelocity = false;
								pInter->staticObjType = objectType;
								pInter->staticObj = this;

								intersections.addOneReserved();
								pInter = &(intersections.makeSpaceForOne());
							}

							CollContact::collIterEnd(iter);
						}
					}
					break;
			}

		}

	}
}
*/

/*
bool SceneEntityMesh::getOBBContact(const OBB& obb, Vector& normal, float& penetration) {

	if (mCollider.isValid()) {

		switch (mCollider->mVolumeWorld.type) {
				case V_OBB:
					{
						return CollContact::contactOBB(obb, mCollider->mVolumeWorld.toOBB(), normal, penetration, true, Scene_Up);
					}
					break;
		}
	}

	return false;
}
*/

} }