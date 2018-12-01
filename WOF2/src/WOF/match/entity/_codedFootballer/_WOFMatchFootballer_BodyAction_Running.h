#include "select.h"
#ifdef FOOTBALLER_BODY2

#ifndef _WOFMatchFootballer_BodyAction_Running_h
#define _WOFMatchFootballer_BodyAction_Running_h

#include "WOFMatchFootballer_BodyDefs.h"

namespace WOF { namespace match {

	class Footballer_BodyAction_Running : public FootballerBodyDefs, public FootballerBodyDefs::ActionImpl {
	public:

		virtual ActionType getType() { return Action_Running; }
	
		virtual bool switchTo(BodyActions& actions, Body& body, const ActionType& nextAction, ActionImpl& nextActionImpl, Event& outResult);
		virtual bool start(BodyActions& actions, Body& body, Event& outFailureResult);
		virtual void frameMoveTick(BodyActions& actions, Body& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);

	protected:

		void onRequestHeadFacing(Body& body, Facing& facing);
		void onRequestBodyFacing(Body& body, Facing& facing);
		void onRequestRunDir(Body& body);
		void onRequestRunSpeed(Body& body);
		
		bool handleRequests(BodyActions& actions, Body& body);

	protected:

		float speedToAnimSpeed(const float& speed);
	};
} }

#endif

#endif