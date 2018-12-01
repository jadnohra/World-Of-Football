#include "WOFMatchControlSwitcher.h"

#include "../WOFMatch.h"
#include "../WOFMatchObjectIncludes.h"

#include "WOFMatchControlSwitchEvaluator_BallPathIntercept.h"

namespace WOF { namespace match {


void ControlSwitcher::init(Match& match) {

	mMatch = &match;

	mEvaluator_BallPathIntercept[_TeamA].destroy();
	mEvaluator_BallPathIntercept[_TeamB].destroy();

	WE_MMemNew(mEvaluator_BallPathIntercept[_TeamA].ptrRef(), ControlSwitchEvaluator_BallPathIntercept());
	WE_MMemNew(mEvaluator_BallPathIntercept[_TeamB].ptrRef(), ControlSwitchEvaluator_BallPathIntercept());

	mEvaluator_BallPathIntercept[_TeamA]->init(match);
	mEvaluator_BallPathIntercept[_TeamB]->init(match);

	mEvaluator_BallPathIntercept[_TeamA]->resetFootballerIterator(&match.getTeam(_TeamA));
	mEvaluator_BallPathIntercept[_TeamB]->resetFootballerIterator(&match.getTeam(_TeamB));
}

void ControlSwitcher::getFootballerDebugText(Match& match, Footballer_Brain& footballer, String& str) {

	if (mValueValidityState.isValid[SwitchControl::VID_BallPathInterceptTime]) {

		float value;
		bool isValid = footballer.mSwitchState.getValue_BallPathInterceptTime(footballer, value);

		if (isValid) {

			str.assignEx(L"%f", value);
		}
	}
}

void ControlSwitcher::frameMove(Match& match, const Time& time) {

	mEvaluator_BallPathIntercept[_TeamA]->frameMove(match, time, mValueValidityState);
}

Footballer* ControlSwitcher::switchGetNextFootballer(Team& team, FootballerSwitchTechnique technique, Player& player, Footballer* pCurrFootballer, bool isManualSwitch) {

	if (technique == FST_Default) {

		technique = mMatch->mFootballerSwitchSetup.technique;
	}

	/*
	SoftPtr<Footballer> owner =  switch_Owner(team, player, pCurrFootballer, isManualSwitch);

	if (owner.isValid()) {

		return owner;
	}

	switch (technique) {

		case FST_ShotAnalysis: {

			return switch_ShotAnalysis(team, player, pCurrFootballer, isManualSwitch, mState.shotAnalysis);

		} break;

		case FST_BallDist: {

			return switch_BallDist(team, player, pCurrFootballer, isManualSwitch);
			
		} break;

		case FST_NextNumber:
		default: {

			return switch_NextNumber(team, player, pCurrFootballer, isManualSwitch);
			
		} break;
	}
	*/


	return NULL;
}

void ControlSwitcher::render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass) {

	/*
	if (flagExtraRenders) {

		for (State_ShotAnalysis::Trajectories::Index i = 0; i < mState.shotAnalysis.trajectories.count; ++i) {

			renderer.draw(mState.shotAnalysis.trajectories.el[i].plane, 100.0f, &Matrix4x3Base::kIdentity);
		}
	}
	*/
}

} }