#ifndef _WOFMatchControlSwitchEvaluator_BallPathIntercept_h
#define _WOFMatchControlSwitchEvaluator_BallPathIntercept_h

#include "WE3/WETL/WETLArray.h"
#include "WE3/math/WEPlane.h"
using namespace WE;

#include "../WOFMatchDataTypes.h"
#include "../WOFMatchObjectDefines.h"
#include "../entity/ball/ballSimul/WOFMatchBallSimulation.h"
#include "WOFMatchControlSwitchEvaluator.h"

namespace WOF { namespace match {

	class Match;

	class ControlSwitchEvaluator_BallPathIntercept : public ControlSwitchEvaluatorBase {
	public:

		enum State {

			State_None = -1, State_Invalid, State_WaitingForSimul, State_Processing, State_Valid, State_Expired
		};

	public:

		ControlSwitchEvaluator_BallPathIntercept();

		void init(Match& match);

		State update(Match& match, const Time& time);
		inline const State& getState() { return mState; }

		inline const SimulationID& getCurrSimulID() { return mCurrSimulID; }

	protected:

		FootballerMultiFrameProcessingSetup mFrameProcessingSetup;
		FootballerMultiFrameProcessingState mFrameProcessingState;

	protected:

		typedef BallSimulation::Index SampleIndex;
		typedef BallSimulation::Index EventIndex;

		struct FootballerState {

			//float playerReactionTime; //in seconds
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

			bool getIntersectionTime(Footballer& footballer, const SampleInterval& interval, BallSimulation& simul, float& intersectionTime) const;
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
			void convertTo(const Time& simulStartTime, SwitchControl::PathInterception& target);
		};

		enum IntersectionSampleType {

			IST_Start, IST_End, IST_Closest
		};

	
		typedef WETL::CountedArray<StraightTrajectory, false, unsigned int> Trajectories;
		typedef WETL::CountedArray<SampleInterval, false, unsigned int> AccessibleHeightIntervals;

		State mState;
		SimulationID mCurrSimulID;
		Trajectories mTrajectories;
		AccessibleHeightIntervals mAccessibleHeightIntervals;


		void onSimulChanged(Match& match, BallSimulation& simul);
		void addTrajectory(Match& match, BallSimulation& simul, SampleIndex& startIndex, SampleIndex& endIndex);
		void getIntervalIntersection(const SampleInterval& interval1, const SampleInterval& interval2, IntervalIntersection& intersection);
		bool getIntersectionTime(Footballer& footballer, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulation::Sample& sample, const bool& ballIsWaiting, IntersectionInfo& result, bool invalidateOnFootballerLate, Time simulTimeShift = 0.0f, bool keepOldResultOnNotFootbalerWait = false) const;
		void getIntersections(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, const SampleInterval& interval, BallSimulation& simul, IntersectionInfo inter[3], int& interCount, Time simulTimeShift = 0.0f) const;
		void findValidIntersection_binarySearch(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, SampleInterval interval, BallSimulation& simul, IntersectionInfo& inter, Time simulTimeShift, bool startSampleValid) const;
		static IntersectionInfo* chooseBetterIntersection(IntersectionInfo& info1, IntersectionInfo& info2);


		void findIntersections(Match& match, Footballer* pFootballer, BallSimulation& simul, SwitchControl::PathInterception& shortestTimeIntercept, SwitchControl::PathInterception& shortestDistIntercept, Time simulTimeShift = 0.0f);
		bool getEndSampleIntersection(Match& match, Footballer* pFootballer, BallSimulation& simul, SwitchControl::PathInterception& result, Time simulTimeShift = 0.0f);
	};


} }

#endif