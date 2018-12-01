#include "WOFMatchControlSwitchEvaluator_BallPathIntercept.h"

#include "../WOFMatch.h"
#include "../WOFMatchObjectIncludes.h"

namespace WOF { namespace match {

ControlSwitchEvaluator_BallPathIntercept::ControlSwitchEvaluator_BallPathIntercept() {

	mFrameProcessingSetup.mode = FootballerMultiFrameProcessingSetup::M_SingleFrame;
}

void ControlSwitchEvaluator_BallPathIntercept::init(Match& match) {

	mFrameProcessingSetup.init(match.getTime(), 30.0f, 3, true);
	mCurrSimulID = -1;
}

void ControlSwitchEvaluator_BallPathIntercept::frameMove(Match& match, const Time& time, SwitchControl::ValueValidityState& validity) {

	SoftPtr<BallSimulManager::Simul> simul = match.getBallSimulManager().getCurrSimul();

	bool simulIsValid;

	if (simul.isValid() && simul->isValid() && !simul->isPartial()) {

		if (simul->getID() != mCurrSimulID) {

			onSimulChanged(match, simul);
		}

		simulIsValid = (mTrajectories.count > 0);

	} else {

		simulIsValid = false;
	}
		

	if (!simulIsValid) {

		validity.isValid[SwitchControl::VID_BallPathInterceptTime] = false;
		validity.isValid[SwitchControl::VID_BallPathInterceptDist] = false;
		validity.isValid[SwitchControl::VID_RestBallDist] = false;

		return;

	} else {

		validity.isValid[SwitchControl::VID_BallPathInterceptTime] = true;
		validity.isValid[SwitchControl::VID_BallPathInterceptDist] = true;
		validity.isValid[SwitchControl::VID_RestBallDist] = true;

	}

	if (!mFootballerIterator.isValid())
		return;

	FootballerIndex footballerCount = mFootballerIterator->getFootballerCount();

	if (mFootballerIteratorIsDirty) {

		mFrameProcessingState.reset(mFrameProcessingSetup, footballerCount);
		mFootballerIteratorIsDirty = false;
	}

	FootballerMultiFrameIter iter;
	SoftPtr<Footballer> footballer;

	CandidateState interceptState;

	if (iter.prepare(mFrameProcessingSetup, mFrameProcessingState, mFootballerIterator)) {

		while (iter.next(mFootballerIterator, footballer.ptrRef())) {

			if (interceptState.initByTrajectory(match, footballer, simul, *this)) {

				{
					SwitchControl::Value& value = footballer->mSwitchState.computed[SwitchControl::VID_BallPathInterceptTime];

					value.isValid = true;
					value.time = time;
					value.val = interceptState.intersection.encounterTime;
					value.endTime = simul->getStartTime() + interceptState.intersection.encounterTime;
				}

				{
					SwitchControl::Value& value = footballer->mSwitchState.computed[SwitchControl::VID_BallPathInterceptDist];

					value.isValid = true;
					value.time = time;
					value.val = interceptState.intersection.footballerArrivalDist;
					value.endTime = simul->getStartTime() + interceptState.intersection.encounterTime;
				}

			} else {

				{
					SwitchControl::Value& value = footballer->mSwitchState.computed[SwitchControl::VID_BallPathInterceptTime];

					value.isValid = false;
				}

				{
					SwitchControl::Value& value = footballer->mSwitchState.computed[SwitchControl::VID_BallPathInterceptDist];

					value.isValid = false;
				}
			}

			if (interceptState.initByTrajectoryEndSample(match, footballer, simul, *this)) {

				{
					SwitchControl::Value& value = footballer->mSwitchState.computed[SwitchControl::VID_RestBallDist];

					value.isValid = true;
					value.time = time;
					value.val = interceptState.intersection.footballerArrivalDist;
					value.endTime = simul->getStartTime() + interceptState.intersection.encounterTime;
				}

			} else {

				{
					SwitchControl::Value& value = footballer->mSwitchState.computed[SwitchControl::VID_RestBallDist];

					value.isValid = false;
				}
			}
		}
	}
}



void ControlSwitchEvaluator_BallPathIntercept::addTrajectory(Match& match, BallSimulManager::Simul& simul, SampleIndex& startIndex, SampleIndex& endIndex) {

	if (startIndex != endIndex) {

		const BallTrajectory::Sample& s1 = dref(simul.getSample(startIndex));
		const BallTrajectory::Sample& s2 = dref(simul.getSample(endIndex));

		Vector3 t;
		Vector3 n;
		
		s1.pos.subtract(s2.pos, t);

		if (!t.isZero()) {

			t.normalize();
			t.cross(match.getCoordAxis(Scene_Up), n);

			//test probably needs isZero(threshold)

			if (n.isZero()) {

				n = match.getCoordAxis(Scene_Up);
			}

			if (!n.isZero()) {

				StraightTrajectory& traj = mTrajectories.addOne();

				traj.startIndex = startIndex;
				traj.endIndex = endIndex;

				traj.plane.init(s1.pos, n);
			}

		} else {

			assrt(false);
		}
	}
}

void ControlSwitchEvaluator_BallPathIntercept::getIntervalIntersection(const SampleInterval& interval1, const SampleInterval& interval2, IntervalIntersection& intersection) {

	if (interval1.startIndex <= interval2.endIndex) {

		if (interval1.endIndex >= interval2.startIndex) {

			intersection.isValid = true;

			if (interval1.startIndex <= interval2.startIndex) {

				intersection.startIndex = interval2.startIndex;

			} else {

				intersection.startIndex = interval1.startIndex;
			}

			if (interval1.endIndex <= interval2.endIndex) {

				intersection.nextAdvanceFirstInterval = true;
				intersection.endIndex = interval1.endIndex;

			} else {

				intersection.nextAdvanceFirstInterval = false;
				intersection.endIndex = interval2.endIndex;
			}

		} else {

			intersection.isValid = false;
			intersection.nextAdvanceFirstInterval = true;
		}

	} else {

		intersection.isValid = false;
		intersection.nextAdvanceFirstInterval = false;
	}
}

void ControlSwitchEvaluator_BallPathIntercept::onSimulChanged(Match& match, BallSimulManager::Simul& simul) {

	mTrajectories.count = 0;

	if (simul.getSampleCount() >= 2) {

		SampleIndex startIndex;
		SampleIndex endIndex = -1;

		EventIndex evtCount = simul.getEventCount();

		for (EventIndex i = 0; i < evtCount; ++i) {

			startIndex = endIndex + 1;

			const BallSimulManager::Simul::Event& evt = dref(simul.getEvent(i));

			if (evt.type == BallSimulManager::Simul::ET_Discontinuity) {

				Vector3 dir1;
				Vector3 dir2;
				bool isDiscontinuity;

				if (simul.analyzeDiscontinuity(evt.sampleIndex, evt.relSampleIndex, dir1, dir2, isDiscontinuity)) {

					if (isDiscontinuity && evt.sampleIndex >= 1) {

						//test if dir1,dir2 plane is almost vertical, if yes dont treat as discont.
						{
							Vector3 normal;

							dir1.cross(dir2, normal);

							if (!normal.isZero()) {

								const float tolerance = cosf(degToRad(3.0f));

								if (fabsf(normal.dot(match.getCoordAxis(Scene_Up)) <= tolerance)) {

									isDiscontinuity = false;

								}
							}
						}	

						//test for total dir reversal in this case even if vertical plane
						//a new start and end time are needed for the new trajectory
						if (!isDiscontinuity) {

							if (dir1.dot(dir2) <= 0.0f) {

								isDiscontinuity = true;
							}
						}

						if (isDiscontinuity) {
							
							endIndex = evt.sampleIndex - 1;

							if (endIndex  < simul.getSampleCount()) {

								addTrajectory(match, simul, startIndex, endIndex);
							}
						}
					}
				}
			}
		}

		startIndex = endIndex + 1;

		if (startIndex  < simul.getSampleCount()) {

			endIndex = simul.getSampleCount() - 1;

			addTrajectory(match, simul, startIndex, endIndex);
		}

	} else {

		
	}


	mAccessibleHeightIntervals.count = 0;

	{

		SampleIndex sampleCount = simul.getSampleCount();

		bool isStarted = false;
		SampleIndex startIndex;

		float maxPassHeight = match.mFootballerSwitchSetup.maxPassHeight;

		for (SampleIndex i = 0; i < sampleCount; ++i) {

			const BallSimulManager::Simul::Sample& sample = dref(simul.getSample(i));

			if (sample.pos.el[Scene_Up] <= maxPassHeight) {

				if (!isStarted) {

					isStarted = true;
					startIndex = i;
				}

			} else {

				if (isStarted) {

					if (i >= 1) {

						SampleInterval& interval = mAccessibleHeightIntervals.addOne();

						interval.startIndex = startIndex;
						interval.endIndex = i - 1;
					}

					isStarted = false;
				}
			}
		}

		if (isStarted && (startIndex < sampleCount)) {

			SampleInterval& interval = mAccessibleHeightIntervals.addOne();

			interval.startIndex = startIndex;
			interval.endIndex = sampleCount - 1;
		}
	}


	mCurrSimulID = simul.getID();
}

ControlSwitchEvaluator_BallPathIntercept::FootballerState::FootballerState(Match& match) {

	distanceTolerance = match.mFootballerSwitchSetup.distanceTolerance;
	playerReactionTime = match.mFootballerSwitchSetup.playerReactionTime; 
}

void ControlSwitchEvaluator_BallPathIntercept::FootballerState::init(Footballer& footballer) {

	vel = footballer.body_toVelocity_Running(Footballer::BR_Normal);
	activationTime = footballer.body_getStateSwitchTime(Footballer::BS_Running);
	penaltyTime = activationTime + playerReactionTime;
}

ControlSwitchEvaluator_BallPathIntercept::IntersectionInfo* ControlSwitchEvaluator_BallPathIntercept::chooseBetterIntersection(IntersectionInfo& info1, IntersectionInfo& info2) {

	return info1.encounterTime < info2.encounterTime ? &info1 : &info2;
}

void ControlSwitchEvaluator_BallPathIntercept::getIntersectionTime(Footballer& footballer, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulManager::Simul::Sample& sample, const bool& ballIsWaiting, IntersectionInfo& result, bool invalidateOnFootballerLate) const {

	//for now dont take optimal height into account

	result.sampleIndex = sampleIndex;

	const Point& pos = footballer.mTransformWorld.getPosition();

	float dist = distance(pos, sample.pos);

	result.footballerArrivalDist = dist;

	if (dist <= footballerState.distanceTolerance) {

		result.footballerArrivalTime = 0.0f;

	} else {

		result.footballerArrivalTime = footballerState.penaltyTime + (dist) / footballerState.vel;
	}

	if (sample.time > result.footballerArrivalTime) {

		result.type = IT_FootballerWait;
		result.isValidEncounter = true;
		result.encounterTime = sample.time;

	} else {

		if (ballIsWaiting) {

			result.type = IT_BallWait;
			
			result.isValidEncounter = true;
			result.encounterTime = result.footballerArrivalTime;

		} else {

			result.type = IT_FootballerLate;

			result.isValidEncounter = false;
			result.encounterTime = FLT_MAX;
			result.lateTime = result.footballerArrivalTime - sample.time;
		}
	}

	if (invalidateOnFootballerLate && result.type == IT_FootballerLate)
		result.isValid = false;
	else
		result.isValid = true;
}

bool ControlSwitchEvaluator_BallPathIntercept::getIntersectionTime(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, const SampleInterval& interval, BallSimulManager::Simul& simul, IntersectionInfo& result) const {

	enum SampleType {

		ST_Start, ST_End, ST_Closest
	};

	int interCount = 0;

	IntersectionInfo inter[3]; //beginning, end and closest samples

	const BallSimulManager::Simul::Sample& startSample = dref(simul.getSample(interval.startIndex));
	const BallSimulManager::Simul::Sample& endSample = dref(simul.getSample(interval.endIndex));

	getIntersectionTime(footballer, footballerState, interval.startIndex, startSample, false, inter[ST_Start], true);
	++interCount;

	getIntersectionTime(footballer, footballerState, interval.endIndex, endSample, false, inter[ST_End], true);
	++interCount;

	float closestDistSq;
	float u;
	Point closestPt;

	const Point& footballerPos = footballer.mTransformWorld.getPosition();

	closestDistSq = distanceSq(startSample.pos, endSample.pos, footballerPos, closestPt, u);

	bool isBetweenPoints = (u >= 0.0f) && (u <= 1.0f);

	if (isBetweenPoints) {

		float factor = sqrtf(distanceSq(closestPt, startSample.pos) / distanceSq(startSample.pos, endSample.pos));
		int closestIndex = lerp((int) interval.startIndex, (int) interval.endIndex, factor);

		if ((closestIndex > interval.startIndex) && 
			(closestIndex < interval.endIndex)) {

			getIntersectionTime(footballer, footballerState, closestIndex, dref(simul.getSample(closestIndex)), false, inter[ST_Closest], true);
			++interCount;
		}
	}

	if (interCount) {

		IntersectionInfo* pBest;

		pBest = &(inter[0]);

		for (int i = 1; i < interCount; ++i) {

			pBest = chooseBetterIntersection(*pBest, inter[i]);
		}

		if (pBest->isValidEncounter) {

			result = *pBest;

			return true;
		}

		return false;
	} 

	return false;
}

bool ControlSwitchEvaluator_BallPathIntercept::CandidateState::initByTrajectoryEndSample(Match& match, Footballer* pFootballer, BallSimulManager::Simul& simul, ControlSwitchEvaluator_BallPathIntercept& state) {

	intersection.isValid = false;

	if (simul.getSampleCount() > 0) {

		Footballer& footballer = dref(pFootballer);

		FootballerState footballerState(match);
		footballerState.init(footballer);

		Trajectories::Index trajIndex = state.mTrajectories.count - 1;

		const StraightTrajectory* pTraj = &(state.mTrajectories.el[trajIndex]);

		SampleIndex sampleIndex = simul.getSampleCount() - 1;
		const BallSimulManager::Simul::Sample& sample = dref(simul.getSample(sampleIndex));

		state.getIntersectionTime(footballer, footballerState, sampleIndex, sample, true, this->intersection, true);

		return this->intersection.isValidEncounter;
	}

	return false;
}

bool ControlSwitchEvaluator_BallPathIntercept::CandidateState::initByTrajectory(Match& match, Footballer* pFootballer, BallSimulManager::Simul& simul, ControlSwitchEvaluator_BallPathIntercept& state) {

	intersection.isValid = false;

	if (state.mTrajectories.count > 0) {

		Footballer& footballer = dref(pFootballer);

		FootballerState footballerState(match);
		footballerState.init(footballer);

		Trajectories::Index trajIndex = 0;
		const StraightTrajectory* pTraj;
		const SampleInterval* pInterval;
		IntervalIntersection intersection;

		for (AccessibleHeightIntervals::Index i = 0; i < state.mAccessibleHeightIntervals.count; ++i) {

			bool repeat;
			bool isLastSample;

			do {

				repeat = false;

				pInterval = &(state.mAccessibleHeightIntervals.el[i]);
				pTraj = &(state.mTrajectories.el[trajIndex]);
				
				state.getIntervalIntersection(*pInterval, *pTraj, intersection);

				if (intersection.nextAdvanceFirstInterval == false) {

					trajIndex++;

					repeat = (trajIndex < state.mTrajectories.count);
				}

				isLastSample = (!repeat) && (i >= state.mAccessibleHeightIntervals.count - 1);

				if (intersection.isValid) {

					if (state.getIntersectionTime(*pTraj, footballer, footballerState, intersection, simul, this->intersection)) {

						return true;
					}
				} 
				
				

			} while (repeat);
		}
	}

	return false;
}


} }