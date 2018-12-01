#include "BallControllerBoneAttach.h"

#include "BallControllerType.h"

#include "../../../scene/SceneNode.h"
#include "../../../Match.h"
#include "../Ball.h"


namespace WOF { namespace match { 

BallControllerBoneAttach::BallControllerBoneAttach() {
}


int BallControllerBoneAttach::getControllerType() {

	return BallController_BoneAttach;
}


void BallControllerBoneAttach::setPosition(const Point& pos) {
}

void BallControllerBoneAttach::setAttachOffset(const Vector& offset) {

	mAttachOffset = offset;
}

void BallControllerBoneAttach::attachTo(BoneInstance* pBone) {

	mBone = pBone;
}

bool BallControllerBoneAttach::isAttached() {

	return mBone.isValid();
}

BoneInstance* BallControllerBoneAttach::getAttachedBone() {

	return mBone;
}


void BallControllerBoneAttach::frameMove(Match& match, const Clock& time, Time& outEndTime) {

	outEndTime = time.getPhysTime() + time.getPhysFrameMoveTime();

	if (mBone.isValid()) {

		Vector3 offset;
		mBone->worldTransform.transformPoint(mAttachOffset, offset);

		mBall->nodeMarkDirty();
		SceneTransform& transfLocal = mBall->mTransformLocal;

		transfLocal = mBone->worldTransform;
		transfLocal.setPosition(offset);
	}
}



} } 