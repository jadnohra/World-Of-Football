#ifndef _WERenderMaterialCreator_h
#define _WERenderMaterialCreator_h

#include "../WERenderer.h"
#include "WERenderMaterial.h"
#include "WERenderMaterialLoadInfo.h"

namespace WE {

	class RenderMaterialCreator {
	public:

		static RenderMaterial* create(RenderMaterialLoadInfo& loadInfo);
	};

}

#endif