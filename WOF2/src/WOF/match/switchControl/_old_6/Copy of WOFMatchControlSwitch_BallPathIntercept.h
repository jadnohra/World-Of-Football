#ifndef _WOFMatchControlSwitch_BallPathIntercept_h
#define _WOFMatchControlSwitch_BallPathIntercept_h

#include "WE3/WETL/WETLArray.h"
#include "WE3/math/WEPlane.h"
using namespace WE;

#include "../WOFMatchDataTypes.h"
#include "../WOFMatchObjectDefines.h"
#include "../entity/ball/ballSimul/WOFMatchBallSimulation.h"
#include "WOFMatchControlSwitchEvaluator.h"

namespace WOF { namespace match {

	class Match;

	class ControlSwitch_BallPathIntercept : public ControlSwitchEvaluatorBase {
	public:

		enum State {

			State_None = -1, State_Invalid, State_WaitingForSimul, State_Processing, State_Valid, State_Expired
		};

	public:

		ControlSwitch_BallPathIntercept();

		void init(Match& match);

		State update(Match& match, const Time& time, bool& consideredFootballersUpdated);
		inline const State& getState() { return mState; }

		inline const SimulationID& getCurrSimulID() { return mCurrSimulID; }

		inline const FootballerIndex& getConsideredFootballerCount() { return mConsideredFootballers.count; }
		inline Footballer* getConsideredFootballer(const FootballerIndex& index) { return mConsideredFootballers[index]; }

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

		struct TrajectoryInterval : SampleInterval {

			const StraightTrajectory* trajectory;
		};

		typedef WETL::CountedArray<StraightTrajectory, false, unsigned int> Trajectories;
		typedef WETL::CountedArray<SampleInterval, false, unsigned int> SampleIntervals;
		typedef WETL::CountedArray<TrajectoryInterval, false, unsigned int> TrajectoryIntervals;

		State mState;
		SimulationID mCurrSimulID;
		Trajectories mTrajectories;
		SampleIntervals mAccessibleHeightIntervals;
		TrajectoryIntervals mConsideredIntervals;
		SampleIndex mStartSampleIndex;

		SoftFootballerArray mIgnoredFootballers;
		SoftFootballerArray mConsideredFootballers;
		SoftFootballerArray mTempConsideredFootballers;

		void onSimulChanged(Match& match, BallSimulation& simul, const Time& time);
		bool updateStartSampleIndex(Match& match, BallSimulation& simul, const Time& time);
		void addTrajectory(Match& match, BallSimulation& simul, SampleIndex& startIndex, SampleIndex& endIndex);
		void getIntervalIntersection(const SampleInterval& interval1, const SampleInterval& interval2, IntervalIntersection& intersection);
		bool getIntersectionTime(Footballer& footballer, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulation::Sample& sample, PathIntercept& result, const Time& simulStartTime, const Time& currTime, bool fillSampleData, PathInterceptType filterType = PI_None) const;
		bool getIntersectionTime(Footballer& footballer, const FootballerState& footballerState, const SampleIndex& sampleIndex, const Vector3& samplePos, const Time& _sampleTime, PathIntercept& result, const Time& simulStartTime, const Time& currTime, bool fillSampleData, PathInterceptType filterType = PI_None) const;
		void sortConsideredFootballer(const FootballerIndex& footballerIndex, Object* pCommandOwner, float commandOwnerScoreShift = 0.0f);
		bool updateShortestTimeInterceptState(Match& match, Footballer& footballer, BallSimulation& simul, const Time& time, const SampleIndex* pStartSampleIndex = NULL);
	};


} }

#endif