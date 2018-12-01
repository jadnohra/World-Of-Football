#ifndef _InputClient_h_WE_Input
#define _InputClient_h_WE_Input

#include "../WETL/WETLArray.h"
#include "../WETL/WETLHashMap.h"
#include "../WERef.h"
#include "../string/WEString.h"
#include "Tracker.h"
#include "Source.h"
#include "InputManager.h"

namespace WE { namespace Input {

	class InputClient : public Ref, public Tracker, public DestructorNotifiable {
	public:

		typedef unsigned int Index;
		typedef unsigned int IDType;

	public:

		void setName(const TCHAR* name) { mName.assign(name); }
		const TCHAR* getName() { return mName.c_tstr(); }

		bool addSource(Source* pSource, bool mapIDByVar = false, bool mapSourceByID = false, IDType ID = -1);
		bool removeSource(Source* pSource);

		bool hasSourceID(Source* pSource);
		bool getSourceID(Source* pSource, IDType& ID);
		
		//only works if mapSourceByID was set
		Source* getSourceByID(IDType ID); //no release needed

		Source* startIterateEvents();
		Source* nextEvent(); //no release needed
		void breakIterateEvents();

		void clearEvents();

		void destroy();

	public:

		virtual void onSourceChanged(Source* pSource);
		virtual void inputDestructNotify(void* pObject);
		
	public:

		InputClient(InputManager* pManager);
		virtual ~InputClient();

		void setManager(InputManager* pManager);

	protected:

		bool startTracking(InputManager& manager, Source* pSource);
		bool stopTracking(InputManager& manager, Source* pSource);

	protected:

		typedef WETL::CountedArray<Source*, false, Index> Events;
		typedef WETL::CountedRefArray<Source*, false, Index> Sources;
		typedef WETL::HashMap<Source*, IDType, Index> IDByVarMap;
		typedef WETL::HashMap<IDType, Source*, Index> VarByIDMap;

		SoftPtr<InputManager> mManager;
		String mName;

		Sources mSources;
		IDByVarMap mIDByVarMap;
		VarByIDMap mVarByIDMap;

		Events mEvents;
		Events mPendingEvents;

		bool mIsIteratingEvents;
		Index mIterateIndex;
	};

} }

#endif