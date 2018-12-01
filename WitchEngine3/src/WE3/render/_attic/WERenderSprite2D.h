#ifndef _WERenderSprite2D_h
#define _WERenderSprite2D_h

#include "WERenderObjects.h"
#include "../loadContext/WELoadContext.h"

#include "../math/WEVector.h"
#include "../math/WEMatrix.h"

namespace WE {

	class Renderer;

	class RenderSprite2D {
	public:

		enum Anchor {

			Anchor_None = -1, Anchor_Left, Anchor_Center, Anchor_Right
		};

	public:

		RenderSprite2D(bool enableColor = false, bool scaleByViewport = false, bool sizeIsTextureScale = false);

		bool loadTexture(const TCHAR* filePath, RenderLoadContext* pLoadContext, Renderer* pRenderer = NULL, bool genMipMaps = true, const TCHAR* mipFilter = NULL, bool setSizeIsTextureScale = true);

		void setSizeIsTextureScale(bool value) { mIsDirty = true; mSizeIsTextureScale = true;}
				
		void setScaleByViewport(bool scale);
		
		void setDir(const float& dirX, const float& dirY);
		void setPosition(const float& posX, const float& posY);
		void setAnchor(const Anchor& anchorX, const Anchor& anchorY);
		void setSize(const float& sizeX, const float& sizeY);

		inline bool hasTexture() { return mTexture.isValid(); }

		void enableColor(bool enable);
		inline RenderColor& color() { return mColor; } 
		inline const RenderColor& getColor() { return mColor; } 

		void render(Renderer& renderer, RenderColor* pColor = NULL);
		bool updatePoints(Renderer& renderer);

		bool getSize(float& sizeX, float& sizeY, Renderer* pRenderer);

	protected:

		enum PointIndex {
			
			Point_None = -1, Point_LeftTop, Point_RightTop, Point_RightBottom, Point_LeftBottom
		};

	protected:

		bool mIsDirty;

		HardPtr<RenderColor> mColor;
		bool mScaleByViewport;

		SoftRef<RenderTexture> mTexture;
		
		Vector3 mPos;
		bool mSizeIsTextureScale;
		Vector3 mSize;
		Anchor mAnchor[2];

		Vector3 mDir;

		Vector3 mPoints[4];
	};

}

#endif