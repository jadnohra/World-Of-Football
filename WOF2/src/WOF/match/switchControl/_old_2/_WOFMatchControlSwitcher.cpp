#include "WOFMatchControlSwitcher.h"

#include "WOFMatch.h"
#include "WOFMatchObjectIncludes.h"

namespace WOF { namespace match {

ControlSwitcher::State_ShotAnalysis::State_ShotAnalysis()
	: simulID(-1) {
}

void ControlSwitcher::State_ShotAnalysis::addTrajectory(Match& match, BallSimulManager::Simul& simul, SampleIndex& startIndex, SampleIndex& endIndex) {

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

				StraightTrajectory& traj = trajectories.addOne();

				traj.startIndex = startIndex;
				traj.endIndex = endIndex;

				traj.plane.init(s1.pos, n);
			}

		} else {

			assrt(false);
		}
	}
}

void ControlSwitcher::State_ShotAnalysis::getIntervalIntersection(const SampleInterval& interval1, const SampleInterval& interval2, IntervalIntersection& intersection) {

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

void ControlSwitcher::State_ShotAnalysis::onSimulChanged(Match& match, BallSimulManager::Simul& simul) {

	trajectories.count = 0;

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


	accessibleHeightIntervals.count = 0;

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

						SampleInterval& interval = accessibleHeightIntervals.addOne();

						interval.startIndex = startIndex;
						interval.endIndex = i - 1;
					}

					isStarted = false;
				}
			}
		}

		if (isStarted && (startIndex < sampleCount)) {

			SampleInterval& interval = accessibleHeightIntervals.addOne();

			interval.startIndex = startIndex;
			interval.endIndex = sampleCount - 1;
		}
	}


	simulID = simul.getID();
}

ControlSwitcher::State_ShotAnalysis::FootballerState::FootballerState(Match& match) {

	distanceTolerance = match.mFootballerSwitchSetup.distanceTolerance;
	playerReactionTime = match.mFootballerSwitchSetup.playerReactionTime; 
}

void ControlSwitcher::State_ShotAnalysis::FootballerState::init(Footballer& footballer) {

	vel = footballer.body_toVelocity_Running(Footballer::BR_Normal);
	activationTime = footballer.body_getStateSwitchTime(Footballer::BS_Running);
	penaltyTime = activationTime + playerReactionTime;
}

ControlSwitcher::State_ShotAnalysis::IntersectionInfo* ControlSwitcher::State_ShotAnalysis::chooseBetterIntersection(IntersectionInfo& info1, IntersectionInfo& info2) {

	return info1.encounterTime < info2.encounterTime ? &info1 : &info2;
}

