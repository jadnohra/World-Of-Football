#include "select.h"
#ifdef FOOTBALLER_BODY2

#ifndef _WOFMatchFootballer_BodyActions_h
#define _WOFMatchFootballer_BodyActions_h

#include "WE3/WEAssert.h"
using namespace WE;

#include "WOFMatchFootballer_BodyDefs.h"

#include "WOFMatchFootballer_BodyAction_Idle.h"
#include "WOFMatchFootballer_BodyAction_Running.h"
#include "WOFMatchFootballer_BodyAction_Tackling.h"

namespace WOF { namespace match {

	class Footballer_BodyActions : public FootballerBodyDefs {
	public:
	
		Footballer_BodyActions();

		bool init(Body& body, ActionImpl& startAction);

		void frameMoveTick(Body& body, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);
			
		float estimateActionRemainingTime();

	public:

		bool trySwitchTo(Body& body, const ActionType& action, bool allowRedundant, Event& outEvt);

		inline ActionImpl* getActive() { return mActive; }

		inline void setActive(Body& body, ActionImpl* pAction) { mActive = pAction; }
		inline Footballer_BodyAction_Idle* getActionIdle() { return &mIdle; }
		inline Footballer_BodyAction_Running* getActionRunning() { return &mRunning; }
		inline Footballer_BodyAction_Tackling* getActionTackling() { return &mTackling; }

		inline ActionImpl* getAction(const ActionType& type) { return mActionMap[type]; }
			
	protected:

		void handleRequests(Body& body);

	protected:

		SoftPtr<ActionImpl> mActive;

		Footballer_BodyAction_Idle mIdle;
		Footballer_BodyAction_Running mRunning;
		Footballer_BodyAction_Tackling mTackling;

		SoftPtr<ActionImpl> mActionMap[ActionTypeCount];
	};

} }

#endif

#endif