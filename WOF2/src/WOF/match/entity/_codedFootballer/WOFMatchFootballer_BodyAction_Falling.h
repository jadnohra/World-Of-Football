#ifdef FOOTBALLER_BODY3

#ifndef _WOFMatchFootballer_BodyAction_Falling_h
#define _WOFMatchFootballer_BodyAction_Falling_h

#include "WOFMatchFootballer_BodyDefs.h"

namespace WOF { namespace match {

	class Footballer_BodyAction_Falling : public FootballerBodyDefs, public FootballerBodyDefs::ActionImpl {
	public:
	
		virtual ActionType getType() { return Action_Falling; }
		
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

		void cancelSetFallType();
		bool setFallType(Body& body, const Time& time, const FallType& val);
		bool setFallTypeFromTackle(Body& body, const Time& time, Body* pTackler);
		const FallType* getSetFallType();

		inline const FallType& getFallType() { return mFallType; }

	protected:

		AnimEnum fallTypeToAnimEnum(const FallType& type);

		bool switchOnEnded(Body& body, const Time& time);
		bool handleRequests(Body& body, const Time& time);

	protected:

		SoftPtr<ActionImpl> mQueuedAction;

		Request<FallType> mRequestFallType;

		FallType mFallType;
	};
	
} }

#endif

#endif