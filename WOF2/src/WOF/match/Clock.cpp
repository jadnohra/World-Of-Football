#include "Clock.h"

#include "WE3/WEMacros.h"
using namespace WE;

namespace WOF { namespace match {

void Clock::init(const float& FPS, const unsigned int& _physFPSMultiplier) {

	physFPSMultiplier = _physFPSMultiplier;

	mTime.init(FPS);
	mBallPhysTime.init(FPS * (float) physFPSMultiplier);
}

void Clock::start() {

	mTime.start(0.0f);
	mBallPhysTime.start(0.0f);
}

void Clock::add(const Time& _fTime, const Time& _fElapsedTime) {

	Time fTime = safeSizeCast<double, Time>(_fTime);
	float fElapsedTime = _fElapsedTime;

	if (mTime.t0 == 0) {

		mTime.start(fTime);
		fElapsedTime = 0.0f;

		mBallPhysTime.start(fTime);
	} 


	mTime.add(fElapsedTime);
	mBallPhysTime.add(fElapsedTime);
}

void Clock::accept() {

	mTime.accept();
	mBallPhysTime.accept();
}

void Clock::consume() {

	mTime.consume();
	mBallPhysTime.consume();
}

} }