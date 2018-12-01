#include "WOFMatchControlSwitchEvaluator_Ownership.h"

#include "../WOFMatch.h"

namespace WOF { namespace match {

ControlSwitchEvaluator_Ownership::ControlSwitchEvaluator_Ownership() {

	mFrameProcessingSetup.mode = FootballerMultiFrameProcessingSetup::M_SingleFrame;
}

void ControlSwitchEvaluator_Ownership::init(Match& match) {

	mFrameProcessingSetup.init(match.getTime(), 30.0f, 3.0f, true);
}

void ControlSwitchEvaluator_Ownership::frameMove(Match& match, const DiscreeteTime& time) {

	if (!mFootballerIterator.isValid())
		return;

	FootballerIndex footballerCount = mFootballerIterator->getFootballerCount();

	if (mFootballerIteratorIsDirty) {

		mFrameProcessingState.reset(mFrameProcessingSetup, footballerCount);
		mFootballerIteratorIsDirty = false;
	}

	FootballerMultiFrameIter iter;
	SoftPtr<Footballer> footballer;

	if (iter.prepare(mFrameProcessingSetup, mFrameProcessingState, mFootballerIterator)) {

		while (iter.next(mFootballerIterator, footballer.ptrRef())) {

			Footballer::BallOwnership& ownership = footballer->getBallOwnership();
			SwitchControl::Value& val = footballer->mSwitchState.value[SwitchControl::VID_BallOwnership];

			val.time = time.t_discreet;

			if (ownership.isBallOwner) {

				 val.val = ownership.ballControl;

			} else {

				val.val = 0.0f;
			}
		}
	}
}

} }