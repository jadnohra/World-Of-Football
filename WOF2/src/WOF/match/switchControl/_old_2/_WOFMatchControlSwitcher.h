#ifndef _WOFMatchControlSwitcher_h
#define _WOFMatchControlSwitcher_h

#include "WE3/WETL/WETLArray.h"
#include "WE3/math/WEPlane.h"
using namespace WE;

#include "WOFMatchDataTypes.h"
#include "WOFMatchObjectDefines.h"
#include "WOFMatchBallSimulManager.h"

namespace WOF { namespace match {

	class ControlSwitcher {
	public:

		void init(Match& match);

		Footballer* switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch);

		void render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass);

	protected:

		struct State_ShotAnalysis {

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

				float footballerArrivalTime; //footballer to trajectory
				float lateTime; //only for IT_FootballerLate

				bool isValidEncounter;
				float encounterTime;

				SampleIndex sampleIndex;

				IntersectionInfo() : isValid(false) {}
			};

			typedef WETL::CountedArray<StraightTrajectory, false, unsigned int> Trajectories;
			typedef WETL::CountedArray<SampleInterval, false, unsigned int> AccessibleHeightIntervals;

			SimulationID simulID;
			Trajectories trajectories;
			AccessibleHeightIntervals accessibleHeightIntervals;

			State_ShotAnalysis();

			void onSimulChanged(Match& match, BallSimulManager::Simul& simul);
			void addTrajectory(Match& match, BallSimulManager::Simul& simul, SampleIndex& startIndex, SampleIndex& endIndex);
			void getIntervalIntersection(const SampleInterval& interval1, const SampleInterval& interval2, IntervalIntersection& intersection);
			void getIntersectionTime(Footballer& footballer, const FootballerState& footballerState, const SampleIndex& sampleIndex, const BallSimulManager::Simul::Sample& sample, const bool& ballIsWaiting, IntersectionInfo& result, bool invalidateOnFootballerLate) const;
			bool getIntersectionTime(const StraightTrajectory& traj, Footballer& footballer, const FootballerState& footballerState, const SampleInterval& interval, BallSimulManager::Simul& simul, IntersectionInfo& result) const;
			static IntersectionInfo* chooseBetterIntersection(IntersectionInfo& info1, IntersectionInfo& info2);
		};

		struct State {

			State_ShotAnalysis shotAnalysis;
		};

	protected:

		Footballer* switch_Owner(Team& team, Player& player, Footballer* pCurrFootballer, bool isManualSwitch);
		Footballer* switch_NextNumber(Team& team, Player& player, Footballer* pCurrFootballer, bool isManualSwitch);
		Footballer* switch_BallDist(Team& team, Player& player, Footballer* pCurrFootballer, bool isManualSwitch);
		Footballer* switch_ShotAnalysis(Team& team, Player& player, Footballer* pCurrFootballer, bool isManualSwitch, State_ShotAnalysis& state);

	protected:

		SoftPtr<Match> mMatch;
		
		State mState;
	};

} }

#endif