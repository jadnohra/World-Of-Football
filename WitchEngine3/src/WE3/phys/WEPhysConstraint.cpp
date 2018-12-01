#include "WEPhysConstraint.h"


namespace WE {

PhysScalar PhysConstraint::getErrorReductionParam() {

	return mErrorReductionParam;
}

void PhysConstraint::setErrorReductionParam(PhysScalar val) {

	mErrorReductionParam = val;
}

void PhysConstraint::forceInactive(bool inactive) {

	mForceInactive = inactive;
}

bool PhysConstraint::isActive() {

	return mIsActive;
}

void PhysConstraint::setJacobianIndex(int index) {

	mJacobianIndex = index;
}


int PhysConstraint::getJacobianIndex() {

	return mJacobianIndex;
}

void PhysConstraint::saveSolution(PhysScalar* vector) {

	mSavedSolution = vector;
}

PhysScalar* PhysConstraint::getSavedSolution() {

	return mSavedSolution;
}


}