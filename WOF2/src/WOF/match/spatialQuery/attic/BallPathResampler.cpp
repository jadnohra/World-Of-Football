#include "BallPathResampler.h"

/*
namespace WOF { namespace match {

bool BallPathTimeResampler::reset(BallSimulation* pSimul, const Time& t, const Time& dt) {

	mSimul = pSimul;

	if (!mSimul.isValid() || !mSimul->isValid())
		return false;

	mSimulID = mSimul->getID();
	mStartTime = t;
	mTimeInterval = dt;
	mCurrTime = mStartTime;

	mSample.isValid = false;

	return true;
}



bool BallPathTimeResampler::Sample::setup(const Time& time, BallSimulation& simul) {

	isValid = false;

	if (!simul.searchForTime(time, sample1, sample2))
		return false;

	if (sample1 + 1 == simul.getSampleCount()) {
		
		//last resting sample

		this->time = time;
		pos = simul.getSample(sample1)->pos;
		speed = simul.getEndState().vel.mag();
		isResting = true;
		
		lerpFactor = 0.0f;
		
	} else {

		Time relTime = time - simul.getStartTime();

		this->time = time;

		const BallTrajectory::Sample& s1 = dref(simul.getSample(sample1));
		const BallTrajectory::Sample& s2 = dref(simul.getSample(sample2));

		lerpFactor = (relTime - s1.time) / (s2.time - s1.time);

		lerp(s1.pos, s2.pos, lerpFactor, pos);

		Vector3 v;

		s2.pos.subtract(s1.pos, v);
		v.div(s2.time - s1.time);
		speed = v.mag();

		isResting = false;
	}

	return true;
}

const BallPathTimeResampler::Sample* BallPathTimeResampler::getNextSample() {

	assrt(mSimulID == mSimul->getID());

	if (mSample.isValid && mSample.isResting)
		return NULL;

	Time sampleTime = mCurrTime;
	
	if (!mSample.setup(mCurrTime, mSimul)) 
		return NULL;

	mCurrTime += mTimeInterval;

	return &mSample;
}

} }
*/