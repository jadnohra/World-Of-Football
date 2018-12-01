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

	class ControlSwitchEvaluatorBase {
	public:

		ControlSwitchEvaluatorBase() : mFootballerIteratorIsDirty(true) {}

		void resetFootballerIterator(FootballerIterator* pIterator) {

			mFootballerIterator = pIterator;
			mFootballerIteratorIsDirty = true;
		}
	
	protected:

		bool mFootballerIteratorIsDirty;
		SoftPtr<FootballerIterator> mFootballerIterator;
	};

} }

#endif