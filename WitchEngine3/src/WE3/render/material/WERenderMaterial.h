#ifndef _WEMaterial_h
#define _WEMaterial_h

#include "../../WERef.h"
#include "../../WEDataSource.h"
#include "../../WETL/WETLArray.h"

#include "../loadContext/WERenderLoadContext.h"

#include "WERenderMaterialInstance.h"

namespace WE {

	class RenderMaterial : public Ref {
	public:

		RenderMaterial();
		virtual ~RenderMaterial();
	
		bool isRenderReady();
		bool prepareForRenderer(RenderLoadContext& renderLoadContext);
		RenderMaterialInstance* createInstance(RenderLoadContext& renderLoadContext, int qualityLevel = 0);
		static void destroyInstance(RenderMaterialInstance*& pInst);

		bool set(Renderer& renderer, RenderMaterialInstance* pInst); //returns true if unset needed
		void unset(Renderer& renderer, RenderMaterialInstance* pInst);

	public:

		bool init(DataChunk& resolvedSourceChunk);
		

	protected:

		void setDefaults(Renderer* pRenderer = NULL);

		RefWrap<DataChunk> mSource;

		RenderColor mAmbient;
		RenderColor mDiffuse;
		RenderColor mSpecular;
		RenderColor mEmissive;
		float mSpecularPower;
		float mDepthBias;

		union {

			struct {

				unsigned mIsRenderReady : 1;
				unsigned mEnableLighting : 1;
				unsigned mEnableSpecular : 1;
				unsigned mEnableBlend : 1;
				unsigned mDisableCull : 1;
				unsigned mEnableShadows : 1;
				unsigned mFillMode : 1;

				unsigned mTexFiltMin : 2;
				unsigned mTexFiltMag : 2;
				unsigned mTexFiltMip : 2;
				unsigned mTexWrap : 1;
			};

			struct {

				unsigned mBitFlagBits : 32;
			};
		};

		struct LightSwitch {

			BYTE index;
			BYTE state;
			BYTE savedState;
		};

		struct LightSwitcher {

			typedef WETL::CountedArray<LightSwitch, false, unsigned int, WETL::ResizeExact> LightSwitches;

			LightSwitches lightSwitches;
		};

		HardPtr<LightSwitcher> mLightSwitcher;
	};

}

#endif