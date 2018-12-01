#include "../entity/ball/Ball.h"

#include "CollRegistry.h"

#include "../entity/ball/simul/BallSimulation.h"
#include "../inc/Collision.h"
#include "../scene/SceneNode.h"

namespace WOF { namespace match {

struct VolumeContext {

	SoftPtr<CollRegistry> registry;
	SoftPtr<CollRecordPool2> pool;
	
	VolumeStruct* pVolume;
	SoftPtr<ObjectBinding> binding;

	VolumeContext() {}
	VolumeContext(CollRegistry* pReg, CollRecordPool2* pPool, VolumeStruct* pVol, ObjectBinding* pBinding) 
		: registry(pReg), pool(pPool), pVolume(pVol), binding(pBinding) {}
};

bool fct_impl_addCollision_Tri1_VolumeContext(const TriangleEx1& tri, void* context) {

	VolumeContext& ctx = dref((VolumeContext*) context);

	if (intersect(dref(ctx.pVolume), tri)) {

		SoftPtr<CollRecordTri> rec = ctx.registry->addTri(ctx.pool);

		rec->binding = ctx.binding;
		rec->tri = &tri;
	}

	return true;
}

void CollRegistry::update(DynamicVolumeCollider& dynCollider, VolumeStruct& dynCollVolume, CollRecordPool2& pool, SpatialObjectGroup& queryObject, SpatialObjectGroup::TriContainer& collider) {

	VolumeRef volHolder;
	SoftPtr<VolumeStruct> boundingVol = collider.getBoundingVolume(volHolder);

	if (volHolder.isValid() && !intersect(dynCollVolume, boundingVol)) 
		return;

	VolumeContext context(this, &pool, &dynCollVolume, collider.getBinding());

	queryObject.getTris(collider, fct_impl_addCollision_Tri1_VolumeContext, &context);
}


void CollRegistry::update(VolRecords& records, DynamicVolumeCollider& dynCollider, VolumeStruct& dynCollVolume, CollRecordPool2& pool, SpatialVolumeBinding* pCollider, VolumeStruct& collVolume) {

	if (intersect(dynCollVolume, collVolume)) {

		SoftPtr<CollRecordVol> rec = addVol(records, pool);

		rec->binding = pCollider;
		rec->vol = &collVolume;
	}
}

void CollRegistry::update(CollEngine& engine, DynamicVolumeCollider& dynCollider, bool useProxies) {

	SoftPtr<SpatialObjectGroup> queryObject = engine.getQueryObject();

	update(engine.getCollMaskMatrix(), engine.getSpatialManager(), queryObject, dynCollider, engine.getCollRecordPool(), useProxies);

	engine.putQueryObject(queryObject);
}

void CollRegistry::update(CollMaskMatrix& maskMatrix, SpatialManager& spaceManager, SpatialObjectGroup& queryObject, DynamicVolumeCollider& dynCollider, CollRecordPool2& pool, bool useProxies) {

	empty(pool);

	SoftPtr<VolumeStruct> dynVolumeHolder;

	if (spaceManager.queryDynamicVolume(&dynCollider, dynVolumeHolder.ptrRef(), queryObject)) {

		VolumeStruct& dynVolume = dynVolumeHolder;

		SoftPtr<VolumeStruct> collVolume;
		SoftPtr<SpatialVolumeBinding> volCollider;

		SpatialObjectGroup::Index count;
		
		count = queryObject.getDynamicVolumeCount();

		if (!useProxies) {

			for (SpatialObjectGroup::Index i = 0; i < count; ++i) {

				volCollider = queryObject.getDynamicVolume(i, collVolume.ptrRef());
				const ObjectType& volColliderType = volCollider->getObjectType();

				if (maskMatrix.mask(dynCollider.getObjectType(), volColliderType)) {

					if (!typeIsProxy(volColliderType)) {

						update(mDynVolRecords, dynCollider, dynVolume, pool, volCollider, collVolume);
					}
				}
			}	

		} else {

			for (SpatialObjectGroup::Index i = 0; i < count; ++i) {

				volCollider = queryObject.getDynamicVolume(i, collVolume.ptrRef());
				const ObjectType& volColliderType = volCollider->getObjectType();

				if (maskMatrix.mask(dynCollider.getObjectType(), volColliderType)) {

					if (typeIsProxy(volColliderType) || !objectHasProxy(volCollider->getObject())) {

						update(mDynVolRecords, dynCollider, dynVolume, pool, volCollider, collVolume);
					}
				}
			}	

		}


		count = queryObject.getVolumeCount();

		for (SpatialObjectGroup::Index i = 0; i < count; ++i) {

			 volCollider = queryObject.getVolume(i, collVolume.ptrRef());

			  if (maskMatrix.mask(dynCollider.getObjectType(), volCollider->getObjectType())) {

				update(mVolRecords, dynCollider, dynVolume, pool, volCollider, collVolume);
			  }
		}

		count = queryObject.getTriContainerCount();

		for (SpatialObjectGroup::Index i = 0; i < count; ++i) {

			SpatialObjectGroup::TriContainer& triCont = queryObject.getTriContainer(i);

			 if (maskMatrix.mask(dynCollider.getObjectType(), triCont.getBinding()->getObjectType())) {

				update(dynCollider, dynVolume, pool, queryObject, triCont);
			}
		}
		
	}
	
}

void CollRegistry::empty(CollRecordPool2& pool) {

	for (Index i = 0; i < mTriRecords.count; ++i) {

		pool.tris.put(mTriRecords[i]);
	}

	mTriRecords.count = 0;

	for (Index i = 0; i < mVolRecords.count; ++i) {

		pool.vols.put(mVolRecords[i]);
	}

	mVolRecords.count = 0;

	for (Index i = 0; i < mDynVolRecords.count; ++i) {

		pool.vols.put(mDynVolRecords[i]);
	}

	mDynVolRecords.count = 0;
}

CollRecordTri* CollRegistry::addTri(CollRecordPool2& pool) {

	return mTriRecords.addOne(pool.tris.get());
}

void CollRegistry::removeTri(const Index& index, CollRecordPool2& pool){

	pool.tris.put(mTriRecords[index]);
	mTriRecords.removeSwapWithLast(index, false);
}

CollRecordVol* CollRegistry::addVol(VolRecords& records, CollRecordPool2& pool) {

	return records.addOne(pool.vols.get());
}

void CollRegistry::removeVol(VolRecords& records, const Index& index, CollRecordPool2& pool){

	pool.vols.put(records[index]);
	records.removeSwapWithLast(index, false);
}

void CollRegistry::render(Renderer& renderer, bool renderVolumes, const RenderColor* pVolColor, bool renderTris, const RenderColor* pTriColor, bool triWireFrame) {

	if (renderTris) {

		for (Index i = 0; i < mTriRecords.count; ++i) {

			mTriRecords[i]->render(renderer, pTriColor, triWireFrame);
		}
	}

	if (renderVolumes) {

		for (Index i = 0; i < mTriRecords.count; ++i) {

			mTriRecords[i]->render(renderer, pVolColor);
		}
	}
}

void CollRegistry::addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials) {

	for (VolRecords::Index i = 0; i < mVolRecords.count; ++i) {

		SoftPtr<SceneNode> node = SceneNode::nodeFromObject(mVolRecords[i]->binding->owner);

		if (!pSimul2 || !pSimul2->ignoreCollision(node)) {

			node->addSweptBallContacts(matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);
		}
	}

	for (TriRecords::Index i = 0; i < mTriRecords.count; ++i) {

		SoftPtr<SceneNode> node = SceneNode::nodeFromObject(mTriRecords[i]->binding->owner);

		//performance boost by sorting TriRecords by owner/binding!
		if (!pSimul2 || !pSimul2->ignoreCollision(node)) {

			SweptIntersection* pInter = &(intersections.makeSpaceForOne());
			const TriangleEx1& tri = dref(mTriRecords[i]->tri);

			if (sweptIntersect(radius, pos, sweepVector, tri.vertices, tri.normal, 
					pInter->contactPoint, *pInter, *pInter)) {

				pInter->recheckGeometry = (pInter->sweptFraction == 0.0f);
				pInter->pMaterial = matManager.getCombinedMaterial(sphereMat, node->getTriMaterial(mTriRecords[i]->binding));
				pInter->contactHasVelocity = false;
				pInter->staticObj = mTriRecords[i]->binding->getObject();

				intersections.addOneReserved();
				pInter = &(intersections.makeSpaceForOne());
			}
		}
	}

	for (VolRecords::Index i = 0; i < mDynVolRecords.count; ++i) {

		SoftPtr<SceneNode> node = SceneNode::nodeFromObject(mDynVolRecords[i]->binding->owner);

		if (!pSimul2 || !pSimul2->ignoreCollision(node)) {

			node->addSweptBallContacts(matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);
		}
	}
}

void CollRegistry::constrain(const OBB& dynCollider, Vector3& constrainContext, const Vector3& inVel, Vector3& outVel, Vector3& outCorrection, const int& ignoreAxis, const bool& considerTriEdges, float maxPenetrationDepth) {

	//Vector3 normal;
	//float depth;
	//float maxDepth = 0.0f;
//
//	outCorrection.zero();
//	outVel = inVel;
//
//	if (inVel.isZero()) {
//
//		constrainContext.normalize(normal);
//		maxPenetrationDepth = 0.0f;
//
//	} else {
//
//		if (!inVel.equals(constrainContext)) {
//
//			constrainContext = inVel;
//			maxPenetrationDepth = 0.0f;
//		}
//
//		inVel.normalize(normal);
//	}
//
//	normal.negate();
//	
//	for (Index i = 0; i < mRecords.count; ++i) {
//
//		if (mRecords[i]->getPenetrationDepth(dynCollider, normal, depth)) {
//
//			if (depth > maxDepth)
//				maxDepth = depth;
//		}
//	}
//
//	if (maxDepth >= maxPenetrationDepth) {
//
//		maxDepth -= maxPenetrationDepth;
//	}
//
//	normal.mul(maxDepth, outCorrection);
	

	/*
		
	outCorrection.zero();
	outVel = inVel;

	OBB tempCollider = dynCollider;

	Vector3 allowedNormalDir;
	Vector3 normal;
	float depth;

	Vector3 temp;
	//SameSignCombineState combineState[3];

	outVel.negate(allowedNormalDir);

	for (Index i = 0; i < mRecords.count; ++i) {

		if (mRecords[i]->intersectDepth(tempCollider, normal, depth, ignoreAxis, &allowedNormalDir, considerTriEdges)) {

			project(outVel, normal, temp);
			outVel.subtract(temp);

			//outVel.negate(allowedNormalDir);

			if (depth >= maxPenetrationDepth) {

				depth = depth - maxPenetrationDepth;
				normal.mul(depth, temp);

				//outCorrection.sameSignCombine(temp, combineState);
				//MOBBCenter(dynCollider).add(outCorrection, MOBBCenter(tempCollider));
					
				outCorrection.add(temp);
				MOBBCenter(tempCollider).add(temp);

				//Vector3 normal2;
				//float depth2;

				//if (mRecords[i]->intersectDepth(tempCollider, normal2, depth2, ignoreAxis, &allowedNormalDir, considerTriEdges) && depth2 != 0.0f) {

					//assrt(false);
				//}
			}
		}
	}
	*/
}

bool CollRegistry::constrainByTriRecord(const Index& recordIndex, const OBB& dynCollider, Vector3& constrainContext, const Vector3& inVel, Vector3& outVel, Vector3& outCorrection, const int& ignoreAxis, const bool& considerTriEdges, float maxPenetrationDepth, bool& isTouching, float guideAngleSinSpread) {

	outCorrection.zero();
	outVel = inVel;

	Vector3 allowedNormalDir;
	Vector3 normal;
	float depth;

	Vector3 temp;

	outVel.negate(allowedNormalDir);
	allowedNormalDir.normalize();

	if (getContactDepth(dynCollider, dref(mTriRecords[recordIndex]->tri), normal, depth, ignoreAxis, considerTriEdges, NULL, guideAngleSinSpread)) {

		project(outVel, normal, temp);
		outVel.subtract(temp);

		if (depth >= maxPenetrationDepth) {

			depth = depth - maxPenetrationDepth;
			normal.mul(depth, temp);

			outCorrection.add(temp);

			//Vector3 normal2;
			//float depth2;

			//if (mRecords[i]->intersectDepth(dynCollider, normal2, depth2, ignoreAxis, &allowedNormalDir, considerTriEdges) && depth2 != 0.0f) {

				//assrt(false);
			//}
		}

		isTouching = (depth == 0.0f);

		return true;
	}

	return false;
}

void CollRegistry::gatherTriContactNormals(const OBB& dynCollider, ContactNormalSolver& solver, const int& ignoreAxis, bool considerTriEdges) {

	float depth;

	for (TriRecords::Index i = 0; i < mTriRecords.count; ++i) {

		ContactNormalSolver::Contact& contact = solver.add();

		if (!getContactDepth(dynCollider, dref(mTriRecords[i]->tri), contact.normal, depth, ignoreAxis, considerTriEdges)) {

			solver.remove();
		}
	}
}

void CollRegistry::gatherTriContactNormals(const AAB& dynCollider, ContactNormalSolver& solver, bool considerTriEdges) {

	for (TriRecords::Index i = 0; i < mTriRecords.count; ++i) {

		ContactNormalSolver::Contact& contact = solver.add();

		if (!getContactDepth(dynCollider, dref(mTriRecords[i]->tri), contact.normal, NULL, considerTriEdges)) {

			solver.remove();
		}
	}
}

void CollRegistry::gatherTriContactNormals(const OBB& dynCollider, ContactNormalSolver& solver, bool considerTriEdges) {

	for (TriRecords::Index i = 0; i < mTriRecords.count; ++i) {

		ContactNormalSolver::Contact& contact = solver.add();

		if (!getContactDepth(dynCollider, dref(mTriRecords[i]->tri), contact.normal, NULL, considerTriEdges)) {

			solver.remove();
		}
	}
}

void CollRegistry::gatherTriNormals(ContactNormalSolver& solver, const Vector3* pFilterNormal) {

	for (TriRecords::Index i = 0; i < mTriRecords.count; ++i) {

		const Vector3& normal = dref(mTriRecords[i]->tri).normal;

		if (!pFilterNormal || pFilterNormal->dot(normal) > 0.0f) {

			ContactNormalSolver::Contact& contact = solver.add();

			contact.normal = normal;
		}
	}
}

void CollRegistry::gatherDynVolContactNormalsSwept(const Vector3& dynMoveDir, const float& moveDist, VolumeStruct& dynCollider, ContactNormalSolver& solver, bool useSimplifiedTest) {

	gatherVolContactNormalsSwept(dynMoveDir, moveDist, mDynVolRecords, dynCollider, solver, useSimplifiedTest);
}

void CollRegistry::gatherVolContactNormalsSwept(const Vector3& dynMoveDir, const float& moveDist, VolumeStruct& dynCollider, ContactNormalSolver& solver, bool useSimplifiedTest) {

	gatherVolContactNormalsSwept(dynMoveDir, moveDist, mVolRecords, dynCollider, solver, useSimplifiedTest);
}

void CollRegistry::gatherVolContactNormalsSwept(const Vector3& dynMoveDir, const float& moveDist, VolRecords& volRecords, VolumeStruct& dynCollider, ContactNormalSolver& solver, bool useSimplifiedTest) {

	for (VolRecords::Index i = 0; i < volRecords.count; ++i) {

		ContactNormalSolver::Contact& contact = solver.add();

		if (!getContactDepthSwept(dynMoveDir, moveDist, dynCollider, dref((VolumeStruct*) volRecords[i]->vol), contact.normal, NULL, useSimplifiedTest)) {

			solver.remove();

		} 
	}
}

} }