#ifdef FOOTBALLER_BODY3

#ifndef _WOFMatchFootballer_BodyAction_Jumping_h
#define _WOFMatchFootballer_BodyAction_Jumping_h

#include "WOFMatchFootballer_BodyDefs.h"

namespace WOF { namespace match {

	class Footballer_BodyAction_Jumping : public FootballerBodyDefs, public FootballerBodyDefs::ActionImpl {
	public:
	
		virtual ActionType getType() { return Action_Jumping; }
		
		virtual bool start(Body& body, const Time& time, ActionImpl* pCurrAction);
		virtual void stop(Body& body, const Time& time);

		virtual void frameMoveTick(Body& footballer, const Time& time, const TickCount& tickIndex, const float& tickLength);

		virtual bool switchActionIdle(Body& body, const Time& time);
		virtual bool switchActionRunning(Body& body, const Time& time);
		virtual bool switchActionTackling(Body& body, const Time& time);
		virtual bool switchActionJumping(Body& body, const Time& time);
		virtual void cancelSwitchAction();

	public:

		
	public:

		Time estimateRemainingTime(Body& body);

		void cancelSetJumpType();
		bool setJumpType(Body& body, const Time& time, const JumpType& val);
		const JumpType* getSetJumpType();
		bool setJumpTypeFromRelTargetPos(Body& body, const Time& time, const Vector3* pPos);
		bool setJumpTypeFromTargetBodyDir(Body& body, const Time& time, const Vector3* pDir);

		inline const JumpType& getJumpType() { return mJumpType; }

	protected:

		AnimEnum jumpTypeToAnimEnum(const JumpType& type);

		bool switchOnEnded(Body& body, const Time& time);
		bool handleRequests(Body& body, const Time& time);

	protected:

		SoftPtr<ActionImpl> mQueuedAction;

		Request<JumpType> mRequestJumpType;

		JumpType mJumpType;
	};
	
} }

#endif

#endif