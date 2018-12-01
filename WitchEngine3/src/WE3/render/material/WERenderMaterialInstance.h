#ifndef _WERenderMaterialInstance_h
#define _WERenderMaterialInstance_h

#include "../WERenderer.h"
#include "../../WERef.h"

#include "../WERenderObjects.h"

namespace WE {

	class RenderMaterial;

	class RenderMaterialInstance {
	public:

		RenderMaterialInstance();
		virtual ~RenderMaterialInstance();

		RenderMaterialInstance* createClone();

	public:

		RefWrap<RenderTexture> mTexture;
		RefWrap<RenderTexture> mSpecularMap;

		//for now some states are stored in the material
		RefWrap<RenderMaterial> mMaterial;
	};

}

#endif