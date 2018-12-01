#ifndef _WERenderSprite_h
#define _WERenderSprite_h

#include "WERenderObjects.h"
#include "WERenderer.h"
#include "../loadContext/WELoadContext.h"
#include "../data/WEDataSource.h"

#include "../WEPtr.h"
#include "../scene/WESceneTransform.h"
#include "../math/WEVector.h"
#include "../math/WEMatrix.h"

namespace WE {

	class Renderer;

	/*
		The only thing that keeps us for adding a mesh into a RenderSprite
		is the fact that we cannot load vertex colors?
		or maybe the way we handle material instances not allowing easy replacement of colors?
	*/
	class RenderSprite {
	public:

		enum RenderBlendTypeEnum {

			Blend_KeepCurrent = 0, Blend_Alpha, Blend_Multiply, Blend_Add
		};

		class Skin {
		public:

			HardPtr<RenderColor> mColor;
			SoftRef<RenderTexture> mTexture;
			RenderBlendTypeEnum mBlendType;
			RenderTextureFilterEnum mTexFilter;

		public:

			RenderColor* enableColor(bool enable);
			bool loadTexture(const TCHAR* filePath, RenderLoadContext& loadContext, bool genMipMaps = true, const TCHAR* mipFilter = NULL);

			inline RenderColor& color() { return mColor; }

			inline RenderColor* getColor() { return mColor; }
			inline RenderTexture* getTexture() { return mTexture; } // no release needed

			void load(BufferString& tempStr, DataChunk* pChunk, RenderLoadContext& loadContext, float defaultAlpha = 1.0f, String* pTexturesPath = NULL, const TCHAR* attrNameColor = L"color", const TCHAR* attrNameTexture = L"texture", const TCHAR* attrNameAlpha = L"alpha", const TCHAR* attrNameBlendType = L"blendType", const TCHAR* attrNameTexFilter = L"texFilter");

			Skin();
		};

		struct SoftSkin {

			SoftPtr<RenderColor> mColor;
			SoftPtr<RenderTexture> mTexture;
			RenderBlendTypeEnum mBlendType;
			RenderTextureFilterEnum mTexFilter;

			SoftSkin();

			inline RenderColor& color() { return mColor; }

			inline RenderColor* getColor() { return mColor; }
			inline RenderTexture* getTexture() { return mTexture; } // no release needed

			inline void setIfEmpty(RenderColor* pColor) { if (!mColor.isValid()) mColor = pColor; }
			inline void setIfEmpty(RenderTexture* pTex) { if (!mTexture.isValid()) mTexture = pTex; }
			inline void setIfEmpty(const RenderBlendTypeEnum& val) { if (mBlendType == Blend_KeepCurrent) mBlendType = val; }
			inline void setIfEmpty(const RenderTextureFilterEnum& val) { if (mTexFilter == RTF_None) mTexFilter = val; }

			void setIfEmpty(Skin* pSkin) { 
				
				if (pSkin) {
					
					setIfEmpty(pSkin->getColor());
					setIfEmpty(pSkin->getTexture());
					setIfEmpty(pSkin->mBlendType);
					setIfEmpty(pSkin->mTexFilter);
				}
			}
		};

		typedef unsigned int Index;

	public:

		RenderSprite(bool enableSkin = false, bool enableColor = false, bool enableTransform = false, const DimMap_2_3* pDimMap = &DimMap_2_3::k/*, Render2DScaling viewportScaling = R2DS_Width*/);

		inline void setDimMap(const DimMap_2_3& dimMap) { mDimMap = &dimMap; }

		Skin* enableSkin(bool enable);
		inline Skin* getSkin() { return mSkin; }
		inline Skin& skin() { return mSkin; }
		inline RenderColor& color() { return mSkin->color(); }

		inline bool loadTexture(const TCHAR* filePath, RenderLoadContext& loadContext, bool genMipMaps = true, const TCHAR* mipFilter = NULL) {

			return mSkin->loadTexture(filePath, loadContext, genMipMaps, mipFilter);
		}

		void load(BufferString& tempStr, DataChunk* pChunk, RenderLoadContext& loadContext, float defaultAlpha = 1.0f, String* pTexturesPath = NULL, 
					const TCHAR* attrNameColor = L"color", const TCHAR* attrNameTexture = L"texture", 
					const TCHAR* attrNameBlendType = L"blendType", const TCHAR* attrNameTexFilter = L"texFilter", 
					const TCHAR* attrNameSize = NULL, const TCHAR* attrNameScale = NULL, const TCHAR* attrNameAlpha = NULL, CoordSysConv* pConv = NULL,
					const TCHAR* attrNamePos = NULL, const TCHAR* attrNameOrient = NULL, const TCHAR* attrNameAnchor = NULL,
					const TCHAR* attrNameScaleType = NULL, const TCHAR* attrNamePosType = NULL);
	
