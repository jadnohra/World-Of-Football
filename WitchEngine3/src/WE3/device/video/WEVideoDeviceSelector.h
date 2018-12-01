#ifndef _WEVideoDeviceSeletor_h
#define _WEVideoDeviceSeletor_h

#include "WEVideoDeviceSelection.h"

namespace WE {

	class VideoDeviceSelector {
	public:

		virtual bool fillDeviceSelection(VideoDeviceSelectionContext& selection, int currTryCount) = 0;
	};
}

#endif