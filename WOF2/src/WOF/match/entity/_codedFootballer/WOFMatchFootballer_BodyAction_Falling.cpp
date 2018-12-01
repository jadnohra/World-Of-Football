#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY3

#include "WOFMatchFootballer_BodyAction_Falling.h"
#include "../../WOFMatch.h"

namespace WOF { namespace match {

bool Footballer_BodyAction_Falling::start(Body& body, const Time& time, ActionImpl* pCurrAction) {

	Match& match = body.mNodeMatch;

	{
		FallType fallType = mRequestFallType.isValid ? mRequestFallType.value : Fall_Fwd;
		mRequestFallType.onConsumedValue();

		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.setAnimation(body.getAnimIndex(fallTypeToAnimEnum(fallType)), 1.0f);
		skelInst.setLoop(false);
	}

	{
		Match& match = body.mNodeMatch;

		float soundMag = match.getCoordSys().convUnit(14.0f);
		match.getSoundManager().playStockSound(SoundManager::Sound_Fall, body.getPosition(), soundMag, true);
	}
	
	return true;
}

bool Footballer_BodyAction_Falling::setFallTypeFromTackle(Body& body, const Time& time, Body* pTackler) {

	FallType fallType;

	if (pTackler) {

		Vector3 tacklerDir;
		
		pTackler->getPosition().subtract(body.getPosition(), tacklerDir);
		/*
		float dot = body.getCurrFacing().dot(relPos);
		fallType = dot >= 0.0f ? Fall_Fwd : Fall_Back;
		*/

		Vector3 discreeteDir;

		tacklerDir.normalize();

		int step = discretizeDirection(body.getCurrFacing(), body.mNodeMatch->getCoordAxis(Scene_Up), tacklerDir, 4, discreeteDir);

		switch(step) {

			case 0: fallType = Fall_Fwd; break;
			case 1: fallType = Fall_Right; break;
			case 2: fallType = Fall_Back; break;
			case 3: fallType = Fall_Left; break;
			default: assrt(false); fallType = Fall_Fwd; break;
		}

	} else {

		fallType = Fall_Fwd;
	}
	
	return setFallType(body, time, fallType);
}

FootballerBodyDefs::AnimEnum Footballer_BodyAction_Falling::fallTypeToAnimEnum(const FallType& type) {

	switch (type) {

		case Fall_Fwd: return Anim_FallingFwd;
		case Fall_Back: return Anim_FallingBack;
		case Fall_Left: return Anim_FallingLeft;
		case Fall_Right: return Anim_FallingRight;
	}

	assrt(false);
	return Anim_FallingFwd;
}

void Footballer_BodyAction_Falling::stop(Body& body, const Time& time) {
}

void Footballer_BodyAction_Falling::cancelSwitchAction() {

	mQueuedAction.destroy();
}

Time Footballer_BodyAction_Falling::estimateRemainingTime(Body& body) {

	return body.getSkelInst().getRemainingTime();
}

void Footballer_BodyAction_Falling::cancelSetFallType() {

	mRequestFallType.isValid = false;
}

bool Footballer_BodyAction_Falling::setFallType(Body& body, const Time& time, const FallType& val) {

	bool set = true;

	if (set) {

		mRequestFallType.value = val;
		mRequestFallType.onSetValue();
	}

	return set;
}

const FootballerBodyDefs::FallType* Footballer_BodyAction_Falling::getSetFallType() {

	return mRequestFallType.isValid ? &mRequestFallType.value : NULL;
}

bool Footballer_BodyAction_Falling::switchActionIdle(Body& body, const Time& time) {

	mQueuedAction = &body.getActionIdle();

	return false;
}

bool Footballer_BodyAction_Falling::switchActionRunning(Body& body, const Time& time) {

	mQueuedAction = &body.getActionRunning();

	return false;
}

bool Footballer_BodyAction_Falling::switchActionTackling(Body& body, const Time& time) {

	mQueuedAction = &body.getActionTackling();

	return false;
}

bool Footballer_BodyAction_Falling::switchActionJumping(Body& body, const Time& time) {

	mQueuedAction = &body.getActionJumping();

	return false;
}

bool Footballer_BodyAction_Falling::switchOnEnded(Body& body, const Time& time) {

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

bool Footballer_BodyAction_Falling::handleRequests(Body& body, const Time& time) {

	return true;
}

void Footballer_BodyAction_Falling::frameMoveTick(Body& body, const Time& time, const TickCount& tickIndex, const float& tickLength) {

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

	bool wasBlocked = false;
	{
		if (body.nodeIsDirty()) {

			body.updateCollisions();
			body.resolveCollisions(body.getDefaultCollResolveMode(), true, wasBlocked);
		}
	}
}

} }

#endif