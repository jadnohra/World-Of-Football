#include "FootballerBallPathInterceptAnalyzedSimulation.h"

#include "../WOFMatch.h"
#include "../WOFMatchObjectIncludes.h"

namespace WOF { namespace match {

typedef FootballerBallPathInterceptAnalyzedSimulation AnalyzedSimulation;

FootballerBallPathInterceptAnalyzedSimulation::FootballerBallPathInterceptAnalyzedSimulation() {
}

void AnalyzedSimulation::init(Match& match) {

	mState = State_None;
	mCurrSimulID = -1;
}


AnalyzedSimulation::State 
AnalyzedSimulation::update(Match& match, BallSimulation2& simul, const Time& time) {

	if (!simul.isValid()) {

		mState = State_Invalid;
		return mState;
	}

	bool firstValidTime = false;
	

	if (simul.isProcessing()) {

		mState = State_WaitingForSimul;

	} else {

		if (simul.getID() != mCurrSimulID) {

			onSimulChanged(match, simul, time);
			firstValidTime = true;

		} else {

			firstValidTime = false;
		}

		if (mTrajectories.count > 0) {

			mState = State_Valid;

		} else {

			mState = State_Expired;
		}
	}

	if (mState != State_Valid)
		return mState;

	updateStartSampleIndex(match, simul, time);
	
	if (!checkValidity(match, simul, time)) {

		mState = State_Invalid;
		return mState;
	}

	return mState;
}

bool AnalyzedSimulation::checkValidity(Match& match, BallSimulation2& simul, const Time& _time) {

	/*
	Time time = _time - simul.getStartTime();

	if (mConsideredIntervals.count) {

		TrajectoryInterval& interval = mConsideredIntervals[0];

		Index preIndex = -1;
		Index postIndex = -1;

		for (Index i = interval.startIndex; i < interval.endIndex; ++i) {

			const Sample& sample = getSample(i);

			if (time == sample.time) {

				preIndex = i;
				postIndex = i;

				break;

			} else if (time > sample.time) {

				preIndex = i;

			} else {

				postIndex = i;
				break;
			}
		}

		if (preIndex != -1 && postIndex != -1) {
			
			Ball& ball = match.getBall();
			const Vector3& currPos = ball.mTransformWorld.getPosition();
			float distSq;

			if (preIndex == postIndex) {

				distSq = distanceSq(getSample(preIndex)->pos, currPos);

			} else {

				float u;
				Point closestPos;

				distSq = distanceSq(getSample(preIndex)->pos, getSample(postIndex)->pos, currPos, closestPos, u);
			}

			float maxDist = ball.getRadius();

			if (distSq > maxDist * maxDist)
				return false;
		}
	}
	*/

	return true;
}

bool AnalyzedSimulation::updateStartSampleIndex(Match& match, BallSimulation2& simul, const Time& time) {

	bool changed = false;

	for (TrajectoryIntervals::Index i = 0; i < mConsideredIntervals.count; ++i) {

		TrajectoryInterval& interval = mConsideredIntervals[i];

		const Sample& lastSample = simul.getSample(interval.endIndex);

		if (lastSample.time  < time) {

			changed = true;

			mConsideredIntervals.removeSwapWithLast(i, true);
			--i;

		} else {

			SampleIndex index = interval.startIndex;
			SampleIndex lastPreIndex = interval.startIndex;

			while (index < interval.endIndex) {

				const Sample& sample = simul.getSample(index);

				if (sample.time >= time) {

					break;

				} else {

					lastPreIndex = index;
					++index;
				}
			}

			if (interval.startIndex != lastPreIndex) {

				changed = true;			
				interval.startIndex = lastPreIndex;
			}

			break;
		}
	}

	return changed;
}

bool AnalyzedSimulation::updateShortestTimeInterceptState(Match& match, Footballer& footballer, BallSimulation2& simul, const Time& time, const SampleIndex* pStartSampleIndex) {

	const Point& footballerPos = footballer.mTransformWorld.getPosition();
	
	mTempFootballerState.init(footballer);

	const SimulationID& simulID = simul.getID();

	PathIntercept& resultIntersec = footballer.getBallPathIntercept();
	resultIntersec.prepareForAnalysis(simulID, time);


	for (TrajectoryIntervals::Index i = 0; i < mConsideredIntervals.count; ++i) {

		TrajectoryInterval& interval = mConsideredIntervals[i];
		bool considerInterval;

		if (pStartSampleIndex) {

			considerInterval = (*pStartSampleIndex <= interval.endIndex);

		} else {

			considerInterval = true;
		}

		if (considerInterval) {

			const BallSimulation2::Sample& startSample = simul.getSample(interval.startIndex);
			const BallSimulation2::Sample& endSample = simul.getSample(interval.endIndex);

			PathIntercept earlySampleIntersec;

			getIntersectionTime(footballer, simulID, mTempFootballerState, interval.startIndex, startSample, earlySampleIntersec, time, false);

			if (earlySampleIntersec.type == PI_InterceptorWait) {

				resultIntersec.setWithoutData(earlySampleIntersec);
				resultIntersec.fillData(interval.startIndex, startSample);

				return true;
				
			} else {

				if (interval.startIndex != interval.endIndex) {

					float closestDistSq;
					float u;
					Point closestPt;

					closestDistSq = distanceSq(startSample.pos, endSample.pos, footballerPos, closestPt, u);
					bool isBetweenPoints = isBetweenSegPoints(u);

					if (isBetweenPoints) {

						SampleIndex closestIndex = lerp<SampleIndex>(interval.startIndex, interval.endIndex, u);

						if (closestIndex < interval.endIndex) {

							SampleIndex nextClosestIndex = closestIndex + 1;
							const Time& closestTime = simul.getSample(nextClosestIndex).time;
							// be nice and take the time of next closest sample instead of lerping

							if (getIntersectionTime(footballer, simulID, mTempFootballerState, nextClosestIndex, closestPt, closestTime, resultIntersec, time, false, PI_InterceptorWait)) {

								resultIntersec.setData(closestTime, closestIndex, closestPt);

								return true;
							}
						}

					} 

					PathIntercept lateSampleIntersec;

					getIntersectionTime(footballer, simulID, mTempFootballerState, interval.endIndex, endSample, lateSampleIntersec, time, false);

					if (lateSampleIntersec.type == PI_InterceptorWait) {

						//we could do a binary search instead
						resultIntersec.setWithoutData(lateSampleIntersec);
						SampleIndex searchIndex = interval.endIndex - 1;

						while (searchIndex > interval.startIndex) {

							if (!getIntersectionTime(footballer, simulID, mTempFootballerState, searchIndex, simul.getSample(searchIndex), resultIntersec, time, false, PI_InterceptorWait)) {

								resultIntersec.fillData(simul, searchIndex + 1);

								return true;
							}

							--searchIndex;
						}

						//strange, shoudn't happen
						resultIntersec.sampleIndex = searchIndex;
						return true;
					} 
				}
			}
		}
	}

	return false;
}


void AnalyzedSimulation::addTrajectory(Match& match, BallSimulation2& simul, SampleIndex& startIndex, SampleIndex& endIndex) {

	if (startIndex != endIndex) {

		const BallSimulation2::Sample& s1 = simul.getSample(startIndex);
		const BallSimulation2::Sample& s2 = simul.getSample(endIndex);

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

			//assrt(false);
		}
	}
}

void AnalyzedSimulation::getIntervalIntersection(const SampleInterval& interval1, const SampleInterval& interval2, IntervalIntersection& intersection) {

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

void AnalyzedSimulation::onSimulChanged(Match& match, BallSimulation2& simul, const Time& time) {

	mTrajectories.count = 0;

	if (simul.getSampleCount() >= 2) {

		SampleIndex startIndex;
		SampleIndex endIndex = -1;

		ok most of this is not needed anymore, rtransfer the rest to BallSimulation2...

		EventIndex evtCount = simul.getEventCount();

		for (EventIndex i = 0; i < evtCount; ++i) {

			startIndex = endIndex + 1;

			const BallSimulation2::Event& evt = dref(simul.getEvent(i));

			if (evt.type == BallSimulation2::ET_Discontinuity) {

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

			const BallSimulation2::Sample& sample = simul.getSample(i);

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


	mConsideredIntervals.count = 0;

	{
		if (mTrajectories.count > 0) {

			Trajectories::Index trajIndex = 0;
			const StraightTrajectory* pTraj;
			const SampleInterval* pInterval;
			IntervalIntersection interval;

			for (SampleIntervals::Index i = 0; i < mAccessibleHeightIntervals.count; ++i) {

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

						TrajectoryInterval& newInterval = mConsideredIntervals.addOne();
						((SampleInterval&) newInterval) = interval;
						newInterval.trajectory = pTraj;
					} 
					
				} while (repeat);
			}
		}
	}

	mCurrSimulID = simul.getID();
	mCurrSimul = &simul;
	
	mStartSampleIndex = 0;
	updateStartSampleIndex(match, simul, time);
}

bool AnalyzedSimulation::getIntersectionTime(Footballer& footballer, const SimulationID& simulID, const FootballerState& footballerState, const SampleIndex& sampleIndex, const Vector3& samplePos, const Time& sampleTime, PathIntercept& result, const Time& currTime, bool fillSampleData, PathInterceptType filterType) const {

	if (currTime > sampleTime) {

		return false;
	}

	//for now dont take optimal height into account
	const Point& pos = footballer.mTransformWorld.getPosition();

	float distToTraj = distance(pos, samplePos);
	float timeToTraj;

	timeToTraj = footballerState.penaltyTime + (distToTraj) / footballerState.vel;

	/*
	distToTraj -= footballerState.distanceTolerance;

	if (distToTraj <= 0.0f) {

		distToTraj = 0.0f;
		timeToTraj = 0.0f;

	} else {

		timeToTraj = footballerState.penaltyTime + (distToTraj) / footballerState.vel;
	}
	*/

	float arrivalTime = currTime + timeToTraj;
	float footballerWaitTime = sampleTime - (arrivalTime);
	PathInterceptType type = (footballerWaitTime >= 0.0f) ? PI_InterceptorWait : PI_InterceptedWait;

	if (filterType != PI_None && filterType != type)
		return false;

	result.arrivalTime = arrivalTime;
	result.waitTime = footballerWaitTime;
	result.type = type;

	if (fillSampleData) {

		result.simulID = simulID;
		result.sampleIndex = sampleIndex;
		result.interceptPos = samplePos;
		result.expiryTime = sampleTime;
	}

	return true;
}

bool AnalyzedSimulation::getIntersectionTime(Footballer& footballer, const SimulationID& simulID, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulation2::Sample& sample, PathIntercept& result, const Time& currTime, bool fillSampleData, PathInterceptType filterType) const {

	return getIntersectionTime(footballer, simulID, footballerState, sampleIndex, sample.pos, sample.time, result, currTime, fillSampleData, filterType);
}

bool AnalyzedSimulation::getIntersectionTime(Footballer& footballer, const SimulationID& simulID, const SampleIndex& sampleIndex, const BallSimulation2::Sample& sample, PathIntercept& result, const Time& currTime, bool fillSampleData, PathInterceptType filterType) {

	mTempFootballerState.init(footballer);

	return getIntersectionTime(footballer, simulID, mTempFootballerState, sampleIndex, sample, result, currTime, fillSampleData, filterType);
}

bool AnalyzedSimulation::getIntersectionTime(Footballer& footballer, const SimulationID& simulID, const SampleIndex& sampleIndex, const Vector3& samplePos, const Time& sampleTime, PathIntercept& result, const Time& currTime, bool fillSampleData, PathInterceptType filterType) {

	mTempFootballerState.init(footballer);

	return getIntersectionTime(footballer, simulID, mTempFootballerState, sampleIndex, samplePos, sampleTime, result, currTime, fillSampleData, filterType);
}

void AnalyzedSimulation::render(Ball& ball, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass) {

	if (flagExtraRenders && mState == State_Valid && mConsideredIntervals.count) {

		for (TrajectoryIntervals::Index i = 0; i < mConsideredIntervals.count; ++i) {

			TrajectoryInterval& interval = mConsideredIntervals[i];

			mCurrSimul->render(ball, renderer, &(interval.startIndex), &(interval.endIndex));
		}
	}
}

AnalyzedSimulation::FootballerState::FootballerState() {
}

void AnalyzedSimulation::FootballerState::init(Footballer& footballer) {

	distanceTolerance = footballer.mNodeMatch->mSwitchControlSetup.pathInterceptDistTolerance;

	activationTime = 0.0f;
	vel = footballer.getRunSpeed();

	penaltyTime = activationTime; //+ playerReactionTime;
}

} }