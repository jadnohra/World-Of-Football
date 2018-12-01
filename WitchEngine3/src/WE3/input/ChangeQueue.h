#ifndef _ChangeQueue_h_WE_Input
#define _ChangeQueue_h_WE_Input

#include "../WETL/WETLArray.h"
#include "Tracker.h"
#include "Source.h"


namespace WE { namespace Input {

	class ChangeQueue : public Tracker {
	public:

		typedef unsigned int Index;

	public:

		bool startTracking(InputManager& manager, Source* pVar) { return pVar->addTracker(manager, this); }
		void stopTracking(InputManager& manager, Source* pVar) { pVar->removeTracker(manager, this); }

		Index getCount() { return mChanges.count; }
		Source* get(const Index& index) { return mChanges[index]; } //no release needed

		void clear() { mChanges.count = 0; }

	public:

		virtual void onSourceChanged(Source* pVar) { 
		
			mChanges.addOne(pVar); 
		}

	protected:

		//it would be safer (and slower) to use Hard Refs...
		typedef WETL::CountedArray<Source*, false, Index> Changes;

		Changes mChanges;
	};
} }

#endif