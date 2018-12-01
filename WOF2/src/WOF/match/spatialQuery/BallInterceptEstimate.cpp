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
		enumInt(Estimate_None, L"Estimate_None").
		enumInt(Estimate_SimulInterceptorWait, L"Estimate_SimulInterceptorWait").
		enumInt(Estimate_SimulBallWait, L"Estimate_SimulBallWait").
		enumInt(Estimate_SimulLastBallWait, L"Estimate_SimulLastBallWait").
		enumInt(Estimate_CurrBall, L"Estimate_CurrBall").
		enumInt(Estimate_RestingBall, L"Estimate_RestingBall").
		enumInt(Analyse_None, L"Analyse_None").
		enumInt(Analyse_Path, L"Analyse_Path").
		enumInt(Analyse_Coll, L"Analyse_Coll").
		enumInt(Valid_Both, L"Valid_Both").
		func(&BallInterceptEstimate::enableAnalysis, L"enableAnalysis").
		func(&BallInterceptEstimate::enableBallOutOfPlayEstimates, L"enableBallOutOfPlayEstimates").
		func(&BallInterceptEstimate::script_isValid, L"isValid").
		func(&BallInterceptEstimate::script_isValidMode, L"isValidMode").
		func(&BallInterceptEstimate::isPathValid, L"isPathValid").
		func(&BallInterceptEstimate::isCollValid, L"isCollValid").
		func(&BallInterceptEstimate::script_analyseValidity, L"analyseValidity").
		func(&BallInterceptEstimate::invalidatePath, L"invalidatePath").
		func(&BallInterceptEstimate::invalidateColl, L"invalidateColl").
		func(&BallInterceptEstimate::script_invalidate, L"invalidate").
		func(&BallInterceptEstimate::script_invalidateMode, L"invalidateMode").
		func(&BallInterceptEstimate::script_updatePath, L"updatePath").
		func(&BallInterceptEstimate::script_updateColl, L"updateColl").
		func(&BallInterceptEstimate::script_getPathType, L"getPathType").
		func(&BallInterceptEstimate::script_getCollType, L"getCollType").
		func(&BallInterceptEstimate::script_getType, L"getType").
		func(&BallInterceptEstimate::script_getModeType, L"getModeType").
		func(&BallInterceptEstimate::isPathSimulated, L"isPathSimulated").
		func(&BallInterceptEstimate::isPathResting, L"isPathResting").
		func(&BallInterceptEstimate::isPathInstantaneous, L"isPathInstantaneous").
		func(&BallInterceptEstimate::script_isSimulated, L"isSimulated").
		func(&BallInterceptEstimate::script_isResting, L"isResting").
		func(&BallInterceptEstimate::script_isInstantaneous, L"isInstantaneous").
		func(&BallInterceptEstimate::script_isModeSimulated, L"isModeSimulated").
		func(&BallInterceptEstimate::script_isModeResting, L"isModeResting").
		func(&BallInterceptEstimate::script_isModeInstantaneous, L"isModeInstantaneous").
		func(&BallInterceptEstimate::getPathVel, L"getPathVel").
		func(&BallInterceptEstimate::getPathPos, L"getPathPos").
		func(&BallInterceptEstimate::getCollVel, L"getCollVel").
		func(&BallInterceptEstimate::getCollPos, L"getCollPos").
		func(&BallInterceptEstimate::script_getVel, L"getVel").
		func(&BallInterceptEstimate::script_getPos, L"getPos").
		func(&BallInterceptEstimate::script_getModeVel, L"getModeVel").
		func(&BallInterceptEstimate::script_getModePos, L"getModePos").
		func(&BallInterceptEstimate::script_getPathInterceptTime, L"getPathInterceptTime").
		func(&BallInterceptEstimate::script_getCollInterceptTime, L"getCollInterceptTime").
		func(&BallInterceptEstimate::script_getInterceptTime, L"getInterceptTime").
		func(&BallInterceptEstimate::script_getModeInterceptTime, L"getModeInterceptTime").
		func(&BallInterceptEstimate::script_estimateTimeUntilValidity, L"estimateTimeUntilValidity");
}

BallInterceptEstimate::FootballerInterceptState::FootballerInterceptState() {
}

