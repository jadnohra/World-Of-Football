#include "WEPhysBodyGroup.h"


namespace WE {


PhysBodyGroup::PhysBodyGroup() {
}

PhysBodyGroup::~PhysBodyGroup() {

	destroy();
}

void PhysBodyGroup::destroy() {

	mVolatileConstraintPtrs.destroy();
	mConstraintPtrs.destroy();
	mBodyPtrs.destroy();
}

void PhysBodyGroup::init(PhysIndex estBodyCount, PhysIndex estFixConstraintCount, PhysIndex estVolatileConstraintCount) {

	mBodyPtrs.reserve(estBodyCount);
	mConstraintPtrs.reserve(estFixConstraintCount);
	mVolatileConstraintPtrs.reserve(estVolatileConstraintCount);
}

void PhysBodyGroup::addBody(PhysRigidBody* pBody) {

	pBody->index = mBodyPtrs.count;
	mBodyPtrs.addOne(pBody);
}


void PhysBodyGroup::addConstraint(PhysConstraint* pConstraint, bool isVolatile) {

	if (isVolatile) {

		mVolatileConstraintPtrs.addOne(pConstraint);
	} else {

		mConstraintPtrs.addOne(pConstraint);
	}
}

void PhysBodyGroup::resetVolatileConstraints() {

	mVolatileConstraintPtrs.count = 0;
}



void PhysBodyGroup::resizeContainers() const {
}


void PhysBodyGroup::fillGeneralizedVelocity(PhysScalar* u) const {

	PhysIndex row = 0;

	for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

		u[row++] = mBodyPtrs.el[i]->v.el[0];
		u[row++] = mBodyPtrs.el[i]->v.el[1];
		u[row++] = mBodyPtrs.el[i]->v.el[2];
		u[row++] = mBodyPtrs.el[i]->w.el[0];
		u[row++] = mBodyPtrs.el[i]->w.el[1];
		u[row++] = mBodyPtrs.el[i]->w.el[2];
	}
}

void PhysBodyGroup::putGeneralizedVelocity(PhysScalar* u) const {

	PhysIndex row = 0;

	for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

		mBodyPtrs.el[i]->v.el[0] = u[row++];
		mBodyPtrs.el[i]->v.el[1] = u[row++];
		mBodyPtrs.el[i]->v.el[2] = u[row++];
		mBodyPtrs.el[i]->w.el[0] = u[row++];
		mBodyPtrs.el[i]->w.el[1] = u[row++];
		mBodyPtrs.el[i]->w.el[2] = u[row++];
	}
}

void PhysBodyGroup::fillGeneralizedPosition(PhysScalar* s) const {

	PhysIndex row = 0;

	for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

		s[row++] = mBodyPtrs.el[i]->pos.el[0];
		s[row++] = mBodyPtrs.el[i]->pos.el[1];
		s[row++] = mBodyPtrs.el[i]->pos.el[2];
		s[row++] = mBodyPtrs.el[i]->orient.el[0];
		s[row++] = mBodyPtrs.el[i]->orient.el[1];
		s[row++] = mBodyPtrs.el[i]->orient.el[2];
		s[row++] = mBodyPtrs.el[i]->orient.el[3];
	}
}

void PhysBodyGroup::fillGeneralizedExternalForce(PhysScalar* fext, bool includeVelocityForces) const {
	
	PhysIndex row = 0;
	Vector3 temp[2];

	if (includeVelocityForces) {

		for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

			if (mBodyPtrs.el[i]->isFixed) {

				fext[row++] = PhysZero;
				fext[row++] = PhysZero;
				fext[row++] = PhysZero;
				fext[row++] = PhysZero;
				fext[row++] = PhysZero;
				fext[row++] = PhysZero;

			} else {

				fext[row++] = mBodyPtrs.el[i]->force[0];
				fext[row++] = mBodyPtrs.el[i]->force[1];
				fext[row++] = mBodyPtrs.el[i]->force[2];

				mBodyPtrs.el[i]->ITensor.invMulVector(mBodyPtrs.el[i]->w, temp[0]);
				//check cross for direction
				temp[0].cross(mBodyPtrs.el[i]->w, temp[1]);
				
				fext[row++] = mBodyPtrs.el[i]->torque[0] - temp[1].el[0];
				fext[row++] = mBodyPtrs.el[i]->torque[1] - temp[1].el[1];
				fext[row++] = mBodyPtrs.el[i]->torque[2] - temp[1].el[2];
			}

		}

	} else {

		for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

			if (mBodyPtrs.el[i]->isFixed) {

				fext[row++] = PhysZero;
				fext[row++] = PhysZero;
				fext[row++] = PhysZero;
				fext[row++] = PhysZero;
				fext[row++] = PhysZero;
				fext[row++] = PhysZero;

			} else {

				fext[row++] = mBodyPtrs.el[i]->force[0];
				fext[row++] = mBodyPtrs.el[i]->force[1];
				fext[row++] = mBodyPtrs.el[i]->force[2];
				fext[row++] = mBodyPtrs.el[i]->torque[0];
				fext[row++] = mBodyPtrs.el[i]->torque[1];
				fext[row++] = mBodyPtrs.el[i]->torque[2];
			}
		}
	}
}

