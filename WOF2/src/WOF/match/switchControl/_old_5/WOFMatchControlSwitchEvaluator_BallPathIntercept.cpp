#include "WOFMatchControlSwitchEvaluator_BallPathIntercept.h"

#include "../WOFMatch.h"
#include "../WOFMatchObjectIncludes.h"
#include "../WOFMatchFootballerIteratorImpl.h"

namespace WOF { namespace match {

ControlSwitchEvaluator_BallPathIntercept::ControlSwitchEvaluator_BallPathIntercept() {

	mFrameProcessingSetup.mode = FootballerMultiFrameProcessingSetup::M_SingleFrame;
}

void ControlSwitchEvaluator_BallPathIntercept::init(Match& match) {

	mState = State_None;
	mFrameProcessingSetup.init(match.getTime(), 30.0f, 1, true);
	mCurrSimulID = -1;
}


ControlSwitchEvaluator_BallPathIntercept::State 
ControlSwitchEvaluator_BallPathIntercept::update(Match& match, const Time& time) {

	SoftPtr<BallSimulation> simul = match.getBall().getSimul();
	
	if (!simul.isValid()) {

		mState = State_Invalid;
		return mState;
	}

	bool updateFootballers = false;

	if (simul.isValid() && simul->isValid()) {

		if (simul->isPartial()) {

			mState = State_WaitingForSimul;

		} else {

			if (simul->getID() != mCurrSimulID) {

				onSimulChanged(match, simul);
				updateFootballers = true;

			} else {

				updateFootballers = false;
			}

			if (mTrajectories.count > 0) {

				Time endTime;
				simul->getEndTime(endTime);

				if (endTime <= time) {

					mState = State_Expired;

				} else {

					mState = State_Valid;
				}

			} else {

				mState = State_Expired;
			}
		}

	} else {

		mState = State_Invalid;
	}

	if ((mState != State_Valid) || !updateFootballers)
		return mState;

	if (!mFootballerIterator.isValid())
		return mState;

	FootballerIndex footballerCount = mFootballerIterator->getFootballerCount();

	if (mFootballerIteratorIsDirty) {

		mFrameProcessingState.reset(mFrameProcessingSetup, footballerCount);
		mFootballerIteratorIsDirty = false;
	}

	FootballerMultiFrameIterOneShot iter(mFrameProcessingSetup, mFrameProcessingState, mFootballerIterator);
	SoftPtr<Footballer> footballer;

	//CandidateState interceptState;

	if (iter.startFrame()) {

		Time simulTimeShift = simul->getStartTime() - time;

		while (iter.next(footballer.ptrRef())) {

			SwitchControl::Value_PathIntercept& value = footballer->mSwitchState.value_pathIntercept;

			value.simulID = mCurrSimulID;

			findIntersections(match, footballer, simul, value.shortestTimeIntercept, value.shortestDistIntercept, simulTimeShift);
			getEndSampleIntersection(match, footballer, simul, value.restBallIntercept, simulTimeShift);

			{
				#ifdef _DEBUG
				if (value.shortestTimeIntercept.isValid && value.shortestDistIntercept.isValid) {

					if (value.shortestTimeIntercept.interceptTime > value.shortestDistIntercept.interceptTime) {

						assrt(false);
					}
				}

				if (value.shortestDistIntercept.isValid && !value.shortestTimeIntercept.isValid) {

					assrt(false);
				}
				#endif
			}

			
			const Time timeTolerance = 0.2f;

			if (value.restBallIntercept.equals(value.shortestTimeIntercept, timeTolerance)) {

				value.shortestTimeIntercept.isValid = false;
			}

			if (value.restBallIntercept.equals(value.shortestDistIntercept, timeTolerance)) {

				value.shortestDistIntercept.isValid = false;
			}
			
			/*
			String dbgStr;

			dbgStr.assignEx(L"foot %d %f", footballer->mNumber, value.shortestTimeIntercept.isValid ? value.shortestTimeIntercept.interceptTime : -1.0f);
			match.getConsole().print(dbgStr.c_tstr());
			log(dbgStr.c_tstr());
			*/
		}
	}

	return mState;
}



void ControlSwitchEvaluator_BallPathIntercept::addTrajectory(Match& match, BallSimulation& simul, SampleIndex& startIndex, SampleIndex& endIndex) {

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

				traj.plane.init(s1.pos, n, true);
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

void ControlSwitchEvaluator_BallPathIntercept::onSimulChanged(Match& match, BallSimulation& simul) {

	mTrajectories.count = 0;

	if (simul.getSampleCount() >= 2) {

		SampleIndex startIndex;
		SampleIndex endIndex = -1;

		EventIndex evtCount = simul.getEventCount();

		for (EventIndex i = 0; i < evtCount; ++i) {

			startIndex = endIndex + 1;

			const BallSimulation::Event& evt = dref(simul.getEvent(i));

			if (evt.type == BallSimulation::ET_Discontinuity) {

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

		float footballerReachHeight = match.mSwitchControlSetup.footballerReachHeight;

		for (SampleIndex i = 0; i < sampleCount; ++i) {

			const BallSimulation::Sample& sample = dref(simul.getSample(i));

			if (sample.pos.el[Scene_Up] <= footballerReachHeight) {

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

	distanceTolerance = match.mSwitchControlSetup.pathInterceptDistTolerance;
	//playerReactionTime = match.mSwitchControlSetup.playerReactionTime; 
}

void ControlSwitchEvaluator_BallPathIntercept::FootballerState::init(Footballer& footballer) {

	#ifdef FOOTBALLER_BODY3
	activationTime = 0.0f;
	vel = footballer.getRunSpeed();
	#endif

	#ifdef FOOTBALLER_BODY2
	vel = footballer.getRunTypeSpeed(Footballer::Run_Normal);
	activationTime = footballer.estimateActionSwitchTimeTo(Footballer::Action_Running);

	if (activationTime < 0.0f)
		activationTime = 60.0f;
	#endif

	#ifdef FOOTBALLER_BODY1
	vel = footballer.body_toVelocity_Running(Footballer::BR_Normal);
	activationTime = footballer.body_getStateSwitchTime(Footballer::BS_Running);
	#endif
	
	penaltyTime = activationTime; //+ playerReactionTime;
}

ControlSwitchEvaluator_BallPathIntercept::IntersectionInfo* ControlSwitchEvaluator_BallPathIntercept::chooseBetterIntersection(IntersectionInfo& info1, IntersectionInfo& info2) {

	return info1.encounterTime < info2.encounterTime ? &info1 : &info2;
}

bool ControlSwitchEvaluator_BallPathIntercept::getIntersectionTime(Footballer& footballer, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulation::Sample& sample, const bool& ballIsWaiting, IntersectionInfo& result, bool invalidateOnFootballerLate, Time simulTimeShift, bool keepOldResultOnNotFootbalerWait) const {

	//for now dont take optimal height into account
	const Point& pos = footballer.mTransformWorld.getPosition();

	float dist = distance(pos, sample.pos);

	float footballerArrivalDist; 
	float footballerArrivalTime; 

	footballerArrivalDist = (dist - footballerState.distanceTolerance);

	if (footballerArrivalDist <= 0.0f) {

		footballerArrivalDist = 0.0f;
		footballerArrivalTime = 0.0f;

	} else {

		footballerArrivalTime = footballerState.penaltyTime + (footballerArrivalDist) / footballerState.vel;
	}

	if (sample.time + simulTimeShift > footballerArrivalTime) {

		result.type = IT_FootballerWait;
		result.isValidEncounter = true;
		result.encounterTime = sample.time;

	} else {

		if (keepOldResultOnNotFootbalerWait) {

			return false;

		} else {

			if (ballIsWaiting) {

				result.type = IT_BallWait;
				
				result.isValidEncounter = true;
				result.encounterTime = footballerArrivalTime;

			} else {

				result.type = IT_FootballerLate;

				result.isValidEncounter = false;
				result.encounterTime = FLT_MAX;
				result.lateTime = footballerArrivalTime - (sample.time + simulTimeShift);
			}
		}
	}

	if (invalidateOnFootballerLate && result.type == IT_FootballerLate)
		result.isValid = false;
	else
		result.isValid = true;

	if (result.isValidEncounter) {

		result.sampleIndex = sampleIndex;
		result.footballerArrivalDist = footballerArrivalDist;
		result.footballerArrivalTime = footballerArrivalTime;
	}

	return result.isValid;
}

void ControlSwitchEvaluator_BallPathIntercept::findValidIntersection_binarySearch(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, SampleInterval interval, BallSimulation& simul, IntersectionInfo& inter, Time simulTimeShift, bool startSampleValid) const {

	while(true) {

		SampleIndex sampleIndex = (interval.startIndex + interval.endIndex) / 2;
		const BallSimulation::Sample& sample = dref(simul.getSample(sampleIndex));

		bool hasIntersectionTime = getIntersectionTime(footballer, footballerState, sampleIndex, sample, false, inter, true, simulTimeShift, true);

		if (sampleIndex == interval.startIndex)
			break;

		if (hasIntersectionTime) {

			if (startSampleValid) {

				interval.startIndex = sampleIndex;

			} else {

				interval.endIndex = sampleIndex;
			}

		} else {

			if (startSampleValid) {

				interval.endIndex = sampleIndex;

			} else {

				interval.startIndex = sampleIndex;
			}
		}
	}
}

void ControlSwitchEvaluator_BallPathIntercept::getIntersections(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, const SampleInterval& interval, BallSimulation& simul, IntersectionInfo inter[3], int& interCount, Time simulTimeShift) const {

	interCount = 0;

	const BallSimulation::Sample& startSample = dref(simul.getSample(interval.startIndex));
	const BallSimulation::Sample& endSample = dref(simul.getSample(interval.endIndex));

	getIntersectionTime(footballer, footballerState, interval.startIndex, startSample, false, inter[IST_Start], true, simulTimeShift);
	if (inter[IST_Start].isValid)
		++interCount;

	getIntersectionTime(footballer, footballerState, interval.endIndex, endSample, false, inter[IST_End], true, simulTimeShift);
	if (inter[IST_End].isValid)
		++interCount;

	if (inter[IST_Start].isValid == inter[IST_End].isValid) {

		const bool& bothValid = inter[IST_Start].isValid;

		float closestDistSq;
		float u;
		Point closestPt;

		const Point& footballerPos = footballer.mTransformWorld.getPosition();

		closestDistSq = distanceSq(startSample.pos, endSample.pos, footballerPos, closestPt, u);

		bool isBetweenPoints = (u >= 0.0f) && (u <= 1.0f);

		if (isBetweenPoints) {

			/*
				maybe (but i doubt it) if both invalid we need here to make an extensive (binary?) search			
			*/

			float factor = u; //sqrtf(distanceSq(closestPt, startSample.pos) / distanceSq(startSample.pos, endSample.pos));
			SampleIndex closestIndex = lerp<SampleIndex>(interval.startIndex, interval.endIndex, factor);

			if ((closestIndex > interval.startIndex) && (closestIndex < interval.endIndex)) {

				getIntersectionTime(footballer, footballerState, closestIndex, dref(simul.getSample(closestIndex)), false, inter[IST_Closest], true, simulTimeShift);
				
			} else {

				inter[IST_Closest].isValid = false;
			}

		} else {

			inter[IST_Closest].isValid = false;
		}


	} else if (inter[IST_Start].isValid) {


		findValidIntersection_binarySearch(traj, footballer, footballerState, interval, simul, inter[IST_Closest], simulTimeShift, true);

		if (inter[IST_Closest].isValid) {

			if (inter[IST_Closest].sampleIndex == inter[IST_Start].sampleIndex)
				inter[IST_Closest].isValid = false;

		} else {

			assrt(false);
		}

	} else if (inter[IST_End].isValid) {

		findValidIntersection_binarySearch(traj, footballer, footballerState, interval, simul, inter[IST_Closest], simulTimeShift, false);

		if (inter[IST_Closest].isValid) {

			if (inter[IST_Closest].sampleIndex == inter[IST_End].sampleIndex)
				inter[IST_Closest].isValid = false;

		} else {

			//can happen if interval already indivisible
			//assrt(false);
		}
	} 

	if (inter[IST_Closest].isValid)
		++interCount;
	
}

/*
void ControlSwitchEvaluator_BallPathIntercept::getIntersections(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, const SampleInterval& interval, BallSimulation& simul, IntersectionInfo inter[3], int& interCount, Time simulTimeShift) const {

	interCount = 0;

	const BallSimulation::Sample& startSample = dref(simul.getSample(interval.startIndex));
	const BallSimulation::Sample& endSample = dref(simul.getSample(interval.endIndex));

	getIntersectionTime(footballer, footballerState, interval.startIndex, startSample, false, inter[IST_Start], true, simulTimeShift);
	++interCount;

	getIntersectionTime(footballer, footballerState, interval.endIndex, endSample, false, inter[IST_End], true, simulTimeShift);
	++interCount;

	float closestDistSq;
	float u;
	Point closestPt;

	const Point& footballerPos = footballer.mTransformWorld.getPosition();

	closestDistSq = distanceSq(startSample.pos, endSample.pos, footballerPos, closestPt, u);

	bool isBetweenPoints = (u >= 0.0f) && (u <= 1.0f);

	this u calculation is not really correct we are assuming footballer is static
		probably better used bisection wtih getIntersectionTime to determin a valid middle time
		if IST_Start or IST_End were valid (depending on shortest dist or shortest time)

	if (isBetweenPoints) {

		float factor = u; //sqrtf(distanceSq(closestPt, startSample.pos) / distanceSq(startSample.pos, endSample.pos));
		int closestIndex = lerp((int) interval.startIndex, (int) interval.endIndex, factor);

		if ((closestIndex > (int) interval.startIndex) && 
			(closestIndex < (int) interval.endIndex)) {

			while (!getIntersectionTime(footballer, footballerState, closestIndex, dref(simul.getSample(closestIndex)), false, inter[IST_Closest], true, simulTimeShift)) {

				++closestIndex;

				if (closestIndex == interval.endIndex)
					break;

			}

			if (inter[IST_Closest].isValid)
				++interCount;
		}
	}
}
*/

void ControlSwitchEvaluator_BallPathIntercept::IntersectionInfo::convertTo(const Time& simulStartTime, SwitchControl::PathInterception& target) {

	target.isValid = isValidEncounter;
	target.isExpired = false;

	if (isValid) {

		target.interceptTime = simulStartTime + encounterTime;
		target.interceptDist = footballerArrivalDist;
	}
}

bool ControlSwitchEvaluator_BallPathIntercept::getEndSampleIntersection(Match& match, Footballer* pFootballer, BallSimulation& simul, SwitchControl::PathInterception& result, Time simulTimeShift) {

	result.isValid = false;

	if (simul.getSampleCount() > 0) {

		Footballer& footballer = dref(pFootballer);

		FootballerState footballerState(match);
		footballerState.init(footballer);

		Trajectories::Index trajIndex = mTrajectories.count - 1;

		const StraightTrajectory* pTraj = &(mTrajectories.el[trajIndex]);

		SampleIndex sampleIndex = simul.getSampleCount() - 1;
		const BallSimulation::Sample& sample = dref(simul.getSample(sampleIndex));

		IntersectionInfo temp;

		getIntersectionTime(footballer, footballerState, sampleIndex, sample, true, temp, true, simulTimeShift);

		if (temp.isValidEncounter) {

			temp.convertTo(simul.getStartTime(), result);
		}

		return result.isValid;
	}

	return false;
}


void ControlSwitchEvaluator_BallPathIntercept::findIntersections(Match& match, Footballer* pFootballer, BallSimulation& simul, SwitchControl::PathInterception& shortestTimeIntercept, SwitchControl::PathInterception& shortestDistIntercept, Time simulTimeShift) {

	shortestTimeIntercept.isValid = false;
	shortestDistIntercept.isValid = false;

	if (mTrajectories.count > 0) {

		Footballer& footballer = dref(pFootballer);

		FootballerState footballerState(match);
		footballerState.init(footballer);

		Trajectories::Index trajIndex = 0;
		const StraightTrajectory* pTraj;
		const SampleInterval* pInterval;
		IntervalIntersection interval;

		IntersectionInfo inter[3];
		int interCount;

		Time simulStartTime = simul.getStartTime();
		
		for (AccessibleHeightIntervals::Index i = 0; i < mAccessibleHeightIntervals.count; ++i) {

			bool repeat;
			bool isLastSample;

			do {

				repeat = false;

				pInterval = &(mAccessibleHeightIntervals.el[i]);
				pTraj = &(mTrajectories.el[trajIndex]);
				
				getIntervalIntersection(*pInterval, *pTraj, interval);

				if (interval.nextAdvanceFirstInterval == false) {

					trajIndex++;

					repeat = (trajIndex < mTrajectories.count);
				}

				isLastSample = (!repeat) && (i >= mAccessibleHeightIntervals.count - 1);

				if (interval.isValid) {

					getIntersections(*pTraj, footballer, footballerState, interval, simul, inter, interCount, simulTimeShift);

					for (int i = 0, validCount = 0; i < 3 && validCount < interCount; ++i) {

						if (inter[i].isValid) {

							++validCount;

							if (!shortestTimeIntercept.isValid || (inter[i].encounterTime + simulStartTime) < shortestTimeIntercept.interceptTime) {

								inter[i].convertTo(simulStartTime, shortestTimeIntercept);
							}

							if (!shortestDistIntercept.isValid || inter[i].footballerArrivalDist < shortestDistIntercept.interceptDist) {

								inter[i].convertTo(simulStartTime, shortestDistIntercept);
							}
						}
					}
				} 
				
			} while (repeat);
		}
	}
}


} }