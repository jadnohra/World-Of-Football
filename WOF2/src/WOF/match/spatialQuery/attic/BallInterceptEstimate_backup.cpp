#include "BallInterceptEstimate.h"

#include "../entity/footballer/Footballer.h"
#include "../entity/ball/Ball.h"
#include "../Match.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallInterceptEstimate, CBallInterceptEstimate);

namespace WOF { namespace match {

const TCHAR* BallInterceptEstimate::ScriptClassName = L"CBallInterceptEstimate";


void BallInterceptEstimate::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<BallInterceptEstimate>(ScriptClassName).
		enumInt(Update_NoChange, L"Update_NoChange").
		enumInt(Update_NewEstimate, L"Update_NewEstimate").
		enumInt(Update_Invalidated, L"Update_Invalidated").
		enumInt(Estimate_None, L"Type_None").
		enumInt(Estimate_SimulInterceptorWait, L"Type_SimulInterceptorWait").
		enumInt(Estimate_SimulBallWait, L"Type_SimulBallWait").
		enumInt(Estimate_SimulLastBallWait, L"Type_SimulLastBallWait").
		enumInt(Estimate_CurrBall, L"Type_CurrBall").
		enumInt(Estimate_RestingBall, L"Type_RestingBall").
		func(&BallInterceptEstimate::script_isValid, L"isValid").
		func(&BallInterceptEstimate::script_invalidate, L"invalidate").
		func(&BallInterceptEstimate::script_update, L"update").
		func(&BallInterceptEstimate::script_getType, L"getType").
		func(&BallInterceptEstimate::script_isSimulated, L"isSimulated").
		func(&BallInterceptEstimate::script_isResting, L"isResting").
		func(&BallInterceptEstimate::script_isInstantaneous, L"isInstantaneous").
		func(&BallInterceptEstimate::script_getVel, L"getVel").
		func(&BallInterceptEstimate::script_getPos, L"getPos");
}

BallInterceptEstimate::FootballerInterceptState::FootballerInterceptState() {
}

void BallInterceptEstimate::FootballerInterceptState::init(Footballer& footballer) {

	distanceTolerance = footballer.mNodeMatch->mSwitchControlSetup.pathInterceptDistTolerance;

	activationTime = 0.0f;
	vel = footballer.getRunSpeed();

	penaltyTime = activationTime; //+ playerReactionTime;
}

CtVector3* BallInterceptEstimate::script_getPos() {

	switch (mType) {

		case Estimate_SimulInterceptorWait:
		case Estimate_SimulBallWait:
		case Estimate_SimulLastBallWait:{

			if (mIsSingleSample)
				return &mSimul->getSampleRef(mSampleIndex[0]).pos;

			return &mInterceptPos;

		} break;

		case Estimate_CurrBall:
		case Estimate_RestingBall: {

			return &mInterceptPos;

		} break;
	}

	return NULL;
}

CtVector3* BallInterceptEstimate::script_getVel() {

	switch (mType) {

		case Estimate_SimulInterceptorWait:
		case Estimate_SimulBallWait:
		case Estimate_SimulLastBallWait: {

			if (mIsSingleSample)
				return &mSimul->getSampleRef(mSampleIndex[0]).vel;

			return &mInterceptBallVel;

		} break;

		case Estimate_CurrBall:
		case Estimate_RestingBall: {

			return &mInterceptBallVel;

		} break;
	}

	return NULL;
}

bool BallInterceptEstimate::prepareForAnalysis(BallSimulation& _simul, const float& time) {

	mType = Estimate_None;
	mSimul = &_simul;
	mSimulID = _simul.getID();
	mAnalysisTime = time;

	return _simul.isValid();
}

void BallInterceptEstimate::prepareForAnalysis(const float& time) {

	mType = Estimate_None;
	mSimul.destroy();
	mSimulID = -1;
	mAnalysisTime = time;
}

BallInterceptEstimate::Type BallInterceptEstimate::analyzeSampleIntercept(Footballer& footballer, const FootballerInterceptState& footballerState, const Vector3& samplePos, const Time& sampleTime, const Time& currTime, float& outArrivalTime, float& outFootballerWaitTime) {

	if (currTime > sampleTime) {

		//assrt(false);
		return Estimate_None;
	}

	//for now dont take optimal height into account
	const Point& pos = footballer.mTransformWorld.getPosition();

	float distToTraj = distance(pos, samplePos);
	float timeToTraj;

	timeToTraj = footballerState.penaltyTime + (distToTraj) / footballerState.vel;

	outArrivalTime = currTime + timeToTraj;
	outFootballerWaitTime = sampleTime - outArrivalTime;

	/*
	That's a 2d tolerance (dist on pitch not in air) ... so should be applied as such!!
	bool useDistTolerance = true;

	if (useDistTolerance && outFootballerWaitTime < 0.0f) {

		if (distToTraj <= footballerState.distanceTolerance) {

			outArrivalTime = currTime;
			outFootballerWaitTime = 0.0f;
		}
	}
	*/

	return  (outFootballerWaitTime >= 0.0f) ? Estimate_SimulInterceptorWait : Estimate_SimulBallWait;
}

bool BallInterceptEstimate::setSample(const BallSimulation::Index& _sampleIndex) {

	if (mIsSingleSample && _sampleIndex == mSampleIndex[0])
		return false;

	mIsSingleSample = true;
	mSampleIndex[0] = _sampleIndex;

	return true;
}

bool BallInterceptEstimate::setSample(const BallSimulation::Index& fromIndex, const BallSimulation::Index& toIndex, const Vector3& pos, const float& u) {

	if (!mIsSingleSample 
			&& (fromIndex == mSampleIndex[0]) && (toIndex == mSampleIndex[1])
			&& mInterceptPos.equals(pos)) {

		return false;
	}
		

	mIsSingleSample = false;

	mSampleIndex[0] = fromIndex;
	mSampleIndex[1] = toIndex;

	mInterceptPos = pos;
	lerp(mSimul->getSample(fromIndex).vel, mSimul->getSample(toIndex).vel, u, mInterceptBallVel);

	return true;
}

bool BallInterceptEstimate::setSample(Ball& ball) {

	if (ball.isResting()) {

		if (mRestingID == ball.getRestingID()) 
			return false;

		mRestingID = ball.getRestingID();
		mInterceptPos = ball.getPos();
		mInterceptBallVel.zero();

	} else {

		if (mInterceptPos.equals(ball.getPos()))
			return false;

		mInterceptPos = ball.getPos();
		mInterceptBallVel = dref(ball.getVel());
	}

	return true;
}

bool BallInterceptEstimate::validate(const Time& currTime, BallSimulation* pSimul, Ball* pBall) {

	switch (mType) {

		case Estimate_SimulInterceptorWait: {

			if (pSimul && pSimul->isValid() 
					&& mSimulID == pSimul->getID()	
					&& currTime - mAnalysisTime <= mWaitTime
					&& currTime <= getInterceptTime()) {

				return true;
			}
		} break;

		case Estimate_SimulBallWait: {

			if (pSimul && pSimul->isValid() 
					&& mSimulID == pSimul->getID()	
					&& currTime - mAnalysisTime <= -mWaitTime
					&& currTime <= getInterceptTime()) {

				return true;
			}
		} break;

		case Estimate_SimulLastBallWait: {

			if (pSimul && pSimul->isValid() 
					&& mSimulID == pSimul->getID()	
					&& currTime - mAnalysisTime <= -mWaitTime) {

				return true;
			}

		} break;

		case Estimate_RestingBall: {

			if (pBall && pBall->isResting() 
					&& pBall->getRestingID() == mRestingID) {

				return true;
			}
		} break;

	}

	mType = Estimate_None;
	return false;
}


bool BallInterceptEstimate::tryEstimateRestingBall(Ball& ball, Footballer& footballer, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged) {

	prepareForAnalysis(currTime);

	if (!ball.isResting())
		return false;

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	mType = Estimate_RestingBall;
	analyzeSampleIntercept(footballer, *pFootballerState, ball.getPos(), currTime, currTime, mArrivalTime, mWaitTime);
	estimateChanged = setSample(ball);

	return true;
}

bool BallInterceptEstimate::tryEstimateCurrBall(Ball& ball, Footballer& footballer, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged) {

	prepareForAnalysis(currTime);

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	mType = Estimate_CurrBall;
	analyzeSampleIntercept(footballer, *pFootballerState, ball.getPos(), currTime, currTime, mArrivalTime, mWaitTime);
	estimateChanged = setSample(ball);

	return true;
}

bool BallInterceptEstimate::tryEstimateLastSimulSampleIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged) {

	if (!prepareForAnalysis(simul, currTime))
		return false;

	BallSimulation::Index lastReachableIntervalIndex = simul.getSyncFlowFootballerReachableIntervalIndex();

	if (lastReachableIntervalIndex >= simul.getFootballerReachableIntervalCount())
		return false;

	const BallSimulation::SampleInterval& reachableInterval = simul.getFootballerReachableInterval(lastReachableIntervalIndex);

	BallSimulation::Index sampleIndex = tmax(simul.getSyncFlowTrajSampleIndex(), reachableInterval.to);
	const BallSimulation::Sample& sample = simul.getSample(sampleIndex);

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	mType = analyzeSampleIntercept(footballer, *pFootballerState, sample.pos, sample.time, currTime, mArrivalTime, mWaitTime);
	estimateChanged = setSample(sampleIndex);
	
	if (mType == Estimate_SimulBallWait)
		mType = Estimate_SimulLastBallWait;

	return true;
}

