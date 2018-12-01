#ifndef _InputControllerProfileLoader_h_WE_Input
#define _InputControllerProfileLoader_h_WE_Input

#include "../../input/InputManager.h"
#include "../../data/WEDataSourcePool.h"

namespace WE { namespace Input {

	class Controller;
	class SourceHolder;

	class Loader {
	public:

		static void loadControllerProfiles(InputManager& manager, const TCHAR* inputProfileFolderPath, DataSourcePool& dataSourcePool);
		static void processInputProfile(Input::InputManager& manager, DataChunk& inputProfileChunk, BufferString& tempStr);

		static void loadDeviceInfos(Input::InputManager& manager, DataChunk& deviceInfosChunk, BufferString& tempStr);
		static bool loadDeviceInfo(Input::DeviceInfo& deviceInfo, DataChunk& deviceInfoChunk, BufferString& tempStr);


	public:

		static bool acceptSource(SourceHolder& source, float filterMin, float filterMax, bool checkMarked);
		static bool getSource(Controller& controller, String* pAddress, DataChunk* pChunk, bool allowDummy, float filterMin, float filterMax, bool checkMarked, Controller* pMarkController, SourceHolder& source);
	};

} }

#endif