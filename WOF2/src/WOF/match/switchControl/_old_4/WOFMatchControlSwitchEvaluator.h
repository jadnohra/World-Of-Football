#ifndef _WOFMatchControlSwitchEvaluator_h
#define _WOFMatchControlSwitchEvaluator_h

#include "WE3/WEPtr.h"
using namespace WE;

#include "../WOFMatchDataTypes.h"
#include "../WOFMatchTime.h"
#include "../WOFMatchFootballerIterator.h"
#include "WOFMatchControlSwitchStates.h"

namespace WOF { namespace match {

	class Match;

	class ControlSwitchEvaluator {
	public:

		ControlSwitchEvaluator() : mFootballerIteratorIsDirty(true) {}

		virtual ~ControlSwitchEvaluator() {}

		virtual void init(Match& match) {}

		virtual void resetFootballerIterator(FootballerIterator* pIterator) {

			mFootballerIterator = pIterator;
			mFootballerIteratorIsDirty = true;
		}

		virtual void updateValidity(Match& match, const Time& time, SwitchControl::ValueValidity& validity) {};
		virtual void frameMove(Match& match, const Time& time, const SwitchControl::ValueValidity& validity) {};

	protected:

		bool mFootballerIteratorIsDirty;
		SoftPtr<FootballerIterator> mFootballerIterator;
		
	};

} }

#endif