void PhysBodyGroup::fillInverseMass(PhysScalar* invM, PhysScalar* invI) const {

	PhysIndex rowM = 0;
	PhysIndex rowI = 0;

	for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

		invM[rowM++] = mBodyPtrs.el[i]->massInv;

		//y only those entries?? based on OpenTissue ... 
		invI[rowI++] = mBodyPtrs.el[i]->ITensor.invEl(0, 0);
		invI[rowI++] = mBodyPtrs.el[i]->ITensor.invEl(0, 1);
		invI[rowI++] = mBodyPtrs.el[i]->ITensor.invEl(0, 2);
		invI[rowI++] = mBodyPtrs.el[i]->ITensor.invEl(1, 1);
		invI[rowI++] = mBodyPtrs.el[i]->ITensor.invEl(1, 2);
		invI[rowI++] = mBodyPtrs.el[i]->ITensor.invEl(2, 2);
	}
}

void PhysBodyGroup::fillGeneralizedInverseMass(PhysBigMatrix& invM) const {

	int offset = 0;
	PhysBigMatrixOffset matOff;

	for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

		invM.el(offset, offset) = mBodyPtrs.el[i]->massInv;
		invM.el(offset + 1, offset + 1) = mBodyPtrs.el[i]->massInv;
		invM.el(offset + 2, offset + 2) = mBodyPtrs.el[i]->massInv;

		offset += 3;
		matOff.offRow = offset;
		matOff.offCol= offset;

		mBodyPtrs.el[i]->ITensor.fillInvMatrix(invM, matOff);
		offset += 3;
	}
}


void PhysBodyGroup::evaluateConstraints(ConstraintPtrArray& constraintPtrs, int& jacRowIndex, PhysIndex& activeConstraintCount) {

	bool isActive;
	int jacRowCount;
	PhysConstraint* pConstraint;


	for (PhysIndex i = 0; i < constraintPtrs.count; i++) {

		pConstraint = constraintPtrs.el[i];
		pConstraint->evaluate(isActive, jacRowCount);

		if (isActive) {
			
			activeConstraintCount++;
			pConstraint->setJacobianIndex(jacRowIndex);
			jacRowIndex += jacRowCount;
		}

	}

}

void PhysBodyGroup::evaluateConstraints(PhysIndex& activeConstraintCount) {

	int jacRowIndex;
	
	activeConstraintCount = 0;
	jacRowIndex = 0;
	
	evaluateConstraints(mConstraintPtrs, jacRowIndex, activeConstraintCount);
	evaluateConstraints(mVolatileConstraintPtrs, jacRowIndex, activeConstraintCount);
	
}

void PhysBodyGroup::fillConstraintLCPElements(ConstraintPtrArray& constraintPtrs, PhysConstraintLCPElements& lcpElements) {
	
	PhysConstraint* pConstraint;
	
	for (PhysIndex i = 0; i < constraintPtrs.count; i++) {

		pConstraint = constraintPtrs.el[i];

		if (pConstraint->isActive()) {

			lcpElements.fillJacobianIndex = pConstraint->getJacobianIndex();
			lcpElements.fillJacobianIndexBodyI = pConstraint->mpBody[PhysConstraint::BodyI]->tempTag;
			lcpElements.fillJacobianIndexBodyJ = pConstraint->mpBody[PhysConstraint::BodyJ]->tempTag;
			
			pConstraint->fillConstraintLCPElements(lcpElements);

			lcpElements.fillJacobianBodyIndicesIndex += 2;
		}
	}
}

void PhysBodyGroup::fillConstraintLCPElements(PhysConstraintLCPElements& lcpElements) {

	int tag = 0;

	for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

		mBodyPtrs.el[i]->tempTag = tag++;
	}

	lcpElements.fillJacobianBodyIndicesIndex = 0;

	fillConstraintLCPElements(mConstraintPtrs, lcpElements);
	fillConstraintLCPElements(mVolatileConstraintPtrs, lcpElements);
}

void PhysBodyGroup::doPositionUpdate(PhysScalar dt) const {
	
	PhysScalar halfDt;
	Vector3 dPos;
	Quaternion tempQ;
	
	halfDt = dt * 0.5f;

	for (PhysIndex i = 0; i < mBodyPtrs.count; i++) {

		mBodyPtrs.el[i]->doPositionUpdate(dt, halfDt, dPos, tempQ);
	}
}

void PhysBodyGroup::doPositionUpdate(PhysRigidBody& body, PhysScalar dt) {
	
	PhysScalar halfDt;
	Vector3 dPos;
	Quaternion tempQ;
	
	halfDt = dt * 0.5f;

	body.doPositionUpdate(dt, halfDt, dPos, tempQ);
}

}