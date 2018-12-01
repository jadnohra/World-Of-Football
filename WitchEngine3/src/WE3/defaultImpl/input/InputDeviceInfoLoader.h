#ifndef _InputLoader_h_WE_Input
#define _InputLoader_h_WE_Input

#include "../../input/InputManager.h"
#include "../../data/WEDataSourcePool.h"

namespace WE { namespace Input {

	class Loader {
	public:

		static void loadControllerProfiles(InputManager& manager, const TCHAR* inputProfileFolderPath, DataSourcePool& dataSourcePool);
		static void processInputProfile(Input::InputManager& manager, DataChunk& inputProfileChunk, BufferString& tempStr);

		static void loadDeviceInfos(Input::InputManager& manager, DataChunk& deviceInfosChunk, BufferString& tempStr);
	};

} }

#endif