void BallInterceptEstimate::FootballerInterceptState::init(Footballer& footballer) {

	distanceTolerance = footballer.mNodeMatch->mSwitchControlSetup.pathInterceptDistTolerance;

	activationTime = 0.0f;
	vel = footballer.getDefaultRunSpeed();

	penaltyTime = activationTime; //+ playerReactionTime;
}

BallInterceptEstimate::BallInterceptEstimate(bool enablePathAnalysis, bool enableCollAnalysis) {

	mEnableBallOutOfPlayEstimates = false;
	enableAnalysis(enablePathAnalysis, enableCollAnalysis);
}

void BallInterceptEstimate::enableBallOutOfPlayEstimates(bool enable) {

	mEnableBallOutOfPlayEstimates = enable;
}

void BallInterceptEstimate::enableAnalysis(bool pathAnalysis, bool collAnalysis) {

	if (pathAnalysis) {

		if (!mPathAnalysis.isValid()) {

			WE_MMemNew(mPathAnalysis.ptrRef(), PathAnalysis());
		}

	} else {

		mPathAnalysis.destroy();
	}

	if (collAnalysis) {

		if (!mCollAnalysis.isValid()) {

			WE_MMemNew(mCollAnalysis.ptrRef(), CollAnalysis());
		}

	} else {

		mCollAnalysis.destroy();
	}
}

BallInterceptEstimate::AnalysisMode BallInterceptEstimate::analyseValidity() {

	bool isPathValid = mPathAnalysis.isValid() && this->isPathValid();
	bool isCollValid = mCollAnalysis.isValid() && this->isCollValid();

	return (isPathValid && isCollValid) ? Valid_Both : (isPathValid ? Analyse_Path : (isCollValid ? Analyse_Coll : Analyse_None));
}

bool BallInterceptEstimate::setSimulation(BallSimulation& _simul, const float& time) {

	mSimul = &_simul;
	mSimulID = _simul.getID();

	return _simul.isValidSync();
}

void BallInterceptEstimate::Analysis::prepare(const float& time) {

	estimateType = Estimate_None;
	analysisTime = time;
}



bool BallInterceptEstimate::validatePath(const Time& currTime, BallSimulation* pSimul, Ball* pBall, bool setEstimateType, bool& outTryEstimates) {

	PathAnalysis& analysis = mPathAnalysis.dref();

	if (!mEnableBallOutOfPlayEstimates 
		&& !pBall->isInPlay()) {

		outTryEstimates = false;

	} else {

		switch (analysis.estimateType) {

			case Estimate_SimulInterceptorWait: {

				if (pSimul && pSimul->isValidSync() 
						&& mSimulID == pSimul->getID()	
						&& currTime - analysis.analysisTime <= analysis.waitTime
						&& currTime <= getPathInterceptTime()) {

					return true;
				}
			} break;

			case Estimate_SimulBallWait: {

				if (pSimul && pSimul->isValidSync() 
						&& mSimulID == pSimul->getID()	
						&& currTime - analysis.analysisTime <= -analysis.waitTime
						&& currTime <= getPathInterceptTime()) {

					return true;
				}
			} break;

			case Estimate_SimulLastBallWait: {

				if (pSimul && pSimul->isValidSync() 
						&& mSimulID == pSimul->getID()	
						&& currTime - analysis.analysisTime <= -analysis.waitTime) {

					return true;
				}

			} break;

			case Estimate_RestingBall: {

				if (pBall && pBall->isResting() 
						&& pBall->getRestingID() == analysis.restingID) {

					return true;
				}
			} break;
		}
	}

	if (setEstimateType)
		analysis.estimateType = Estimate_None;

	return false;
}

bool BallInterceptEstimate::validateColl(const Time& currTime, BallSimulation* pSimul, Ball* pBall, bool setEstimateType, bool& outTryEstimates) {

	CollAnalysis& analysis = mCollAnalysis.dref();

	if (!mEnableBallOutOfPlayEstimates 
		&& !pBall->isInPlay()) {

		outTryEstimates = false;

	} else {

		switch (analysis.estimateType) {

			case Estimate_SimulInterceptorWait: {

				if (pSimul && pSimul->isValidSync() 
						&& mSimulID == pSimul->getID()	
						&& currTime - analysis.analysisTime <= analysis.waitTime
						&& currTime <= getCollInterceptTime()) {

					return true;
				}
			} break;
		}
	}

	if (setEstimateType)
		analysis.estimateType = Estimate_None;

	return false;
}

