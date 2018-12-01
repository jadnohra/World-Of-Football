#include "../Match.h"
#include "../Team.h"
#include "../entity/footballer/Footballer.h"

#include "ScriptTeamFootballerIterator.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::ScriptTeamFootballerIterator, CTeamFootballerIterator);

namespace WOF { namespace match {

const TCHAR* ScriptTeamFootballerIterator::ScriptClassName = L"CTeamFootballerIterator";

void ScriptTeamFootballerIterator::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<ScriptTeamFootballerIterator>(ScriptClassName).
		func(&ScriptTeamFootballerIterator::script_setTeam, L"setTeam").
		func(&ScriptTeamFootballerIterator::setMaxConsecutiveNextCount, L"setMaxConsecutiveNextCount").
		func(&ScriptTeamFootballerIterator::reset, L"reset").
		func(&ScriptTeamFootballerIterator::next, L"next");
}

ScriptTeamFootballerIterator::ScriptTeamFootballerIterator() {

	mMaxConsecutiveNextCount = 0;
	reset();
}

void ScriptTeamFootballerIterator::setTeam(Match& match, TeamEnum team) {

	mTeam = &(match.getTeam(team));
	reset();
}

void ScriptTeamFootballerIterator::setMaxConsecutiveNextCount(int count) {

	mMaxConsecutiveNextCount = count <= 0 ? 0 : count;
}

void ScriptTeamFootballerIterator::reset() {

	mNextIndex = 0;
}

void ScriptTeamFootballerIterator::resetConsecutiveCycle() {

	mConsecutiveNextCount = 0;
	mConsecutiveCycleStartIndex = -1;
}

ScriptedFootballer* ScriptTeamFootballerIterator::next() {

	SoftPtr<ScriptedFootballer> ret;

	if (mTeam.isValid() && mTeam->getRawFootballerCount()) {

		if (mMaxConsecutiveNextCount) {

			if (mConsecutiveNextCount == mMaxConsecutiveNextCount) {

				resetConsecutiveCycle();

			} else {

				if (mNextIndex >= mTeam->getRawFootballerCount()) {

					mNextIndex = 0;
				}

				if (mNextIndex != mConsecutiveCycleStartIndex) {

					ret = &(mTeam->getRawFootballer(mNextIndex));

					if (mConsecutiveNextCount == 0)
						mConsecutiveCycleStartIndex = mNextIndex;

					++mNextIndex;
					++mConsecutiveNextCount;

				} else {

					resetConsecutiveCycle();
				}
			}

		} else {

			if (mNextIndex >= mTeam->getRawFootballerCount()) {

				mNextIndex = 0;

			} else {

				ret = &(mTeam->getRawFootballer(mNextIndex));
				++mNextIndex;
			}
		}
	}

	return ret;
}

} }