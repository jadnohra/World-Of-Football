#ifndef _WOFMatchControlSwitchEvaluator_h
#define _WOFMatchControlSwitchEvaluator_h

#include "WE3/WEPtr.h"
using namespace WE;

#include "../WOFMatchDataTypes.h"
#include "../WOFMatchTime.h"
#include "../WOFMatchFootballerIterator.h"
#include "WOFMatchControlSwitchFootballerState.h"

namespace WOF { namespace match {

	class Match;

	class ControlSwitchEvaluator {
	public:

		ControlSwitchEvaluator() : mFootballerIteratorIsDirty(true) {}

		virtual void init(Match& match) {}

		virtual void resetFootballerIterator(FootballerIterator* pIterator) {

			mFootballerIterator = pIterator;
			mFootballerIteratorIsDirty = true;
		}

		virtual void frameMove(Match& match, const Time& time, SwitchControl::ValueValidityState& validity) {};

	protected:

		bool mFootballerIteratorIsDirty;
		SoftPtr<FootballerIterator> mFootballerIterator;
		
	};

} }

#endif