BallInterceptEstimate::EstimateType BallInterceptEstimate::Analysis::analyzeSampleIntercept(Footballer& footballer, const FootballerInterceptState& footballerState, const Vector3& samplePos, const Time& sampleTime, const Time& currTime, float& outArrivalTime, float& outFootballerWaitTime) {

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

bool BallInterceptEstimate::PathAnalysis::setSample(BallSimulation& simul, const BallSimulation::Index& _sampleIndex) {

	if (isSingleSample && _sampleIndex == sampleIndex[0])
		return false;

	isSingleSample = true;
	sampleIndex[0] = _sampleIndex;

	return true;
}

bool BallInterceptEstimate::PathAnalysis::setSample(BallSimulation& simul, const BallSimulation::Index& fromIndex, const BallSimulation::Index& toIndex, const Vector3& pos, const float& u) {

	if (!isSingleSample 
			&& (fromIndex == sampleIndex[0]) && (toIndex == sampleIndex[1])
			&& interceptPos.equals(pos)) {

		return false;
	}
		

	isSingleSample = false;

	sampleIndex[0] = fromIndex;
	sampleIndex[1] = toIndex;

	interceptPos = pos;
	lerp(simul.getSample(fromIndex).vel, simul.getSample(toIndex).vel, u, interceptBallVel);

	return true;
}

bool BallInterceptEstimate::PathAnalysis::setSample(Ball& ball) {

	if (ball.isResting()) {

		if (restingID == ball.getRestingID()) 
			return false;

		restingID = ball.getRestingID();
		interceptPos = ball.getPos();
		interceptBallVel.zero();

	} else {

		if (interceptPos.equals(ball.getPos()))
			return false;

		interceptPos = ball.getPos();
		interceptBallVel = dref(ball.getVel());
	}

	return true;
}

CtVector3* BallInterceptEstimate::PathAnalysis::getPos(BallSimulation* pSimul) {

	switch (estimateType) {

		case Estimate_SimulInterceptorWait:
		case Estimate_SimulBallWait:
		case Estimate_SimulLastBallWait:{

			if (isSingleSample)
				return &pSimul->getSampleRef(sampleIndex[0]).pos;

			return &interceptPos;

		} break;

		case Estimate_CurrBall:
		case Estimate_RestingBall: {

			return &interceptPos;

		} break;
	}

	return NULL;
}

CtVector3* BallInterceptEstimate::PathAnalysis::getVel(BallSimulation* pSimul) {

	switch (estimateType) {

		case Estimate_SimulInterceptorWait:
		case Estimate_SimulBallWait:
		case Estimate_SimulLastBallWait: {

			if (isSingleSample)
				return &pSimul->getSampleRef(sampleIndex[0]).vel;

			return &interceptBallVel;

		} break;

		case Estimate_CurrBall:
		case Estimate_RestingBall: {

			return &interceptBallVel;

		} break;
	}

	return NULL;
}


CtVector3* BallInterceptEstimate::CollAnalysis::getPos(BallSimulation* pSimul) {

	switch (estimateType) {

		case Estimate_SimulInterceptorWait: {

			return &pSimul->getCollSampleRef(sampleIndex).pos;
		} break;
	}

	return NULL;
}

CtVector3* BallInterceptEstimate::CollAnalysis::getVel(BallSimulation* pSimul) {

	switch (estimateType) {

		case Estimate_SimulInterceptorWait: {

			return &pSimul->getCollSampleRef(sampleIndex).postVel;
		} break;

	}

	return NULL;
}

BallInterceptEstimate::EstimateType BallInterceptEstimate::PathAnalysis::tryEstimateRestingBall(Ball& ball, Footballer& footballer, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType) {

	setAnalysisTime(currTime);

	if (!ball.isResting()) {

		if (setEstimateType)
			estimateType = Estimate_None;

		return Estimate_None;
	}

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	
	analyzeSampleIntercept(footballer, *pFootballerState, ball.getPos(), currTime, currTime, arrivalTime, waitTime);
	estimateChanged = setSample(ball);
	
	EstimateType type = Estimate_RestingBall;

	if (setEstimateType)
		estimateType = type;

	return type;
}

BallInterceptEstimate::EstimateType BallInterceptEstimate::PathAnalysis::tryEstimateCurrBall(Ball& ball, Footballer& footballer, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType) {

	setAnalysisTime(currTime);

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	
	analyzeSampleIntercept(footballer, *pFootballerState, ball.getPos(), currTime, currTime, arrivalTime, waitTime);
	estimateChanged = setSample(ball);

	EstimateType type = Estimate_CurrBall;

	if (setEstimateType)
		estimateType = type;

	return type;
}


BallInterceptEstimate::EstimateType BallInterceptEstimate::PathAnalysis::tryEstimateLastSimulSampleIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType) {

	setAnalysisTime(currTime);

	if (simul.isSyncExpired() || simul.getFootballerReachableIntervalCount() == 0) {

		if (setEstimateType)
			estimateType = Estimate_None;

		return Estimate_None;
	}

	BallSimulation::Index lastReachableIntervalIndex = simul.getFootballerReachableIntervalCount() - 1;

	const BallSimulation::SampleInterval& reachableInterval = simul.getFootballerReachableInterval(lastReachableIntervalIndex);

	BallSimulation::Index sampleIndex = tmax(simul.getSyncFlowTrajSampleIndex(), reachableInterval.to);
	const BallSimulation::Sample& sample = simul.getSample(sampleIndex);

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	estimateChanged = setSample(simul, sampleIndex) || estimateType == Estimate_None;
	EstimateType type = analyzeSampleIntercept(footballer, *pFootballerState, sample.pos, sample.time, currTime, arrivalTime, waitTime);
	
	if (type == Estimate_SimulBallWait)
		type = Estimate_SimulLastBallWait;

	if (setEstimateType)
		estimateType = type;

	return type;
}


