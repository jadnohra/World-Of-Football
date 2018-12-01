#include "select.h"
#ifdef FOOTBALLER_BODY2

#ifndef _WOFMatchFootballer_BodyAction_Tackling_h
#define _WOFMatchFootballer_BodyAction_Tackling_h

#include "WOFMatchFootballer_BodyDefs.h"

namespace WOF { namespace match {

	class Footballer_BodyAction_Tackling : public FootballerBodyDefs, public FootballerBodyDefs::ActionImpl {
	public:

		virtual ActionType getType() { return Action_Tackling; }
	
		virtual bool switchTo(BodyActions& actions, Body& body, const ActionType& nextAction, ActionImpl& nextActionImpl, Event& outResult);
		virtual bool start(BodyActions& actions, Body& body, Event& outFailureResult);
		virtual void frameMoveTick(BodyActions& actions, Body& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);

	protected:

		bool initFromPastAction(BodyActions& actions, Body& body, Event& outFailureResult);

		bool switchOnEnded(BodyActions& actions, Body& body);

		void onRequestHeadFacing(Body& body, Facing& facing);
		void onRequestBodyFacing(Body& body, Facing& facing);
				
		bool handleRequests(BodyActions& actions, Body& body);

	protected:

		Vector3 mVelocity;
		float mAcceleration;
	};
} }

#endif

#endif