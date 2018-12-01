#include "WOFMatchFootballer.h"

#include "../WOFMatch.h"

namespace WOF {

MatchFootballer::MatchFootballer() {

	mTeam = Match_TeamInvalid;
	mNumber = MatchFootballerNumberInvalid;
}

void MatchFootballer::cancelInit() {

	mTeam = Match_TeamInvalid;
	mNumber = MatchFootballerNumberInvalid;
	brain_cancelInit();
}

bool MatchFootballer::isInited() {

	return (mTeam != Match_TeamInvalid) && (mNumber!= MatchFootballerNumberInvalid);
}

bool MatchFootballer::bindNode(Match& match, MatchTeamEnum team, MatchFootballerNumber number, SkeletalMeshPropertyManager& skelMeshProps) {

	mMatch = &match;
	mTeam = team;
	mNumber = number;

	if (brain_init(match, skelMeshProps) == false) {

		assrt(false);
		cancelInit();
		return false;
	}

	return true;
}

}