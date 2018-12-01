#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY3

#include "WE3/math/WEMathUtil.h"
using namespace WE;

#include "WOFMatchFootballer_BodyAction_Running.h"
#include "../../WOFMatch.h"

namespace WOF { namespace match {

Footballer_BodyAction_Running::Footballer_BodyAction_Running()
	: mIsActive(false), mHasRequests(false) {
}

bool Footballer_BodyAction_Running::init(Body& body, Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	memcpy(mSpeedTable, match.mTweakRunSpeedTable, sizeof(mSpeedTable));
	
	return true;
}

float Footballer_BodyAction_Running::speedToAnimSpeed(Body& body, const float& speed) { 
	
	return body.mNodeMatch->getCoordSys().invConvUnit(speed); 
}

/*
const FootballerBodyCaps::RunResponseCap& Footballer_BodyAction_Running::getCurrRunResponseCap(Body& body) {

	return body.getBallOwnership().ballControl > 0.0f ? (RunResponseCap&) mDribbleCap : (RunResponseCap&) mRunCap;
}
*/

bool Footballer_BodyAction_Running::start(Body& body, const Time& time, ActionImpl* pCurrAction) {

	mRunType = Run_None;

	if (!handleRequests(body, time)) {

		return false;
	}
	
	validateRunState(body);

	mIsActive = true;
	return true;
}

void Footballer_BodyAction_Running::stop(Body& body, const Time& time) {

	mRunType = Run_None;
	mIsActive = false;
}

void Footballer_BodyAction_Running::cancelSwitchAction() {

	mRequestAction.destroy();
}

bool Footballer_BodyAction_Running::switchActionRunning(Body& body, const Time& time) {

	assrt(body.getAction() == this);

	return true;
}

bool Footballer_BodyAction_Running::switchActionIdle(Body& body, const float& time) {

	//assrt(body.getAction() == this);

	bool set = true;

	if (set) {

		mRequestAction = &body.getActionIdle();
	}

	return set;
}

bool Footballer_BodyAction_Running::switchActionTackling(Body& body, const float& time) {

	//assrt(body.getAction() == this);

	bool set = true;

	if (set) {

		mRequestAction = &body.getActionTackling();
	}

	return set;
}

bool Footballer_BodyAction_Running::switchActionJumping(Body& body, const float& time) {

	//assrt(body.getAction() == this);

	bool set = true;

	if (set) {

		mRequestAction = &body.getActionJumping();
	}

	return set;
}

void Footballer_BodyAction_Running::cancelSetBodyFacing() {

	mRequestBodyFacing.isValid = false;
}

bool Footballer_BodyAction_Running::setBodyFacing(Body& body, const Time& time, const Vector3& val) {

	bool set = true;

	if (set) {

		mHasRequests = true;

		mRequestBodyFacing.value = val;
		mRequestBodyFacing.onSetValue();
	}

	return set;
}

void Footballer_BodyAction_Running::cancelSetRunDir() {

	mRequestRunDir.isValid = false;
}

bool Footballer_BodyAction_Running::setRunDir(Body& body, const Time& time, const Vector3& val) {

	bool set = true;

	if (set) {

		mHasRequests = true;

		mRequestRunDir.value = val;
		mRequestRunDir.onSetValue();
	}

	return set;
}

void Footballer_BodyAction_Running::cancelSetRunSpeed() {

	mRequestRunSpeedType.isValid = false;
}

bool Footballer_BodyAction_Running::setRunSpeed(Body& body, const Time& time, const RunSpeedType& val) {

	bool set = true;

	if (set) {

		mHasRequests = true;

		mRequestRunSpeedType.value = val;
		mRequestRunSpeedType.onSetValue();
	}

	return set;
}

float Footballer_BodyAction_Running::getCurrSpeed(Body& body) {

	validateRunState(body);

	return getSpeedValue(getRunType(), getRunSpeedType());
}

void Footballer_BodyAction_Running::updateRunState(Body& body) {

	const float tolerance = sinf(degToRad(1.0f));

	RunType newState;

	float dot = body.getCurrFacing().dot(mRunDir);

	if (fabs(1.0f - dot) <= tolerance) {

		newState = Run_Normal;

	} else {

		Vector3 discreeteDir;

		int step = discretizeDirection(body.getCurrFacing(), body.mNodeMatch->getCoordAxis(Scene_Up), mRunDir, 4, discreeteDir);

		switch(step) {

			case 0: newState = Run_StrafeFwd; break;
			case 1: newState = Run_StrafeRight; break;
			case 2: newState = Run_StrafeBack; break;
			case 3: newState = Run_StrafeLeft; break;
			default: assrt(false); newState = Run_StrafeFwd; break;
		}
	}

	if (newState != mRunType) {

		mRunType = newState;

		AnimationIndex::Type animIndex;

		switch(mRunType) {

			case Run_Normal: animIndex = body.getAnimIndex(Anim_Running); break;
			case Run_StrafeFwd: animIndex = body.getAnimIndex(Anim_StrafingFwd); break;
			case Run_StrafeLeft: animIndex = body.getAnimIndex(Anim_StrafingLeft); break;
			case Run_StrafeRight: animIndex = body.getAnimIndex(Anim_StrafingRight); break;
			case Run_StrafeBack: animIndex = body.getAnimIndex(Anim_StrafingBack); break;
			default: assrt(false); break;
		}

		SkeletonInstance& skelInst = body.getSkelInst();
		
		if (animIndex != skelInst.getAnimation()) {

			skelInst.setAnimation(animIndex, speedToAnimSpeed(body, getSpeedValue(mRunType, mRunSpeedType)));
			skelInst.setLoop(true);
		}
	}
}

void Footballer_BodyAction_Running::onBodyCollision(Body& body, CollRecordVol& record, void* pContext) {

	Time& time = dref((Time*) pContext);
	SoftPtr<Footballer> footballer = Footballer::footballerFromObject(record.getOwner());

	if (footballer->getActionType() == Action_Tackling) {

		mRequestAction = &body.getActionFallingAndSetupFromTackle(time, &body);
	}
}

void Footballer_BodyAction_Running::handleBodyCommEvts(Body& body, const Time& time) {

	for (BodyCommEvents::Index i = 0; i < body.getBodyCommEventCount(); ++i) {

		BodyCommEvent& evt = body.getBodyCommEvent(i);

		if (evt.type == BodyComm_Tackled) {

			mRequestAction = &body.getActionFallingAndSetupFromTackle(time, evt.source);
		}
	}
}

bool Footballer_BodyAction_Running::handleSwitches(Body& body, const Time& time) {

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

bool Footballer_BodyAction_Running::handleRequests(Body& body, const Time& time) {

	if (!handleSwitches(body, time))
		return false;

	if (mHasRequests) {

		mHasRequests = false;

		bool needUpdateRunState = false;
		bool needUpdateAnimSpeed = false;

		if (mRequestBodyFacing.isValid) {

			body.orientDirection(mRequestBodyFacing.value);
			mRequestBodyFacing.onConsumedValue();

			needUpdateRunState = true;
		}

		if (mRequestRunDir.isValid) {

			mRunDir = mRequestRunDir.value;
			mRequestRunDir.onConsumedValue();

			needUpdateRunState = true;
		}

		if (mRequestRunSpeedType.isValid) {

			mRunSpeedType = mRequestRunSpeedType.value;
			mRequestRunSpeedType.onConsumedValue();

			needUpdateAnimSpeed = true;
		}

		if (needUpdateRunState) {

			needUpdateAnimSpeed = true;

			updateRunState(body);
		}

		if (needUpdateAnimSpeed) {

			SkeletonInstance& skelInst = body.getSkelInst();
			skelInst.setSpeedFactor(speedToAnimSpeed(body, getSpeedValue(mRunType, mRunSpeedType)));
		}
	}

	return true;
}

void Footballer_BodyAction_Running::frameMoveTick(Body& body, const Time& time, const TickCount& tickIndex, const float& tickLength) {

	handleBodyCommEvts(body, time);

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
		mRunDir.mul(tickLength * getSpeedValue(mRunType, mRunSpeedType), posDiff);
		body.mTransformLocal.movePosition(posDiff);
	}

	bool wasBlocked = false;
	{
		if (body.nodeIsDirty()) {

			body.updateCollisions(this, (void*) &time);
			body.resolveCollisions(body.getDefaultCollResolveMode(), true, wasBlocked, posDiff);
		}
	}

	if (!handleSwitches(body, time)) {

		return;
	}
}

} }

#endif