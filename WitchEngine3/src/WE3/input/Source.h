#ifndef _Source_h_WE_Input
#define _Source_h_WE_Input

#include "../WERef.h"
#include "../WEPtr.h"
#include "../WETime.h"
#include "../WETL/WETLArray.h"
using namespace WE;

#include "Controller.h"

namespace WE { namespace Input {

	class InputManager;
	class Tracker;
	class Controller;
	
	class Source : public VRef {
	public:

		Source() : mRefCount(1) {}

		virtual int Release() { --mRefCount; int ret = mRefCount; if (mRefCount == 0) delete this; return ret; }
		virtual void AddRef() { ++mRefCount; }

		virtual void* getComponent(int compID) { return NULL; }
		virtual Time getTime() = 0;

		virtual const TCHAR* getName() = 0;
		virtual const TCHAR* getType() = 0;

		virtual bool addTracker(InputManager& manager, Tracker* pTracker);
		virtual bool removeTracker(InputManager& manager, Tracker* pTracker);
		
		virtual Source* getBaseSource() { return NULL; } //release when done

		virtual void mark(Controller* pOwner) = 0; //no ref added to owner
		virtual Controller* getMark() = 0; //no relase needed
		bool isMarked();
		
		
	protected:

		void notifyTrackers();

	protected:

		typedef WETL::CountedArray<Tracker*, false, unsigned int, WETL::ResizeExact> Trackers;

		int mRefCount;
		
		Trackers mTrackers;
	};
	
} }

#endif