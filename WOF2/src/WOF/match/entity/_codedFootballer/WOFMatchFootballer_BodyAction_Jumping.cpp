#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY3

#include "WOFMatchFootballer_BodyAction_Jumping.h"
#include "../../WOFMatch.h"

namespace WOF { namespace match {

bool Footballer_BodyAction_Jumping::start(Body& body, const Time& time, ActionImpl* pCurrAction) {

	Match& match = body.mNodeMatch;

	{
		JumpType jumpType = mRequestJumpType.isValid ? mRequestJumpType.value : Jump_Up;
		mRequestJumpType.onConsumedValue();

		SkeletonInstance& skelInst = body.getSkelInst();

		skelInst.setAnimation(body.getAnimIndex(jumpTypeToAnimEnum(jumpType)), 1.0f);
		skelInst.setLoop(false);
	}
	
	{
		Match& match = body.mNodeMatch;

		float soundMag = match.getCoordSys().convUnit(14.0f);
		match.getSoundManager().playStockSound(SoundManager::Sound_Jump, body.getPosition(), soundMag, true);
	}

	return true;
}

bool Footballer_BodyAction_Jumping::setJumpTypeFromRelTargetPos(Body& body, const Time& time, const Vector3* pPos) {

	JumpType jumpType;

	if (pPos) {

		float horizDist = pPos->el[Scene_Right] - body.getPosition().el[Scene_Right];
		float bodySizeHoriz = MOBBExtents(body.mCollider.volume.toOBB()).el[Scene_Right];

		if (horizDist > bodySizeHoriz) {

			jumpType = Jump_UpRight;

		} else if (horizDist < -bodySizeHoriz) {

			jumpType = Jump_UpLeft;

		} else {

			jumpType = Jump_Up;
		}

	} else {

		jumpType = Jump_Up;
	}
	
	return setJumpType(body, time, jumpType);
}

bool Footballer_BodyAction_Jumping::setJumpTypeFromTargetBodyDir(Body& body, const Time& time, const Vector3* pDir) {

	JumpType jumpType;

	if (pDir && !pDir->isZero()) {

		float ang = angle(body.getCurrFacing(), *pDir, body.mNodeMatch->getCoordAxis(Scene_Up));

		if ((fabs(ang) <= k1DegreeInRad)) {

			jumpType = Jump_Up;

		} else if (ang > 0.0f) {

			jumpType = Jump_UpRight;

		} else if (ang < 0.0f) {

			jumpType = Jump_UpLeft;
		} 

	} else {

		jumpType = Jump_Up;
	}
	
	return setJumpType(body, time, jumpType);
}

FootballerBodyDefs::AnimEnum Footballer_BodyAction_Jumping::jumpTypeToAnimEnum(const JumpType& type) {

	switch (type) {

		case Jump_Up: return Anim_JumpingUp;
		case Jump_UpLeft: return Anim_JumpingUpLeft;
		case Jump_UpRight: return Anim_JumpingUpRight;
	}

	assrt(false);
	return Anim_JumpingUp;
}

void Footballer_BodyAction_Jumping::stop(Body& body, const Time& time) {
}

void Footballer_BodyAction_Jumping::cancelSwitchAction() {

	mQueuedAction.destroy();
}

Time Footballer_BodyAction_Jumping::estimateRemainingTime(Body& body) {

	return body.getSkelInst().getRemainingTime();
}

void Footballer_BodyAction_Jumping::cancelSetJumpType() {

	mRequestJumpType.isValid = false;
}

bool Footballer_BodyAction_Jumping::setJumpType(Body& body, const Time& time, const JumpType& val) {

	bool set = true;

	if (set) {

		mRequestJumpType.value = val;
		mRequestJumpType.onSetValue();
	}

	return set;
}

const FootballerBodyDefs::JumpType* Footballer_BodyAction_Jumping::getSetJumpType() {

	return mRequestJumpType.isValid ? &mRequestJumpType.value : NULL;
}

bool Footballer_BodyAction_Jumping::switchActionIdle(Body& body, const Time& time) {

	mQueuedAction = &body.getActionIdle();

	return false;
}

bool Footballer_BodyAction_Jumping::switchActionRunning(Body& body, const Time& time) {

	mQueuedAction = &body.getActionRunning();

	return false;
}

bool Footballer_BodyAction_Jumping::switchActionTackling(Body& body, const Time& time) {

	mQueuedAction = &body.getActionTackling();

	return false;
}

bool Footballer_BodyAction_Jumping::switchActionJumping(Body& body, const Time& time) {

	mQueuedAction = &body.getActionJumping();

	return false;
}

bool Footballer_BodyAction_Jumping::switchOnEnded(Body& body, const Time& time) {

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

bool Footballer_BodyAction_Jumping::handleRequests(Body& body, const Time& time) {

	return true;
}

void Footballer_BodyAction_Jumping::frameMoveTick(Body& body, const Time& time, const TickCount& tickIndex, const float& tickLength) {

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