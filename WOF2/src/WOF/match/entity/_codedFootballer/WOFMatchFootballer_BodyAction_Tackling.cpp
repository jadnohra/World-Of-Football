#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY3

#include "WOFMatchFootballer_BodyAction_Tackling.h"
#include "../../WOFMatch.h"
#include "WOFMatchFootballer.h"

namespace WOF { namespace match {

bool Footballer_BodyAction_Tackling::load(Body& body, BufferString& tempStr, DataChunk* pActions, CoordSysConv* pConv) {

	if (pActions) {

		SoftRef<DataChunk> action = pActions->getChild(L"dynamicTackle");

		if (action.isValid()) {

			SkeletonInstance& skelInst = body.getSkelInst();

			float animLength = skelInst.getAnimationLength(body.getAnimIndex(Anim_Tackling), 1.0f, false);
			
			/*
			mDynamicTackle.destroy();
			WE_MMemNew(mDynamicTackle.ptrRef(), FootballerBody_ConfigurableAction());

			if (mDynamicTackle->load(body, tempStr, action, pConv, animLength, body.mNodeMatch->getTime().getFrameMoveTime(), this)) {
				
			} else {

				mDynamicTackle.destroy();
				return false;
			}
			*/
		}
	}

	return true;
}

bool Footballer_BodyAction_Tackling::start(Body& body, const Time& time, ActionImpl* pCurrAction) {

	Match& match = body.mNodeMatch;

	Vector3 startVel;
	bool hasStartVel =  (match.mTweakTackleSlideTime != 0.0f) && (match.mTweakTackleSpeedCoeff != 0.0f);
	
	if (hasStartVel) {
	
		if (mRequestTackleVel.isValid) {

			startVel = mRequestTackleVel.value;
			hasStartVel = !(startVel.isZero());

			mRequestTackleVel.onConsumedValue();

			if (hasStartVel) {

				Vector3 bodyFacingDir;
				startVel.normalize(bodyFacingDir);

				body.orientDirection(bodyFacingDir);
			}

		} else if (pCurrAction && pCurrAction->getType() == Action_Running) {

			SoftPtr<Footballer_BodyAction_Running> running = (Footballer_BodyAction_Running*) pCurrAction;

			body.getCurrFacing().mul(running->getCurrSpeed(body), startVel);
			
		} else {

			hasStartVel = false;
		}
	}

	float animSpeedFactor = 1.0f;

	if (hasStartVel) {

		startVel.mul(match.mTweakTackleSpeedCoeff, mVelocity);
		mAcceleration = mVelocity.mag() / match.mTweakTackleSlideTime;

		/*
		if (mDynamicTackle.isValid()) {

			//animSpeedFactor = mDynamicTackle->getSpeedFactor(body, Vector3::kZero, startVel, 1.0f, match.getTime().getFrameMoveTime());
		}
		*/

	} else {

		mVelocity.zero();
		mAcceleration = 0.0f;
	}


	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.setAnimation(body.getAnimIndex(Anim_Tackling), animSpeedFactor);
		skelInst.setLoop(false);
	}

	{
		Match& match = body.mNodeMatch;

		float soundMag = match.getCoordSys().convUnit(14.0f);
		match.getSoundManager().playStockSound(SoundManager::Sound_Tackle, body.getPosition(), soundMag, true);
	}
	
	return true;
}

void Footballer_BodyAction_Tackling::stop(Body& body, const Time& time) {
}

void Footballer_BodyAction_Tackling::cancelSwitchAction() {

	mQueuedAction.destroy();
}

Time Footballer_BodyAction_Tackling::estimateRemainingTime(Body& body) {

	return body.getSkelInst().getRemainingTime();
}

void Footballer_BodyAction_Tackling::cancelSetTackleVel() {

	mRequestTackleVel.isValid = false;
}

const Vector3* Footballer_BodyAction_Tackling::getSetTackleVel() {

	return mRequestTackleVel.isValid ? &mRequestTackleVel.value : NULL;
}

bool Footballer_BodyAction_Tackling::setTackleVel(Body& body, const Time& time, const Vector3& val) {

	bool set = true;

	if (set) {

		mRequestTackleVel.value = val;
		mRequestTackleVel.onSetValue();
	}

	return set;
}

bool Footballer_BodyAction_Tackling::switchActionIdle(Body& body, const Time& time) {

	mQueuedAction = &body.getActionIdle();

	return false;
}

bool Footballer_BodyAction_Tackling::switchActionRunning(Body& body, const Time& time) {

	mQueuedAction = &body.getActionRunning();

	return false;
}

bool Footballer_BodyAction_Tackling::switchActionTackling(Body& body, const Time& time) {

	mQueuedAction = &body.getActionTackling();

	return false;
}

bool Footballer_BodyAction_Tackling::switchActionJumping(Body& body, const Time& time) {

	mQueuedAction = &body.getActionJumping();

	return false;
}

bool Footballer_BodyAction_Tackling::switchOnEnded(Body& body, const Time& time) {

	if (body.getSkelInst().reachedAnimationEnd()) {

		SoftPtr<ActionImpl> actionIdle = &body.getActionIdle();

		if (!mQueuedAction.isValid()) {

			mQueuedAction = actionIdle;
		}

		while (mQueuedAction.isValid()) {

			SoftPtr<ActionImpl> switchAction = mQueuedAction;
			mQueuedAction.destroy();

			switch(switchAction->getType()) {

				default: {

					if (switchAction->start(body, time, this)) {

						stop(body, time);
						body.setActiveAction(switchAction);

						cancelSetTackleVel();

						return true;
					} 
				}
			}

			if (mQueuedAction != actionIdle) {

				mQueuedAction = actionIdle;

			} else {

				assrt(false);
				return true;
			}
		}
	}

	return false;
}

bool Footballer_BodyAction_Tackling::handleRequests(Body& body, const Time& time) {

	return true;
}

void Footballer_BodyAction_Tackling::frameMoveTick(Body& body, const Time& time, const TickCount& tickIndex, const float& tickLength) {

	if (switchOnEnded(body, time)) {

		return;
	}

	if (!handleRequests(body, time)) {

		return;
	}

	body.nodeMarkDirty();

	{
		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.addTime(tickLength);
	}

	Vector3 posDiff;

	{
		mVelocity.substractNoReverse(mAcceleration * tickLength);

		mVelocity.mul(tickLength, posDiff);
		body.mTransformLocal.movePosition(posDiff);
	}

	bool wasBlocked = false;
	{
		if (body.nodeIsDirty()) {

			body.updateCollisions();
			body.sendTackleEvents(time);
			body.resolveCollisions(body.getDefaultCollResolveMode(), true, wasBlocked, posDiff);
		}
	}
}

/*
FootballerBody_ConfigurableAction::Command* Footballer_BodyAction_Tackling::createCommand(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, const Time& totalTime) {

	SoftPtr<FootballerBody_ConfigurableAction::Command> command;

	WE_MMemNew(command.ptrRef(), FootballerBody_ConfigurableAction::StockCommand(FootballerBody_ConfigurableAction::Command_PlaySound));

	return command;
}
*/

} }

#endif