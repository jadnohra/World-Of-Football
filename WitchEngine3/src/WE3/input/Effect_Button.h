#ifndef _Effect_Button_h_WE_Input
#define _Effect_Button_h_WE_Input

#include "../string/WEString.h"

#include "Source.h"
#include "Tracker.h"
#include "Updatable.h"
#include "Component_Button.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class SourceHolder;
	class InputManager;

	class Effect_Button : public Source, public Component_Button, public Tracker, public Updatable {
	public:

		static bool create(InputManager& manager, Source& ref, const TCHAR* name, SourceHolder& source);

	public:		

		virtual Time getTime() { return mRef->getTime(); }

		virtual const TCHAR* getName() { return mName.isEmpty() ? mRef->getName() : mName.c_tstr(); }
		virtual const TCHAR* getType() { return SourceType::Virtual; }

		virtual bool addTracker(InputManager& manager, Tracker* pTracker);
		virtual bool removeTracker(InputManager& manager, Tracker* pTracker);
	
		virtual void* getComponent(int compID);
		virtual Source* getBaseSource() { if (mRef.isValid()) mRef->AddRef(); return mRef; } //release when done

		virtual void mark(Controller* pOwner) { mRef->mark(pOwner); }
		virtual Controller* getMark() { return mRef->getMark(); }

	public:

		virtual void setDetectOn(bool detect) { mDetectOn = detect; }
		virtual void setDetectHold(bool detect) { mDetectHold = detect; }
		virtual void setDetectTap(bool detect) { mDetectTap = detect; }

		virtual bool getDetectOn() { return mDetectOn; }
		virtual bool getDetectHold() { return mDetectHold; }
		virtual bool getDetectTap() { return mDetectTap; }

		virtual void setTapInterval(float tapInterval);
		virtual void setTapCountLimit(int limit) { mTapCountLimit = limit; } 

		virtual bool getIsTapping();
		virtual bool getIsHolding();
		virtual bool getIsOn();

		virtual int getTapCount(bool reset);
		virtual Time getHoldTime(Time time, bool reset);

	public:

		virtual void onSourceChanged(Source* pVarImpl);
		virtual void inputUpdate(const Time& t);

		virtual ~Effect_Button();

	protected:


		Effect_Button(InputManager& manager, Source& ref, const TCHAR* name); 
		bool init();

		void notifyTrackers();

	protected:

		typedef WETL::CountedArray<Tracker*, false, unsigned int, WETL::ResizeExact> Trackers;

		HardRef<Source> mRef;
		SoftPtr<InputManager> mManager;
		String mName;

		bool mDetectTap;
		bool mDetectHold;
		bool mDetectOn;

		Trackers mTrackers;

		float mTapInterval;
		int mTapCountLimit;

		Time mTapEventTime;
		Time mHoldEventTime;

		bool mIsTapping;
		bool mIsHolding;
		bool mIsOn;

		bool mTapLimitReached;

		int mTapCount;
		Time mHoldTime;		
	};

} }

#endif