BallInterceptEstimate::EstimateType BallInterceptEstimate::PathAnalysis::tryEstimateEarliestSimulIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType) {

	setAnalysisTime(currTime);

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

			if (analyzeSampleIntercept(footballer, *pFootballerState, fromSample.pos, fromSample.time, currTime, arrivalTime, waitTime) == Estimate_SimulInterceptorWait) {
				
				estimateChanged = setSample(simul, i1);
				EstimateType type = Estimate_SimulInterceptorWait;
				
				if (setEstimateType)
					estimateType = type;

				return type;
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

					if (analyzeSampleIntercept(footballer, *pFootballerState, closestPt, closestTime, currTime, arrivalTime, waitTime) == Estimate_SimulInterceptorWait) {

						estimateChanged = setSample(simul, iFrom , iTo, closestPt, u);
						EstimateType type = Estimate_SimulInterceptorWait;
				
						if (setEstimateType)
							estimateType = type;

						return type;
					}
				} 
			}
		}
	}

	EstimateType type = Estimate_None;
				
	if (setEstimateType)
		estimateType = type;

	return type;
}

BallInterceptEstimate::EstimateType BallInterceptEstimate::CollAnalysis::tryEstimateEarliestCollSimulIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType) {

	setAnalysisTime(currTime);

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	FootballerInterceptState footballerState;

	if (!pFootballerState) {

		footballerState.init(footballer);
		pFootballerState = &footballerState;
	}

	for (BallSimulation::Index i0 = simul.getSyncFlowCollSampleIndex();
			i0 < simul.getCollSampleCount(); ++i0) {

		const BallSimulation::CollSample& sample = simul.getCollSample(i0);

		if (analyzeSampleIntercept(footballer, *pFootballerState, sample.pos, sample.time, currTime, arrivalTime, waitTime) == Estimate_SimulInterceptorWait) {
			
			estimateChanged = sampleIndex != i0;
			sampleIndex = i0;

			EstimateType type = Estimate_SimulInterceptorWait;
				
			if (setEstimateType)
				estimateType = type;

			return type;
		}
	}

	EstimateType type = Estimate_None;
				
	if (setEstimateType)
		estimateType = type;

	return type;
}


