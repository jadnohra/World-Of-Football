#include "WERenderMaterial.h"

#include "../../helper/load/WEMathDataLoadHelper.h"
#include "../../helper/load/WERenderDataLoadHelper.h"

#include "../texture/WERenderTextureSource.h"

namespace WE {

RenderMaterial::RenderMaterial() : mBitFlagBits(0), mDepthBias(0.0f) {
}	

RenderMaterial::~RenderMaterial() {
}

void RenderMaterial::unset(Renderer& renderer, RenderMaterialInstance* pInst) {

	if (mLightSwitcher.isValid()) {

		for (LightSwitcher::LightSwitches::Index i = 0; i < mLightSwitcher->lightSwitches.count; ++i) {

			LightSwitch& lightSwitch = mLightSwitcher->lightSwitches[i];

			renderer.enableLight(lightSwitch.state ? false : true, lightSwitch.index);
		}
	}
}

bool RenderMaterial::set(Renderer& renderer, RenderMaterialInstance* pInst) {

	assrt(mIsRenderReady == true);

	bool ret = false;

	if (mLightSwitcher.isValid()) {

		bool wasEnabled;

		for (LightSwitcher::LightSwitches::Index i = 0; i < mLightSwitcher->lightSwitches.count; ++i) {

			LightSwitch& lightSwitch = mLightSwitcher->lightSwitches[i];

			renderer.enableLight(lightSwitch.state ? true : false, lightSwitch.index, &wasEnabled);

			if ((lightSwitch.state && !wasEnabled)
				|| (!lightSwitch.state && wasEnabled)) {

				ret = true;
			}
		}
	}

	renderer.setRenderState(RS_EnableLighting, mEnableLighting);
	renderer.setRenderState(RS_DisableCull, mDisableCull);
	renderer.setRenderState(RS_EnableSpecular, mEnableSpecular);
	renderer.setRenderState(RS_EnableBlend, mEnableBlend);
	renderer.setRenderStateF(RSF_DepthBias, mDepthBias);
	renderer.setRenderState(RS_FillMode, mFillMode);

	//renderer.setRenderState(RS_EnableShadows, mEnableShadows);

	renderer.setRenderStateCol(RSCOL_MaterialAmbient, mAmbient);
	renderer.setRenderStateCol(RSCOL_MaterialDiffuse, mDiffuse);
	renderer.setRenderStateCol(RSCOL_MaterialEmissive, mEmissive);

	
	if (mEnableBlend) {

		renderer.setRenderState(RS_BlendFactorSrc, RBF_SrcAlpha);
		renderer.setRenderState(RS_BlendFactorDest, RBF_InvSrcAlpha);
	}
	

	if (mEnableSpecular) {

		renderer.setRenderStateCol(RSCOL_MaterialSpecular, mSpecular);
		renderer.setRenderStateF(RSF_MaterialSpecularPower, mSpecularPower);
	}

	renderer.setRenderStateMaterial();

	if (pInst) {

		if (pInst->mTexture.isValid()) {

			renderer.setTextureFilter(RTS_Min, (RenderTextureFilterEnum) mTexFiltMin, 0);
			renderer.setTextureFilter(RTS_Mag, (RenderTextureFilterEnum) mTexFiltMag, 0);
			renderer.setTextureFilter(RTS_Mip, (RenderTextureFilterEnum) mTexFiltMip, 0);

			renderer.setTextureWrapping(mTexWrap ? true : false, 0);

			renderer.setTexture(pInst->mTexture.ptr(), 0);
			//renderer.setupTextureStageColor(0, RTOP_Modulate, RTArg_Texture, RTArg_Current); 
		}

		/*
		if (pInst->mSpecularMap.isValid() && pInst->mTexture.isValid()) {

			renderer.setTexture(pInst->mSpecularMap.ptr(), 1);
			renderer.setupTextureStageColor(1, RTOP_Add, RTArg_Texture, RTArg_Current); 

		} else {

			renderer.setupTextureStageColor(1, RTOP_Disable, RTArg_Texture, RTArg_Current); 
		}
		*/
		
	}

	return ret;
}

void RenderMaterial::destroyInstance(RenderMaterialInstance*& pInst) {

	MMemDelete(pInst);
}

RenderMaterialInstance* RenderMaterial::createInstance(RenderLoadContext& renderLoadContext, int qualityLevel) {

	if ((mSource.isValid() == false) || (isRenderReady() == false)) {

		return NULL;
	}

	//Parametrizable stuff goes here

	RenderMaterialInstance* pInst = NULL;

	MMemNew(pInst, RenderMaterialInstance());
	
	RefWrap<DataChunk> child;
	BufferString tempStr;
	
	child.set(mSource->getFirstChild(), false);

	unsigned int texCount = 0;
	unsigned int maxTexCount = renderLoadContext.renderer().getTexStageCount();

	while (child.isValid()) {

		if (child->equals(L"texture")) {
			
			RefWrap<DataChunk> texChunk(child->getChild(L"source"), false);

			if (texChunk.isValid()) {

				pInst->mTexture.set(renderLoadContext.textureSource().getFromDataChunk(texChunk.dref(), renderLoadContext), false);
				++texCount;
			}

		} /*else if (child->equals(L"specularMap")) {
			
			RefWrap<DataChunk> texChunk(child->getChild(L"source"), false);

			if (texChunk.isValid()) {

				pInst->mSpecularMap.set(renderLoadContext.textureSource().getFromDataChunk(texChunk.dref(), renderLoadContext), false);
				++texCount;
			}
		} */

		if (texCount >= maxTexCount)
			break;

		toNextSibling(child);
	}

	pInst->mMaterial.set(this, true);

	return pInst;
}

bool RenderMaterial::isRenderReady() {

	return mIsRenderReady;
}

bool RenderMaterial::prepareForRenderer(RenderLoadContext& renderLoadContext) {

	if (isRenderReady()) {

		return true;
	}

	setDefaults(&renderLoadContext.renderer());

	if (mSource.isValid() == false) {

		return false;
	}

	RefWrap<DataChunk> child;
	
	BufferString tempStr;
	bool bTemp;
	int iTemp;
	
	child.set(mSource->getFirstChild(), false);
	
	//None parametrizable stuff goes here

	while (child.isValid()) {
		
		if (child->equals(L"lightSwitch")) {

			if (!mLightSwitcher.isValid()) {

				WE_MMemNew(mLightSwitcher.ptrRef(), LightSwitcher());
			}

			LightSwitch& lightSwitch = mLightSwitcher->lightSwitches.addOne();

			iTemp = 0;

			if (child->scanAttribute(tempStr, L"enable", L"%d", &iTemp)) {

				lightSwitch.index = iTemp;
				lightSwitch.state = 1;

			} else if (child->scanAttribute(tempStr, L"disable", L"%d", &iTemp)) {

				lightSwitch.index = iTemp;
				lightSwitch.state = 0;

			} else {

				assrt(false);
				mLightSwitcher->lightSwitches.removeSwapWithLast(mLightSwitcher->lightSwitches.count - 1, true);
			}

		} else if (child->equals(L"lighting")) {

			bTemp = true;
			child->scanAttribute(tempStr, L"enable", bTemp);
			mEnableLighting = bTemp;

		} else if (child->equals(L"shadows")) {

			bTemp = false;
			child->scanAttribute(tempStr, L"enable", bTemp);
			mEnableShadows = bTemp;

		} else if (child->equals(L"alpha")) {

			bTemp = false;
			child->scanAttribute(tempStr, L"enable", bTemp);
			mEnableBlend = bTemp;

		} else if (child->equals(L"culling")) {

			bTemp = false;
			child->scanAttribute(tempStr, L"disable", bTemp);
			mDisableCull = bTemp;

		} else if (child->equals(L"depthBias")) {

			child->scanAttribute(tempStr, L"value", L"%f", &mDepthBias);

		} else if (child->equals(L"fillMode")) {

			if (child->getAttribute(L"value", tempStr)) {

				if (tempStr.equals(L"wireFrame")) {

					mFillMode = RFM_WireFrame;
				} else {

					mFillMode = RFM_Solid;
				}
				
			}
			
		} else if (child->equals(L"ambient")) {

			RenderDataLoadHelper::extract(tempStr, child.dref(), mAmbient, true);

		} else if (child->equals(L"emmisive")) {

			RenderDataLoadHelper::extract(tempStr, child.dref(), mEmissive, true);

		} else if (child->equals(L"diffuse")) {

			RenderDataLoadHelper::extract(tempStr, child.dref(), mDiffuse, true);

		} else if (child->equals(L"specular")) {

			RenderDataLoadHelper::extract(tempStr, child.dref(), mSpecular, true);

			bTemp = true;
			child->scanAttribute(tempStr, L"enable", bTemp);
			mEnableSpecular = bTemp;

			child->scanAttribute(tempStr, L"power", L"%f", &mSpecularPower);

		} else if (child->equals(L"texture")) {

			RenderTextureFilterEnum filters[3];

			RenderDataLoadHelper::loadRenderTextureFilters(tempStr, child.dref(), filters);

			mTexFiltMin = filters[RTS_Min];
			mTexFiltMag = filters[RTS_Mag];
			mTexFiltMip = filters[RTS_Mip];

			bool doWrap = true;

			child->scanAttribute(tempStr, L"wrap", doWrap);
			mTexWrap = doWrap ? 1 : 0;
		} 

		toNextSibling(child);
	}

	mIsRenderReady = true;

	return true;
}

void RenderMaterial::setDefaults(Renderer* pRenderer) {

	bool saveRenderReady = mIsRenderReady;

	mAmbient = RenderColor::kBlack;
	mDiffuse = RenderColor::kBlack;
	mSpecular = RenderColor::kBlack;
	mEmissive = RenderColor::kBlack;

	mSpecularPower = 10.0f;
	mBitFlagBits = 0;

	mIsRenderReady = saveRenderReady;
}

bool RenderMaterial::init(DataChunk& resolvedSource) {

	if (mSource.isValid()) {

		assrt(false);
		return false;
	}

	mSource.set(&resolvedSource, true);

	return true;
}

}