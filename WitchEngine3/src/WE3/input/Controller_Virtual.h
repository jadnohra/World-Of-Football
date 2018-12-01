#ifndef _Controller_Virtual_h_WE_Input
#define _Controller_Virtual_h_WE_Input

#include "../string/WEString.h"
#include "../WETL/WETLArray.h"
#include "../WETL/WETLHashMap.h"
using namespace WE;

#include "Controller.h"
#include "Source.h"
#include "SourceHolder.h"
#include "Tracker.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class Controller_Virtual : public Controller {
	public:

		Controller_Virtual(const TCHAR* name = NULL, const TCHAR* selectName = NULL, float deviceDeadZone = 0.0f);
		virtual ~Controller_Virtual();

	public:

		virtual const TCHAR* getType() { return L"Virtual"; }
		virtual const TCHAR* getName();
		virtual const TCHAR* getSelectName() { return mSelectName.c_tstr(); }

		virtual bool isDevice() { return false; }

		virtual Index getSourceCount(); 

		virtual bool getSource(const TCHAR* name, SourceHolder& source, const Time& t); 
		virtual bool getSourceAt(const Index& index, SourceHolder& source, const Time& t); 

		virtual bool setName(const TCHAR* name);
		virtual bool addSource(SourceHolder& source, const TCHAR* name);

		virtual float getDeadZone();

	protected:

		virtual void updateImpl(const Time& t) {}

	protected:

		typedef StringHash Key;

		class SrcImpl : public Source, public Tracker {
		public:

			SrcImpl(Source& var, const TCHAR* name) : mRef(&var), mName(name) {}

			inline Key getKey() { return mName.hash(); }

		public:

			virtual void* getComponent(int compID) { return mRef->getComponent(compID); }
			virtual Time getTime() { return mRef->getTime(); }

			virtual const TCHAR* getName() { return mName.c_tstr(); }
			virtual const TCHAR* getType() { return SourceType::Virtual; }

			virtual bool addTracker(InputManager& manager, Tracker* pTracker);
			virtual bool removeTracker(InputManager& manager, Tracker* pTracker);

			virtual void mark(Controller* pOwner) { mRef->mark(pOwner); }
			virtual Controller* getMark() { return mRef->getMark(); }

		public:

			virtual void onSourceChanged(Source* pVarImpl);

		protected:

			String mName;
			SourceHolder mRef;
		};

	protected:

		bool getSource(const Key& key, SourceHolder& source, const Time& t); 

	protected:

		typedef WETL::RefHashMap<Key, SrcImpl*, Index> VarMap;

		String mName;
		String mSelectName;
		VarMap mVarMap;

		float mDeviceDeadZone;
	};

} }

#endif