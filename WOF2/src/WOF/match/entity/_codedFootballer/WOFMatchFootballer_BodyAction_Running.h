#ifdef FOOTBALLER_BODY3

#ifndef _WOFMatchFootballer_BodyAction_Running_h
#define _WOFMatchFootballer_BodyAction_Running_h

#include "WE3/coordSys/WECoordSysConv.h"
#include "WOFMatchFootballer_BodyDefs.h"

namespace WOF { namespace match {

	
	class Footballer_BodyAction_Running : public FootballerBodyDefs, public FootballerBodyDefs::ActionImpl, public FootballerBodyDefs::BodyCollisionListener  {
	public:
	
		virtual ActionType getType() { return Action_Running; }
		
		//uses the current parameter settings
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

		Footballer_BodyAction_Running();

		bool init(Body& body, Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		inline const DribbleCap& getDribbleCap() { return mDribbleCap; }
		inline const RunCap& getRunCap() { return mRunCap; }

		void cancelSetBodyFacing();
		bool setBodyFacing(Body& body, const Time& time, const Vector3& val);

		void cancelSetRunDir();
		bool setRunDir(Body& body, const Time& time, const Vector3& val);
		
		void cancelSetRunSpeed();
		bool setRunSpeed(Body& body, const Time& time, const RunSpeedType& val);
		
		float getCurrSpeed(Body& body);
		inline float getSpeedValue(const RunType& runType, const RunSpeedType& speedType) { return mSpeedTable[runType][speedType]; }

		inline const RunSpeedType& getRunSpeedType() { return mRunSpeedType; } 
		inline const RunType& getRunType() { return mRunType; } 

		inline void validateRunState(Body& body) { if (mRunType == Run_None) updateRunState(body); }

	protected:
	
		virtual void onBodyCollision(Body& body, CollRecordVol& record, void* pContext = NULL);

	protected:

		bool setRunSpeed(Body& body, const Time& time, const float& val);

		bool handleSwitches(Body& body, const Time& time);
		bool handleRequests(Body& body, const Time& time);
		void handleBodyCommEvts(Body& body, const Time& time);

		float speedToAnimSpeed(Body& body, const float& speed);
		
		void updateRunState(Body& body);

	protected:

		bool mIsActive;
		
		DribbleCap mDribbleCap;
		RunCap mRunCap;

		float mSpeedTable[RunTypeCount][RunSpeedTypeCount];

		bool mHasRequests;

		SoftPtr<ActionImpl> mRequestAction;
		Request<Vector3> mRequestBodyFacing;
		Request<Vector3> mRequestRunDir;
		Request<RunSpeedType> mRequestRunSpeedType;

		Vector3 mRunDir;
		RunSpeedType mRunSpeedType;
		RunType mRunType;
	};
	
	
} }

#endif

#endif