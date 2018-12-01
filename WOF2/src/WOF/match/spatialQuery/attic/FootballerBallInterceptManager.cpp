#include "FootballerBallInterceptManager.h"

#include "../WOFMatch.h"
#include "../WOFMatchObjectIncludes.h"

namespace WOF { namespace match {

FootballerBallInterceptManager::FootballerBallInterceptManager() 
	: mNeedsUpdate(false), mState(State_None) {

	mSimul.setSampleDistance(0.0f, true);
}

void FootballerBallInterceptManager::init(Match& match) {

	if (mMatch.isValid()) {

		assrt(false);
	}

	mMatch = &match;
	mMatch->getBall().addListener(*this);

	{
		BallSimulation& simul = mSimul;

		simul.addIgnoreType(MNT_Footballer);
		simul.setRegisterCollisions(true);

		simul.enableEvent(BallSimulation::ET_Discontinuity, true);

		float defaultDist = mMatch->getCoordSys().convUnit(1.0f);

		if (simul.getSampleDistance() == 0.0f) {

			simul.setSampleDistance(defaultDist, true);
		}
	}

	mAnalyzedSimul.init(match);
}

void FootballerBallInterceptManager::load(Match& match, BufferString& tempStr, DataChunk& mainChunk, CoordSysConv* pConv) {

	SoftRef<DataChunk> chunk = mainChunk.getChild(L"ballPathIntercept");

	if (chunk.isValid()) {

		//mBallSimulManager.load(tempStr, chunk, pConv);
		mProcessRate.load(match, tempStr, chunk, pConv);

		{
			float sampleDistance;

			if (chunk->scanAttribute(tempStr, L"sampleDistance", L"%f", &sampleDistance)) {

				if (pConv) 
					pConv->toTargetUnits(sampleDistance);

				mSimul.setSampleDistance(sampleDistance, true);
			}
		}
	}
}

void FootballerBallInterceptManager::onBallExecutedCommand(Ball& ball, const BallCommand* pCommand) {

	if (pCommand)
		mCommand = dref(pCommand);
	else
		mCommand.invalidate();

	mNeedsUpdate = true;
}

void FootballerBallInterceptManager::frameMove(Match& match, const Time& time) {

	mSimul.reSyncToBall(mMatch->getBall(), mNeedsUpdate, mProcessRate);
	mNeedsUpdate = false;
	
	switch (mAnalyzedSimul.update(match, mSimul, time)) {

		case AnalyzedSimulation::State_Valid: mState = State_Valid; break;
		
		case AnalyzedSimulation::State_WaitingForSimul: 
		case AnalyzedSimulation::State_Processing: { 

			mState = State_Processing; 

		} break;

		default: mState = State_Invalid; break;
	}
}

void FootballerBallInterceptManager::render(Renderer& renderer, const bool& flagExtraRenders, RenderPassEnum pass) {

	mAnalyzedSimul.render(mMatch->getBall(), renderer, flagExtraRenders, pass);
}

bool FootballerBallInterceptManager::requestUpdate(Match& match, Footballer& footballer, const Time& time) {

	//limit this and delay some requests for performance.
	update(match, footballer, time);

	return true;
}

bool FootballerBallInterceptManager::update(Match& match, Footballer& footballer, const Time& time) {

	PathIntercept& pathIntercept = footballer.getBallPathIntercept();
	SoftPtr<BallSimulation::Index> sampleIndex;

	if (pathIntercept.simulID == mAnalyzedSimul.getSimulID()) {

		if (pathIntercept.analysisTime == time) {

			return pathIntercept.type == PI_InterceptorWait;

		} else {

			if (pathIntercept.type == PI_InterceptorWait)
				sampleIndex = &pathIntercept.sampleIndex;
		}
	}

	return mAnalyzedSimul.updateShortestTimeInterceptState(match, footballer, mSimul, time, sampleIndex);
}

bool FootballerBallInterceptManager::isStillIntercepting(Match& match, Footballer& footballer, const Time& time) {

	PathIntercept& pathIntercept = footballer.getBallPathIntercept();
	
	if (pathIntercept.simulID == mAnalyzedSimul.getSimulID()) {

		if (pathIntercept.type == PI_InterceptorWait
			&& pathIntercept.expiryTime >= time) {

			return mAnalyzedSimul.getIntersectionTime(footballer, mAnalyzedSimul.getSimulID(), pathIntercept.sampleIndex, dref(mAnalyzedSimul.getSample(pathIntercept.sampleIndex)), pathIntercept, mAnalyzedSimul.getSimul()->getStartTime(), time, true);
		}
	} 

	return false;
}


} }
