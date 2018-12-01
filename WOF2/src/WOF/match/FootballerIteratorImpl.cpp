#include "entity/footballer/Footballer.h"

#include "FootballerIteratorImpl.h"

#include "Match.h"

namespace WOF { namespace match {

FootballerIndex FootballerIterator_ActiveFotballers::getFootballerCount(Filter filter) {

	return mMatch->getTeam(Team_A).getFootballerCount(filter) + mMatch->getTeam(Team_B).getFootballerCount(filter);
}

bool FootballerIterator_ActiveFotballers::iterateFootballers(Footballer*& pInOutFootballer, FootballerIndex& iter, Filter filter) {

	if (pInOutFootballer == NULL) {

		mIterTeamA = true;
	}

	if (mIterTeamA) {

		if (mMatch->getTeam(Team_A).iterateFootballers(pInOutFootballer, iter, filter)) {

			return true;

		} else {

			pInOutFootballer = NULL;
			mIterTeamA = false;
		}
	} 

	return mMatch->getTeam(Team_B).iterateFootballers(pInOutFootballer, iter, filter);
}
			
} }

