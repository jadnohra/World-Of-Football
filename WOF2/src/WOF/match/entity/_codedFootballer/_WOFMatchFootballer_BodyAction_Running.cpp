#include "select.h"
#ifdef FOOTBALLER_BODY2

#include "WOFMatchFootballer_BodyAction_Running.h"
#include "WOFMatchFootballer_Body2.h"
#include "WOFMatchFootballer_BodyActions.h"

#include "../../WOFMatch.h"

namespace WOF { namespace match {

void Footballer_BodyAction_Running::onRequestHeadFacing(Body& body, Facing& facing) {

	body.addEvent(Evt_ReqRejected, Var_HeadFacing);
}

void Footballer_BodyAction_Running::onRequestBodyFacing(Body& body, Facing& facing) {

	facing.setTargetFromRequest();
	body.addEvent(Evt_ReqExecuted, Var_BodyFacing);
}

void Footballer_BodyAction_Running::onRequestRunDir(Body& body) {

	body.mRunDir.setValueFromRequest();
	body.addEvent(Evt_ReqExecuted, Var_RunDir);
}

float Footballer_BodyAction_Running::speedToAnimSpeed(const float& speed) { 
	
	return 1.0f; 
}

void Footballer_BodyAction_Running::onRequestRunSpeed(Body& body) {

	body.mRunSpeed.setValueFromRequest();
	body.addEvent(Evt_ReqExecuted, Var_RunSpeed);

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.setSpeedFactor(body.mNodeMatch->getCoordSys().invConvUnit(body.mRunSpeed.value));
	}
}

bool Footballer_BodyAction_Running::start(BodyActions& actions, Body& body, Event& outFailureResult) {

	{
		if (body.mHeadFacing.isChangingWithNoRequests()) {

			body.addEvent(Evt_ReqCancelled, Var_HeadFacing, Cause_ActionEnded);
		}

		if (body.mRunDir.isChangingWithNoRequests()) {

			body.addEvent(Evt_ReqExecuted, Var_RunDir);
		}

		if (body.mRunSpeed.isChangingWithNoRequests()) {

			body.addEvent(Evt_ReqExecuted, Var_RunSpeed);
		}
	}

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.setAnimation(body.getAnimIndex(Anim_Running), 1.0f);
		skelInst.setLoop(true);
	}

	actions.setActive(body, this);

	return true;
}

bool Footballer_BodyAction_Running::handleRequests(BodyActions& actions, Body& body) {

	if (body.mPendingRequests.count) {

		for (Index i = 0; i < body.mPendingRequests.count; ++i) {

			switch (body.mPendingRequests[i]) {

				case Var_HeadFacing: onRequestHeadFacing(body, body.mHeadFacing); break;
				case Var_BodyFacing: onRequestBodyFacing(body, body.mBodyFacing); break;
				case Var_RunDir: onRequestRunDir(body); break;
				case Var_RunSpeed: onRequestRunSpeed(body); break;
			}
		}

		body.clearPendingRequests();
	}

	return true;
}


void Footballer_BodyAction_Running::frameMoveTick(BodyActions& actions, Body& body, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

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

	{
		body.nodeMarkDirty();

		Vector3 posDiff;

		body.mRunDir.value.mul(tickLength * body.mRunSpeed.value, posDiff);
		body.mTransformLocal.movePosition(posDiff);
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

bool Footballer_BodyAction_Running::switchTo(BodyActions& actions, Body& body, const ActionType& nextAction, ActionImpl& nextActionImpl, Event& outResult) {

	if (!nextActionImpl.start(actions, body, outResult)) {

		return false;
	}

	outResult.set(Evt_ReqExecuted, Var_Action);
	return true;
}

} }

#endif