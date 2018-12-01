#ifndef _WOFMatchControlSwitcher_h
#define _WOFMatchControlSwitcher_h

#include "WE3/WETL/WETLArray.h"
#include "WE3/math/WEPlane.h"
#include "WE3/WEPtr.h"
using namespace WE;

#include "../WOFMatchDataTypes.h"
#include "../WOFMatchObjectDefines.h"
#include "../WOFMatchBallSimulManager.h"
#include "WOFMatchControlSwitchEvaluator.h"
#include "../WOFMatchFootballerIteratorImpl.h"

namespace WOF { namespace match {

	class Footballer_Brain;

	class ControlSwitcher {
	public:

		void init(Match& match);

		Footballer* switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch);

		void frameMove(Match& match, const Time& time);
		void render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass);

		void getFootballerDebugText(Match& match, Footballer_Brain& footballer, String& str);

		void onActiveFootballersChange();

	protected:

		struct ScoreUpdateState {
		};

	protected:

		FootballerIterator& getFootballerIterator();
		void updateSwitchScore(const Time& currTime, Footballer& footballer, ScoreUpdateState& updateState, BallSimulManager::Simul* pSimul);
		void insertScoreSorted(Footballer& footballer, SoftFootballerArray& sortedTopArray);
		void updateTopArray(Team& team, ScoreUpdateState& updateState);
		void updatePlayerInfluence(Footballer& footballer, ScoreUpdateState& updateState);

		void insertScoreSorted_ModePathIntercept(Match& match, Footballer& footballer, SoftFootballerArray& sortedTopArray, BallSimulManager::Simul& simul, const Time& time);
		bool compareIsBetterThan_ModePathIntercept(Match& match, Footballer& compared, SwitchControl::Value_PathIntercept& intercept, Footballer& ref, const Time& time);
		void updateTopFootballerArray_ModePathIntercept(Match& match, const Time& time);

		void insertScoreSorted_ModeBallDist(Match& match, Footballer& footballer, SoftFootballerArray& sortedTopArray, const Time& time);
		bool compareIsBetterThan_ModeBallDist(Match& match, Footballer& compared, Footballer& ref, const Time& time);
		void updateTopFootballerArray_ModeBallDist(Match& match, const Time& time);

		void updateTopFootballerArray(Match& match, const Time& time);

		bool playerWantsToSwitch(Match& match, Player& player);

	protected:

		SoftPtr<Match> mMatch;
		
		SwitchControl::ValueValidity mValueValidity;

		HardPtr<ControlSwitchEvaluator> mEvaluator_BallPathIntercept[2];

		FootballerMultiFrameProcessingSetup mFrameProcessingSetup;
		FootballerMultiFrameProcessingState mFrameProcessingState;

		FootballerIterator_ActiveFotballers mFootballerIter;

		SoftFootballerArray mTopFootballers[2];
	};

} }

#endif