bool BallInterceptEstimate::tryEstimateEarliestSimulIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged) {

	if (!prepareForAnalysis(simul, currTime))
		return false;

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	for (BallSimulation::Index i0 = simul.getSyncFlowFootballerReachableIntervalIndex();
			i0 < simul.getFootballerReachableIntervalCount(); ++i0) {

		const BallSimulation::SampleInterval& reachableInterval = simul.getFootballerReachableInterval(i0);

		BallSimulation::Index iFirst = tmax(simul.getSyncFlowTrajSampleIndex(), reachableInterval.from);

		for (BallSimulation::Index i1 = iFirst; i1 <= reachableInterval.to; ++i1) {

			const BallSimulation::Sample& fromSample = simul.getSample(i1);

			if (analyzeSampleIntercept(footballer, *pFootballerState, fromSample.pos, fromSample.time, currTime, mArrivalTime, mWaitTime) == Estimate_SimulInterceptorWait) {
				
				mType = Estimate_SimulInterceptorWait;
				estimateChanged = setSample(i1);

				return true;
			}
		
			const BallSimulation::Index& iFrom = i1;
			BallSimulation::Index iTo = iFrom + 1;

			if (iTo <= reachableInterval.to) {

				const BallSimulation::Sample& toSample = simul.getSample(iTo);

				float closestDistSq;
				float u;
				Point closestPt;

				closestDistSq = distanceSq(fromSample.pos, toSample.pos, footballerPos, closestPt, u);
				bool isBetweenPoints = isBetweenSegPoints(u);

				if (isBetweenPoints) {

					Time closestTime = lerp(fromSample.time, toSample.time, u);

					if (analyzeSampleIntercept(footballer, *pFootballerState, closestPt, closestTime, currTime, mArrivalTime, mWaitTime) == Estimate_SimulInterceptorWait) {

						mType = Estimate_SimulInterceptorWait;
						estimateChanged = setSample(iFrom , iTo, closestPt, u);
						return true;
					}
				} 
			}
		}
	}

	return false;
}

