#ifndef h_WOF_match_FootballerIteratorImpl
#define h_WOF_match_FootballerIteratorImpl

#include "FootballerIterator.h"

namespace WOF { namespace match {

	class Match;

	class FootballerIterator_ActiveFotballers : public FootballerIterator {
	public:

		void init(Match& match) { mMatch = &match; }

		virtual FootballerIndex getFootballerCount(Filter filter = F_None);
		virtual bool iterateFootballers(Footballer*& pInOutFootballer, FootballerIndex& iter, Filter filter = F_None);

	protected:

		SoftPtr<Match> mMatch;
		FootballerIndex mIterTeamA;
	};
			
} }


#endif