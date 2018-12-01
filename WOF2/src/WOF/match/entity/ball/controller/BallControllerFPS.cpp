#include "BallControllerFPS.h"

#include "BallControllerType.h"

#include "../../../scene/SceneNode.h"
#include "../../../Match.h"
#include "../Ball.h"


namespace WOF { namespace match { 

BallControllerFPS::BallControllerFPS() {

	mIsLoadFiring = false;
	mLoadFireRotation = 0.0f;
}


int BallControllerFPS::getControllerType() {

	return BallController_FPS;
}

void BallControllerFPS::setVibration(float vibration) {

	mVibration = vibration;
}

void BallControllerFPS::setRotation(float rotation) {

	mRotation = rotation;
}

float BallControllerFPS::getVibration() {

	return mVibration;
}

float BallControllerFPS::getRotation() {

	return mRotation;
}

void BallControllerFPS::setPosition(const Point& pos) {

	mPos = pos;
}

void BallControllerFPS::setNodeAttachOffset(const Vector& offset) {

	mAttachOffset = offset;
}

void BallControllerFPS::attachPositionToNode(SceneNode* pNode) {

	mPosAttachNode = pNode;
}

bool BallControllerFPS::isAttachedPositionToNode() {

	return mPosAttachNode.isValid();
}

bool BallControllerFPS::isAttachedPositionToNode(SceneNode* pNode) {

	return mPosAttachNode.ptr() == pNode;
}

bool BallControllerFPS::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	return false;
}

void BallControllerFPS::startLoadFiring(const Clock& time) {

	if (mIsLoadFiring) {

		return;
	}

	mIsLoadFiring = true;

	mLoadFireStartTime = time.getPhysTime();
	mLoadFireRotation = 0.0f;
}

void BallControllerFPS::stopLoadFiring(Vector3& vel) {

	if (mIsLoadFiring == false) {

		return;
	}

	mIsLoadFiring = false;

	if (mPosAttachNode.isValid()) {

		mPosAttachNode->mTransformWorld.getDirection(Scene_Forward).mul(mLoadFireRotation, vel);
		
	} else {

		vel.zero();
	}
}

void BallControllerFPS::frameMove(Match& match, const Clock& time, Time& outEndTime) {

	outEndTime = time.getPhysTime() + time.getPhysFrameMoveTime();

	if (mPosAttachNode.isValid()) {

		Vector3 offset;
		mPosAttachNode->mTransformWorld.transformPoint(mAttachOffset, offset);

		setPosition(offset);
	}

	Quaternion orientation;
	Vector axis;
	Vector pos;
	float rotation;

	if (mIsLoadFiring) {

		mLoadFireRotation = 
			MMin(20.0f, 3.0f * (time.mTime.t_discreet - mLoadFireStartTime));

		rotation = mLoadFireRotation;

	} else {

		rotation = mRotation;
	}

	const FastUnitCoordSys& coordSys = match.getCoordSys();

	coordSys.setRUF(0.3f, 0.3f, -0.3f, axis.el);
	axis.normalize();


	mBall->nodeMarkDirty();
	SceneTransform& transfLocal = mBall->mTransformLocal;

	orientation.fromMatrix(transfLocal.castToCtMatrix3x3Base());


	/*
	coordSys.setRUF(1.0f, 0.0f, 0.0f, axis.el);
	axis.normalize();

	static float totalTime = 0.0f;
	static float dt = 0.01f;
	bool doRot = false;

	mVibration = 0.0f;

	if (GetAsyncKeyState('P') < 0) {

		doRot = true;


	} else {

		if (totalTime != 0.0f) {

			String dbgStr;
			static int ct = 0;

			dbgStr.assignEx(L"%d. %f", ct++, totalTime);

			mBall->mNodeMatch->getConsole().show(true, true);
			mBall->mNodeMatch->getConsole().print(dbgStr.c_tstr());

			totalTime = 0.0f;
		}
	}

	
	if (doRot) 
	{
		Vector3 w;
		Quaternion orientationTemp;

		axis.mul(k2Pi, w);

		mul(w, orientation, orientationTemp);
		orientationTemp.mul(dt * 0.5f);
		orientation.add(orientationTemp);
		orientation.normalize();

		totalTime += dt;

		transfLocal.setOrientation(orientation);
	}
	*/

	
	if (match.mTweakRotateAttachedBall) {

		Quaternion orientationDiff;
		Quaternion orientationTemp;

		orientationDiff.derivativeFromAngularVelocity(axis, rotation * time.getFramePhysTickCount());
		orientationDiff.mul(time.getFramePhysTickLength() * 0.5f);

		mul(orientationDiff, orientation, orientationTemp);
		orientation.add(orientationTemp);
		orientation.normalize();

		transfLocal.setOrientation(orientation);
	}
		
	
	if (match.mTweakVibrateAttachedBall) {

		float od;
		float t = time.getPhysTime();

		Vector3 vibOff;

		od = sinf(mVibration * 3.0f * t) * 0.04f;
		coordSys.setValue(CSD_Up, od, vibOff.el);
		
		od = cosf(mVibration * 2.6f * t) * 0.04f;
		coordSys.setValue(CSD_Right, od, vibOff.el);

		od = cosf(mVibration * 3.4f * t) * 0.01f;
		coordSys.setValue(CSD_Forward, od, vibOff.el);

		mPos.add(vibOff, pos);

		transfLocal.setPosition(pos);

	} else {

		pos = mPos;

		transfLocal.setPosition(pos);
	}
}



} } 