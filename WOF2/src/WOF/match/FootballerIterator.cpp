#include "entity/footballer/Footballer.h"
#include "entity/ball/Ball.h"

#include "FootballerIterator.h"

namespace WOF { namespace match {

	
bool FootballerIterator::iterateLoopFootballers(Footballer*& pInOutFootballer, FootballerIndex& iter, Filter filter) {

	if (iterateFootballers(pInOutFootballer, iter, filter))
		return true;

	pInOutFootballer = NULL;

	if (iterateFootballers(pInOutFootballer, iter, filter))
		return true;

	return false;
}

Footballer* FootballerIterator::getFootballerAt(const FootballerIndex& index, Filter filter) {

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;
	FootballerIndex i = 0;

	while(iterateLoopFootballers(footballer.ptrRef(), iter, filter)) {

		if (i == index)
			return footballer;

		++i;
	}

	return NULL;
}


void FootballerMultiFrameProcessingSetup::init(const Clock& timing, float refFPS, TickCount refFrameCount, bool exactIters) {

	referenceFPS = refFPS;
	referenceFrameCount = refFrameCount;
	exactIterations = exactIters;

	if (referenceFrameCount <= 1) {

		frameCount = 1;

	} else {

		frameCount = (TickCount) ((float) referenceFrameCount * ((float) timing.mTime.ticksPerSec / (float) referenceFPS));

		if (frameCount <= 1)
			frameCount = 1;
	}

	mode = frameCount <= 1 ? M_SingleFrame : M_MultiFrame;
}

FootballerMultiFrameProcessingState::FootballerMultiFrameProcessingState() 
	: frameIndex(0), footballerIndex(0), allFootballerCount(0), footballersPerFrame(0) {
}

void FootballerMultiFrameProcessingState::reset(const FootballerMultiFrameProcessingSetup& setup, const FootballerIndex& footballerCount) { 
			
	frameIndex = 0; 
	footballerIndex = 0; 
	allFootballerCount = footballerCount;
	footballersPerFrame = (FootballerIndex) ceilf((float) footballerCount / (float) setup.frameCount);
}

void FootballerMultiFrameProcessingState::startFrame(const FootballerMultiFrameProcessingSetup& setup, FootballerIndex& startFootballerIndex, FootballerIndex& footballerCount) {

	if (setup.mode == FootballerMultiFrameProcessingSetup::M_MultiFrame) {

		startFootballerIndex = footballerIndex;

		if (frameIndex >= setup.frameCount - 1) {

			if (setup.exactIterations) {

				footballerCount = allFootballerCount - startFootballerIndex;

			} else {

				footballerCount = footballersPerFrame;
			}

		} else {

			footballerCount = footballersPerFrame;
		}
		
	} else {

		startFootballerIndex = 0;
		footballerCount = allFootballerCount;
	}
}

void FootballerMultiFrameProcessingState::endFrame(const FootballerMultiFrameProcessingSetup& setup, const FootballerIndex& startFootballerIndex, const FootballerIndex& footballerCount) {

	if (setup.mode == FootballerMultiFrameProcessingSetup::M_MultiFrame) {

		if (frameIndex >= setup.frameCount - 1) {

			frameIndex = 0;

		} else {

			++frameIndex;
		}

		footballerIndex = (footballerIndex + footballerCount) % allFootballerCount;
	} 
}

void FootballerMultiFrameProcessingState::cancelFrame(const FootballerMultiFrameProcessingSetup& setup, const FootballerIndex& startFootballerIndex, const FootballerIndex& footballerCount) {
}

FootballerMultiFrameIter::FootballerMultiFrameIter() : isStarted(false) {
}

bool FootballerMultiFrameIter::startFrame(const FootballerMultiFrameProcessingSetup& setup, FootballerMultiFrameProcessingState& state, FootballerIterator& footballerIter) {

	if (isStarted) {

		assrt(false);
		return false;
	}

	state.startFrame(setup, startIndex, count);

	if (count == 0)
		return false;

	footballer.destroy();
	iterIndex = 0;

	for (FootballerIndex i = 0; i < startIndex; ++i) {

		if (!footballerIter.iterateFootballers(footballer.ptrRef(), iteratorArg)) {

			assrt(false);
			return false;
		}
	}

	isStarted = true;

	return true;
}

void FootballerMultiFrameIter::endFrame(const FootballerMultiFrameProcessingSetup& setup, FootballerMultiFrameProcessingState& state) {

	if (!isStarted) {

		assrt(false);
		return;
	}

	isStarted = false;
	state.endFrame(setup, startIndex, count);
}

void FootballerMultiFrameIter::cancelFrame(const FootballerMultiFrameProcessingSetup& setup, FootballerMultiFrameProcessingState& state) {

	if (!isStarted) {

		assrt(false);
		return;
	}

	isStarted = false;
	state.cancelFrame(setup, startIndex, count);
}

bool FootballerMultiFrameIter::next(FootballerIterator& footballerIter, Footballer*& pFootballer) {

	if (!isStarted) {

		assrt(false);
		return false;
	}

	if (iterIndex >= count)
		return false;

	++iterIndex;

	if (!footballerIter.iterateLoopFootballers(footballer.ptrRef(), iteratorArg)) {

		assrt(false);
		return false;
	}

	pFootballer = footballer;
	
	return true;
}

FootballerMultiFrameIterOneShot::FootballerMultiFrameIterOneShot(const FootballerMultiFrameProcessingSetup& _setup, FootballerMultiFrameProcessingState& _state, FootballerIterator& _footballerIter) 
	: pSetup(&_setup), state(&_state), footballerIter(&_footballerIter) {
}

FootballerMultiFrameIterOneShot::~FootballerMultiFrameIterOneShot() {

	if (isStarted)
		_endFrame();
}

bool FootballerMultiFrameIterOneShot::startFrame() {

	return FootballerMultiFrameIter::startFrame(dref(pSetup), state, footballerIter);
}

bool FootballerMultiFrameIterOneShot::next(Footballer*& pFootballer) {

	return FootballerMultiFrameIter::next(footballerIter, pFootballer);
}

void FootballerMultiFrameIterOneShot::_endFrame() {

	FootballerMultiFrameIter::endFrame(dref(pSetup), state);
}

void FootballerMultiFrameIterOneShot::cancelFrame() {

	FootballerMultiFrameIter::cancelFrame(dref(pSetup), state);
}
			
} }


