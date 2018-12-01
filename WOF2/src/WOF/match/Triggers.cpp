#include "Triggers.h"

#include "Match.h"

namespace WOF { namespace match {

MatchTrigger_Goal::MatchTrigger_Goal() : mTeam(Team_Invalid), mLastTriggerTime(0.0f) {
}

void MatchTrigger_Goal::init(Match& match, TeamEnum team) {

	mMatch = &match;
	mTeam = team;
}

void MatchTrigger_Goal::execute(const Vector3* pPos) {

	const Time& time = mMatch->getClock().getTime();

	if (time - mLastTriggerTime > 2.0f) {

		mMatch->onGoalTrigger(mTeam);
	}

	mLastTriggerTime = time;
}


} }
