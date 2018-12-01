#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY3

#include "WOFMatchFootballer_BodyAction_Idle.h"
#include "WOFMatchFootballer.h"

namespace WOF { namespace match {

bool Footballer_BodyAction_Idle::start(Body& body, const Time& time, ActionImpl* pCurrAction) {

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.setAnimation(body.getAnimIndex(Anim_Idle), staminaToAnimSpeed(1.0f));
		skelInst.setLoop(true);
	}
	
	return true;
}

void Footballer_BodyAction_Idle::stop(Body& body, const Time& time) {
}

void Footballer_BodyAction_Idle::cancelSwitchAction() {

	mRequestAction.destroy();
}

bool Footballer_BodyAction_Idle::switchActionIdle(Body& body, const Time& time) {

	//assrt(body.getAction() == this);

	return true;
}

bool Footballer_BodyAction_Idle::switchActionRunning(Body& body, const Time& time) {

	//assrt(body.getAction() == this);

	bool set = true;

	if (set) {

		mRequestAction = &body.getActionRunning();
	}

	return true;
}

bool Footballer_BodyAction_Idle::switchActionTackling(Body& body, const Time& time) {

	//assrt(body.getAction() == this);

	bool set = true;

	if (set) {

		mRequestAction = &body.getActionTackling();
	}

	return true;
}

bool Footballer_BodyAction_Idle::switchActionJumping(Body& body, const Time& time) {

	//assrt(body.getAction() == this);

	bool set = true;

	if (set) {

		mRequestAction = &body.getActionJumping();
	}

	return true;
}

void Footballer_BodyAction_Idle::handleBodyCommEvts(Body& body, const Time& time) {

	for (BodyCommEvents::Index i = 0; i < body.getBodyCommEventCount(); ++i) {

		BodyCommEvent& evt = body.getBodyCommEvent(i);

		if (evt.type == BodyComm_Tackled) {

			mRequestAction = &body.getActionFallingAndSetupFromTackle(time, evt.source);
		}
	}
}

bool Footballer_BodyAction_Idle::handleRequests(Body& body, const Time& time) {

	if (mRequestAction.isValid()) {

		SoftPtr<ActionImpl> switchAction = mRequestAction;
		mRequestAction.destroy();

		switch(switchAction->getType()) {

			default: {

				if (switchAction->start(body, time, this)) {

					stop(body, time);
					body.setActiveAction(switchAction);

					return false;
				} 
			}
		}
	}

	return true;
}

void Footballer_BodyAction_Idle::frameMoveTick(Body& body, const Time& time, const TickCount& tickIndex, const float& tickLength) {

	handleBodyCommEvts(body, time);

	if (!handleRequests(body, time)) {

		return;
	}

	body.nodeMarkDirty();

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.addTime(tickLength);
	}

	bool wasBlocked = false;
	{
		if (body.nodeIsDirty()) {

			body.updateCollisions();
			body.resolveCollisions(body.getDefaultCollResolveMode(), true, wasBlocked);
		}
	}

	if (!handleRequests(body, time)) {

		return;
	}
}

} }

#endif