void ControlSwitcher::State_ShotAnalysis::getIntersectionTime(Footballer& footballer, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulManager::Simul::Sample& sample, const bool& ballIsWaiting, IntersectionInfo& result, bool invalidateOnFootballerLate) const {

	//for now dont take optimal height into account

	result.sampleIndex = sampleIndex;

	const Point& pos = footballer.mTransformWorld.getPosition();

	float dist = distance(pos, sample.pos);

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

bool ControlSwitcher::State_ShotAnalysis::getIntersectionTime(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, const SampleInterval& interval, BallSimulManager::Simul& simul, IntersectionInfo& result) const {

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

Footballer* ControlSwitcher::switch_Owner(Team& team, Player& player, Footballer* pCurrFootballer, bool isManualSwitch) {

	//heuristic, use ball ownership to select footballer
	{
		BallOwnershipManager& ownership = mMatch->getBallOwnershipManager();

		FootballerIndex index;

		bool currFootballerIsOwner = ownership.findOwner(pCurrFootballer, index);
		
		if (!isManualSwitch) {

			if (currFootballerIsOwner)
				return pCurrFootballer;

			SoftPtr<Footballer> firstOwner = ownership.iterateOwner(team.getTeamEnum(), (index = 0));

			if (firstOwner.isValid())
				return firstOwner;

		} else {

			SoftPtr<Footballer> firstOwner = ownership.iterateOwner(team.getTeamEnum(), (index = 0));
			SoftPtr<Footballer> owner = firstOwner;

			while (owner.isValid() && (owner.ptr() == pCurrFootballer)) {

				owner = ownership.iterateOwner(team.getTeamEnum(), index);
			}

			if (owner.isValid())
				return owner;
		}
	}

	return NULL;
}

Footballer* ControlSwitcher::switch_ShotAnalysis(Team& team, Player& player, Footballer* pCurrFootballer, bool isManualSwitch, State_ShotAnalysis& state) {

	struct CandidateState {

		State_ShotAnalysis::IntersectionInfo intersection;

		bool initByTrajectoryEndSample(Match& match, Footballer* pFootballer, BallSimulManager::Simul& simul, State_ShotAnalysis& state) {

			intersection.isValid = false;

			if (simul.getSampleCount() > 0) {

				Footballer& footballer = dref(pFootballer);

				State_ShotAnalysis::FootballerState footballerState(match);
				footballerState.init(footballer);

				State_ShotAnalysis::Trajectories::Index trajIndex = state.trajectories.count - 1;

				const State_ShotAnalysis::StraightTrajectory* pTraj = &(state.trajectories.el[trajIndex]);

				State_ShotAnalysis::SampleIndex sampleIndex = simul.getSampleCount() - 1;
				const BallSimulManager::Simul::Sample& sample = dref(simul.getSample(sampleIndex));

				state.getIntersectionTime(footballer, footballerState, sampleIndex, sample, true, this->intersection, true);

				return this->intersection.isValidEncounter;
			}

			return false;
		}

		bool initByTrajectory(Match& match, Footballer* pFootballer, BallSimulManager::Simul& simul, State_ShotAnalysis& state) {

			intersection.isValid = false;

			if (state.trajectories.count > 0) {

				Footballer& footballer = dref(pFootballer);

				State_ShotAnalysis::FootballerState footballerState(match);
				footballerState.init(footballer);

				State_ShotAnalysis::Trajectories::Index trajIndex = 0;
				const State_ShotAnalysis::StraightTrajectory* pTraj;
				const State_ShotAnalysis::SampleInterval* pInterval;
				State_ShotAnalysis::IntervalIntersection intersection;

				for (State_ShotAnalysis::AccessibleHeightIntervals::Index i = 0; i < state.accessibleHeightIntervals.count; ++i) {

					bool repeat;
					bool isLastSample;

					do {

						repeat = false;

						pInterval = &(state.accessibleHeightIntervals.el[i]);
						pTraj = &(state.trajectories.el[trajIndex]);
						
						state.getIntervalIntersection(*pInterval, *pTraj, intersection);

						if (intersection.nextAdvanceFirstInterval == false) {

							trajIndex++;

							repeat = (trajIndex < state.trajectories.count);
						}

						isLastSample = (!repeat) && (i >= state.accessibleHeightIntervals.count - 1);

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
	};

	struct Candidate : CandidateState {

		SoftPtr<Footballer> footballer;
		State_ShotAnalysis::IntersectionInfo intersection;

		void test(Footballer* pFootballer, CandidateState& state, BallSimulManager::Simul& simul,
					const bool& favourDiffFootballer, Footballer* pCurrFootballer, const float& favourDiffPenalty) {

			if (state.intersection.isValidEncounter) {

				bool take = false;

				if (intersection.isValid) {

					if (favourDiffFootballer && footballer.ptr() == pCurrFootballer) {

						take = (state.intersection.encounterTime < intersection.encounterTime + favourDiffPenalty);

					} else {

						take = state.intersection.encounterTime < intersection.encounterTime;
					}

				} else {

					take = true;
				}

				if (take) {

					intersection = state.intersection;
					footballer = pFootballer;
				}
			}
		}
	};

	const float favourDiffPenalty = 1.5f; //in seconds 
	bool favourDiffFootballer = player.readMindWantsSwitchDifferentFootballer();

	if (pCurrFootballer && !isManualSwitch && !favourDiffFootballer) {

		return pCurrFootballer;
	}

	SoftPtr<BallSimulManager::Simul> simul = mMatch->getBallSimulManager().getCurrSimul();

	//of course ... split this in many frames and allow isPartial
	//also add heuristic -> dribble shot -> no switch
	if (simul.isValid() && simul->isValid() && !simul->isPartial()) {

		if (simul->getID() != state.simulID) {

			state.onSimulChanged(mMatch, simul);
		}

		if (state.trajectories.count == 0) {

			return switch_BallDist(team, player, pCurrFootballer, isManualSwitch);
		}

		//return switch_NextNumber(team, pCurrFootballer, allowSameFootballer);

		float instantTakeIntersectionTimeThreshold = 0.0f;

		CandidateState candidateState;
		SoftPtr<Footballer> candidate;
		
		Candidate bestCandidate;
		
		if (!isManualSwitch && pCurrFootballer) {

			bool tested = false;

			candidate = pCurrFootballer;

			if (candidateState.initByTrajectory(mMatch, candidate, simul, state)) {

				if (candidateState.intersection.isValidEncounter && 
					candidateState.intersection.encounterTime <= instantTakeIntersectionTimeThreshold) {

					return candidate;		
				}

				bestCandidate.test(candidate, candidateState, simul, favourDiffFootballer, pCurrFootballer, favourDiffPenalty);
				tested = true;
			} 

			if (!tested) {

				if (candidateState.initByTrajectoryEndSample(mMatch, candidate, simul, state)) {

					bestCandidate.test(candidate, candidateState, simul, favourDiffFootballer, pCurrFootballer, favourDiffPenalty);
				}
			}
		}

		candidate = team.getFirstFootballer(true);

		while (candidate.isValid()) {

			bool allowCandidate = true;
			bool tested = false;

			if (isManualSwitch) {

				allowCandidate = (candidate.ptr() != pCurrFootballer);
			} 

			if (allowCandidate) {

				if (candidateState.initByTrajectory(mMatch, candidate, simul, state)) {

					if (candidateState.intersection.isValidEncounter && 
						candidateState.intersection.encounterTime <= instantTakeIntersectionTimeThreshold) {

						return candidate;		
					}

					bestCandidate.test(candidate, candidateState, simul, favourDiffFootballer, pCurrFootballer, favourDiffPenalty);
					tested = true;
				} 

				if (!tested) {

					if (candidateState.initByTrajectoryEndSample(mMatch, candidate, simul, state)) {

						bestCandidate.test(candidate, candidateState, simul, favourDiffFootballer, pCurrFootballer, favourDiffPenalty);
					}
				}
			}

			candidate = team.getNextFootballer(candidate->mNumber, true);
		} 

		if (bestCandidate.footballer.isValid()) {

			return bestCandidate.footballer;

		} 
	}

	/*
	//heuristic, use ball ownership to select footballer
	{
		BallOwnershipManager& ownership = mMatch->getBallOwnershipManager();

		FootballerIndex index;

		bool currFootballerIsOwner = ownership.findOwner(pCurrFootballer, index);
		
		if (!isManualSwitch) {

			if (currFootballerIsOwner)
				return pCurrFootballer;

			SoftPtr<Footballer> firstOwner = ownership.iterateOwner(team.getTeamEnum(), (index = 0));

			if (firstOwner.isValid())
				return firstOwner;

		} else {

			SoftPtr<Footballer> firstOwner = ownership.iterateOwner(team.getTeamEnum(), (index = 0));
			SoftPtr<Footballer> owner = firstOwner;

			while (owner.isValid() && (owner.ptr() == pCurrFootballer)) {

				owner = ownership.iterateOwner(team.getTeamEnum(), index);
			}

			if (owner.isValid())
				return owner;
		}
	}
	*/

	return switch_BallDist(team, player, pCurrFootballer, isManualSwitch);
}

Footballer* ControlSwitcher::switch_BallDist(Team& team, Player& player, Footballer* pCurrFootballer, bool isManualSwitch) {

	struct Candidate {

		SoftPtr<Footballer> footballer;
		float relBallDist;

		Candidate() : relBallDist(FLT_MAX) {}

		void test(Footballer* pFootballer, SpatialState::FootballeBallState& state) {

			if (state.getRelBallDist() < relBallDist) {

				relBallDist = state.getRelBallDist();
				footballer = pFootballer;
			}
		}
	};

	SpatialState& spatial = mMatch->getSpatialState();
	SpatialState::FootballeBallState state;

	SoftPtr<Ball> ball = &(mMatch->getBall());
	SoftPtr<Footballer> candidate;
	
	Candidate bestCandidate;
	
	if (!isManualSwitch && pCurrFootballer) {

		candidate = pCurrFootballer;

		if (spatial.getFootballerBallState(candidate, ball, state)) {

			if (mMatch->mFootballerSwitchSetup.enableBuffer
				&& (state.getRelBallDist() <= mMatch->mFootballerSwitchSetup.bufferDistance) ) {

				return pCurrFootballer;		
			}

			bestCandidate.test(candidate, state);
		}
	}

	candidate = team.getFirstFootballer(true);

	while (candidate.isValid()) {

		bool allowCandidate = true;

		if (isManualSwitch) {

			allowCandidate = (candidate.ptr() != pCurrFootballer);
		} 

		if (allowCandidate) {

			if (spatial.getFootballerBallState(candidate, ball, state)) {

				bestCandidate.test(candidate, state);

			} 
		}

		candidate = team.getNextFootballer(candidate->mNumber, true);
	} 

	return bestCandidate.footballer;
}

Footballer* ControlSwitcher::switch_NextNumber(Team& team, Player& player, Footballer* pCurrFootballer, bool isManualSwitch) {

	if (pCurrFootballer == NULL) {
		
		pCurrFootballer = team.getFirstFootballer(true);

		if (pCurrFootballer != NULL) {

			return pCurrFootballer;
		}
	}

	if (pCurrFootballer != NULL) {

		if (!isManualSwitch) {

			return pCurrFootballer;
		}

		SoftPtr<Footballer> nextFootballer;

		nextFootballer = team.getNextFootballer(pCurrFootballer->mNumber, true);

		if (nextFootballer.isValid())
			return nextFootballer;

		nextFootballer = team.getFirstFootballer(true);

		return nextFootballer;
	} 

	return NULL;

}

void ControlSwitcher::init(Match& match) {

	mMatch = &match;
}

Footballer* ControlSwitcher::switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch) {

	if (technique == FST_Default) {

		technique = mMatch->mFootballerSwitchSetup.technique;
	}

	SoftPtr<Footballer> owner =  switch_Owner(team, player, pCurrFootballer, isManualSwitch);

	if (owner.isValid()) {

		return owner;
	}

	switch (technique) {

		case FST_ShotAnalysis: {

			return switch_ShotAnalysis(team, player, pCurrFootballer, isManualSwitch, mState.shotAnalysis);

		} break;

		case FST_BallDist: {

			return switch_BallDist(team, player, pCurrFootballer, isManualSwitch);
			
		} break;

		case FST_NextNumber:
		default: {

			return switch_NextNumber(team, player, pCurrFootballer, isManualSwitch);
			
		} break;
	}


	return NULL;
}

void ControlSwitcher::render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass) {

	if (flagExtraRenders) {

		for (State_ShotAnalysis::Trajectories::Index i = 0; i < mState.shotAnalysis.trajectories.count; ++i) {

			renderer.draw(mState.shotAnalysis.trajectories.el[i].plane, 100.0f, &Matrix4x3Base::kIdentity);
		}
	}
}

} }