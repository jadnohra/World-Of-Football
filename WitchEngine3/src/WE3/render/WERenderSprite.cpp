#include "WERenderer.h"
#include "WERenderSprite.h"

#include "loadContext/WERenderLoadContextImpl.h"
#include "texture/WERenderTextureCreator.h"
#include "../helper/load/WERenderDataLoadHelper.h"
#include "../helper/load/WEMathDataLoadHelper.h"

namespace WE {

RenderSprite::SoftSkin::SoftSkin() : mBlendType(Blend_KeepCurrent), mTexFilter(RTF_None) {
}

RenderSprite::Skin::Skin() : mBlendType(Blend_KeepCurrent), mTexFilter(RTF_None) {
}

void RenderSprite::Skin::load(BufferString& tempStr, DataChunk* pChunk, RenderLoadContext& loadContext, float defaultAlpha, String* pTexturesPath, const TCHAR* attrNameColor, const TCHAR* attrNameTexture, const TCHAR* attrNameAlpha, const TCHAR* attrNameBlendType, const TCHAR* attrNameTexFilter) {

	if (pChunk) {

		if (attrNameColor && pChunk->hasAttribute(attrNameColor)) {

			this->enableColor(true);
			RenderDataLoadHelper::extract(tempStr, dref(pChunk), this->color(), true, attrNameColor, defaultAlpha, false);
		}

		float alpha = defaultAlpha;

		if (attrNameAlpha && pChunk->hasAttribute(attrNameAlpha)) {

			this->enableColor(true);
			pChunk->scanAttribute(tempStr, attrNameAlpha, L"%f", &alpha);

			this->color().alpha = alpha;
		}

		if (attrNameBlendType && pChunk->getAttribute(attrNameBlendType, tempStr)) {

			if (tempStr.equals(L"default") || tempStr.equals(L"alpha")) {
				mBlendType = Blend_Alpha;
			} else if (tempStr.equals(L"multiply")) {
				mBlendType = Blend_Multiply;
			} else if (tempStr.equals(L"add")) {
				mBlendType = Blend_Add;
			} else {

				assrt(false);
			}
		}

		if (attrNameTexFilter && pChunk->getAttribute(attrNameTexFilter, tempStr)) {

			fromString(tempStr, RTS_Min, mTexFilter);
		}
		

		if (attrNameTexture && pChunk->getAttribute(attrNameTexture, tempStr)) {

			if (pTexturesPath) {

				BufferString texturePath;

				texturePath.assign(dref(pTexturesPath));
				PathResolver::appendPath(texturePath, tempStr.c_tstr(), true);

				this->loadTexture(texturePath.c_tstr(), loadContext, true);

			} else {

				pChunk->resolveFilePath(tempStr);

				this->loadTexture(tempStr.c_tstr(), loadContext, true);
			}
		}
	}
}

bool RenderSprite::Skin::loadTexture(const TCHAR* filePath, RenderLoadContext& loadContext, bool genMipMaps, const TCHAR* mipFilter) {

	RenderTextureLoadInfo loadInfo(&loadContext);

	loadInfo.filePath = filePath;
	loadInfo.genMipmaps = genMipMaps;
	loadInfo.mipFilter = mipFilter;

	loadInfo.poolId = String::hash(filePath);

	mTexture = loadContext.textureSource().get(loadInfo);

	return mTexture.isValid();
}

RenderColor* RenderSprite::Skin::enableColor(bool enable) {

	if (enable) {

		if (!mColor.isValid())
			WE_MMemNew(mColor.ptrRef(), RenderColor(1.0f, 1.0f, 1.0f, 1.0f));

	} else {

		mColor.destroy();
	}

	return mColor;
}

int RenderSprite::loadVector(BufferString& tempStr, DataChunk& chunk, const TCHAR* attrName, Vector3& vect, CoordSysConv* pConv) {

	int scannedCount = 0;

	if (MathDataLoadHelper::extract(tempStr, chunk, vect, true, true, true, attrName, NULL, true, true, &scannedCount)) {

		if (pConv) {

			pConv->toTargetUnits(vect.el[0]);
			pConv->toTargetUnits(vect.el[1]);
		}
	}

	return scannedCount;
}



void RenderSprite::load(BufferString& tempStr, DataChunk* pChunk, RenderLoadContext& loadContext, float defaultAlpha, String* pTexturesPath, 
						const TCHAR* attrNameColor, const TCHAR* attrNameTexture, const TCHAR* attrNameBlendType, const TCHAR* attrNameTexFilter, 
						const TCHAR* attrNameSize, const TCHAR* attrNameScale, const TCHAR* attrNameAlpha, CoordSysConv* pConv,
						const TCHAR* attrNamePos, const TCHAR* attrNameOrient, const TCHAR* attrNameAnchor,
						const TCHAR* attrNameScaleType, const TCHAR* attrNamePosType) {

	if (mSkin.isValid()) {

		mSkin->load(tempStr, pChunk, loadContext, defaultAlpha, pTexturesPath, attrNameColor, attrNameTexture, attrNameAlpha, attrNameBlendType, attrNameTexFilter);
	}

	if (pChunk) {

		DataChunk& chunk = dref(pChunk);

		bool sizeSet = false;

		Vector3 tempVect;
		int scannedCount;

		RenderQuad::AnchorEnum anchors[2] = { RenderQuad::Anchor_None, RenderQuad::Anchor_None };
		
		if (attrNameAnchor) {

			if (chunk.getAttribute(attrNameAnchor, tempStr)) {

				bool found = false;
				StringPart part(&tempStr);

				tempStr.findNext(L',', 0, found, part.startIndex, part.count, false);

				if (found) {

					String string;
					StringPart part2(&tempStr);

					part.assignTo(string);

					anchors[0] = fromString(string, RenderQuad::Anchor_None);
					part.getRemainingPart(tempStr.c_tstr(), part2, tempStr.length());

					if (part2.count) {

						++part2.startIndex;
						--part2.count;

						part2.assignTo(string);
						anchors[1] = fromString(string, RenderQuad::Anchor_None);

					} else {

						anchors[1] = anchors[0];
					}

				} else {

					anchors[0] = fromString(tempStr, RenderQuad::Anchor_None);
					anchors[1] = anchors[0];
				}
			}
		}

		if (!sizeSet && attrNameSize) {
			
			scannedCount = loadVector(tempStr, chunk, attrNameSize, tempVect, NULL);
			sizeSet = scannedCount > 0;

			if (scannedCount == 2) {

				setSize(tempVect.el[0], tempVect.el[1], anchors[0], anchors[1]);
				
			} else if (scannedCount == 1) {

				setSize(tempVect.el[0], anchors[0], anchors[1]);
			} 
		} 

		bool needsDynamicAnchor = false;

		if (!sizeSet && attrNameScale) {

			scannedCount = loadVector(tempStr, chunk, attrNameScale, tempVect, NULL);
			sizeSet = scannedCount > 0;

			if (scannedCount == 1) {

				setScale(tempVect.el[0], anchors[0], anchors[1]);
			} 

			if (attrNameScaleType && chunk.getAttribute(attrNameScaleType, tempStr)) {

				if (tempStr.equals(L"viewport")) {

					if (!mDynamicSize.isValid())
						WE_MMemNew(mDynamicSize.ptrRef(), DynamicData());

					mDynamicSize->type = Dynamic_ViewportScale;
					mDynamicSize->values[0] = tempVect.el[0]; 
					mDynamicSize->values[1] = -1.0f; 
					needsDynamicAnchor = true;
				}
			}
		}

		if (needsDynamicAnchor && 
				(
					(anchors[0] != RenderQuad::Anchor_None && anchors[0] != RenderQuad::Anchor_Center)
					||	(anchors[1] != RenderQuad::Anchor_None && anchors[1] != RenderQuad::Anchor_Center)
				)
			) {

			if (!mDynamicAnchor.isValid())
				WE_MMemNew(mDynamicAnchor.ptrRef(), DynamicData());

				mDynamicAnchor->type = Dynamic_Anchor;
				mDynamicAnchor->values[0] = (float) anchors[0]; 
				mDynamicAnchor->values[1] = (float) anchors[1]; 
		}
	

		if (attrNamePos) {

			scannedCount = loadVector(tempStr, chunk, attrNamePos, tempVect, NULL);
			sizeSet = scannedCount > 0;

			if (scannedCount == 2) {

				enableTransform(true);
				setPosition2D(tempVect.el[0], tempVect.el[1]);
				
			} 

			if (attrNamePosType && chunk.getAttribute(attrNamePosType, tempStr)) {

				if (tempStr.equals(L"viewport")) {

					if (!mDynamicPos.isValid())
						WE_MMemNew(mDynamicPos.ptrRef(), DynamicData());

					mDynamicPos->type = Dynamic_ViewportPos;
					mDynamicPos->values[0] = tempVect.el[0]; 
					mDynamicPos->values[1] = tempVect.el[1]; 

					enableTransform(true);
				}
			}
		}

		if (attrNameOrient) {

			scannedCount = loadVector(tempStr, chunk, attrNameOrient, tempVect, NULL);
			sizeSet = scannedCount > 0;

			if (scannedCount == 2) {

				enableTransform(true);
				setOrientation2D(tempVect.el[0], tempVect.el[1], true);
			} 
		}
	}
}

RenderSprite::RenderSprite(bool _enableSkin, bool _enableColor, bool _enableTransform, const DimMap_2_3* pDimMap/*, Render2DScaling viewportScaling*/)
	: mQuad(true) {

	mDimMap = pDimMap;
	
	enableSkin(_enableSkin);

	if (mSkin.isValid())
		mSkin->enableColor(_enableColor);

	enableTransform(_enableTransform);
}


SceneTransform* RenderSprite::enableTransform(bool enable) {

	if (enable) {

		if (!mTransform.isValid()) {

			WE_MMemNew(mTransform.ptrRef(), SceneTransform());
			mTransform->identity();
		}

	} else {

		mTransform.destroy();
	}

	return mTransform;
}

RenderSprite::Skin* RenderSprite::enableSkin(bool enable) {

	if (enable) {

		if (!mSkin.isValid()) 
			WE_MMemNew(mSkin.ptrRef(), Skin());

	} else {

		mSkin.destroy();
	}

	return mSkin;
}

void RenderSprite::setOrientation2D(const float& orientX, const float& orientY, bool renormalize) {

	mTransform->row[mDimMap->mapY].el[mDimMap->mapX] = orientX;
	mTransform->row[mDimMap->mapY].el[mDimMap->mapY] = orientY;
	mTransform->row[mDimMap->mapY].el[mDimMap->mapNone] = 0.0f;

	if (renormalize) {

		mTransform->row[mDimMap->mapY].normalize();
	}
	
	mTransform->row[mDimMap->mapNone].el[mDimMap->mapX] = 0.0f;
	mTransform->row[mDimMap->mapNone].el[mDimMap->mapY] = 0.0f;
	mTransform->row[mDimMap->mapNone].el[mDimMap->mapNone] = 1.0f;

	mTransform->row[mDimMap->mapY].cross(mTransform->row[mDimMap->mapNone], mTransform->row[mDimMap->mapX]);
}

void RenderSprite::setPosition2D(const float& posX, const float& posY) {

	mTransform->position().el[mDimMap->mapX] = posX;
	mTransform->position().el[mDimMap->mapY] = posY;
}

float RenderSprite::getScaleForSize(const float& sizeX, RenderTexture* pTexture) {

	SoftPtr<RenderTexture> texture = pTexture? pTexture : mSkin->mTexture;

	if (texture.isValid()) {

		float scale = 1.0f;

		unsigned int w = 1;
		unsigned int h = 1;

		texture->getLoadSize(w, h);

		return sizeX / (float) w;
	}

	return 1.0f;
}

void RenderSprite::setScale(const float& scaleX, RenderQuad::AnchorEnum anchorX, RenderQuad::AnchorEnum anchorY, RenderTexture* pTexture) {

	SoftPtr<RenderTexture> texture = pTexture? pTexture : mSkin->mTexture;

	unsigned int w = 1;
	unsigned int h = 1;

	if (texture.isValid())
		texture->getLoadSize(w, h);

	setSize(scaleX * (float) w, scaleX * (float) h, anchorX, anchorY);
}

void RenderSprite::setSize(const float& sizeX, RenderQuad::AnchorEnum anchorX, RenderQuad::AnchorEnum anchorY, RenderTexture* pTexture) {

	SoftPtr<RenderTexture> texture = pTexture? pTexture : mSkin->mTexture;

	unsigned int w = 1;
	unsigned int h = 1;

	if (texture.isValid())
		texture->getLoadSize(w, h);

	setSize(sizeX, sizeX * (float) h / (float) w, anchorX, anchorY);
}

void RenderSprite::getSize(float& sizeX, float& sizeY) {

	mQuad.getSize(sizeX, sizeY, *mDimMap);
}

void RenderSprite::setSize(const float& scaleX, const float& scaleY, RenderQuad::AnchorEnum anchorX, RenderQuad::AnchorEnum anchorY) {

	mQuad.setupBySize(scaleX, scaleY, anchorX, anchorY, *mDimMap);
}

void RenderSprite::setOffset3D(const float& offset) {

	mQuad.setOffset(offset, *mDimMap);
}

void RenderSprite::setMaterial(Renderer& renderer, const RenderColor* pColor, const RenderBlendTypeEnum& blendType, const RenderTextureFilterEnum& texFilter) {

	if (texFilter != RTF_None) {

		renderer.setTextureFilter(RTS_Min, texFilter, 0);
		renderer.setTextureFilter(RTS_Mag, texFilter, 0);
		renderer.setTextureFilter(RTS_Mip, texFilter, 0);
	}

	if (pColor && pColor->alpha != 1.0f) {

		renderer.setRenderState(RS_EnableBlend, 1);

		switch (blendType) {

			case Blend_Alpha: {

				renderer.setRenderState(RS_BlendFactorSrc, RBF_SrcAlpha);
				renderer.setRenderState(RS_BlendFactorDest, RBF_InvSrcAlpha);

			} break;

			case Blend_Multiply: {

				renderer.setRenderState(RS_BlendFactorSrc, RBF_Zero);
				renderer.setRenderState(RS_BlendFactorDest, RBF_SrcColor);

			} break;

			case Blend_Add: {

				renderer.setRenderState(RS_BlendFactorSrc, RBF_SrcAlpha);
				renderer.setRenderState(RS_BlendFactorDest, RBF_One);

			} break;

			default: {
			} break;
		}

	} else {

		renderer.setRenderState(RS_EnableBlend, 0);
	}
}

bool RenderSprite::updateDynamicData2D(Renderer& renderer) {

	RenderViewportInfo viewportInfo;
	bool viewportInfoSet = false;

	if (renderer.wasReset()) {

		if (mDynamicSize.isValid()) {

			switch (mDynamicSize->type) {
				
				case Dynamic_ViewportScale: {

					if (!viewportInfoSet) 
						viewportInfoSet = renderer.getViewportInfo(viewportInfo);

					if (!viewportInfoSet) 
						return false;

					RenderQuad::AnchorEnum anchors[2] = { RenderQuad::Anchor_None, RenderQuad::Anchor_None };
					
					if (mDynamicAnchor.isValid()) {

						anchors[0] = (RenderQuad::AnchorEnum) (int) mDynamicAnchor->values[0];
						anchors[1] = (RenderQuad::AnchorEnum) (int) mDynamicAnchor->values[1];
					}
		
					setSize(mDynamicSize->values[0] * viewportInfo.width, anchors[0], anchors[1]);

				} break;
			}
		}

		if (mDynamicPos.isValid()) {

			switch (mDynamicPos->type) {
				
				case Dynamic_ViewportPos: {

					if (!viewportInfoSet) 
						viewportInfoSet = renderer.getViewportInfo(viewportInfo);

					if (!viewportInfoSet) 
						return false;

					setPosition2D(mDynamicPos->values[0] * viewportInfo.width, 
									mDynamicPos->values[1] * viewportInfo.height);

				} break;
			}
		}
	}

	return true;
}


void RenderSprite::render2D(Renderer& renderer, const SceneTransform* pTransform, 
							const RenderColor* _pColor, RenderTexture* pTexture, 
							const RenderBlendTypeEnum& blendType, RenderTextureFilterEnum& texFilter) {

	if (!updateDynamicData2D(renderer))
		return;
	
	const RenderColor* pColor = _pColor ? _pColor : mSkin->mColor;
	SoftPtr<RenderTexture> texture = pTexture ? pTexture : mSkin->mTexture;

	setMaterial(renderer, pColor, blendType, texture.isValid() ? texFilter : RTF_None);

	renderer.draw2D(mQuad, pTransform ? pTransform : mTransform, R2DS_None, pColor, texture, true);
}

void RenderSprite::render3D(Renderer& renderer, const SceneTransform* pTransform, 
							const RenderColor* _pColor, RenderTexture* pTexture, 
							const RenderBlendTypeEnum& blendType, RenderTextureFilterEnum& texFilter) {

	const RenderColor* pColor = _pColor ? _pColor : mSkin->mColor;
	SoftPtr<RenderTexture> texture = pTexture ? pTexture : mSkin->mTexture;

	setMaterial(renderer, pColor, blendType, texture.isValid() ? texFilter : RTF_None);

	renderer.setTransform(pTransform ? dref(pTransform) : mTransform, RT_World);
	renderer.draw(mQuad, pColor, texture);
}

}