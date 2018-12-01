#include "Match.h"

#include "Team.h"


namespace WOF { namespace match {

void Team::constructorTactical() {

	mTacticalInstantaneousBallOwnership = 0.5f;
	mTacticalBallOwnership = 0.5f;
	mTacticalBallOwnershipCatchupDelay = 5.0f;
	mTacticalInstantaneousBallOwnershipChangeTime = 0.0f;
}

void Team::updateTactical(const Clock& clock) {

	{
		Ball& ball = mMatch->getBall();

		const FootballerIndex& ownerCount = ball.getTeamOwnerCount(mTeam);
		const FootballerIndex& oppOwnerCount = ball.getTeamOwnerCount(opponentTeam(mTeam));

		float ownerCountDiff = (float) ownerCount - (float) oppOwnerCount;
		float totalOwnerCount = (float) ownerCount + (float) oppOwnerCount;
		float ownerCountRatio = totalOwnerCount == 0.0f ? 0.5f : ownerCountDiff / totalOwnerCount;

		float newInstValue = tcap(ownerCountRatio, 0.0f, 1.0f);

		if (newInstValue != mTacticalInstantaneousBallOwnership) {

			mTacticalInstantaneousBallOwnership = newInstValue;
			mTacticalInstantaneousBallOwnershipChangeTime = clock.getTime();
		}

		float instDiff = (mTacticalInstantaneousBallOwnership - mTacticalBallOwnership);
		
		if (instDiff != 0.0f) {

			float timeDiff = clock.getTime() - mTacticalInstantaneousBallOwnershipChangeTime;

			instDiff *= tmin(timeDiff / mTacticalBallOwnershipCatchupDelay, 1.0f);

			mTacticalBallOwnership += instDiff;
			mTacticalBallOwnership = tcap(mTacticalBallOwnership, 0.0f, 1.0f);
		}
	}
}

} }