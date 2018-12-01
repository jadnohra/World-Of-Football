#ifndef h_WOF_match_ControlSwitcher
#define h_WOF_match_ControlSwitcher

#include "WE3/WETL/WETLArray.h"
#include "WE3/math/WEPlane.h"
#include "WE3/WEPtr.h"
#include "WE3/render/WERenderer.h"
using namespace WE;

#include "../DataTypes.h"
#include "../FootballerIteratorImpl.h"
#include "../spatialQuery/SpatialMatchState.h"

namespace WOF { namespace match {

	class Footballer;
	class Team;
	class Player;
	class Footballer_Brain;
	
	class ControlSwitcher {
	public:

		void init(Match& match);

		Footballer* switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch);

		void frameMove(Match& match, const Time& time);
		void render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass);

		void getFootballerDebugText(Match& match, Footballer_Brain& footballer, String& str);

		void onActiveFootballersChange();

	public:

		const FootballerIndex& getTopFootballerCount(TeamEnum team) { return mTopFootballers[team].count; }

	protected:

		void frameMove_Automatic(Match& match, const Time& time);
		void frameMove_Manual(Match& match, const Time& time);

		void frameMove_PathIntercept(Match& match, const Time& time, const bool& modeWasPathIntercept, const bool& allAreOwners, SpatialMatchState::BallIntercept& interceptEvaluator);
		void frameMove_BallDist(Match& match, const Time& time, const bool& modeWasPathIntercept, const bool& allAreOwners);
		void updatePlayerInfluences(Match& match, const Time& time);

		struct ScoreUpdateState {};

	protected:

		FootballerIterator& getFootballerIterator(Match& match);

		void updateSwitchScore(const Time& currTime, Footballer& footballer, ScoreUpdateState& updateState, BallSimulation* pSimul);
		void insertScoreSorted(Footballer& footballer, SoftFootballerArray& sortedTopArray);
		void updateTopArray(Team& team, ScoreUpdateState& updateState);
		void updatePlayerInfluence(Footballer& footballer, ScoreUpdateState& updateState);

		void insertScoreSorted_BallDist(Match& match, Footballer& footballer, SoftFootballerArray& sortedTopArray, const Time& time);
		bool compareIsBetterThan_BallDist(Match& match, Footballer& compared, Footballer& ref, const Time& time);
		void updateTopFootballerArray_BallDist(Match& match, const Time& time);
		
		void updateTopFootballerArray_PathIntercept(Match& match, const Time& time);
		//void updateTopFootballerArray(Match& match, const Time& time);

		bool makeSwitches(Match& match, const Time& time);
		void updatePlayerState(Match& match, Player& player);

	protected:

		SoftPtr<Match> mMatch;

		SimulationID mProcessedPathInterceptSimulID;
		Time mNextUpdateTime;
		Time mPathInterceptUpdateTime;

		bool mModeWasPathIntercept;
		Time mPathInterceptLastChangeTime;
		
		FootballerMultiFrameProcessingSetup mFrameProcessingSetup;
		FootballerMultiFrameProcessingState mFrameProcessingState;

		SoftFootballerArray mTopFootballers[2];
		SoftFootballerArray mTempFootballers;
	};

} }

#endif