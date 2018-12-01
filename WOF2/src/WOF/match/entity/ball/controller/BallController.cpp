#include "BallController.h"

#include "BallControllerType.h"
#include "../Ball.h"

namespace WOF { namespace match {


BallController::~BallController() {
}

int BallController::getControllerType() {

	return BallController_Base;
}

bool BallController::isAttached() {

	return mBall.isValid();
}

bool BallController::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	return false;
}

void BallController::setPosition(const Point& pos) {

	mBall->nodeMarkDirty();
	mBall->mTransformWorld.setPosition(pos);
}

void BallController::attach(Match& match, Ball* pBall) {

	handleBallPreDetach();
	mBall = pBall;
	handleBallPostAttach();
}

void BallController::handleBallPostAttach() {

	if (mBall.isValid()) {

		mBall->bindController(this);
	}
}

void BallController::handleBallPreDetach() {

	if (mBall.isValid()) {

		mBall->bindController(NULL);
	}
}

/*
void BallController::fillPhysCollisionProperties(Ball* pBall, BspCollider* pCollider, 
										 PhysRigidBody** ppColliderRigidBody, PhysMaterial** ppMaterial) {
	
	assrt(false);
}
*/

} }


