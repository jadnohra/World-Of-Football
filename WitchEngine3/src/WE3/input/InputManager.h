#ifndef _InputManager_h_Input
#define _InputManager_h_Input

#include "../WETL/WETLArray.h"
#include "../native/WENativeAppInfo.h"
#include "../WELog.h"
using namespace WE;

#include "Updatable.h"
#include "DestrctuorNotifiable.h"
#include "DeviceInfo.h"

namespace WE { namespace Input {

	class Controller;

	class InputManager {
	public:

		typedef unsigned int Index;

	public:

		~InputManager();

		void detectControllers(const NativeAppInfo& appInfo, const Time& t, Log* controllerDumpLog = NULL);

		Index getControllerCount();
		Controller* getController(const Index& index); //release when done
		Controller* findController(const TCHAR* name, unsigned int instanceIndex = 0, bool ignoreCase = false, bool visibleOnly = false); //release when done
		Controller* selectController(const TCHAR* selectName, unsigned int instanceIndex = 0, bool ignoreCase = false, bool visibleOnly = false); //release when done

		void removeController(const Index& index);
		void removeController(Controller* pController);

		void addController(Controller* pController);

		void removeUpdatable(Updatable* pUpdatable);
		void addUpdatable(Updatable* pUpdatable);

		void update(const Time& t);

		void removeDestructorNotifiable(DestructorNotifiable* pDestructorNotifiable);
		void addDestructorNotifiable(DestructorNotifiable* pDestructorNotifiable);

		Index getDeviceInfoCount();
		DeviceInfo& getDeviceInfo(const Index& index); 

		void removeDeviceInfo(const Index& index);
		void addDeviceInfo(DeviceInfo* pDeviceInfo); //ptr ownership taken by InputManager

	protected:

		void dumpLogController(Controller& controller, const Time& t, unsigned int instanceIndex, bool isEnabled, Log& log);
		
		void removeDestructorNotifiable(const Index& index);
		void removeUpdatable(const Index& index);

	protected:

		typedef WETL::CountedRefArray<Controller*, false, Index, WETL::ResizeExact> Controllers;
		typedef WETL::CountedArray<Updatable*, false, Index, WETL::ResizeExact> Updatables;
		typedef WETL::CountedArray<DestructorNotifiable*, false, Index, WETL::ResizeExact> DestructorNotifiables;
		typedef WETL::CountedPtrArray<DeviceInfo*, false, Index, WETL::ResizeExact> DeviceInfos;

		Controllers mControllers;
		Updatables mUpdatables;
		DestructorNotifiables mDestructorNotifiables;
		DeviceInfos mDeviceInfos;
	};

} }

#endif