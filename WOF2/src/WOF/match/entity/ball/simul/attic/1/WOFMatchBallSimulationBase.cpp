#include "WOFMatchBallSimulationBase.h"

namespace WOF { namespace match {

	
BallSimulation_Filtered::BallSimulation_Filtered() 
	: mHasDuration(false), mDuration(0), 
		mEnabledEndAtResting(true), mStartTime(0), mRegisterCollisions(false), mCollisionSampleCount(0) {
}

void BallSimulation_Filtered::reset(bool _resetEnded, bool resetIgnoreTypes, bool resetBreakTypes) {

	mEndReason = ER_None;
	mBallIsTouching = false;

	if (_resetEnded)
		mEnded = false;

	if (resetIgnoreTypes)
		mIgnores.resize(0);

	if (resetBreakTypes)
		mBreaks.resize(0);
}

void BallSimulation_Filtered::resetTrajectory(const Time& startTime, const bool& clearTrajectory) {

	mStartTime = startTime;

	if (clearTrajectory) {

		mCollisionSampleCount = 0;
	}
}

void BallSimulation_Filtered::addIgnoreType(const ObjectType& type) {

	Index index = mIgnores.size;

	mIgnores.resize(mIgnores.size + 1);
	mIgnores.el[index] = type;
}

void BallSimulation_Filtered::addBreakType(const ObjectType& type) {

	Index index = mIgnores.size;

	mBreaks.resize(mIgnores.size + 1);
	mBreaks.el[index] = type;
}

void BallSimulation_Filtered::setEnableEndAtResting(bool enable) {

	mEnabledEndAtResting = enable;
}


bool BallSimulation_Filtered::simulEnded() {

	return mEnded;
}

void BallSimulation_Filtered::setDuration(Time time) {

	mDuration = time;
	mHasDuration = mDuration != 0;
}

bool BallSimulation_Filtered::getDuration(Time& time) {

	if (mHasDuration) {

		mEndReason = ER_Duration;
		time = mDuration;
		return true;
	}

	return false;
}

bool BallSimulation_Filtered::ignoreCollision(Object* pObject) {

	const ObjectType& type = pObject->objectType;
	
	for (Index i = 0; i < mIgnores.size; ++i) {

		if (mIgnores.el[i] == type) {

			return true;
		}
	}

	return false;
}

void BallSimulation_Filtered::signalCollision(const Index& prevSampleIndex, Object* pObject, const Point& pos, const Vector3& v, const Vector3& w, const Time& time) { 
	
	if (!mEnded) {

		if (mRegisterCollisions) {

			CollSamples::Index last = mCollisionSampleCount;
			++mCollisionSampleCount;

			mCollisionSamples.resize(mCollisionSampleCount);
			mCollisionSamples.el[last].set(pObject, pos, v, time - mStartTime, prevSampleIndex);
		}

		const ObjectType& type = pObject->objectType;

		for (Index i = 0; i < mBreaks.size; ++i) {

			if (mBreaks.el[i] == type) {

				mEnded = true;
				mEndReason = ER_BreakType;
				break;
			}
		}
	}
}

void BallSimulation_Filtered_Trajectory::signalCollision(Object* pObject, const Point& pos, const Vector3& v, const Vector3& w, const Time& time) { 
	
	BallSimulation_Filtered::signalCollision(mSampleCount - 1, pObject, pos, v, w, time);
}

void BallSimulation_Filtered_Trajectory::reset(const Time& startTime, bool clearTrajectory, bool resetEnded, bool resetIgnoreTypes, bool resetBreakTypes) {

	setStartTime(startTime, clearTrajectory);

	BallSimulation_Filtered::reset(resetEnded, resetIgnoreTypes, resetBreakTypes);
	BallSimulation_Filtered::resetTrajectory(startTime, clearTrajectory);
}

void BallSimulation_Filtered_Trajectory::signalFinalBallState(const Point& pos, const Vector3& v, const Vector3& w, const Time& time) {

	mEndState.pos = pos;
	mEndState.vel = v;
	mEndState.w = w;
	mEndState.time = time;
}

void BallSimulation_Filtered_Trajectory::addEndStateAsSample() {

	//forced last sample
	mEndState.time -= getStartTime();
	BallTrajectory::addSample(mEndState.pos, mEndState.vel, mEndState.time);
}

void BallSimulation_Filtered::signalBallTouchingState(bool touching, const Time& time) { 
	
	mBallIsTouching = touching; 
}

void BallSimulation_Filtered::signalBallResting(const Time& time) {

	if (!mEnded && mEnabledEndAtResting) {

		mEndReason = ER_Resting;
		mEnded = true;
	}
}

void BallSimulation_Filtered_Trajectory::signalBallState(const Point& pos, const Vector3& v, const Vector3& w, const Time& time) {

	if (!mEnded) {

		BallTrajectory::addSample(*((BallTrajectory::Sampler*) this), pos, v, time);
	}
}

void BallSimulation_Filtered_Trajectory::render(Renderer& renderer, const float& ballRadiusStart, RenderListener* pListener, const float* pBallRadiusEnd, const RenderColor* pLineStart, const RenderColor* pLineEnd, const RenderColor* pSphereStart, const RenderColor* pSphereEnd) {

	BallTrajectory::render(renderer, ballRadiusStart, pListener, pBallRadiusEnd, pLineStart, pLineEnd, pSphereStart, pSphereEnd);

	const RenderColor& sphereStart = pSphereStart ? *pSphereStart : RenderColor::kWhite;
	const RenderColor& sphereEnd = pSphereEnd ? *pSphereEnd : sphereStart;

	Sphere sphere;

	sphere.center = mEndState.pos;
	sphere.radius = pBallRadiusEnd ? *pBallRadiusEnd : ballRadiusStart;

	renderer.draw(sphere, &RigidMatrix4x3::kIdentity, &sphereEnd);
}

} }

