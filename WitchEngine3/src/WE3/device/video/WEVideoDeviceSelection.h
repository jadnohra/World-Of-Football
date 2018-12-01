#ifndef _WEVideoDeviceSeletion_h
#define _WEVideoDeviceSeletion_h

#include "../../WEDataTypes.h"

namespace WE {

	struct VideoDeviceSelectionResult {

		DWORD vertexProcessing;
	};

	struct VideoDeviceSelectionContext {

		bool allowRefDevice;
		bool requireAlphaBlending;
		bool requireVertexBlending;
		bool allowSoftwareVertexBlending;
		bool allowNoPaletteVertexBlending;
		bool requireDepthBias;
			
		VideoDeviceSelectionResult result;

		int minStencilBitDepth;
		bool vSync;

		bool tryAntiAlias;
		int antiAliasSamples;

		int maxColorDepth;

		int modifySettingsAlternativeIndex;
		int hasModifySettingsAlternatives;
	};

}

#endif