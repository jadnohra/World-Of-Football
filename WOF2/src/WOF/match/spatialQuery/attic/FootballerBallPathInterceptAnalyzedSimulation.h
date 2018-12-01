#ifndef _FootballerBallPathInterceptAnalyzedSimulation_h_WOF_match
#define _FootballerBallPathInterceptAnalyzedSimulation_h_WOF_match

#include "WE3/WETL/WETLArray.h"
#include "WE3/math/WEPlane.h"
using namespace WE;

#include "../WOFMatchDataTypes.h"
#include "../WOFMatchObjectDefines.h"
#include "../entity/ball/ballSimul/BallSimulation2.h"
#include "SpatialQueryDataTypes.h"

namespace WOF { namespace match {

	class Match;

	class FootballerBallPathInterceptAnalyzedSimulation {
	public:

		enum State {

			State_None = -1, State_Invalid, State_WaitingForSimul, State_Processing, State_Valid, State_Expired
		};

		struct FootballerState {

			//float playerReactionTime; //in seconds
			float distanceTolerance;
			
			float vel;
			float activationTime;
			float penaltyTime;

			FootballerState();
			void init(Footballer& footballer);
		};

		typedef BallSimulation2::Index SampleIndex;
		typedef BallSimulation2::Index EventIndex;
		typedef BallSimulation2::Sample Sample;

	public:

		FootballerBallPathInterceptAnalyzedSimulation();

		void init(Match& match);

		State update(Match& match, BallSimulation2& simul, const Time& time);
		inline const State& getState() { return mState; }
		inline bool isValid() { return mState == State_Valid; }

		inline const SimulationID& getSimulID() { return mCurrSimulID; }
		inline BallSimulation2* getSimul() { return mCurrSimul; }

		inline const Sample& getSample(const SampleIndex& index) { return mCurrSimul->getSample(index); }

		void render(Ball& ball, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass);

	public:

		bool checkValidity(Match& match, BallSimulation2& simul, const Time& time);

		bool updateShortestTimeInterceptState(Match& match, Footballer& footballer, BallSimulation2& simul, const Time& time, const SampleIndex* pStartSampleIndex = NULL);
		bool getIntersectionTime(Footballer& footballer, const SimulationID& simulID, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulation2::Sample& sample, PathIntercept& result, const Time& currTime, bool fillSampleData, PathInterceptType filterType = PI_None) const;
		bool getIntersectionTime(Footballer& footballer, const SimulationID& simulID, const FootballerState& footballerState, const SampleIndex& sampleIndex, const Vector3& samplePos, const Time& _sampleTime, PathIntercept& result, const Time& currTime, bool fillSampleData, PathInterceptType filterType = PI_None) const;
		
		bool getIntersectionTime(Footballer& footballer, const SimulationID& simulID, const SampleIndex& sampleIndex, const BallSimulation2::Sample& sample, PathIntercept& result, const Time& currTime, bool fillSampleData, PathInterceptType filterType = PI_None);
		bool getIntersectionTime(Footballer& footballer, const SimulationID& simulID, const SampleIndex& sampleIndex, const Vector3& samplePos, const Time& _sampleTime, PathIntercept& result, const Time& currTime, bool fillSampleData, PathInterceptType filterType = PI_None);

	protected:
		
		struct SampleInterval {

			SampleIndex startIndex;
			SampleIndex endIndex;
		};

		struct StraightTrajectory : SampleInterval {

			Plane plane; 

			bool getIntersectionTime(Footballer& footballer, const SampleInterval& interval, BallSimulation2& simul, float& intersectionTime) const;
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
		SoftPtr<BallSimulation2> mCurrSimul;
		Trajectories mTrajectories;
		SampleIntervals mAccessibleHeightIntervals;
		TrajectoryIntervals mConsideredIntervals;
		SampleIndex mStartSampleIndex;

		FootballerState mTempFootballerState;

		void onSimulChanged(Match& match, BallSimulation2& simul, const Time& time);
		bool updateStartSampleIndex(Match& match, BallSimulation2& simul, const Time& time);
		void addTrajectory(Match& match, BallSimulation2& simul, SampleIndex& startIndex, SampleIndex& endIndex);
		void getIntervalIntersection(const SampleInterval& interval1, const SampleInterval& interval2, IntervalIntersection& intersection);
	};


} }

#endif