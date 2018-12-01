#ifndef _Controller_DirectInput8Poll_h_WE_Input
#define _Controller_DirectInput8Poll_h_WE_Input

#include "../string/WEString.h"
#include "../WETL/WETLArray.h"
#include "../WETL/WETLHashMap.h"
#include "../binding/directx/9/di8/includeDI8.h"
#include "../native/WENativeAppInfo.h"
using namespace WE;

#include "Controller.h"
#include "Source.h"
#include "Component_Generic1D.h"
#include "ComponentIDs.h"
#include "DeviceInfo.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class Controller_DirectInput8Poll : public Controller {
	public:

		Controller_DirectInput8Poll();
		virtual ~Controller_DirectInput8Poll();

		//name might change after init
		bool init(const NativeAppInfo& appInfo, IDirectInputDevice8* pDevice, const Time& t, DeviceInfo* pDeviceInfo = NULL);
		
		static IDirectInputDevice8* detectDevice(const NativeAppInfo& appInfo, unsigned int deviceIndex, bool& hasMoreDevices, DWORD deviceType = DI8DEVCLASS_GAMECTRL); //release when done


	public:

		virtual const TCHAR* getType();
		virtual const TCHAR* getName() { return mDeviceName.c_tstr(); }
		virtual const TCHAR* getDeviceProducName() { return mDeviceProductName.c_tstr(); }
		virtual const TCHAR* getSelectName() { return getType(); }

		virtual bool isDevice() { return true; }

		virtual Index getSourceCount();

		virtual bool getSource(const TCHAR* name, SourceHolder& source, const Time& t);
		virtual bool getSourceAt(const Index& index, SourceHolder& source, const Time& t);

		virtual float getDeadZone();
		virtual DeviceInfo* getDeviceInfo();

	protected:

		virtual void updateImpl(const Time& t);


	protected:

		typedef StringHash Key;

		class SrcImpl : public Source {
		public:

			SrcImpl(Controller_DirectInput8Poll& parent, LPCDIDEVICEOBJECTINSTANCE pObj);

			inline void setParent(Controller_DirectInput8Poll* pParent) { mParent = pParent; }

			inline const int& getRefCount() { return mRefCount; }
			inline bool hasTrackers() { return mTrackers.count != 0; } 
			inline Key getKey() { return mName.hash(); } 

			inline Time& time() { return mTime; }
			inline float& value() { return mValue; }

			virtual void update(const Time& t, void* pVal) = 0;

		public:

			virtual void AddRef() { Source::AddRef(); if (mParent.isValid()) mParent->onAddRef(this);  }
			virtual int Release() { int ret = Source::Release(); if (ret) { if (mParent.isValid()) mParent->onRelease(this); } return ret; }

			virtual Time getTime() { return mTime; }

			virtual const TCHAR* getName() { return mName.c_tstr(); }

			virtual void mark(Controller* pOwner) { mMarkOwner = pOwner; }
			virtual Controller* getMark() { return mMarkOwner; }

		protected:

			String mName;
			
			SoftPtr<Controller_DirectInput8Poll> mParent;
			SoftPtr<Controller> mMarkOwner;

			Time mTime;
			float mValue;
		};

		class VarImplButton : public SrcImpl, public Component_Generic1D  {
		public:

			VarImplButton(Controller_DirectInput8Poll& parent, LPCDIDEVICEOBJECTINSTANCE pObj) :
			  SrcImpl(parent, pObj) {}

			virtual void* getComponent(int compID) { return compID == ComponentID_Generic1D ? (Component_Generic1D*) this : NULL; }
			virtual void update(const Time& t, void* pVal);

		public:

			virtual bool isDiscrete() { return true; }
			virtual int getStepCount() { return 2; }
			virtual float getMin() { return 0.0f; }
			virtual float getMax() { return 1.0f; }

			virtual float getValue() { return mValue; }

			virtual const TCHAR* getType() { return SourceType::Button; }
		};

		class VarImplAxis : public SrcImpl, public Component_Generic1D  {
		public:

			VarImplAxis(Controller_DirectInput8Poll& parent, LPCDIDEVICEOBJECTINSTANCE pObj);

			virtual void* getComponent(int compID) { return compID == ComponentID_Generic1D ? (Component_Generic1D*) this : NULL; }
			virtual void update(const Time& t, void* pVal);

			virtual const TCHAR* getType() { return mType; }

		public:

			virtual bool isDiscrete() { return false; }
			virtual float getMin() { return -1.0f; }
			virtual float getMax() { return 1.0f; }

			virtual float getValue() { return mValue; }

		protected:

			const TCHAR* mType;
		};

	protected:

		bool getKey(const TCHAR* name, Key& key);
		const TCHAR* getKey(const Index& index, Key& key);

		void onAddRef(SrcImpl* pVar);
		void onRelease(SrcImpl* pVar);

		bool setDataFormat();

		SrcImpl* addSource(LPCDIDEVICEOBJECTINSTANCE pObj);
		bool getSource(const Key& key, const TCHAR* name, SourceHolder& source, const Time& t);

		DIOBJECTDATAFORMAT& addVarFormat() { return mVarFormats.addOne(); }
		DWORD addVarData() { DWORD ret = getVarDataSizeBytes(); mVarDataArray.resize(mVarDataArray.size + 1); return ret; }
		DWORD getVarDataSizeBytes() { return  mVarDataArray.size * sizeof(DWORD); }	

	protected:

		typedef WETL::HashMap<Key, SrcImpl*, Index> VarMap;
		typedef WETL::CountedRefArray<SrcImpl*, false, Index, WETL::ResizeExact> Vars;
		typedef WETL::CountedArray<DIOBJECTDATAFORMAT, false, Index, WETL::ResizeExact> VarFormats;
		typedef WETL::SizeAllocT<DWORD, Index, true> VarDataArray;

		static const int AxisRadius;

		String mType;
		String mDeviceName;
		String mDeviceProductName;
		Vars mVars;
		VarMap mVarMap;
		VarFormats mVarFormats;
		VarDataArray mVarDataArray;

		HardRef<IDirectInputDevice8> mDevice;
		HardPtr<DeviceInfo> mDeviceInfo;

	public:

		static BOOL CALLBACK Callback_countObjects(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);
		static BOOL CALLBACK Callback_addObjects(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);
	};

} }

#endif