#include "select.h"
#ifdef FOOTBALLER_BODY2

#include "WOFMatchFootballer_Body2.h"
#include "WOFMatchFootballer_BodyActions.h"

namespace WOF { namespace match {

Footballer_BodyActions::Footballer_BodyActions() {

	mActionMap[Action_Idle] = &mIdle;
	mActionMap[Action_Running] = &mRunning;
	mActionMap[Action_Tackling] = &mTackling;
}

bool Footballer_BodyActions::init(Body& body, ActionImpl& startAction) {

	mActive = &startAction;
	Event tempEvt;
	
	if (!mActive->start(*this, body, tempEvt)) {

		assrt(false);

		mActive.destroy();
		return false;
	}

	body.mAction.initWithValue(mActive->getType());

	return true;
}

void Footballer_BodyActions::frameMoveTick(Body& body, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {
	
	handleRequests(body);

	mActive->frameMoveTick(*this, body, time, tickIndex, tickLength);
}

float Footballer_BodyActions::estimateActionRemainingTime() {

	return -1.0f;
}

bool Footballer_BodyActions::trySwitchTo(Body& body, const ActionType& action, bool allowRedundant, Event& outEvt) {

	if (!allowRedundant && (action == mActive->getType())) {

		body.mAction.resetRequest();
		outEvt.set(Evt_ReqRedundant, Var_Action);

		return false;

	} else {

		return mActive->switchTo(*this, body, action, dref(getAction(action)), outEvt);
	}
}

void Footballer_BodyActions::handleRequests(Body& body) {

	if (body.mAction.isRequestPending) {

		Event& switchEvt = body.addEvent();
		switchEvt.type = Evt_None;

		trySwitchTo(body, body.mAction.request, false, switchEvt);

		switch (switchEvt.type) {

			case Evt_ReqRedundant: body.mAction.resetRequest(); break;
			case Evt_ReqExecuted: body.mAction.setValueFromRequest(); break;
			case Evt_ReqCancelled:
			case Evt_ReqRejected: body.mAction.resetRequest(); break;

			default: {

				assrt(false);

			} break;
		}
		
		/*
		if (body.mAction.request == mActive->getType()) {

			body.mAction.resetRequest();
			body.addEvent(Evt_ReqRedundant, Var_Action);

		} else {

			Event& switchEvt = body.addEvent();
			switchEvt.type = Evt_None;

			mActive->switchTo(*this, body, body.mAction.request, dref(getAction(body.mAction.request)), switchEvt);

			switch (switchEvt.type) {

				case Evt_ReqExecuted: body.mAction.setValueFromRequest(); break;
				case Evt_ReqCancelled:
				case Evt_ReqRejected: body.mAction.resetRequest(); break;
				default: {

					assrt(false);

				} break;
			}
		}
		*/
	}

	
}


} }

#endif