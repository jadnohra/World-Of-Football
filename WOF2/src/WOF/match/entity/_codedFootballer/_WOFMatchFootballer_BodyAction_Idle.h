#include "select.h"
#ifdef FOOTBALLER_BODY2

#ifndef _WOFMatchFootballer_BodyAction_Idle_h
#define _WOFMatchFootballer_BodyAction_Idle_h

#include "WOFMatchFootballer_BodyDefs.h"

namespace WOF { namespace match {

	class Footballer_BodyAction_Idle : public FootballerBodyDefs, public FootballerBodyDefs::ActionImpl {
	public:
	
		virtual ActionType getType() { return Action_Idle; }

		virtual bool switchTo(BodyActions& actions, Body& body, const ActionType& nextAction, ActionImpl& nextActionImpl, Event& outResult);
		virtual bool start(BodyActions& actions, Body& body, Event& outFailureResult);
		virtual void frameMoveTick(BodyActions& actions, Body& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);

	protected:
	
		void onRequestHeadFacing(Body& body, Facing& facing);
		void onRequestBodyFacing(Body& body, Facing& facing);
	
		bool handleRequests(BodyActions& actions, Body& body);

	protected:

		inline float staminaToAnimSpeed(const float& stamina) { return 1.0f; }
	};
	
} }

#endif

#endif