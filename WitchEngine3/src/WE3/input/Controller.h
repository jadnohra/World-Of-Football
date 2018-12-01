#ifndef _Controller_h_WE_Input
#define _Controller_h_WE_Input

#include "../string/WEString.h"

#include "ClassDefs.h"

namespace WE { namespace Input {

	class Controller : public Ref {
	public:

		typedef unsigned int Index;

	public:

		Controller();
		virtual ~Controller();

		virtual void setManager(InputManager* pManager);
		virtual InputManager* getManager();

		virtual const TCHAR* getType() { return NULL; }
		virtual const TCHAR* getName() { return NULL; }
		virtual const TCHAR* getProductName() { return NULL; }
		virtual const TCHAR* getSelectName() { return NULL; }

		inline void setVisible(bool visible) { mIsVisible = visible; }
		inline bool isVisible() { return mIsVisible; }


		virtual bool isDevice() { return false; }
		
		virtual bool setName(const TCHAR* name) { return false; }
		virtual bool canAddSource() { return false; }
		virtual bool addSource(SourceHolder& source, const TCHAR* name) { return false; }

		void setInputClient(InputClient* pClient);
		InputClient* getInputClient(); //no release needed

		virtual Index getSourceCount() { return 0; }

		//it must be manually ensured that the controller is not destroyed
		//before the source, (e,g: use Controller->AddRef)
		virtual bool getSource(const TCHAR* name, SourceHolder& source, const Time& t) { return false; }
		virtual bool getSourceAt(const Index& index, SourceHolder& source, const Time& t) { return false; }

		virtual void update(const Time& t) {

			if (mLastUpdateTime < t) {

				updateImpl(t);

				mLastUpdateTime = t;
			}
		}

		virtual float getDeadZone() { return -1.0f; }
		virtual DeviceInfo* getDeviceInfo() { return NULL; }

	public:

		bool isActive() { return mActiveCount != 0; }
		
		inline void mark(Controller* pOwner) { mMarkOwner = pOwner; } //no ref added to owner
		inline bool isMarked() { return mMarkOwner.isValid(); }
		inline Controller* getMark() { return mMarkOwner; } //no relase needed

		bool createButton(const TCHAR* variableName, const TCHAR* buttonName, const Time& t, SourceHolder& button, bool addButton = false, const TCHAR* addButtonName = NULL);

	protected:

		unsigned int addActiveCount() { return ++mActiveCount; }
		unsigned int removeActiveCount() { return --mActiveCount; } 

	protected:

		virtual void updateImpl(const Time& t) {}

	protected:

		bool mIsVisible;
		SoftPtr<InputManager> mManager;
		HardRef<InputClient> mInputClient;
		SoftPtr<Controller> mMarkOwner;

		Time mLastUpdateTime;
		unsigned int mActiveCount;
	};

} }

#endif