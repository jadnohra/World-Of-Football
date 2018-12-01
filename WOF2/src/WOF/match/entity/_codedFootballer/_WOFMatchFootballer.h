#ifndef _WOFMatchFootballer_h
#define _WOFMatchFootballer_h

#include "footballer/WOFMatchFootballer_Brain.h"

namespace WOF {

	class MatchFootballer : public MatchFootballer_Brain {
	public:

		MatchFootballer();

		bool isInited();
		bool bindNode(Match& match, MatchTeamEnum team, MatchFootballerNumber number, SkeletalMeshPropertyManager& skelMeshProps);

	public:
		
		MatchTeamEnum mTeam;
		MatchFootballerNumber mNumber;

	protected:

		void cancelInit();
	};
}

#endif