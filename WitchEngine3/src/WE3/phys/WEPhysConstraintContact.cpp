#include "WEPhysConstraintContact.h"

namespace WE {

//Sec. 4.9.7 - pg 83
//Sec. 4.6 - pg 55
//Fig. 4.1 - pg 48

PhysConstraintContact::PhysConstraintContact() {

	mIncludeCollisionLawInErrorTerm = true;
}

PhysConstraintContact::~PhysConstraintContact() {
}

PhysScalar PhysConstraintContact::getRelativeNormalVelocityMag() {

	return mRelNormalVelMag;
}

void PhysConstraintContact::orderBodies(PhysRigidBody& body1, PhysRigidBody& body2, PhysRigidBody*& pBodyI, PhysRigidBody*& pBodyJ, bool& isInverseOrder) {

	if (body2.index > body1.index) {

		pBodyI = &body1;
		pBodyJ = &body2;
		
		isInverseOrder = false;

	} else {

		pBodyI = &body2;
		pBodyJ = &body1;

		isInverseOrder = true;
	}
}

void PhysConstraintContact::getContactVelocity(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, Vector3& normal, Vector3& point, Vector3& relVel, PhysScalar& relVelN) {

	Vector3 pointVel[2];
	Vector3 r[2];
	Vector3 relPointVel;


	bodyI.getBodyPointVector(point, r[BodyI]);
	bodyJ.getBodyPointVector(point, r[BodyJ]);
	
	bodyI.getBodyPointVelocity(r[BodyI], pointVel[BodyI]);
	bodyJ.getBodyPointVelocity(r[BodyJ], pointVel[BodyJ]);

	pointVel[BodyJ].subtract(pointVel[BodyI], relVel);

	relVelN = normal.dot(relVel);
}

void PhysConstraintContact::initContact(PhysRigidBody* pBody1, PhysRigidBody* pBody2, PhysMaterial* pMaterial, 
											Vector3& normal, Vector3& point, PhysScalar distance) {

	/*
	Vector3* pBodyPos[2];

	if (pBody2->index > pBody1->index) {

		mpBody[BodyI] = pBody1;
		mpBody[BodyJ] = pBody2;
		
		pBodyPos[BodyI] = pBody1Pos ? pBody1Pos : &pBody1->pos;
		pBodyPos[BodyJ] = pBody2Pos ? pBody2Pos : &pBody2->pos;

	} else {

		mpBody[BodyI] = pBody2;
		mpBody[BodyJ] = pBody1;

		pBodyPos[BodyI] = pBody2Pos ? pBody2Pos : &pBody2->pos;
		pBodyPos[BodyJ] = pBody1Pos ? pBody1Pos : &pBody1->pos;
	}

	mpMaterial = pMaterial;

	n = normal;
	p = point;
	mDistance = distance;

	mTangFrictionDirCount = 2;


	mpBody[BodyI]->getBodyPointVector(*pBodyPos[BodyI], p, r[BodyI]);
	mpBody[BodyJ]->getBodyPointVector(*pBodyPos[BodyJ], p, r[BodyJ]);

	Vector3 pointVel[2];
	
	mpBody[BodyI]->getBodyPointVelocity(r[BodyI], pointVel[BodyI]);
	mpBody[BodyJ]->getBodyPointVelocity(r[BodyJ], pointVel[BodyJ]);

	pointVel[BodyJ].subtract(pointVel[BodyI], mRelPointVel);

	mRelNormalVelMag = n.dot(mRelPointVel);
	*/
	assert(false);
}

void PhysConstraintContact::cloneInitedContact(PhysConstraintContact& clone) {

	clone.mpMaterial = mpMaterial;
	clone.mpBody[BodyI] = mpBody[BodyI];
	clone.mpBody[BodyJ] = mpBody[BodyJ];

	clone.mTangFrictionDirCount = mTangFrictionDirCount;
	clone.mIncludeCollisionLawInErrorTerm = mIncludeCollisionLawInErrorTerm;

	clone.mDistance = mDistance;
	clone.mRelPointVel = mRelPointVel;
	clone.mRelNormalVelMag = mRelNormalVelMag;

	clone.p = p;
	clone.n = n;

	clone.r[BodyI] = r[BodyI];
	clone.r[BodyJ] = r[BodyJ];
}

void PhysConstraintContact::initFrictionTangentDirections() {
		
	//For. 4.47 - pg 56

	/*
	D.reserve(nFrictionFacetCount);

	PhysScalar factor;
	PhysScalar angle;
	Vector3 temp;

	factor = (_Phys(2) * physPi) / _Phys(nFrictionFacetCount);

	for (short h = 0; h < nFrictionFacetCount; h++) {

		angle = _Phys(h - 1) * factor;

		t1.mul(physCos(angle), D.el[h]);
		t2.mul(physSin(angle), temp);

		D.el[h] += temp;
	}
	*/

}

void PhysConstraintContact::initContactVariables() {

	mTangFrictionDirCount = MMax(mpMaterial->frictionDirCount, 2);

	if (mpMaterial->frictionCoeff <= PhysZero) {

		mTangFrictionDirCount;
	}

	
	
	r[BodyI].cross(n, rxn[BodyI]);
	r[BodyJ].cross(n, rxn[BodyJ]);


	if (mTangFrictionDirCount) {

		Vector3 relPointVelN;
		Vector3 relPointVelT;

		n.mul(mRelNormalVelMag, relPointVelN);
		mRelPointVel.subtract(relPointVelN, relPointVelT);

		if (relPointVelT.isZero()) {

			Vector3 abs = n;
			Vector3 temp;
			abs[0] = physAbs(abs[0]);
			abs[1] = physAbs(abs[1]);
			abs[2] = physAbs(abs[2]);

			if (abs[0] > abs[1]) {

				if (abs[0] > abs[2]) {
					temp.set(0, 1, 0);
				} else {
					temp.set(1, 0, 0);
				}
			} else {

				if (abs[1] > abs[2]) {
					temp.set(0, 0, 1);
				} else {
					temp.set(1, 0, 0);
				}
			}

			n.cross(temp, t2);
			t2.normalize();

			t2.cross(n, t1);
			t1.normalize();

		} else {
			
			relPointVelT.normalize(t1);
			n.cross(t1, t2);
			t2.normalize();
		}
	}

	initFrictionTangentDirections();
	

	short fc = mTangFrictionDirCount;

	if (fc--) {
		r[BodyI].cross(t1, rxt1[BodyI]);
		r[BodyJ].cross(t1, rxt1[BodyJ]);
	}

	if (fc--) {
		r[BodyI].cross(t2, rxt2[BodyI]);
		r[BodyJ].cross(t2, rxt2[BodyJ]);
	}
	
	initFrictionTangentDirections();
}

void PhysConstraintContact::evaluate(bool& _isActive, int& jacobianRowCount) {

	if (mForceInactive) {

		mIsActive = false;

	} else {

		mIsActive = true;
	}

	_isActive = mIsActive;

	if (_isActive != true) {

		return;
	}

	initContactVariables();
	
	jacobianRowCount = mTangFrictionDirCount + 1;
}


void PhysConstraintContact::fillJacobianLinear(PhysBigMatrix& matrix, PhysBigMatrixOffset& sub, int& col, PhysScalar mul) {

	//For. 4.165a,b

	int row = 0;

	sub.setRow3(row++, col, matrix, n.el, mul);

	/*
	for (short f = 0; f < nFrictionFacetCount; f++) {

		walker.setRow3(row++, col, matrix, D.el[f].el, mul);
	}
	*/


	short fc = mTangFrictionDirCount;

	if (fc--) {
		
		sub.setRow3(row++, col, matrix, t1.el, mul);
	}

	if (fc--) {
		
		sub.setRow3(row++, col, matrix, t2.el, mul);
	}

	col += 3;
}


void PhysConstraintContact::fillJacobianAngular(PhysBigMatrix& matrix, PhysBigMatrixOffset& sub, int& col, BodyIndex body, PhysScalar mul) {

	//For. 4.165c,d

	int row = 0;

	sub.setRow3(row++, col, matrix, rxn[body].el, mul);

	/*
	for (short f = 0; f < nFrictionFacetCount; f++) {

		sub.setRow3(row++, col, matrix, D.el[f].el, mul);
	}
	*/

	short fc = mTangFrictionDirCount;

	if (fc--) {
		
		sub.setRow3(row++, col, matrix, rxt1[body].el, mul);
	}

	if (fc--) {
		
		sub.setRow3(row++, col, matrix, rxt2[body].el, mul);
	}

	col += 3;
}

void PhysConstraintContact::fillConstraintLCPElements(PhysConstraintLCPElements& lcpElements) {

	int col = 0;

		
	PhysBigMatrixOffset off;

	off.setup(lcpElements.fillJacobianIndex, 0);

	//For. 4.164 - pg 83
	//Jacobians
	fillJacobianLinear(*(lcpElements.pJacobian), off, col, _Phys(-1.0f)); //BodyI
	fillJacobianLinear(*(lcpElements.pJacobian), off, col, _Phys(1.0f)); //BodyJ
	fillJacobianAngular(*(lcpElements.pJacobian), off, col, BodyI, _Phys(-1.0f));
	fillJacobianAngular(*(lcpElements.pJacobian), off, col, BodyJ, _Phys(1.0f));
	
	//the 'real' column offset is reflected in jacobinBodyIndices
	lcpElements.jacobianBodyIndices[lcpElements.fillJacobianBodyIndicesIndex] = lcpElements.fillJacobianIndexBodyI;
	lcpElements.jacobianBodyIndices[lcpElements.fillJacobianBodyIndicesIndex + 1] = lcpElements.fillJacobianIndexBodyJ;


	//Limits
	fillLimits(lcpElements.limitLows + lcpElements.fillJacobianIndex, 
					lcpElements.limitHighs + lcpElements.fillJacobianIndex);

	//Dependencies
	fillDependencies(lcpElements.dependIndices + lcpElements.fillJacobianIndex, 
						lcpElements.dependFactors + lcpElements.fillJacobianIndex, lcpElements.fillJacobianIndex);

	//error term
	fillErrorTerm(lcpElements.errorTerm + lcpElements.fillJacobianIndex, lcpElements.fps);
}


void PhysConstraintContact::fillLimits(PhysScalar* lows, PhysScalar* highs) {

	lows[0] = PhysZero;
	highs[0] = PhysInf;

	short fc = mTangFrictionDirCount;

	if (fc--) {

		lows[1] = -mpMaterial->frictionCoeff;
		highs[1] = mpMaterial->frictionCoeff;
	}

	if (fc--) {

		lows[2] = -mpMaterial->frictionCoeff;
		highs[2] = mpMaterial->frictionCoeff;
	}
}


void PhysConstraintContact::fillDependencies(int* indices, PhysScalar* factors, int offset) {

	indices[0] = -1;
	factors[0] = PhysZero;

	short fc = mTangFrictionDirCount;

	if (fc--) {

		indices[1] = 0 + offset;
		factors[1] = mpMaterial->frictionCoeff;
	}

	if (fc--) {

		indices[2] = 0 + offset;
		factors[2] = mpMaterial->frictionCoeff;
	}
}

void PhysConstraintContact::fillErrorTerm(PhysScalar* vector, PhysScalar& fps) {

	if(mDistance < PhysZero) {

		vector[0] = fps * mErrorReductionParam * mDistance;

	} else {

		vector[0] = PhysZero;
	}

	short fc = mTangFrictionDirCount;

	if (fc--) {

		vector[1] = PhysZero;
	}

	if (fc--) {

		vector[2] = PhysZero;
	}
    

	if (mIncludeCollisionLawInErrorTerm) {
		//--- Hacking, introducing Newton's collision law to model non-zero normal restitution
        //--- The main idea is that (suggested by Baraff 89'):
        //---
        //---     u_n^+ >= - epsilon u_n^-
        //---

		if (mpMaterial->nRestCoeff > PhysZero) {

			PhysScalar test = -mpMaterial->nRestCoeff * mRelNormalVelMag;

			if (vector[0] < test) {

				vector[0] = test;
			}
		}
	}

}

}