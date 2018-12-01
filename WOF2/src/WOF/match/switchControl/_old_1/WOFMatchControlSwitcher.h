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

namespace WOF { namespace match {

	class Footballer_Brain;

	class ControlSwitcher {
	public:

		void init(Match& match);

		Footballer* switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch);

		void frameMove(Match& match, const Time& time);
		void render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass);

		void getFootballerDebugText(Match& match, Footballer_Brain& footballer, String& str);

	protected:

		SoftPtr<Match> mMatch;
		
		SwitchControl::ValueValidityState mValueValidityState;

		HardPtr<ControlSwitchEvaluator> mEvaluator_BallPathIntercept[2];
	};

} }

#endif