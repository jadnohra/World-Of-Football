#ifndef h_WOF_match_SpatialMatchState
#define h_WOF_match_SpatialMatchState

#include "../DataTypes.h"
#include "../FootballerIterator.h"
#include "BallSimulation.h"
#include "BallInterceptEstimate.h"
#include "SpatialPitchModel.h"

namespace WOF { namespace match {

	class Footballer;
	class Match;

	class SpatialMatchState {
	public:

		enum FinalBallStateEnum {

			FinalBall_None = -1, FinalBall_InPlay, FinalBall_ThrowIn, FinalBall_Corner, FinalBall_Goalie, FinalBall_Goal
		};

		class BallState {
		public:

			struct BallGoalEstimate {

				bool isValid;
				
				Time time;
				Vector3 pos;
				Vector3 vel;

				bool isInstantaneous;
				bool isCrossingGoalPlane;
				bool isInGoalDir;
				bool isBetweenGoalBars;
				float goalBarDistLeft;
				float goalBarDistRight;

				inline bool script_isValid() { return isValid; }
				inline bool script_isInstantaneous() { return isInstantaneous; }
				inline bool script_isCrossingGoalPlane() { return isCrossingGoalPlane; }
				inline bool script_isInGoalDir() { return isInGoalDir; }
				inline bool script_isBetweenGoalBars() { return isBetweenGoalBars; }
				inline float script_getGoalBarDistLeft() { return goalBarDistLeft; }
				inline float script_getGoalBarDistRight() { return goalBarDistRight; }

				inline float script_getTime() { return time; }
				inline Vector3* script_getPos() { return &pos; }
				inline Vector3* script_getVel() { return &vel; }

				float calcBarDist(const SpatialPitchModel::GoalInfo& goalInfo, const Vector3& ballPos, const Vector3& ballVel, const Vector3& barPos);

				static const TCHAR* ScriptClassName;

				static void declareInVM(SquirrelVM& target);
			};

		public:

			BallState();

			void update(Match& match, const Clock& clock);

		public:

			inline bool finalIsValid() { return mFinalState != FinalBall_None; }
			inline SimulationID finalGetID() { return mFinalStateID; }
			inline const FinalBallStateEnum& finalGetState() { return mFinalState; }
			inline const TeamEnum& finalGetPitchInTeam() { return mFinalPitchInOwner; }

		public:

			inline bool script_finalIsValid() { return finalIsValid(); }
			inline unsigned int script_finalGetID() { return finalGetID(); }
			inline int script_finalGetState() { return finalGetState(); }
			inline int script_finalGetPitchInTeam() { return finalGetPitchInTeam(); }
			BallGoalEstimate* script_getInstantGoalEstimate(Match& match, int team, bool validOnly);

		protected:

			void setFinalState(FinalBallStateEnum state);
			void analyzeFinalStateExitPoint(Match& match, BallSimulation& simul, Vector2& exitPoint, CoordSysDirection exitDir[2]);
			void updateFinalState(Match& match, const Clock& clock);

			void updateInstantanousGoalEstimate(Match& match, const Clock& clock, Ball& ball, BallGoalEstimate& estimate, const SpatialPitchModel::GoalInfo& goalInfo);
			void updateInstantanousGoalEstimates(Match& match, const Clock& clock);

		protected:

			FinalBallStateEnum mFinalState;
			bool mFinalIsSimulProcessing;
			BallSimulation::Index mFinalNextProcessIndex;
			BallSimulation::Index mFinalNextProcessTriggerIndex;
			SimulationID mFinalSimulID;
			SimulationID mFinalStateID;
			TeamEnum mFinalPitchInOwner;

			BallGoalEstimate mInstantaneousGoalEstimateBack;
			BallGoalEstimate mInstantaneousGoalEstimateFwd;


		public:

			static const TCHAR* ScriptClassName;

			static void declareInVM(SquirrelVM& target);
		};

	public:

		class BallIntercept : public FootballerIteratorClientBase {
		public:

			class FootballerState : public BallInterceptEstimate {
			public:

				FootballerState() {

					BallInterceptEstimate::enableAnalysis(true, false);
				}

			public:

				FootballerIndex indexBallIntercept;
			};

		public:

			BallIntercept();

			void init(Match& match);

			bool update(Match& match, const Time& time, BallSimulation* pValidSimul, bool& consideredFootballersUpdated);
			inline const bool& isValid() { return mIsValid; }
			inline const Time& getLastChangeTime() { return mLastChangeTime; }

			inline const FootballerIndex& getFootballerCount() { return mFootballers.count; }
			inline Footballer* getFootballer(const FootballerIndex& index) { return mFootballers[index]; }

			inline const FootballerIndex& getFootballerCount(TeamEnum team) { return mTeamFootballers[team].count; }
			inline Footballer* getFootballer(TeamEnum team, const FootballerIndex& index) { return mTeamFootballers[team][index]; }

		protected:

			SimulationID mCurrSimulID;
			bool mIsValid;
			Time mLastChangeTime;

			SoftFootballerArray mDiscardedFootballers;
			SoftFootballerArray mFootballers;
			SoftFootballerArray mTempFootballer;
			SoftFootballerArray mTeamFootballers[2];

			void sortInterceptingFootballer(const FootballerIndex& footballerIndex);
			void sortLastSampleDistFootballer(const FootballerIndex& footballerIndex);
		};

		class BallDist : public FootballerIteratorClientBase {
		public:

			class FootballerState {
			public:

				FootballerIndex indexBallDist;
			};

		public:

			BallDist();

			void init(Match& match);

			void update(Match& match, const Time& time);

			inline const FootballerIndex& getFootballerCount() { return mFootballers.count; }
			inline Footballer* getFootballer(const FootballerIndex& index) { return mFootballers[index]; }

			inline const FootballerIndex& getFootballerCount(TeamEnum team) { return mTeamFootballers[team].count; }
			inline Footballer* getFootballer(TeamEnum team, const FootballerIndex& index) { return mTeamFootballers[team][index]; }
			
		protected:

			SoftFootballerArray mFootballers;
			SoftFootballerArray mTeamFootballers[2];
		};

	public:

		class FootballerState : public BallIntercept::FootballerState, public BallDist::FootballerState {
		public:
		};

	public:

		inline BallIntercept* getValidBallIntercept() { return mBallIntercept.isValid() ? &mBallIntercept : NULL; }
		inline BallIntercept& getBallIntercept() { return mBallIntercept; }
		inline BallDist& getBallDist() { return mBallDist; }
		inline BallState& getBallState() { return mBallState; }

	public:

		SpatialMatchState();

		void init(Match& match);

		void frameMove(Match& match, const Clock& clock);

		void onActiveFootballersChange();

		bool findFootballerFreeSpace(Footballer& footballer, const CtVector3* pSearchDir, bool tryCurrPos, 
										float footballerRadius, float footballerSpeed, float searchDistance, 
										float minSafetyTime, int maxIterCount, 
										PitchRegionShape* pRegion, Vector3& outPos, float& outSafetyTime);

		Footballer* iterNextRegionFootballer(PitchRegionShape* pRegion, TeamEnum refTeam, FootballerIndex& startIndex);

	protected:

		SoftPtr<Match> mMatch;
		BallIntercept mBallIntercept;
		BallDist mBallDist;
		BallState mBallState;

		Matrix2x2 m90DegRotMat;
	};

} }

#endif