		SceneTransform* enableTransform(bool enable);

	public:

		float getScaleForSize(const float& sizeX, RenderTexture* pTexture = NULL); //returns the scale needed to get the desired size

		void setScale(const float& scaleX, RenderQuad::AnchorEnum anchorX = RenderQuad::Anchor_None, RenderQuad::AnchorEnum anchorY = RenderQuad::Anchor_None, RenderTexture* pTexture = NULL); //uses texture to determin sizeX, sizeY
		void setSize(const float& sizeX, RenderQuad::AnchorEnum anchorX = RenderQuad::Anchor_None, RenderQuad::AnchorEnum anchorY = RenderQuad::Anchor_None, RenderTexture* pTexture = NULL); //uses texture to determin sizeY

		void setSize(const float& sizeX, const float& sizeY, RenderQuad::AnchorEnum anchorX = RenderQuad::Anchor_None, RenderQuad::AnchorEnum anchorY = RenderQuad::Anchor_None);
		void getSize(float& sizeX, float& sizeY);

	public:

		//2D mode

		void setOrientation2D(const float& orientX, const float& orientY, bool renormalize = false);
		void setPosition2D(const float& posX, const float& posY);

		void render2D(Renderer& renderer, const SceneTransform* pTransform, 
						const RenderColor* pColor, RenderTexture* pTexture, 
						const RenderBlendTypeEnum& blendType, RenderTextureFilterEnum& texFilter);
			
		template<typename T1>
		inline void render2D(Renderer& renderer, T1& skin, const SceneTransform* pTransform = NULL, const RenderColor* pColor = NULL, RenderTexture* pTexture = NULL) {

			render2D(renderer, pTransform, 
						pColor ? pColor : skin.mColor, pTexture ? pTexture : skin.mTexture, 
						skin.mBlendType, skin.mTexFilter);
		}

		inline void render2D(Renderer& renderer) { render2D(renderer, mSkin.dref()); }

	public:

		//3D mode

		//sets the offset using the DimMap_2_3.mapNone dimension
		void setOffset3D(const float& offset); 

		SceneTransform& transform3D() { return mTransform; }

		void render3D(Renderer& renderer, const SceneTransform* pTransform, 
						const RenderColor* pColor, RenderTexture* pTexture, 
						const RenderBlendTypeEnum& blendType, RenderTextureFilterEnum& texFilter);

		template<typename T1>
		inline void render3D(Renderer& renderer, T1& skin, const SceneTransform* pTransform = NULL, const RenderColor* pColor = NULL, RenderTexture* pTexture = NULL) {

			render3D(renderer, pTransform, 
						pColor ? pColor : skin.mColor, pTexture ? pTexture : skin.mTexture, 
						skin.mBlendType, skin.mTexFilter);
		}

		inline void render3D(Renderer& renderer) { render3D(renderer, mSkin.dref()); }


	protected:

		void setMaterial(Renderer& renderer, const RenderColor* pColor, const RenderBlendTypeEnum& blendType, const RenderTextureFilterEnum& texFilter);
		int loadVector(BufferString& tempStr, DataChunk& chunk, const TCHAR* attrName, Vector3& vect, CoordSysConv* pConv);
		bool updateDynamicData2D(Renderer& renderer);

	protected:

		const DimMap_2_3* mDimMap;
		RenderQuad mQuad;

		HardPtr<SceneTransform> mTransform;
		HardPtr<Skin> mSkin;
	
	protected:

		enum DynamicDataType {

			Dynamic_None = -1, Dynamic_ViewportPos, Dynamic_ViewportScale, Dynamic_Anchor
		};

		struct DynamicData {

			DynamicDataType type;
			float values[2];
		};

		HardPtr<DynamicData> mDynamicAnchor;
		HardPtr<DynamicData> mDynamicSize;
		HardPtr<DynamicData> mDynamicPos;
		
	public:

		template<typename T1>
		static inline RenderColor* chooseColor(T1* pSkin1) {

			return (pSkin1 && pSkin1->getColor()) ? pSkin1->mColor.ptr() : NULL;
		}

		template<typename T1>
		static inline RenderTexture* chooseTexture(T1* pSkin1) {

			return (pSkin1 && pSkin1->getTexture()) ? pSkin1->mTexture.ptr() : NULL;
		}
	};
	
}

#endif