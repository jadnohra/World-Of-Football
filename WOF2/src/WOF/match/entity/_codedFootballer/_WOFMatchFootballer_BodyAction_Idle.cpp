#include "select.h"
#ifdef FOOTBALLER_BODY2

#include "WOFMatchFootballer_BodyAction_Idle.h"
#include "WOFMatchFootballer_Body2.h"
#include "WOFMatchFootballer_BodyActions.h"

namespace WOF { namespace match {

void Footballer_BodyAction_Idle::onRequestHeadFacing(Body& body, Facing& facing) {

	body.addEvent(Evt_ReqRejected, Var_HeadFacing);
}

void Footballer_BodyAction_Idle::onRequestBodyFacing(Body& body, Facing& facing) {

	facing.setTargetFromRequest();
	body.addEvent(Evt_ReqExecuted, Var_BodyFacing);
}

bool Footballer_BodyAction_Idle::start(BodyActions& actions, Body& body, Event& outFailureResult) {

	{
		if (body.mHeadFacing.isChangingWithNoRequests()) {

			body.addEvent(Evt_ReqCancelled, Var_HeadFacing, Cause_ActionEnded);
		}
	}

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.setAnimation(body.getAnimIndex(Anim_Idle), staminaToAnimSpeed(body.mStamina));
		skelInst.setLoop(true);
	}
	
	actions.setActive(body, this);

	return true;
}

bool Footballer_BodyAction_Idle::handleRequests(BodyActions& actions, Body& body) {

	if (body.mPendingRequests.count) {

		if (body.mHeadFacing.isRequestPending) {

			onRequestHeadFacing(body, body.mHeadFacing);
		}

		if (body.mBodyFacing.isRequestPending) {

			onRequestBodyFacing(body, body.mBodyFacing);
		}

		body.clearPendingRequests();
	}

	return true;
}

void Footballer_BodyAction_Idle::frameMoveTick(BodyActions& actions, Body& body, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

	if (!handleRequests(actions, body)) {

		return;
	}

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.addTime(tickLength);
	}

	{
		Facing& facing = body.mBodyFacing;

		if (facing.isChanging) {

			body.nodeMarkDirty();
			body.orientDirection(facing.target);
		} 
	}

	bool wasBlocked = false;
	{
		if (body.nodeIsDirty()) {

			body.processCollisions(kStockCollisionResolveController, wasBlocked);
		}
	}

	if (wasBlocked) {

		body.addEvent(Evt_ForcedTemporaryChange, Vars_BodyPhysics);

		/*
		{
			//maybe not needed, check actual facing
			//Facing& facing = body.mBodyFacing;
			body.addEvent(Evt_ForcedTemporaryChange, Var_BodyFacing);
		}
		*/

	} else {

		{
			Facing& facing = body.mBodyFacing;

			if (facing.isChanging) {

				facing.setValueFromTarget();
				body.addEvent(Evt_ReqExecuted, Var_BodyFacing);
			}
		}
	}
}

bool Footballer_BodyAction_Idle::switchTo(BodyActions& actions, Body& body, const ActionType& nextAction, ActionImpl& nextActionImpl, Event& outResult) {

	if (!nextActionImpl.start(actions, body, outResult)) {

		return false;
	}

	outResult.set(Evt_ReqExecuted, Var_Action);
	return true;
}


} }

#endif