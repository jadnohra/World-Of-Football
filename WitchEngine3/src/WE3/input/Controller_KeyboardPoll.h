#ifndef _Controller_KeyboardPoll_h_WE_Input
#define _Controller_KeyboardPoll_h_WE_Input

#include "../string/WEString.h"
#include "../WETL/WETLArray.h"
#include "../WETL/WETLHashMap.h"
using namespace WE;

#include "Controller.h"
#include "Source.h"
#include "Component_Generic1D.h"
#include "ComponentIDs.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class Controller_KeyboardPoll : public Controller {
	public:

		Controller_KeyboardPoll(const TCHAR* name = L"Keyboard");
		virtual ~Controller_KeyboardPoll();

	public:

		virtual const TCHAR* getType() { return L"Keyboard"; }
		virtual const TCHAR* getName();
		virtual bool setName(const TCHAR* name);
		virtual const TCHAR* getSelectName() { return getType(); }

		virtual bool isDevice() { return true; }

		virtual Index getSourceCount();

		virtual bool getSource(const TCHAR* name, SourceHolder& source, const Time& t) ;
		virtual bool getSourceAt(const Index& index, SourceHolder& source, const Time& t);
	
		virtual float getDeadZone() { return 0.0f; }

	protected:

		virtual void updateImpl(const Time& t);


	protected:

		typedef StringHash Key;

		class SrcImpl : public Source, public Component_Generic1D {
		public:

			SrcImpl(Controller_KeyboardPoll& parent, const int& code, const TCHAR* name) 
				: mParent(&parent), mName(name), mCode(code), mValue(-2.0f) {
			}

			inline void setParent(Controller_KeyboardPoll* pParent) { mParent = pParent; }

			inline const int& getRefCount() { return mRefCount; }
			inline bool hasTrackers() { return mTrackers.count != 0; } 
			inline Key getKey() { return mName.hash(); } 
			inline const int& getCode() { return mCode; } 

			inline Time& time() { return mTime; }
			inline float& value() { return mValue; }

			void update(const Time& t);

		public:

			virtual void AddRef() { Source::AddRef(); if (mParent.isValid()) mParent->onAddRef(this);  }
			virtual int Release() { int ret = Source::Release(); if (ret) { if (mParent.isValid()) mParent->onRelease(this); } return ret; }

			virtual void* getComponent(int compID) { return compID == ComponentID_Generic1D ? (Component_Generic1D*) this : NULL; }
			virtual Time getTime() { return mTime; }

			virtual const TCHAR* getName() { return mName.c_tstr(); }
			virtual const TCHAR* getType() { return SourceType::Button; }

			virtual void mark(Controller* pOwner) { mMarkOwner = pOwner; }
			virtual Controller* getMark() { return mMarkOwner; }

		public:

			virtual bool isDiscrete() { return true; }
			virtual int getStepCount() { return 2; }
			virtual float getMin() { return 0.0f; }
			virtual float getMax() { return 1.0f; }

			virtual float getValue() { return mValue; }

		protected:

			String mName;
			int mCode;
			
			SoftPtr<Controller_KeyboardPoll> mParent;
			SoftPtr<Controller> mMarkOwner;

			Time mTime;
			float mValue;
		};

	protected:

		bool getKey(const TCHAR* name, Key& key, int& code);
		const TCHAR* getKey(const Index& index, Key& key, int& code);

		bool getSource(const Key& key, const int& code, const TCHAR* name, SourceHolder& source, const Time& t);
		SrcImpl* addSource(const TCHAR* name, const Time& t, const Key& key, const int& code);
		
		void onAddRef(SrcImpl* pVar);
		void onRelease(SrcImpl* pVar);

	protected:

		typedef WETL::HashMap<Key, SrcImpl*, Index> VarMap;
		typedef WETL::CountedRefArray<SrcImpl*, false, Index, WETL::ResizeExact> Vars;

		String mName;
		Vars mVars;
		VarMap mVarMap;
	};

} }

#endif