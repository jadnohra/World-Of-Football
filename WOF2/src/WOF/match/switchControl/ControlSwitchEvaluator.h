#ifndef h_WOF_match_ControlSwitchEvaluator
#define h_WOF_match_ControlSwitchEvaluator

#include "WE3/WEPtr.h"
using namespace WE;

#include "../DataTypes.h"
#include "../Clock.h"
#include "../FootballerIterator.h"
#include "ControlSwitchStates.h"

namespace WOF { namespace match {

	class Match;

	class ControlSwitchEvaluatorBase : public SwitchControl {
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