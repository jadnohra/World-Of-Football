#ifndef _InputDeviceInfo_h_Input
#define _InputDeviceInfo_h_Input

#include "../WEPtr.h"
#include "../string/WEString.h"

namespace WE { namespace Input {

	class DeviceInfo {
	public:

		HardPtr<String> name;
		unsigned int index;
		unsigned int cloneIndex;
		bool enable;
		HardPtr<String> profile;
		float deadZone;

	public:

		DeviceInfo(DeviceInfo* pRef = NULL);

		void reset();
	};

} }

#endif