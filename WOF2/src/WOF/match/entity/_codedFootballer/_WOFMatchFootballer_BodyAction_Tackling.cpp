#include "select.h"
#ifdef FOOTBALLER_BODY2

#include "WOFMatchFootballer_BodyAction_Tackling.h"
#include "WOFMatchFootballer_Body2.h"
#include "WOFMatchFootballer_BodyActions.h"

#include "../../WOFMatch.h"

namespace WOF { namespace match {

void Footballer_BodyAction_Tackling::onRequestHeadFacing(Body& body, Facing& facing) {

	body.addEvent(Evt_ReqRejected, Var_HeadFacing);
}

void Footballer_BodyAction_Tackling::onRequestBodyFacing(Body& body, Facing& facing) {

	body.addEvent(Evt_ReqRejected, Var_BodyFacing);
}

bool Footballer_BodyAction_Tackling::initFromPastAction(BodyActions& actions, Body& body, Event& outFailureResult) {

	Match& match = body.mNodeMatch;

	Vector3 startVel;
	bool hasStartVel =  (match.mTweakTackleSlideTime != 0.0f) && (match.mTweakTackleSpeedCoeff != 0.0f);
	
	if (hasStartVel) {
	
		if (body.getActionType() == Action_Running) {

			startVel = body.getRunSpeed().value;
			
		} else {

			hasStartVel = false;
		}
	}

	if (hasStartVel) {

		startVel.mul(match.mTweakTackleSpeedCoeff, mVelocity);
		mAcceleration = mVelocity.mag() / match.mTweakTackleSlideTime;

	} else {

		mVelocity.zero();
		mAcceleration = 0.0f;
	}

	return true;
}

bool Footballer_BodyAction_Tackling::start(BodyActions& actions, Body& body, Event& outFailureResult) {

	if (initFromPastAction(actions, body, outFailureResult)) {

		return false;
	}
	
	{
		if (body.mHeadFacing.isChangingWithNoRequests()) {

			body.addEvent(Evt_ReqCancelled, Var_HeadFacing, Cause_ActionEnded);
		}

		if (body.mBodyFacing.isChangingWithNoRequests()) {

			body.addEvent(Evt_ReqCancelled, Var_BodyFacing, Cause_ActionEnded);
		}
	}

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.setAnimation(body.getAnimIndex(Anim_Tackling), 1.0f);
		skelInst.setLoop(false);
	}

	actions.setActive(body, this);

	return true;
}

bool Footballer_BodyAction_Tackling::handleRequests(BodyActions& actions, Body& body) {

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

bool Footballer_BodyAction_Tackling::switchOnEnded(BodyActions& actions, Body& body) {

	SkeletonInstance& skelInst = body.getSkelInst();

	if (skelInst.reachedAnimationEnd()) {

		Event switchEvt;

		ActionType queuedAction = body.getQueuedAction();

		if ((queuedAction != Action_None) && (queuedAction != Action_Idle)) {

			if (!actions.trySwitchTo(body, queuedAction, true, switchEvt)) {

				assrt(false);
			}
		}

		if (!actions.trySwitchTo(body, Action_Idle, true, switchEvt)) {

			assrt(false);
		}

		return true;
	}

	return false;
}

void Footballer_BodyAction_Tackling::frameMoveTick(BodyActions& actions, Body& body, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

	if (switchOnEnded(actions, body))
		return;

	if (!handleRequests(actions, body)) {

		return;
	}

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.addTime(tickLength);
	}

	{
		body.nodeMarkDirty();

		Vector3 posDiff;

		mVelocity.substractNoReverse(mAcceleration * tickLength);

		mVelocity.mul(tickLength, posDiff);
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

	} else {
		
	}
}

bool Footballer_BodyAction_Tackling::switchTo(BodyActions& actions, Body& body, const ActionType& nextAction, ActionImpl& nextActionImpl, Event& outResult) {

	if (!body.getSkelInst().reachedAnimationEnd()) {

		outResult.set(Evt_ReqRejected, Cause_ActionInProgress);

		return false;
	}

	if (!nextActionImpl.start(actions, body, outResult)) {

		return false;
	}

	outResult.set(Evt_ReqExecuted, Var_Action);
	return true;
}

} }

#endif