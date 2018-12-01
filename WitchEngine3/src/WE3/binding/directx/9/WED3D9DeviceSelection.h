#ifndef _WED3D9DeviceSelection_h
#define _WED3D9DeviceSelection_h

#include "d3d9/includeD3D9.h"
#include "dxut9/includeDXUT9.h"
#include "../../../device/video/WEVideoDeviceSelection.h"

namespace WE {

	class D3D9DeviceSelection {
	public:

		static bool isDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
													D3DFORMAT BackBufferFormat, bool bWindowed, 
													VideoDeviceSelectionContext* pContext );

		static bool modifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, 
													VideoDeviceSelectionContext* pContext );

	};

}

#endif