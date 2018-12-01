#include "WERenderer.h"
#include "WERenderSprite2D.h"

#include "loadContext/WERenderLoadContextImpl.h"
#include "texture/WERenderTextureCreator.h"

namespace WE {

RenderSprite2D::RenderSprite2D(bool _enableColor, bool scaleByViewport, bool sizeIsTextureScale) : mIsDirty(false) {

	mPos.zero();
	mSize.zero();
	mScaleByViewport = false;
	mAnchor[0] = Anchor_Center;
	mAnchor[1] = Anchor_Center;
	mPoints[0].zero();
	mPoints[1].zero();
	mPoints[2].zero();
	mPoints[3].zero();

	mDir.zero();

	enableColor(_enableColor);
	setScaleByViewport(scaleByViewport);
	setSizeIsTextureScale(sizeIsTextureScale);
}

void RenderSprite2D::enableColor(bool enable) {

	if (enable) {

		if (!mColor.isValid())
			WE_MMemNew(mColor.ptrRef(), RenderColor(1.0f, 1.0f, 1.0f, 1.0f));

	} else {

		mColor.destroy();
	}
}

bool RenderSprite2D::loadTexture(const TCHAR* filePath, RenderLoadContext* pLoadContext, Renderer* pRenderer, bool genMipMaps, const TCHAR* mipFilter, bool _setSizeIsTextureScale) {

	SoftPtr<RenderLoadContext> loadContext;
	RenderLoadContextImpl loadContextImpl;

	if (pLoadContext == NULL) {

		if (pRenderer == NULL)
			return false;

		loadContextImpl.setRenderer(pRenderer);
		loadContext = &loadContextImpl;

	} else {

		loadContext = pLoadContext;
	}

	RenderTextureLoadInfo loadInfo(loadContext);

	loadInfo.filePath = filePath;
	loadInfo.genMipmaps = genMipMaps;
	loadInfo.mipFilter = mipFilter;
		
	mTexture = RenderTextureCreator::createAndLoad(loadInfo);

	if (mTexture.isValid() && _setSizeIsTextureScale) {

		setSizeIsTextureScale(true);
	}

	return mTexture.isValid();
}

void RenderSprite2D::setScaleByViewport(bool scale) {

	mScaleByViewport = scale;
}

void RenderSprite2D::setDir(const float& dirX, const float& dirY) {

	mIsDirty = true;

	mDir.x = dirX;
	mDir.y = dirY;
}

void RenderSprite2D::setPosition(const float& posX, const float& posY) {

	mIsDirty = true;
	
	mPos.x = posX;
	mPos.y = posY;
}

void RenderSprite2D::setAnchor(const Anchor& anchorX, const Anchor& anchorY) {

	mIsDirty = true;

	mAnchor[0] = anchorX;
	mAnchor[1] = anchorY;
}

void RenderSprite2D::setSize(const float& sizeX, const float& sizeY) {

	mIsDirty = true;

	mSize.x = sizeX;
	mSize.y = sizeY;
}

void RenderSprite2D::render(Renderer& renderer, RenderColor* pColor) {

	if (!updatePoints(renderer))
		return;

	renderer.draw2DQuad(mPoints[0].x, mPoints[0].y, mPoints[1].x, mPoints[1].y, 
						mPoints[2].x, mPoints[2].y, mPoints[3].x, mPoints[3].y, 
						mScaleByViewport, pColor ? pColor : mColor, mTexture);
}

bool RenderSprite2D::getSize(float& sizeX, float& sizeY, Renderer* pRenderer) {

	if (mTexture.isValid() && mSizeIsTextureScale) {

		unsigned int width;
		unsigned int height;

		assrt(mTexture->getLoadSize(width, height));

		if (mScaleByViewport) {

			RenderViewportInfo viewport;

			if (!dref(pRenderer).getViewportInfo(viewport))
				return false;

			sizeX = mSize.x * (float) width / (float) viewport.width;
			sizeY = mSize.y * (float) height / (float) viewport.height;

		} else {

			sizeX = mSize.x * (float) width;
			sizeY = mSize.y * (float) height;
		}

	} else {

		sizeX = mSize.x;
		sizeY = mSize.y;
	}

	return true;
}

bool RenderSprite2D::updatePoints(Renderer& renderer) {

	if (!mIsDirty && !renderer.wasReset())
		return true;

	mIsDirty = false;

	{
		float sizeX;
		float sizeY;

		if (!getSize(sizeX, sizeY, &renderer))
			return false;

		float offsetX = 0.0f;
		float offsetY = 0.0f;

		switch (mAnchor[0]) {

			case Anchor_Left: 
				offsetX = 0.5f * sizeX;
				break;

			case Anchor_Right: 
				offsetX = -0.5f * sizeX;
				break;
		}

		switch (mAnchor[1]) {

			case Anchor_Left: 
				offsetY = 0.5f * sizeY;
				break;

			case Anchor_Right: 
				offsetY = -0.5f * sizeY;
				break;
		}

		{
			Vector3& pt = mPoints[Point_LeftTop];

			pt.x = -0.5f * sizeX + offsetX;
			pt.y = -0.5f * sizeY + offsetY;
		}

		{
			Vector3& pt = mPoints[Point_RightTop];

			pt.x = 0.5f * sizeX + offsetX;
			pt.y = -0.5f * sizeY + offsetY;
		}

		{
			Vector3& pt = mPoints[Point_RightBottom];

			pt.x = 0.5f * sizeX + offsetX;
			pt.y = 0.5f * sizeY + offsetY;
		}

		{
			Vector3& pt = mPoints[Point_LeftBottom];

			pt.x = -0.5f * sizeX + offsetX;
			pt.y = 0.5f * sizeY + offsetY;
		}
	}

	if (!mDir.isZero()) {

		Matrix33 rotMatrix;

		rotMatrix.row[0] = mDir;
		mDir.cross(Vector3::kZ, rotMatrix.row[1]);
		rotMatrix.row[2] = Vector3::kZ;
		
		rotMatrix.transform(mPoints[0]);
		rotMatrix.transform(mPoints[1]);
		rotMatrix.transform(mPoints[2]);
		rotMatrix.transform(mPoints[3]);
	}

	mPoints[0].add(mPos);
	mPoints[1].add(mPos);
	mPoints[2].add(mPos);
	mPoints[3].add(mPos);

	return true;
}


}