bool BallInterceptEstimate::tryEstimateEarliestCollSimulIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged) {

	if (!prepareForAnalysis(simul, currTime))
		return false;

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	for (BallSimulation::Index i0 = simul.getSyncFlowCollSampleIndex();
			i0 < simul.getCollSampleCount(); ++i0) {

		const BallSimulation::CollSample& sample = simul.getCollSample(i0);

		if (analyzeSampleIntercept(footballer, *pFootballerState, sample.pos, sample.time, currTime, mArrivalTime, mWaitTime) == Estimate_SimulInterceptorWait) {
			
			mCollType = Estimate_SimulInterceptorWait;
			
			estimateChanged = mCollSampleIndex == i0;
			mCollSampleIndex = i0;

			return true;
		}
	}

	return false;
}


BallInterceptEstimate::UpdateResult BallInterceptEstimate::update(Ball& ball, Footballer& footballer, const Time& currTime, BallSimulation* pSimul, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos) {

	//String::debug(L"*******\nupdate: %u\n", footballer.mNumber);

	Type currType = mType;

	if (validate(currTime, pSimul, &ball)) {

		//String::debug(L"no change\n");
		return Update_NoChange;
	}

	bool estimateChanged = true;
	FootballerInterceptState footballerState;

	footballerState.init(footballer);
	
	if (pSimul && pSimul->isValid()) {

		if (tryEstimateEarliestSimulIntercept(ball, footballer, *pSimul, currTime, &footballerState, estimateChanged)) {

			/*
			if (estimateChanged) {

				String dbgStr;
				dbgStr.assignEx(L"earliest: %d %f", mType, getInterceptTime());
				ball.mNodeMatch->getConsole().print(dbgStr.c_tstr());
			}
			*/

			return estimateChanged ? Update_NewEstimate : Update_NoChange;
		}

		if (fallbackLastSimulSample
			&& tryEstimateLastSimulSampleIntercept(ball, footballer, *pSimul, currTime, &footballerState, estimateChanged)) {

			/*
			if (estimateChanged) {

				String dbgStr;
				dbgStr.assignEx(L"last: %d %f", mType, getInterceptTime());
				ball.mNodeMatch->getConsole().print(dbgStr.c_tstr());
			}
			*/

			return estimateChanged ? Update_NewEstimate : Update_NoChange;
		}
	}

	if (fallbackRestingPos
		&& tryEstimateRestingBall(ball, footballer, currTime, &footballerState, estimateChanged)) {

		/*
		if (estimateChanged) {

			String dbgStr;
			dbgStr.assignEx(L"resting: %d %f", mType, getInterceptTime());
			ball.mNodeMatch->getConsole().print(dbgStr.c_tstr());
		}
		*/

		return estimateChanged ? Update_NewEstimate : Update_NoChange;
	}

	if (fallbackCurrPos
		&& tryEstimateCurrBall(ball, footballer, currTime, &footballerState, estimateChanged)) {

		/*		
		if (estimateChanged) {

			String dbgStr;
			dbgStr.assignEx(L"curr: %d %f", mType, getInterceptTime());
			ball.mNodeMatch->getConsole().print(dbgStr.c_tstr());
		}
		*/

		return estimateChanged ? Update_NewEstimate : Update_NoChange;
	}

	{
		ball.mNodeMatch->getConsole().print(L"invalid");
	}

	return Update_Invalidated;
}

int BallInterceptEstimate::script_update(Match* pMatch, ScriptedFootballer* pFootballer, Time currTime, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos) {

	Match& match = dref(pMatch);

	return update(match.getBall(), 
					(Footballer&) dref(pFootballer), 
					currTime, 
					match.getFootballerBallInterceptManager().getValidSimul(), 
					fallbackLastSimulSample, fallbackRestingPos, fallbackCurrPos);
}

} }