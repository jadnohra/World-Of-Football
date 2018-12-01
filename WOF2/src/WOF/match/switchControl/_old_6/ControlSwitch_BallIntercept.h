#ifndef h_WOF_match_ControlSwitch_BallIntercept
#define h_WOF_match_ControlSwitch_BallIntercept

#include "WE3/WETL/WETLArray.h"
#include "WE3/math/WEPlane.h"
using namespace WE;

#include "../DataTypes.h"
#include "ControlSwitchEvaluator.h"

namespace WOF { namespace match {

	class Footballer;
	class Match;
	struct BallCommand;
	class FootballerBallInterceptManager;

	class ControlSwitch_BallIntercept : public ControlSwitchEvaluatorBase {
	public:

		ControlSwitch_BallIntercept();

		void init(Match& match);

		bool update(Match& match, const Time& time, FootballerBallInterceptManager& manager, bool& consideredFootballersUpdated);
		inline const bool& isValid() { return mIsValid; }

		inline const FootballerIndex& getConsideredFootballerCount() { return mConsideredFootballers.count; }
		inline Footballer* getConsideredFootballer(const FootballerIndex& index) { return mConsideredFootballers[index]; }

	protected:

		SimulationID mCurrSimulID;
		bool mIsValid;

		SoftFootballerArray mIgnoredFootballers;
		SoftFootballerArray mConsideredFootballers;
		SoftFootballerArray mTempConsideredFootballers;

		void sortConsideredFootballer(const FootballerIndex& footballerIndex, Object* pCommandOwner, float commandOwnerScoreShift = 0.0f);
	};


} }

#endif