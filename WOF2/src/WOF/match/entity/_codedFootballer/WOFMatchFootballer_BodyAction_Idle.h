#ifdef FOOTBALLER_BODY3

#ifndef _WOFMatchFootballer_BodyAction_Idle_h
#define _WOFMatchFootballer_BodyAction_Idle_h

#include "WOFMatchFootballer_BodyDefs.h"

namespace WOF { namespace match {

	class Footballer_BodyAction_Idle : public FootballerBodyDefs, public FootballerBodyDefs::ActionImpl {
	public:
	
		virtual ActionType getType() { return Action_Idle; }
		
		virtual bool start(Body& body, const Time& time, ActionImpl* pCurrAction);
		virtual void stop(Body& body, const Time& time);

		virtual void frameMoveTick(Body& footballer, const Time& time, const TickCount& tickIndex, const float& tickLength);

		virtual bool switchActionIdle(Body& body, const Time& time);
		virtual bool switchActionRunning(Body& body, const Time& time);
		virtual bool switchActionTackling(Body& body, const Time& time);
		virtual bool switchActionJumping(Body& body, const Time& time);
		virtual void cancelSwitchAction();

	public:

		typedef FootballerBodyCaps::DribbleCap DribbleCap;
		typedef FootballerBodyCaps::RunCap RunCap;
		
	public:

		inline const DribbleCap& getDribbleCap() { return mDribbleCap; }
		inline const RunCap& getRunCap() { return mRunCap; }

	protected:

		bool handleRequests(Body& body, const Time& time);
		void handleBodyCommEvts(Body& body, const Time& time);

		inline float staminaToAnimSpeed(const float& stamina) { return 1.0f; }

	protected:

		DribbleCap mDribbleCap;
		RunCap mRunCap;

		SoftPtr<ActionImpl> mRequestAction;
	};
	
} }

#endif

#endif