BallInterceptEstimate::UpdateResult BallInterceptEstimate::updatePath(Ball& ball, Footballer& footballer, const Time& currTime, BallSimulation* pSimul, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos) {

	//String::debug(L"*******\nupdate: %u\n", footballer.mNumber);

	PathAnalysis& analysis = mPathAnalysis;
	bool tryEstimates = true;
	
	if (validatePath(currTime, pSimul, &ball, false, tryEstimates)) {

		//String::debug(L"no change\n");
		return Update_NoChange;
	}

	if (tryEstimates) {

		bool estFound = false;
		bool estimateChanged = true;
		EstimateType newEstType;
		FootballerInterceptState footballerState;

		footballerState.init(footballer);
		
		if (pSimul && pSimul->isValidSync()) {

			if ((newEstType = analysis.tryEstimateEarliestSimulIntercept(ball, footballer, *pSimul, currTime, &footballerState, estimateChanged, false)) != Estimate_None) {

				estFound = true;
				setSimulation(*pSimul, currTime);
			}

			if (!estFound
				&& fallbackLastSimulSample
				&& (newEstType = analysis.tryEstimateLastSimulSampleIntercept(ball, footballer, *pSimul, currTime, &footballerState, estimateChanged, false)) != Estimate_None) {

				estFound = true;
				setSimulation(*pSimul, currTime);
			}
		}

		if (!estFound
			&& fallbackRestingPos
			&& (newEstType = analysis.tryEstimateRestingBall(ball, footballer, currTime, &footballerState, estimateChanged, false)) != Estimate_None) {

			estFound = true;
		}

		if (!estFound
			&& fallbackCurrPos
			&& (newEstType = analysis.tryEstimateCurrBall(ball, footballer, currTime, &footballerState, estimateChanged, false)) != Estimate_None) {

			estFound = true;
		}

		if (estFound) {
			
			estimateChanged = estimateChanged || newEstType != analysis.estimateType; 
			analysis.estimateType = newEstType;
				
			return estimateChanged ? Update_NewEstimate : Update_NoChange;
		}
	}

	analysis.estimateType = Estimate_None;
	return Update_Invalidated;
}

BallInterceptEstimate::UpdateResult BallInterceptEstimate::updateColl(Ball& ball, Footballer& footballer, const Time& currTime, BallSimulation* pSimul) {

	//String::debug(L"*******\nupdate: %u\n", footballer.mNumber);

	CollAnalysis& analysis = mCollAnalysis;
	bool tryEstimates = true;

	if (validateColl(currTime, pSimul, &ball, false, tryEstimates)) {

		//String::debug(L"no change\n");
		return Update_NoChange;
	}

	if (tryEstimates) {

		bool estFound = false;
		bool estimateChanged = true;
		EstimateType newEstType;
		FootballerInterceptState footballerState;

		footballerState.init(footballer);
		
		if (pSimul && pSimul->isValidSync()) {

			if ((newEstType = analysis.tryEstimateEarliestCollSimulIntercept(ball, footballer, *pSimul, currTime, &footballerState, estimateChanged, false)) != Estimate_None) {

				setSimulation(*pSimul, currTime);
				estFound = true;
			}
		}

		if (estFound) {
			
			estimateChanged = estimateChanged || newEstType != analysis.estimateType; 
			analysis.estimateType = newEstType;
				
			return estimateChanged ? Update_NewEstimate : Update_NoChange;
		}
	}

	analysis.estimateType = Estimate_None;
	return Update_Invalidated;
}

float BallInterceptEstimate::estimateTimeUntilValidity(Footballer& footballer) {

	Match& match = footballer.mNodeMatch;
	BallSimulation& simul = match.getFootballerBallInterceptManager().getSimul();

	if (simul.isValidSync()) 
		return 0.0f;	

	if (simul.isSimulApproximate())
		return -1.0f;

	if (simul.isProcessingSync()) {

		return simul.getMaxRemainingSimulTime(match.getBall());
	}

	return -1.0f;
}

float BallInterceptEstimate::script_estimateTimeUntilValidity(ScriptedFootballer* pFootballer) {

	return estimateTimeUntilValidity((Footballer&) dref(pFootballer));
}

int BallInterceptEstimate::script_updatePath(ScriptedFootballer* pFootballer, Time currTime, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos) {

	Footballer& footballer = (Footballer&) dref(pFootballer);
	Match& match = footballer.mNodeMatch;

	return updatePath(match.getBall(), 
						footballer, 
						currTime, 
						match.getFootballerBallInterceptManager().getValidSimul(), 
						fallbackLastSimulSample, fallbackRestingPos, fallbackCurrPos);
}

int BallInterceptEstimate::script_updateColl(ScriptedFootballer* pFootballer, Time currTime) {

	Footballer& footballer = (Footballer&) dref(pFootballer);
	Match& match = footballer.mNodeMatch;

	return updateColl(match.getBall(), 
						footballer, 
						currTime, 
						match.getFootballerBallInterceptManager().getValidSimul());
}

} }