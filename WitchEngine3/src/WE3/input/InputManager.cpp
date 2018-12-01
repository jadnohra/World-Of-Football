#include "InputManager.h"

#include "Controller.h"
#include "Controller_KeyboardPoll.h"
#include "Controller_DirectInput8Poll.h"
#include "Controller_Virtual.h"

#include "../string/WEBufferString.h"

namespace WE { namespace Input {

InputManager::~InputManager() {

	for (Index i = 0; i < mDestructorNotifiables.count; ++i) {

		mDestructorNotifiables[i]->inputDestructNotify(this);
	}

	for (Index i = 0; i < mControllers.count; ++i) {

		mControllers[i]->setManager(NULL);
	}
}

void InputManager::dumpLogController(Controller& controller, const Time& t, unsigned int instanceIndex, bool isEnabled, Log& log) {

	log.log(L"Dump Log for Controller '%s' (%u), %s", controller.getName(), instanceIndex, isEnabled ? L"enabled" : L"disabled");

	SourceHolder source;
	Controller::Index sourceCount = controller.getSourceCount();

	for (Controller::Index i = 0; i < sourceCount; ++i) {

		if (controller.getSourceAt(i, source, t)) {

			SoftPtr<Component_Generic1D> comp = (Component_Generic1D*) source->getComponent(ComponentID_Generic1D);

			if (comp.isValid()) {

				log.log(L"@%u '%s' type:%s [%g - %g]", i, source->getName(), source->getType(), comp->getMin(), comp->getMax());

			} else {

				log.log(L"@%u '%s' type:%s", i, source->getName(), source->getType());
			}
		}
	}

	log.log(L"-----------");

}

void InputManager::detectControllers(const NativeAppInfo& appInfo, const Time& t, Log* controllerDumpLog) {

	{
		SoftRef<Controller_KeyboardPoll> controller;

		WE_MMemNew(controller.ptrRef(), Controller_KeyboardPoll());

		addController(controller);
	}

	{
		typedef WETL::CountedRefArray<IDirectInputDevice8*, true, unsigned int> Devices;
		Devices devices;

		typedef WETL::CountedArray<unsigned int, true, unsigned int> CloneIndices;
		CloneIndices cloneIndices;

		typedef WETL::CountedArray<DeviceInfo*, true, unsigned int> DeviceInfos;
		DeviceInfos deviceInfos;

		bool hasMoreDevices = true;
		unsigned int deviceIndex = 0;

		while (hasMoreDevices) {

			SoftRef<IDirectInputDevice8> device;
			
			device = Controller_DirectInput8Poll::detectDevice(appInfo, deviceIndex, hasMoreDevices, DI8DEVCLASS_GAMECTRL); 
			++deviceIndex;

			if (device.isValid()) {

				device->AddRef();
				devices.addOne(device);
			}
		}

		//init cloneIndicies
		{
			DIDEVICEINSTANCE devInfoI;
			devInfoI.dwSize = sizeof(DIDEVICEINSTANCE);

			DIDEVICEINSTANCE devInfoJ;
			devInfoJ.dwSize = sizeof(DIDEVICEINSTANCE);

			BufferString deviceNameI;

			for (Devices::Index i = 0; i < devices.count; ++i) {

				unsigned int& cloneIndex = cloneIndices.addOne();
				cloneIndex = 0;

				if (devices[i]->GetDeviceInfo(&devInfoI) == DI_OK) {

					deviceNameI.assign(devInfoI.tszInstanceName);

					for (Devices::Index j = 0; j < i; ++j) {

						if (devices[j]->GetDeviceInfo(&devInfoJ) == DI_OK) {

							if (deviceNameI.equalsIgnoreCase(devInfoJ.tszInstanceName)) {

								++cloneIndex;
							}
						}
					}
				}
			}
		}

		//Assign DeviceInfos
		{

			DIDEVICEINSTANCE devInfo;
			devInfo.dwSize = sizeof(DIDEVICEINSTANCE);

			for (DeviceInfos::Index i = 0; i < mDeviceInfos.count; ++i) {

				DeviceInfo& deviceInfo = dref(mDeviceInfos[i]);

				bool deviceFound = false;
				unsigned int deviceIndex;

				if (deviceInfo.name.isValid()) {

					for (Devices::Index j = 0; j < devices.count; ++j) {

						if (devices[j]->GetDeviceInfo(&devInfo) == DI_OK) {

							if (deviceInfo.name->equalsIgnoreCase(devInfo.tszInstanceName)) {

								if (deviceInfo.cloneIndex == cloneIndices[j]) {
									
									deviceFound = true;
									deviceIndex = j;
									break;
								}
							}
						}
					}

				} else {

					deviceFound = true;
					deviceIndex = deviceInfo.index;
				}

				if (deviceFound) {

					deviceInfos.addOne(&deviceInfo);

				} else {

					deviceInfos.addOne(NULL);
				}
			}
		}

		//Create Controllers
		{
			for (Devices::Index i = 0; i < devices.count; ++i) {

				SoftPtr<DeviceInfo> deviceInfo = i < deviceInfos.count ? deviceInfos[i] : NULL;
				bool isEnabled = !deviceInfo.isValid() || deviceInfo->enable;

				if (isEnabled || controllerDumpLog) {

					SoftRef<Controller_DirectInput8Poll> controller;

					WE_MMemNew(controller.ptrRef(), Controller_DirectInput8Poll());

					if (controller->init(appInfo, devices[i], t, deviceInfo)) {

						if (controllerDumpLog) {

							dumpLogController(controller, t, cloneIndices[i], isEnabled, dref(controllerDumpLog));
						}
						
						if (isEnabled)
							addController(controller);
					} 
				}
			}
		}
	}
}

void InputManager::update(const Time& t) {

	for (Index i = 0; i < mControllers.count; ++i) {

		Controller& controller = dref(mControllers[i]);

		if (controller.isActive()) {

			controller.update(t);
		}
	}

	for (Index i = 0; i < mUpdatables.count; ++i) {

		mUpdatables[i]->inputUpdate(t);
	}
}

InputManager::Index InputManager::getControllerCount() {

	return mControllers.count;
}

Controller* InputManager::getController(const Index& index) {

	SoftPtr<Controller> ret = mControllers[index];
	ret->AddRef();

	return ret;
}
		
void InputManager::removeController(const Index& index) {

	mControllers[index]->setManager(NULL);
	mControllers.removeSwapWithLast(index, true);
}

void InputManager::removeController(Controller* pController) {

	Index index;

	if (mControllers.searchFor(pController, 0, mControllers.count, index)) {

		removeController(index);
	}
}

void InputManager::addController(Controller* pController) {

	pController->setManager(this);
	pController->AddRef();
	mControllers.addOne(pController);
}


void InputManager::removeDestructorNotifiable(const Index& index) {

	mDestructorNotifiables.removeSwapWithLast(index, true);
}

void InputManager::removeDestructorNotifiable(DestructorNotifiable* pDestructorNotifiable) {

	Index index;

	if (mDestructorNotifiables.searchFor(pDestructorNotifiable, 0, mDestructorNotifiables.count, index)) {

		removeDestructorNotifiable(index);
	}
}

void InputManager::addDestructorNotifiable(DestructorNotifiable* pDestructorNotifiable) {

	mDestructorNotifiables.addOne(pDestructorNotifiable);
}

void InputManager::removeUpdatable(const Index& index) {

	mUpdatables.removeSwapWithLast(index, true);
}

void InputManager::removeUpdatable(Updatable* pUpdatable) {

	Index index;

	if (mUpdatables.searchFor(pUpdatable, 0, mUpdatables.count, index)) {

		removeUpdatable(index);
	}
}

void InputManager::addUpdatable(Updatable* pUpdatable) {

	mUpdatables.addOne(pUpdatable);
}

Controller* InputManager::findController(const TCHAR* name, unsigned int controllerInstanceIndex, bool ignoreCase, bool visibleOnly) {

	unsigned int instanceIndex = 0;
	String comp(name);

	for (Index i = 0; i < mControllers.count; ++i) {

		Controller& controller = dref(mControllers[i]);

		if (!visibleOnly || controller.isVisible()) {

			if (ignoreCase ? comp.equalsIgnoreCase(controller.getName()) : comp.equals(controller.getName())) {

				if (instanceIndex == controllerInstanceIndex) {

					controller.AddRef();
					return &controller;
				}

				++instanceIndex;
			}
		}
	}

	return NULL;
}

Controller* InputManager::selectController(const TCHAR* selectName, unsigned int controllerInstanceIndex, bool ignoreCase, bool visibleOnly) {

	unsigned int instanceIndex = 0;
	String comp(selectName);

	for (Index i = 0; i < mControllers.count; ++i) {

		Controller& controller = dref(mControllers[i]);

		if (!visibleOnly || controller.isVisible()) {

			if (ignoreCase ? comp.equalsIgnoreCase(controller.getSelectName()) : comp.equals(controller.getSelectName())) {

				if (instanceIndex == controllerInstanceIndex) {

					controller.AddRef();
					return &controller;
				}

				++instanceIndex;
			}
		}
	}

	return NULL;
}


InputManager::Index InputManager::getDeviceInfoCount() {

	return mDeviceInfos.count;
}

DeviceInfo& InputManager::getDeviceInfo(const Index& index) {

	return dref(mDeviceInfos[index]);
}
		
void InputManager::removeDeviceInfo(const Index& index) {

	mDeviceInfos.removeSwapWithLast(index, true);
}

void InputManager::addDeviceInfo(DeviceInfo* pDeviceInfo) {

	mDeviceInfos.addOne(pDeviceInfo);
}

} }