#include "WOFMatchBallTrajectory.h"

#include "WE3/math/WEMathUtil.h"
#include "WE3/scene/WESceneDirection.h"
using namespace WE;

namespace WOF { namespace match {

BallTrajectory::BallTrajectory() : mSampleCount(0) {
}

void BallTrajectory::clear() {

	mSampleCount = 0;
}

void BallTrajectory::setStartTime(const Time& time, bool _clear) { 
	
	mStartTime =  time;  

	if (_clear)
		clear();
}

bool BallTrajectory::getEndTime(Time& endTime) {

	if (mSampleCount) {

		endTime = mStartTime + (mSamples.el[mSampleCount - 1]).time;

		return true;
	}

	return false;
}

void BallTrajectory::reserve(Index sampleCount, bool addCurrSampleCount) {

	if (addCurrSampleCount)
		sampleCount += mSampleCount;

	mSamples.resize(sampleCount);
}

BallTrajectory::Sample* BallTrajectory::addSample() {

	++mSampleCount;

	mSamples.resize(mSampleCount);

	return &(mSamples.el[mSampleCount - 1]);
}

BallTrajectory::Sample* BallTrajectory::addSample(const Point& pos, const Point& vel, const Time& time) {

	Sample* pSample = addSample();

	pSample->set(pos, vel, time);

	return pSample;
}

void BallTrajectory::render(Renderer& renderer, const float& ballRadiusStart, RenderListener* pListener, const float* pBallRadiusEnd, const RenderColor* pLineStart, const RenderColor* pLineEnd, const RenderColor* pSphereStart, const RenderColor* pSphereEnd, const Index* pStartIndex, const Index* pEndIndex) {

	const float& ballRadiusEnd = pBallRadiusEnd ? *pBallRadiusEnd : ballRadiusStart;
	const RenderColor& lineStart = pLineStart ? *pLineStart : RenderColor::kWhite;
	const RenderColor& lineEnd = pLineEnd ? *pLineEnd : lineStart;
	const RenderColor& sphereStart = pSphereStart ? *pSphereStart : RenderColor::kWhite;
	const RenderColor& sphereEnd = pSphereEnd ? *pSphereEnd : sphereStart;

	
	Sphere sphere;
	
	float factor;
	float lfactor1;
	//float lfactor2;

	float colRange = (float) (mSampleCount + 1);

	RenderColor col1;
	col1.alpha = 1.0f;
	//RenderColor col2;

	Index startIndex = pStartIndex ? dref(pStartIndex) : 0;
	Index endIndex = pEndIndex ? dref(pEndIndex) : mSampleCount;

	for (Index i = 0; i < mSampleCount && i <= endIndex; ++i) {

		factor = (float) i / (float) mSampleCount;
		lfactor1 = (float) i / colRange;
		//lfactor2 = (float) i / colRange;

		sphere.center = mSamples.el[i].pos;
		sphere.radius = lerp(ballRadiusStart, ballRadiusEnd, factor);

		lerp((Vector&) sphereStart, (Vector&) sphereEnd, factor, (Vector&) col1);

		if (i >= startIndex)
			renderer.draw(sphere, &RigidMatrix4x3::kIdentity, &col1);

		if (pListener)
			pListener->onRenderedSample(renderer, i, mSamples.el[i], sphere.radius, col1);

		lerp((Vector&) lineStart, (Vector&) lineEnd, lfactor1, (Vector&) col1);
		//lerp((Vector&) lineStart, (Vector&) lineEnd, lfactor2, (Vector&) col2);

		if (i + 1 < mSampleCount) {

			if (i + 1 >= startIndex)
				renderer.draw(mSamples.el[i].pos, mSamples.el[i + 1].pos, &RigidMatrix4x3::kIdentity, &col1);
		}
	}

}

void BallTrajectory::Sampler::setSampleMethod(SamplingMethod method) {

	mSampleMethod = method;
}

void BallTrajectory::Sampler::setSampleInterval(const Time& interval, bool setMethod) {

	mSampleInterval = interval;

	if (setMethod)
		mSampleMethod = BallTrajectory::SM_Interval;
}

void BallTrajectory::Sampler::setSampleDistance(const float& distance, bool setMethod) {

	mSampleDistance = distance;

	if (setMethod)
		mSampleMethod = BallTrajectory::SM_Distance;
}

void BallTrajectory::Sampler::addSample(BallTrajectory& trajectory, const Point& pos, const Vector3& vel, const Time& time) {

	Time relTime = time - trajectory.getStartTime();
	
	switch (mSampleMethod) {

		case BallTrajectory::SM_Interval: {

			const Time& interval = mSampleInterval;

			BallTrajectory::Index sampleIndex = (BallTrajectory::Index) (relTime / interval);

			BallTrajectory::Index sampleCount = trajectory.getSampleCount();

			if (sampleCount) {

				Time exactSampleTime;

				BallTrajectory::Sample* pLastSample = &mLastSample;
				BallTrajectory::Sample* pNewSample = NULL;
				Time lerpRange;

				for (BallTrajectory::Index i = trajectory.getSampleCount(); i <= sampleIndex; ++i) {

					exactSampleTime = i * interval;

					pNewSample = trajectory.addSample();
					
					lerpRange = relTime - pLastSample->time;

					pNewSample->time = exactSampleTime;
					lerp(pLastSample->pos, pos, (exactSampleTime - pLastSample->time) / lerpRange, pNewSample->pos);

					pLastSample = pNewSample;
				}

				mLastSample.set(pos, vel, relTime);

			} else {

				Time exactSampleTime;
				BallTrajectory::Index i;

				for (i = sampleCount; i <= sampleIndex; ++i) {

					exactSampleTime = i * interval;

					trajectory.addSample(pos, vel, exactSampleTime);
				}

				exactSampleTime = i * interval;
				trajectory.addSample(pos, vel, exactSampleTime);

				mLastSample.set(pos, vel, relTime);
			}

		} break;

		case BallTrajectory::SM_Distance: {


			BallTrajectory::Index  sampleCount = trajectory.getSampleCount();

			if (sampleCount) {

				Point dist;

				pos.subtract(mLastSample.pos, dist);
				
				mTraveledDistance += dist.mag();

				if (mTraveledDistance > mSampleDistance) {

					mTraveledDistance = 0.0f;

					trajectory.addSample(pos, vel, relTime);
				} 

				mLastSample.pos = pos;
				mLastSample.vel = vel;
				

			} else {

				mTraveledDistance = 0.0f;
				mLastSample.pos = pos;
				mLastSample.vel = vel;

				trajectory.addSample(pos, vel, relTime);
			}

		} break;

		default: {

			trajectory.addSample(pos, vel, relTime);

		} break;
	}
}


} }