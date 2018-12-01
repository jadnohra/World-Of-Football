#ifndef _WOFMatchControlSwitchEvaluator_BallPathIntercept_h
#define _WOFMatchControlSwitchEvaluator_BallPathIntercept_h

#include "WE3/WETL/WETLArray.h"
#include "WE3/math/WEPlane.h"
using namespace WE;

#include "WOFMatchControlSwitchEvaluator.h"

#include "../WOFMatchDataTypes.h"
#include "../WOFMatchObjectDefines.h"
#include "../WOFMatchBallSimulManager.h"


namespace WOF { namespace match {

	class Match;

	class ControlSwitchEvaluator_BallPathIntercept : public ControlSwitchEvaluator {
	public:

		ControlSwitchEvaluator_BallPathIntercept();

		virtual void init(Match& match);

		virtual void frameMove(Match& match, const Time& time, SwitchControl::ValueValidityState& validity);

	protected:

		FootballerMultiFrameProcessingSetup mFrameProcessingSetup;
		FootballerMultiFrameProcessingState mFrameProcessingState;

	protected:

		typedef BallSimulManager::Simul::Index SampleIndex;
		typedef BallSimulManager::Simul::Index EventIndex;

		struct FootballerState {

			float playerReactionTime; //in seconds
			float distanceTolerance;

			
			float vel;
			float activationTime;
			float penaltyTime;

			FootballerState(Match& match);
			void init(Footballer& footballer);
		};

		struct SampleInterval {

			SampleIndex startIndex;
			SampleIndex endIndex;
		};

		struct StraightTrajectory : SampleInterval {

			Plane plane; 

			bool getIntersectionTime(Footballer& footballer, const SampleInterval& interval, BallSimulManager::Simul& simul, float& intersectionTime) const;
		};

		struct IntervalIntersection : SampleInterval {

			bool isValid;
			bool nextAdvanceFirstInterval; //use this to advance 1st or 2nd interval for further intersections
		};

		enum IntersectionType {

			IT_None = 0, IT_FootballerWait, IT_BallWait, IT_FootballerLate
		};

		struct IntersectionInfo {

			bool isValid;
			IntersectionType type;


			float footballerArrivalDist; //footballer to trajectory
			float footballerArrivalTime; //footballer to trajectory
			float lateTime; //only for IT_FootballerLate

			bool isValidEncounter;
			float encounterTime;

			SampleIndex sampleIndex;

			IntersectionInfo() : isValid(false) {}
		};

		struct CandidateState {

			IntersectionInfo intersection;

			bool initByTrajectoryEndSample(Match& match, Footballer* pFootballer, BallSimulManager::Simul& simul, ControlSwitchEvaluator_BallPathIntercept& state);
			bool initByTrajectory(Match& match, Footballer* pFootballer, BallSimulManager::Simul& simul, ControlSwitchEvaluator_BallPathIntercept& state);
		};

		typedef WETL::CountedArray<StraightTrajectory, false, unsigned int> Trajectories;
		typedef WETL::CountedArray<SampleInterval, false, unsigned int> AccessibleHeightIntervals;

		SimulationID mCurrSimulID;
		Trajectories mTrajectories;
		AccessibleHeightIntervals mAccessibleHeightIntervals;


		void onSimulChanged(Match& match, BallSimulManager::Simul& simul);
		void addTrajectory(Match& match, BallSimulManager::Simul& simul, SampleIndex& startIndex, SampleIndex& endIndex);
		void getIntervalIntersection(const SampleInterval& interval1, const SampleInterval& interval2, IntervalIntersection& intersection);
		void getIntersectionTime(Footballer& footballer, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulManager::Simul::Sample& sample, const bool& ballIsWaiting, IntersectionInfo& result, bool invalidateOnFootballerLate) const;
		bool getIntersectionTime(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, const SampleInterval& interval, BallSimulManager::Simul& simul, IntersectionInfo& result) const;
		static IntersectionInfo* chooseBetterIntersection(IntersectionInfo& info1, IntersectionInfo& info2);
	};


} }

#endif