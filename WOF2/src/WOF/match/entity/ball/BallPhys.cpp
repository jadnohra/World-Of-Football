#include "Ball.h"

#include "../../Match.h"

namespace WOF { namespace match {

BallControllerPhysImpl* Ball::getActiveControllerPhys() {

	if (mController.ptr() == &mControllerPhys) {

		return &mControllerPhys;
	}

	return NULL;
}

BallControllerPhysImpl* Ball::getControllerPhys() {

	return &mControllerPhys;
}

/*
void Ball::collBroadPhaseOnStart(Object* pObject, CollRecordPool& collPool) {

	bool alreadyFound;

   	mCollRegistry.add(pObject, collPool, alreadyFound);
}

void Ball::collBroadPhaseOnEnd(Object* pObject, CollRecordPool& collPool) {

	mCollRegistry.remove(pObject, collPool);
}
*/

void Ball::physMarkDesynced() {

	mPhysIsDesynced = true;
}

void Ball::physSyncNodeFromPhysBody() {

	if (mPhysIsDesynced) {

		nodeMarkDirty();

		mTransformLocal.setPosition(mPhysBody.pos);
		mTransformLocal.setOrientation(mPhysBody.orient);

		mPhysIsDesynced = false;
	}
}

void Ball::physSyncPhysBodyFromNode() {

	if (mPhysIsDesynced) {

		mTransformLocal.getPosition(mPhysBody.pos);
		mTransformLocal.getOrientation(mPhysBody.orient);

		mPhysIsDesynced = false;
	}
}

void Ball::physGetSweepMoveIntersections(const Vector3& sweepVector, SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials) {

	if (mPhysIsDesynced) {
		
		physSyncNodeFromPhysBody();
		clean();
	}
	
	Point temp;
	mPhysBody.pos.add(sweepVector, temp);

	mNodeLocalTransformIsDirty = true;
	mTransformLocal.setPosition(temp);
	clean();

	if (pSimul2) 
		pSimul2->addSweptBallContacts(*this, mNodeMatch->getWorldMaterialManager(), 
											mCollider.getMaterial(), mSphereLocal.radius, 
											mPhysBody.pos, sweepVector, intersections, pSimul2, executeMaterials);
	else
		mCollRegistry.addSweptBallContacts(mNodeMatch->getWorldMaterialManager(), 
											mCollider.getMaterial(), mSphereLocal.radius, 
											mPhysBody.pos, sweepVector, intersections, pSimul2, executeMaterials);
	
	mNodeLocalTransformIsDirty = true;
	mTransformLocal.setPosition(mPhysBody.pos);
}

} }