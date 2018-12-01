#include "Controller_DirectInput8Poll.h"

#include "InputManager.h"
#include "SourceHolder.h"

#ifdef WIN32
#include "../binding/win/WEWinVirtualKeyCodeMap.h"
#endif

namespace WE { namespace Input {

const int Controller_DirectInput8Poll::AxisRadius = 1000;

struct DetectDevicesContext {

	DetectDevicesContext(IDirectInput8* pDI, unsigned int index) 
		: deviceIndex(index), index(0), di(pDI), found(false) {
	}

	unsigned int index;
	unsigned int deviceIndex;

	bool found;

	SoftPtr<IDirectInput8> di;
	SoftRef<IDirectInputDevice8> device;
};

BOOL CALLBACK Callback_detectDevices(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) {

	DetectDevicesContext& context = dref((DetectDevicesContext*) pvRef);

	if (context.deviceIndex == context.index) {

		context.found = true;
		context.di->CreateDevice(lpddi->guidInstance, &(context.device.ptrRef()), NULL);

		return DIENUM_STOP;
	}

	++context.index;

	return DIENUM_CONTINUE;
}

IDirectInputDevice8* Controller_DirectInput8Poll::detectDevice(const NativeAppInfo& appInfo, unsigned int deviceIndex, bool& hasMoreDevices, DWORD deviceType) {

	SoftRef<IDirectInput8> di;

	if (FAILED( DirectInput8Create(appInfo.mAppInstance, DIRECTINPUT_VERSION, 
					IID_IDirectInput8, (void**) &(di.ptrRef()), NULL))) {

		hasMoreDevices = false;
		return NULL;
	}

	DetectDevicesContext context(di, deviceIndex);

	di->EnumDevices(deviceType, Callback_detectDevices, &context, DIEDFL_ATTACHEDONLY);

	hasMoreDevices = context.found;

	if (context.device.isValid()) 
		context.device->AddRef();

	return context.device;
}



BOOL Controller_DirectInput8Poll::Callback_countObjects(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef) {

	Key& count = dref((Key*) pvRef);

	bool approvedObject = true;

	if (approvedObject) {

		++count;
	}

	return DIENUM_CONTINUE;
}

BOOL Controller_DirectInput8Poll::Callback_addObjects(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef) {

	SoftPtr<Controller_DirectInput8Poll> inst = (Controller_DirectInput8Poll*) pvRef;

	bool approvedObject = true;

	DWORD type = DIDFT_GETTYPE(lpddoi->dwType);

	if(type == DIDFT_AXIS || type == DIDFT_ABSAXIS) {

        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = lpddoi->dwType; // Specify the enumerated axis
		diprg.lMin              = -AxisRadius; 
        diprg.lMax              = AxisRadius; 
    
		if (FAILED(inst->mDevice->SetProperty(DIPROP_RANGE, &diprg.diph ))) {
			
			approvedObject = false;
		} 
    }

	if (approvedObject) {

		inst->addSource(lpddoi);
	}

	return DIENUM_CONTINUE;
}

Controller_DirectInput8Poll::Controller_DirectInput8Poll() {
}

Controller_DirectInput8Poll::~Controller_DirectInput8Poll() {

	for (Vars::Index i = 0; i < mVars.count; ++i) {

		mVars[i]->setParent(NULL);
	}
}

bool Controller_DirectInput8Poll::init(const NativeAppInfo& appInfo, IDirectInputDevice8* pDevice, const Time& t, DeviceInfo* pDeviceInfo) {

	mDeviceInfo.destroy();

	if (mDevice.isValid() || (pDevice == NULL)) {

		assrt(false);
		return false;
	}

	mDevice = pDevice;

	DIDEVICEINSTANCE devInfo;
	devInfo.dwSize = sizeof(DIDEVICEINSTANCE);

	if (mDevice->GetDeviceInfo(&devInfo) == DI_OK) {

		mDeviceName.assign(devInfo.tszInstanceName);
		mDeviceProductName.assign(devInfo.tszProductName);
	}

	switch (GET_DIDEVICE_TYPE(devInfo.dwDevType)) {

		case DI8DEVTYPE_GAMEPAD : {

			mType.assign(L"Joystick");

		} break;

		case DI8DEVTYPE_JOYSTICK : {

			mType.assign(L"Joystick");

		} break;

		case DI8DEVTYPE_KEYBOARD: {

			mType.assign(L"Keyboard");

		} break;

		case DI8DEVTYPE_MOUSE: {

			mType.assign(L"Mouse");

		} break;

		default: {

			mType.assign(L"Device");

		} break;
	}

	Key objectCount = 0;
	mDevice->EnumObjects(Callback_countObjects, &objectCount, DIDFT_AXIS | DIDFT_ABSAXIS | DIDFT_BUTTON);

	mVars.reserve(objectCount);
	mVarFormats.reserve(objectCount);
	mVarMap.reserve(objectCount);

	mDevice->EnumObjects(Callback_addObjects, this, DIDFT_AXIS | DIDFT_ABSAXIS | DIDFT_BUTTON);

	bool success = true;
	
	success = success && !FAILED(mDevice->SetCooperativeLevel(appInfo.mWnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE));
	success = success && setDataFormat();

	if (!success) {

		return false;
	}

	if (pDeviceInfo) {

		WE_MMemNew(mDeviceInfo.ptrRef(), DeviceInfo(pDeviceInfo));
	}

	mDevice->Acquire();

	update(t);

	return true;
}

bool Controller_DirectInput8Poll::setDataFormat() {

	DIDATAFORMAT format;

	format.dwSize = sizeof(DIDATAFORMAT);
	format.dwObjSize = sizeof(DIOBJECTDATAFORMAT);
	format.dwFlags = DIDF_ABSAXIS;
	format.dwDataSize = getVarDataSizeBytes();
	format.dwNumObjs = mVars.count;
	format.rgodf = mVarFormats.el;

	HRESULT hr = mDevice->SetDataFormat(&format);

	if (hr == DIERR_INVALIDPARAM
		|| hr == DIERR_NOTINITIALIZED) {

		assrt(false);
	}

	return hr == DI_OK;
}

DeviceInfo* Controller_DirectInput8Poll::getDeviceInfo() {

	return mDeviceInfo;
}

float Controller_DirectInput8Poll::getDeadZone() {

	return mDeviceInfo.isValid() ? mDeviceInfo->deadZone : -1.0f;
}

void Controller_DirectInput8Poll::updateImpl(const Time& t) {

	HRESULT hr;
	bool updated = false;

	hr = mDevice->Poll();

	if (FAILED(hr)) {

		hr = mDevice->Acquire();

		switch (hr) {
			case DIERR_INPUTLOST: {

				if (setDataFormat()) {

					hr = mDevice->Acquire();

				} else {

					hr = !DI_OK;
				}
				
			} break;
			default:
				break;
		}

		updated = (hr == DI_OK);

	} else {

		updated = true;
	}

	if (updated) {

		if (FAILED(mDevice->GetDeviceState(mVarDataArray.size * sizeof(DWORD), mVarDataArray.el))) {

			updated = false;
			assrt(false);
		}
	} 


	if (updated) {

		DWORD* pVal = mVarDataArray.el;

		for (Vars::Index i = 0; i < mVars.count; ++i) {

			mVars[i]->update(t, pVal);
			++pVal;
		}
	}
}

bool Controller_DirectInput8Poll::getKey(const TCHAR* name, Key& key) {

	key = String::hash(name);
	SoftPtr<SrcImpl> var;

	return (mVarMap.find(key, var.ptrRef()));
}

const TCHAR* Controller_DirectInput8Poll::getKey(const Index& index, Key& key) {

	if (index >= mVars.count)
		return NULL;

	SoftPtr<SrcImpl> var;

	VarMap::Iterator i = mVarMap.iterator();
	Index idx = 0;

	while (mVarMap.next(i, var.ptrRef())) {

		if (idx == index) {

			key = var->getKey();
			return var->getName();
		}

		++idx;
		var.destroy();
	}

	return NULL;
}

Controller::Index Controller_DirectInput8Poll::getSourceCount() {

	return mVars.count;
}

Controller_DirectInput8Poll::SrcImpl* Controller_DirectInput8Poll::addSource(LPCDIDEVICEOBJECTINSTANCE pObj) {

	SoftPtr<SrcImpl> ret;

	switch (DIDFT_GETTYPE(pObj->dwType)) {

		case DIDFT_ABSAXIS:
		case DIDFT_AXIS: {

			WE_MMemNew(ret.ptrRef(), VarImplAxis(*this, pObj));

		} break;

		case DIDFT_BUTTON:
		case DIDFT_PSHBUTTON:
		case DIDFT_TGLBUTTON: {

			WE_MMemNew(ret.ptrRef(), VarImplButton(*this, pObj));

		} break;
	}

	if (ret.isValid()) {

		Key key = ret->getKey();

		mVars.addOne(ret);

		if (!mVarMap.insert(key, ret)) {

			//strange!!! should never happen
			assrt(false);
		}
	}

	return ret;
}

void Controller_DirectInput8Poll::onAddRef(SrcImpl* pVar) {

	if (pVar->getRefCount() == 2) {

		addActiveCount();
	}
}

void Controller_DirectInput8Poll::onRelease(SrcImpl* pVar) {

	if (pVar->getRefCount() == 1) {
		
		removeActiveCount();

		if (pVar->hasTrackers()) {

			//forgot to remove tracker
			assrt(false);
		}
	} 
}

const TCHAR* Controller_DirectInput8Poll::getType() {

	return mType.c_tstr();
}

bool Controller_DirectInput8Poll::getSourceAt(const Index& index, SourceHolder& source, const Time& t) {

	Key key;

	const TCHAR* keyName;

	if ((keyName = getKey(index, key)) == NULL)
		return false;

	return getSource(key, keyName, source, t);
}

bool Controller_DirectInput8Poll::getSource(const TCHAR* name, SourceHolder& source, const Time& t) {

	Key key;

	if (!getKey(name, key))
		return false;

	return getSource(key, name, source, t);
}

bool Controller_DirectInput8Poll::getSource(const Key& key, const TCHAR* name, SourceHolder& source, const Time& t) {

	SoftPtr<SrcImpl> var;
	
	if (mVarMap.find(key, var.ptrRef())) {

		source.set(var);

	} else {

		source.destroy();
	}

	return source.isValid();
}

Controller_DirectInput8Poll::SrcImpl::SrcImpl(Controller_DirectInput8Poll& parent, LPCDIDEVICEOBJECTINSTANCE pObj) 
	: mParent(&parent), mValue(-2.0f) {

	mName.assign(pObj->tszName);

	DIOBJECTDATAFORMAT& format = mParent->addVarFormat();

	format.pguid = NULL;

	if (pObj->guidType == GUID_XAxis) 
		format.pguid = &GUID_XAxis;
	else if (pObj->guidType == GUID_YAxis) 
		format.pguid = &GUID_YAxis;
	else if (pObj->guidType == GUID_ZAxis)
		format.pguid = &GUID_ZAxis;
	else if (pObj->guidType == GUID_RxAxis)
		format.pguid = &GUID_RxAxis;
	else if (pObj->guidType == GUID_RyAxis)
		format.pguid = &GUID_RyAxis;
	else if (pObj->guidType == GUID_RzAxis)
		format.pguid = &GUID_RzAxis;
	else if (pObj->guidType == GUID_Slider)
		format.pguid = &GUID_Slider;
	else if (pObj->guidType == GUID_Button)
		format.pguid = &GUID_Button;
	else if (pObj->guidType == GUID_Key)
		format.pguid = &GUID_Key;
	else if (pObj->guidType == GUID_POV)
		format.pguid = &GUID_POV;
	else if (pObj->guidType == GUID_Unknown)
		format.pguid = &GUID_Unknown;
	
	format.dwOfs = mParent->addVarData();
	format.dwType = pObj->dwType;
	format.dwFlags = pObj->dwFlags;
}

void Controller_DirectInput8Poll::VarImplButton::update(const Time& t, void* pVal) {

	BYTE& val = *((BYTE*) pVal);

	float newValue = (val == 0 ? 0.0f : 1.0f);

	if (newValue != mValue) {

		mValue = newValue;
		mTime = t;

		notifyTrackers();
	}
}

Controller_DirectInput8Poll::VarImplAxis::
	VarImplAxis(Controller_DirectInput8Poll& parent, LPCDIDEVICEOBJECTINSTANCE pObj)
	: SrcImpl(parent, pObj) {


	if (pObj->guidType == GUID_XAxis) 
		mType = SourceType::XAxis;
	else if (pObj->guidType == GUID_YAxis) 
		mType = SourceType::YAxis;
	else if (pObj->guidType == GUID_ZAxis)
		mType = SourceType::ZAxis;
	else if (pObj->guidType == GUID_RxAxis)
		mType = SourceType::XRotAxis;
	else if (pObj->guidType == GUID_RyAxis)
		mType = SourceType::YRotAxis;
	else if (pObj->guidType == GUID_RzAxis)
		mType = SourceType::ZRotAxis;
	else if (pObj->guidType == GUID_Slider)
		mType = SourceType::Slider;
	else if (pObj->guidType == GUID_Button)
		mType = SourceType::Button;
	else if (pObj->guidType == GUID_Key)
		mType = SourceType::Button;
	else if (pObj->guidType == GUID_POV)
		mType = SourceType::POV0;
	else if (pObj->guidType == GUID_Unknown)
		mType = SourceType::Unknown;

}

void Controller_DirectInput8Poll::VarImplAxis::update(const Time& t, void* pVal) {

	LONG& val = *((LONG*) pVal);

	float newValue = ((float) val) / AxisRadius;

	if (newValue != mValue) {

		mValue = newValue;
		mTime = t;

		notifyTrackers();
	}
}




} }