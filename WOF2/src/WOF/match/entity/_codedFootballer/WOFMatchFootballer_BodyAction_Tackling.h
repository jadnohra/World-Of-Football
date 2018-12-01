#ifdef FOOTBALLER_BODY3

#ifndef _WOFMatchFootballer_BodyAction_Tackling_h
#define _WOFMatchFootballer_BodyAction_Tackling_h

#include "WOFMatchFootballer_BodyDefs.h"
//#include "bodyAction/WOFMatchFootballerBody_ConfigurableAction.h"

namespace WOF { namespace match {

	class Footballer_BodyAction_Tackling : 
		public FootballerBodyDefs, 
		public FootballerBodyDefs::ActionImpl/*,  
		public FootballerBody_ConfigurableAction::CommandProcessor*/ {
	public:
	
		virtual ActionType getType() { return Action_Tackling; }
	
		virtual bool load(Body& body, BufferString& tempStr, DataChunk* pActions, CoordSysConv* pConv);

		virtual bool start(Body& body, const Time& time, ActionImpl* pCurrAction);
		virtual void stop(Body& body, const Time& time);

		virtual void frameMoveTick(Body& footballer, const Time& time, const TickCount& tickIndex, const float& tickLength);

		virtual bool switchActionIdle(Body& body, const Time& time);
		virtual bool switchActionRunning(Body& body, const Time& time);
		virtual bool switchActionTackling(Body& body, const Time& time);
		virtual bool switchActionJumping(Body& body, const Time& time);
		virtual void cancelSwitchAction();

	public:

		Time estimateRemainingTime(Body& body);

		void cancelSetTackleVel();
		bool setTackleVel(Body& body, const Time& time, const Vector3& val);
		const Vector3* getSetTackleVel();

	public:

		//virtual FootballerBody_ConfigurableAction::Command* createCommand(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, const Time& totalTime);

	protected:

		bool switchOnEnded(Body& body, const Time& time);
		bool handleRequests(Body& body, const Time& time);

	protected:

		SoftPtr<ActionImpl> mQueuedAction;

		Vector3 mVelocity;
		float mAcceleration;

		Request<Vector3> mRequestTackleVel;
		//HardPtr<FootballerBody_ConfigurableAction> mDynamicTackle;
	};
	
} }

#endif

#endif