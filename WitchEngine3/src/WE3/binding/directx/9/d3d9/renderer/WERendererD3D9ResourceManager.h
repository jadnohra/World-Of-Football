#ifndef _WERendererD3D9ResourceManager_h
#define _WERendererD3D9ResourceManager_h

#include "includeRenderer.h"
#include "../includeD3D9.h"
#include "WERendererD3D9Enums.h"

namespace WE {

	class RendererD3D9_VolatileResource {
	public:

		virtual void onDeviceState(IDirect3DDevice9* pDevice, RendererD3D9_DeviceState state) = 0;
	};

	class RendererD3D9_ResourceManager {
	public:

		typedef WETL::IndexShort ResourceIndex;	

		typedef WETL::CountedArray<RendererD3D9_VolatileResource*, false, ResourceIndex::Type> ResourceArray;
		ResourceArray mVolatileResources;

		RendererD3D9_ResourceManager();
		~RendererD3D9_ResourceManager();

		void init(ResourceIndex::Type count);

		void add(RendererD3D9_VolatileResource* pResource);
		bool remove(RendererD3D9_VolatileResource* pResource);

		void onDeviceState(IDirect3DDevice9* pDevice, RendererD3D9_DeviceState state);
	};

}

#endif