#ifndef _WOFMatchControlSwitchEvaluator_Ownership_h
#define _WOFMatchControlSwitchEvaluator_Ownership_h

#include "WOFMatchControlSwitchEvaluator.h"

namespace WOF { namespace match {

	class ControlSwitchEvaluator_Ownership : public ControlSwitchEvaluator {
	public:

		ControlSwitchEvaluator_Ownership();

		virtual void init(Match& match);

		virtual void frameMove(Match& match, const DiscreeteTime& time);

	protected:

		FootballerMultiFrameProcessingSetup mFrameProcessingSetup;
		FootballerMultiFrameProcessingState mFrameProcessingState;
	};


} }

#endif