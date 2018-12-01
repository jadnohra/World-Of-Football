#include "WERendererD3D9.h"

#include "WERendererD3D9Convert.h"
#include "WERendererD3D9Enums.h"
#include "../../../WEDirect3D.h"
#include "../../../../../WEDataSource.h"

namespace WE {

const TCHAR* RendererD3D9::kName = L"WERendererD3D9";

RendererD3D9::Settings::Settings() {

	reset();
}

void RendererD3D9::Settings::reset() {

	CapsOverrides_MaxVertexBlendMatrices = -1;
	CapsOverrides_MaxVertexBlendMatrixIndex = -1;
}

void RendererD3D9::Settings::load(DataChunk* pChunk) {

	reset();

	if (pChunk != NULL) {

		BufferString tempStr;

		SoftRef<DataChunk> child = pChunk->getFirstChild();

		while (child.isValid()) {

			if (child->equals(L"CapsOverrides")) {

				child->scanAttribute(tempStr, L"MaxVertexBlendMatrices", L"%d", &CapsOverrides_MaxVertexBlendMatrices);
				child->scanAttribute(tempStr, L"MaxVertexBlendMatrixIndex", L"%d", &CapsOverrides_MaxVertexBlendMatrixIndex);
			}

			toNextSibling(child);
		}
	}
}

RendererD3D9::RendererD3D9(DataChunk* pSettingsChunk) {

	mSettings.load(pSettingsChunk);

	mpDevice = NULL;
	mWasReset = false;

	memset(&mCaps, 0, sizeof(D3DCAPS9));
	memset(&mBackBufferDesc, 0, sizeof(D3DSURFACE_DESC));
	mViewportAspectRatio = 0.0f;

	Direct3D::getCoordSys(mCoordSys);
	
	mDrawObject.setRenderer(this);

	mCurrPass = RP_Start;

	mResourceManager.init(10);
	mResourceManager.add(&mDrawObject);

	
	mTexStageCount = 3;
	mTexSamplerNative[RTS_Min] = D3DSAMP_MINFILTER;
	mTexSamplerNative[RTS_Mag] = D3DSAMP_MAGFILTER;
	mTexSamplerNative[RTS_Mip] = D3DSAMP_MIPFILTER;
	mTexFilterNative[RTF_None] = D3DTEXF_NONE;
	mTexFilterNative[RTF_Point] = D3DTEXF_POINT;
	mTexFilterNative[RTF_Linear] = D3DTEXF_LINEAR;
	mTexFilterNative[RTF_Anisotropic] = D3DTEXF_ANISOTROPIC;

	mShadowingAlphaBlend = true;
	mShadowingStencilTest = true;

	memset(&mShadowMaterial, 0, sizeof(D3DMATERIAL9));
	mShadowMaterial.Diffuse.a = 0.75f;
	mShadowMaterial.Ambient.r = 0.15f;
	mShadowMaterial.Ambient.g = 0.15f;
	mShadowMaterial.Ambient.b = 0.15f;

	mShadowPlanarLightDir.set(0.2f, 0.8f, 0.3f);
	mShadowPlanarPlane.init(Point(0.0f, 10.0f, 0.0f), Vector(0.0f, 1.0f, 0.0f), true);

	toRenderStateValue(RenderColor(1.0f, 1.0f, 1.0f), mWhiteColor);
	
	mViewportWidthScreenAdjusted = 0.0f;
	mViewportHeightScreenAdjusted = 0.0f;

	initStartupValues();

	//CoordSys semRenderer;
	//getSemantics(semRenderer);
	//setDefaultConverter(semRenderer);
}

RendererD3D9::~RendererD3D9() {

	releaseDevice();
}

const TCHAR* RendererD3D9::getName() {

	return kName;
}

bool RendererD3D9::isReady() {

	return (mpDevice != NULL);
}

void RendererD3D9::cleanObjectCache() {
}

bool RendererD3D9::getViewportInfo(RenderViewportInfo& info) {

	info.width = mBackBufferDesc.Width;
	info.height = mBackBufferDesc.Height;

	return true;
}

float RendererD3D9::getViewportWidth(bool screenAspectRatioAdjusted, float* pOutHeight) {

	if (screenAspectRatioAdjusted) {

		if (pOutHeight)
			*pOutHeight = mViewportHeightScreenAdjusted;

		return mViewportWidthScreenAdjusted;
	} 

	if (pOutHeight)
		*pOutHeight = (float) mBackBufferDesc.Height;
	
	return (float) mBackBufferDesc.Width;
}

const CoordSys& RendererD3D9::getCoordSys() {

	return mCoordSys;
}

unsigned int RendererD3D9::getTexStageCount() {

	return tmin(mCaps.MaxTextureBlendStages, mCaps.MaxSimultaneousTextures);
}

void RendererD3D9::setTransform(const Matrix4x3Base& trans, RenderTransformEnum type, int index ) {

	RendererD3D9_convert(trans, mTempMatrix);
	
	switch (type) {
		case RT_World:			
			MCheckD3DStmt(mpDevice->SetTransform(D3DTS_WORLDMATRIX(index), &mTempMatrix));
			break;
		case RT_View:
			assrt(mCurrPass == RP_Normal);
			MCheckD3DStmt(mpDevice->SetTransform(D3DTS_VIEW, &mTempMatrix));
			break;
		case RT_Projection:
			MCheckD3DStmt(mpDevice->SetTransform(D3DTS_PROJECTION, &mTempMatrix));
			break;
	}
	
}

void RendererD3D9::setTransform(const Matrix4x4& trans, RenderTransformEnum type, int index ) {

	RendererD3D9_convert(trans, mTempMatrix);
	
	switch (type) {
		case RT_World:			
			MCheckD3DStmt(mpDevice->SetTransform(D3DTS_WORLDMATRIX(index), &mTempMatrix));
			break;
		case RT_View:
			assrt(mCurrPass == RP_Normal);
			MCheckD3DStmt(mpDevice->SetTransform(D3DTS_VIEW, &mTempMatrix));
			break;
		case RT_Projection:
			MCheckD3DStmt(mpDevice->SetTransform(D3DTS_PROJECTION, &mTempMatrix));
			break;
	}
}

void RendererD3D9::initStartupValues() {

	RenderStateValue ambientColor;
	toRenderStateValue(RenderColor(1.0f, 1.0f, 1.0f), ambientColor);

	mRS_EnableSpecular = false;
	mRS_AmbientColor = ambientColor;
	mRS_CullMode = RendererD3D9_convertCull(false);
	mRSF_DepthBias = 0.0f;
	mRS_BlendFactorDest = RendererD3D9_convertBlendFactor(RBF_InvSrcAlpha);
	mRS_EnableBlend = false;
	mRS_EnableLighting = false;
	mRS_EnableSpecular = false;
	mRS_FillMode = 
	//mRS_FVF = 0;
	mRS_VertexBlendIndexed = false;
	mRS_SoftwareVertexProcessing = false;
	mRS_BlendFactorSrc = RendererD3D9_convertBlendFactor(RBF_SrcAlpha);
	mRS_VertexBlendMatrixCount = 1;

	mTexFilterMax[RTS_Min] = RTF_Anisotropic;
	mTexFilterMax[RTS_Mag] = RTF_Anisotropic;
	mTexFilterMax[RTS_Mip] = RTF_Anisotropic;

	for (int stage = 0; stage < mTexStageCount; ++stage) {

		mTexFilter[RTS_Min][stage] =  RTF_Point;
		mTexFilter[RTS_Mag][stage] = RTF_Point;
		mTexFilter[RTS_Mip][stage] = RTF_Point;
		mTexWrap[stage] = false;

		mTexFilterOverrideEnabled[RTS_Min][stage] = false;
		mTexFilterOverrideEnabled[RTS_Mag][stage] = false;
		mTexFilterOverrideEnabled[RTS_Mip][stage] = false;

		mTexStageColorOp[stage] = stage == 0 ? RTOP_Modulate : RTOP_Disable;
		mTexStageColorArg1[stage] = RTArg_Texture;
		mTexStageColorArg2[stage] = RTArg_Current;
	}

	mRS_EnableShadowing = true;

	mCurr_Shader = 0;

	/*
	mCurr_Material = NULL;
	mCurr_VertexBuffer = NULL; 
	mCurr_IndexBuffer = NULL; 
	mCurr_VertexBufferPart = NULL; 
	mCurr_IndexBufferPart = NULL; 
	*/
	reset_Material();

	/*
	for (int i = 0; i < 8; ++i) {

		mLightSet[i] = false;
	}
	*/
}

void RendererD3D9::syncDeviceToValues() {

	if (mpDevice == NULL) {

		return;
	}
	
	apply_RS_EnableSpecular(mpDevice, mRS_EnableSpecular);
	apply_RS_AmbientColor(mpDevice, mRS_AmbientColor);
	apply_RS_CullMode(mpDevice, mRS_CullMode);
	apply_RSF_DepthBias(mpDevice, mRSF_DepthBias);
	apply_RS_BlendFactorDest(mpDevice, mRS_BlendFactorDest);
	apply_RS_EnableBlend(mpDevice, mRS_EnableBlend);
	apply_RS_EnableLighting(mpDevice, mRS_EnableLighting);
	apply_RS_EnableSpecular(mpDevice, mRS_EnableSpecular);
	//apply_RS_FVF(mpDevice, mRS_FVF);
	apply_RS_VertexBlendIndexed(mpDevice, mRS_VertexBlendIndexed);
	apply_RS_SoftwareVertexProcessing(mpDevice, mRS_SoftwareVertexProcessing);
	apply_RS_BlendFactorSrc(mpDevice, mRS_BlendFactorSrc);
	apply_RS_VertexBlendMatrixCount(mpDevice, mRS_VertexBlendMatrixCount);

	for (int stage = 0; stage < mTexStageCount; ++stage) {

		apply_TexFilter(mpDevice, RTS_Min, mTexFilter[RTS_Min][stage], stage);
		apply_TexFilter(mpDevice, RTS_Mag, mTexFilter[RTS_Mag][stage], stage);
		apply_TexFilter(mpDevice, RTS_Mip, mTexFilter[RTS_Mip][stage], stage);
		apply_TexWrap(mpDevice, mTexWrap[stage], stage);
	}

	mpDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 
											mCaps.MaxAnisotropy > 1 ? 1 + (mCaps.MaxAnisotropy / 2) : 1);
	

	apply_EnableShadowing(mpDevice, mRS_EnableShadowing);
	apply_Shader(mpDevice, mCurr_Shader);
	
	for (LightEnables::Index i = 0; i < mLights.count; ++i) {

		setLight(&mLights[i], i);
	}

	for (LightEnables::Index i = 0; i < mLightEnables.count; ++i) {

		enableLight(mLightEnables[i], i);
	}
		
	apply_Material();
}

void RendererD3D9::setShader(RenderShader shader) {

	checkSet_Curr_Shader(shader);
}

void RendererD3D9::toRenderStateValue(const RenderColor& from, RenderStateValue& _to) {

	D3DCOLOR& to = (D3DCOLOR&) _to;

	RendererD3D9_convert(from, to);
}

void RendererD3D9::reset_Material() {

	memset(&mRS_Material, 0, sizeof(D3DMATERIAL9));
}

void RendererD3D9::apply_Material() {

	MCheckD3DStmt(mpDevice->SetMaterial(&mRS_Material));
	reset_Material();
}

void RendererD3D9::setRenderStateMaterial() {

	//assrt(mCurrPass == RP_Normal);

	apply_Material();
}

void RendererD3D9::setupTextureStageColor(const int& stage, const RenderTextureOp& op, const RenderTextureArg& arg1, const RenderTextureArg& arg2) {

	checkSet_TexStageColorSetup(op, arg1, arg2, stage);
}

void RendererD3D9::setRenderStateF(const RenderStateType& state, const RenderStateFloatValue& value) {

#define M_setRenderState_case(stateType) case stateType: checkSet_##stateType(value); break

	switch (state) {
		case RSF_MaterialSpecularPower:
			mRS_Material.Power = value;
			break;
		M_setRenderState_case(RSF_DepthBias);
		default:
			assrt(false);
			return;
	}

}

void RendererD3D9::setRenderStateCol(const RenderStateType& state, const RenderColor& value) {

	switch (state) {
		case RSCOL_MaterialAmbient:
			RendererD3D9_convert(value, mRS_Material.Ambient);
			break;
		case RSCOL_MaterialDiffuse:
			RendererD3D9_convert(value, mRS_Material.Diffuse);
			break;
		case RSCOL_MaterialSpecular:
			RendererD3D9_convert(value, mRS_Material.Specular);
			break;
		case RSCOL_MaterialEmissive:
			RendererD3D9_convert(value, mRS_Material.Emissive);
			break;
		default:
			assrt(false);
			return;
	}

}

RenderPassEnum RendererD3D9::nextPass(RenderPassEnum currPass) {

	switch (currPass) {
		case RP_Start:
			mCurrPass = RP_Normal;
			break;
		case RP_Normal:
			mCurrPass = mRS_EnableShadowing ? RP_Shadowing : RP_End;
			break;
		case RP_Shadowing:
			mCurrPass = RP_End;
			break;
		default:
			assrt(false);
			break;	
	}

	switchPassState(currPass, mCurrPass);

	return mCurrPass;
}

RenderPassEnum RendererD3D9::getPass() {

	return mCurrPass;
}

void RendererD3D9::setRenderState(const RenderStateType& state, const RenderStateValue& value) {

#define M_setRenderState_case(stateType) case stateType: checkSet_##stateType(value); break

	switch (state) {
		M_setRenderState_case(RS_VertexBlendIndexed);
		M_setRenderState_case(RS_VertexBlendMatrixCount);
		M_setRenderState_case(RS_EnableBlend);
		M_setRenderState_case(RS_EnableSpecular);
		M_setRenderState_case(RS_AmbientColor);
		M_setRenderState_case(RS_EnableLighting);
		M_setRenderState_case(RS_EnableShadowing);
		M_setRenderState_case(RS_FillMode);
		case RS_DisableCull:
			checkSet_RS_CullMode(RendererD3D9_convertCull(value));
			break;
		case RS_BlendFactorSrc:
			checkSet_RS_BlendFactorSrc(RendererD3D9_convertBlendFactor(value));
			break;
		case RS_BlendFactorDest:
			checkSet_RS_BlendFactorDest(RendererD3D9_convertBlendFactor(value));
			break;
		default:
			assrt(false);
			return;
	}
}

/*
void RendererD3D9::setMaterial(RenderMaterial* pMaterial) {

	RendererD3D9_convert(pMaterial, mCurr_Material);

	if (mCurr_Material) {
		
		mCurr_Material->set(*this);
	} 
}

void RendererD3D9::setVertexBuffer(RenderVertexBuffer* pVB, bool setPart) {

	RendererD3D9_convert(pVB, mCurr_VertexBuffer);

	if (mCurr_VertexBuffer) {

		mCurr_VertexBuffer->set(*this);
		
		if (setPart) {

			mCurr_VertexBufferPart = &(mCurr_VertexBuffer->mPart);
		}
	}
}

void RendererD3D9::setIndexBuffer(RenderIndexBuffer* pIB, bool setPart) {

	RendererD3D9_convert(pIB, mCurr_IndexBuffer);

	if (mCurr_IndexBuffer) {

		mCurr_IndexBuffer->set(*this);

		if (setPart) {

			mCurr_IndexBufferPart = &(mCurr_IndexBuffer->mPart);
		}
	}
}

void RendererD3D9::setVertexBufferPart(RenderBufferPart* pPart) {

	mCurr_VertexBufferPart = pPart;
}

void RendererD3D9::setIndexBufferPart(RenderBufferPart* pPart) {

	mCurr_IndexBufferPart = pPart;
}
*/

void RendererD3D9::setTextureWrapping(bool enabled, int stage) {

	checkSet_TexWrap(enabled, stage);
}

void RendererD3D9::setTextureFilter(RenderTextureSamplerEnum sampler, RenderTextureFilterEnum filter, int stage) {

	checkSet_TexFilter(sampler, filter, stage);
}

void RendererD3D9::setTextureFilterMax(RenderTextureSamplerEnum sampler, RenderTextureFilterEnum filter) {

	mTexFilterMax[sampler] = filter;

	for (int stage = 0; stage < mTexStageCount; ++stage) {

		if (sampler == RTS_Mip) {

			set_TexFilter(sampler, RTF_None, stage);
		} else {
			
			set_TexFilter(sampler, RTF_Point, stage);
		}
	}
}

void RendererD3D9::setTextureFilterOverride(bool enabled, RenderTextureSamplerEnum sampler, RenderTextureFilterEnum filter, int stage) {

	mTexFilterOverrideEnabled[sampler][stage] = enabled;
	mTexFilterOverride[sampler][stage] = filter;
}

void RendererD3D9::apply_Light(const RenderLight& light, int index) {

	D3DLIGHT9 lt;

	RendererD3D9_convert(light, lt);
	MCheckD3DStmt(mpDevice->SetLight(index, &lt));
}


void RendererD3D9::setLight(RenderLight* pLight, int index) {

	if (pLight == NULL) {

		enableLight(false, index);

	} else {

		if (pLight) 
			apply_Light(*pLight, index);
	}
}

void RendererD3D9::enableLight(bool enable, int index, bool* pWasEnabled) {

	bool wasAdded = false;

	while (index + 1 > mLightEnables.count) {

		mLightEnables.addOne();
		wasAdded = true;
	}

	if (wasAdded) {

		BOOL wasEnabled = false;

		mpDevice->GetLightEnable(index, &wasEnabled);

		mLightEnables[index] = wasEnabled;
	}


	if (pWasEnabled) {

		*pWasEnabled = mLightEnables[index];
	}

	if (mLightEnables[index] != enable) {

		mLightEnables[index] = enable;
		mpDevice->LightEnable(index, enable);
	}
}


void RendererD3D9::releaseDevice() {

	if (mpDevice) {

		mpDevice->Release();
	}
}

IDirect3DDevice9* RendererD3D9::getDevice() {

	return mpDevice;
}

void RendererD3D9::setDevice(IDirect3DDevice9* pDevice) {

	bool softReset = false;

	if (mpDevice) {

		softReset = (mpDevice == pDevice);

		releaseDevice();
	}

	mpDevice = pDevice;
	mWasReset = true;

	if (mpDevice) {

		mpDevice->AddRef();

		memset(&mCaps, 0, sizeof(D3DCAPS9));
		MCheckD3DStmt(mpDevice->GetDeviceCaps(&mCaps));

		IDirect3DSurface9* pBackBuffer = NULL;
		MCheckD3DStmt(mpDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer));

		if (pBackBuffer) {

			ZeroMemory( &mBackBufferDesc, sizeof(D3DSURFACE_DESC) );
			pBackBuffer->GetDesc(&mBackBufferDesc);

			mViewportAspectRatio = (float) mBackBufferDesc.Width / (float) mBackBufferDesc.Height;

			{
				D3DDISPLAYMODE displayMode;

				MCheckD3DStmt(mpDevice->GetDisplayMode(0, &displayMode));

				float displayAspect = (float) displayMode.Width / (float) displayMode.Height;

				mViewportWidthScreenAdjusted = mBackBufferDesc.Height * displayAspect;
				mViewportHeightScreenAdjusted = mBackBufferDesc.Width / displayAspect;
			}

			pBackBuffer->Release();
		}


		mResourceManager.onDeviceState(mpDevice, RD3D9_DS_Reset);
		syncDeviceToValues();
	} 

	{

		if (softReset) {

			if (mTextFont.isValid()) {

				mTextFont->OnResetDevice();
			}

		} else {

			mTextFont.destroy();

			if (mpDevice) {

				D3DXCreateFont( mpDevice, 12, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
								OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
								L"Arial", mTextFont.pptr() );
			}
		}

		mTextSprite.destroy();

		if (mpDevice) {

			D3DXCreateSprite( mpDevice, mTextSprite.pptr() );
		}

		if (mpDevice) {
	
			mTextHelper.destroy();
			MMemNew(mTextHelper.ptrRef(), CDXUTTextHelper(mTextFont.ptr(), mTextSprite, 12));

		} else {

			mTextHelper.destroy();
		}
	}
}


void RendererD3D9::onLostDevice() {

	//releaseCachedRenderObjects();
	mResourceManager.onDeviceState(mpDevice, RD3D9_DS_Lost);

	{
	
		if (mTextFont.isValid()) {

			mTextFont->OnLostDevice();
		}

		mTextSprite.destroy();
		mTextHelper.destroy();
	}
}

bool RendererD3D9::wasReset() {

	return mWasReset;
}

void RendererD3D9::setWasReset(bool value) {

	mWasReset = value;
}

bool RendererD3D9::begin() {

	return true;
}

void RendererD3D9::end() {

	//assrt(mStackRS.blockCount() == 0);
	//assrt(mStackRS.savedRenderStateCount() == 0);

	mWasReset = false;
}

void RendererD3D9::processQuery(RenderVertexBlendingQuery& query) {

	query.outIsSupported = false;

	if (query.inTotalMatrixCount > 256) {

		query.outVertexBlend = false;
		return;
	}

	if (query.inVertexBlend) {

		DWORD Caps_MaxVertexBlendMatrices = mSettings.CapsOverrides_MaxVertexBlendMatrices >= 0 ? mSettings.CapsOverrides_MaxVertexBlendMatrices : mCaps.MaxVertexBlendMatrices;
		DWORD Caps_MaxVertexBlendMatrixIndex = mSettings.CapsOverrides_MaxVertexBlendMatrixIndex >= 0 ? mSettings.CapsOverrides_MaxVertexBlendMatrixIndex : mCaps.MaxVertexBlendMatrixIndex;

		int capsMaxVertexBlendMatrices = safeSizeCast<DWORD, int>(Caps_MaxVertexBlendMatrices);
		int capsMaxMatrixIndexCount = safeSizeCast<DWORD, int>((Caps_MaxVertexBlendMatrixIndex + 1) / (query.inHasNormals ? 2 : 1));
		
		if (capsMaxVertexBlendMatrices == 0) {

			//normally we could use software emulation
			query.outIsSupported = false;
			return;
		}

		query.outIsSupported = true;
		query.outVertexBlend = true;
		query.outMaxBlendMatrixCount = MMin(query.inMaxBlendMatrixCount, capsMaxVertexBlendMatrices);

		if (query.inUseIndexed) {

			if (query.inTotalMatrixCount <= capsMaxMatrixIndexCount) {

				//indexed, no split
				query.outUseIndexed = true;
				query.outIndexedSplit = false;

			} else {

				if (query.inAllowIndexedSplit && capsMaxMatrixIndexCount >= query.inIndexedSplitMinMatrixCount) {

					//hardware indexed, split
					query.outUseIndexed = true;
					query.outIndexedSplit = true;
					query.outIndexedSplitMaxMatrixCount = capsMaxMatrixIndexCount;

				} else {

					//none-indexed
					query.outUseIndexed = false;
				}
			}

		} else {

			//none-indexed
			query.outUseIndexed = false;
		}

	} else {

		query.outIsSupported = true;
		query.outVertexBlend = false;
	}

	/*
	if (query.inVertexBlend) {

		query.pRenderer = this;

		if (mCaps.MaxVertexBlendMatrices < 4) {

			//software
			query.outVertexBlend = true;
			query.outUseIndexed = true;
			query.outMaxBlendMatrixCount = query.inMaxBlendMatrixCount;
			query.reserved = (void*) Res_UseSoftware;

			assrt(false);

		} else {

			int capsMaxVertexBlendMatrices = safeSizeCast<DWORD, int>(mCaps.MaxVertexBlendMatrices);
			int capsMaxMatrixIndexCount = safeSizeCast<DWORD, int>((mCaps.MaxVertexBlendMatrixIndex + 1) / (query.inHasNormals ? 2 : 1));

			if (query.inTotalMatrixCount <= capsMaxMatrixIndexCount) {

				//hardware indexed, no split
				query.outVertexBlend = true;
				query.outUseIndexed = true;
				query.outIndexedSplit = false;
				query.outMaxBlendMatrixCount = MMin(query.inMaxBlendMatrixCount, capsMaxVertexBlendMatrices);

				query.reserved = (void*) Res_UseHardware;

			} else {

				if (query.inAllowIndexedSplit && capsMaxMatrixIndexCount >= query.inIndexedSplitMinMatrixCount) {

					//hardware indexed, split
					query.outVertexBlend = true;
					query.outUseIndexed = true;
					query.outIndexedSplit = true;
					query.outIndexedSplitMaxMatrixCount = capsMaxMatrixIndexCount;
					query.outMaxBlendMatrixCount = MMin(query.inMaxBlendMatrixCount, capsMaxVertexBlendMatrices);

					query.reserved = (void*) Res_UseHardware;

				} else {

					//hardware none indexed
					query.outVertexBlend = true;
					query.outUseIndexed = false;
					query.outMaxBlendMatrixCount = MMin(query.inMaxBlendMatrixCount, capsMaxVertexBlendMatrices);

					query.reserved = (void*) Res_UseHardware;
				}
			}
		}

	} else {
		query.outVertexBlend = false;
	}
	*/

}

void RendererD3D9::freeQuery(RenderVertexBlendingQuery& query) {

	query.pRenderer = NULL;
}

void RendererD3D9::drawText(const TCHAR* text, int x, int y, const RenderColor* pColor) {

	if (mTextHelper.isValid()) {

		mTextHelper->Begin();

		mTextHelper->SetInsertionPos(x, y);

		if (pColor) {

			mTextHelper->SetForegroundColor(D3DXCOLOR( pColor->el[RC_Red], pColor->el[RC_Green], pColor->el[RC_Blue], pColor->alpha ));
		}

		mTextHelper->DrawTextLine(text);

		mTextHelper->End();

	} else {

		assrt(false);
	}
}

/*
void RendererD3D9::drawIndexed() {

	//assert (mCurr_VertexBuffer != NULL);
	
	/*
	XX
	MCheckD3DStmt(
		mpDevice->DrawIndexedPrimitive(mCurr_IndexBuffer->mPrimType, 0, 0, mCurr_VertexBufferPart->primitiveCount, 
										mCurr_IndexBufferPart->startIndex, mCurr_IndexBufferPart->primitiveCount)
	);
}
*/

/*
void RendererD3D9::draw() {

	//assert (mCurr_VertexBuffer != NULL);
	
	XX
	MCheckD3DStmt(
		mpDevice->DrawPrimitive(D3DPT_TRIANGLELIST, mCurr_VertexBufferPart->startIndex, mCurr_VertexBufferPart->primitiveCount)
	);
}
*/

/*
void RendererD3D9::draw(RenderVertexBuffer* pVB, RenderBufferPart* pVBPart) {

	RendererD3D9_VertexBuffer& vb = RendererD3D9_convert(pVB);

	vb.set(*this);

	MCheckD3DStmt(mpDevice->DrawPrimitive(D3DPT_TRIANGLELIST, pVBPart->startIndex, pVBPart->primitiveCount));

	if (mCurr_VertexBuffer) mCurr_VertexBuffer->set(*this);
}

void RendererD3D9::drawIndexed(RenderIndexBuffer* pIB, RenderBufferPart* pIBPart) {

	RendererD3D9_IndexBuffer& ib = RendererD3D9_convert(pIB);

	if (pIBPart == NULL) {

		pIBPart = &(ib.mPart);
	}


	ib.set(*this);

	MCheckD3DStmt(mpDevice->DrawIndexedPrimitive(ib.mPrimType, 0, 0, mCurr_VertexBufferPart->primitiveCount, 
													pIBPart->startIndex, pIBPart->primitiveCount));

	if (mCurr_IndexBuffer) mCurr_IndexBuffer->set(*this);
}

void RendererD3D9::drawIndexed(RenderIndexBuffer* pIB, RenderVertexBuffer* pVB, RenderBufferPart* pIBPart, RenderBufferPart* pVBPart) {

	RendererD3D9_VertexBuffer& vb = RendererD3D9_convert(pVB);
	RendererD3D9_IndexBuffer& ib = RendererD3D9_convert(pIB);

	if (pVBPart == NULL) {

		pVBPart = &(vb.mPart);
	}
	if (pIBPart == NULL) {

		pIBPart = &(ib.mPart);
	}


	vb.set(*this);
	ib.set(*this);

	MCheckD3DStmt(mpDevice->DrawIndexedPrimitive(ib.mPrimType, 0, 0, pVBPart->primitiveCount, 
													pIBPart->startIndex, pIBPart->primitiveCount));

	if (mCurr_VertexBuffer) mCurr_VertexBuffer->set(*this);
	if (mCurr_IndexBuffer) mCurr_IndexBuffer->set(*this);
}
*/

/*
void RendererD3D9::drawIndexed() {

	assert (mRO_VertexBuffer != NULL);
	//assert (mRO_IndexBuffer != NULL);

	MCheckD3DStmt(mpDevice->DrawIndexedPrimitive(mRO_IndexBuffer->mPrimType, 0, 0, mRO_VertexBufferPart->primitiveCount, 
		mRO_IndexBufferPart->startIndex, mRO_IndexBufferPart->primitiveCount));
}

void RendererD3D9::draw() {

	assert (mRO_VertexBuffer != NULL);
	//assert (mRO_IndexBuffer != NULL);

	MCheckD3DStmt(mpDevice->DrawPrimitive(D3DPT_TRIANGLELIST, mRO_VertexBufferPart->startIndex, mRO_VertexBufferPart->primitiveCount));
}

void RendererD3D9::draw(RenderVertexBuffer* pVB, RenderBufferPart* pVBPart) {

	RendererD3D9_VertexBuffer& vb = RendererD3D9_convert(pVB);

	if (pVBPart == NULL) {

		pVBPart = &(vb.mPart);
	}

	bool setVB = (mRO_VertexBuffer != pVB);

	if (setVB) vb.set(*this, true, false);

	MCheckD3DStmt(mpDevice->DrawPrimitive(D3DPT_TRIANGLELIST, pVBPart->startIndex, pVBPart->primitiveCount));

	if (setVB && mRO_VertexBuffer) mRO_VertexBuffer->set(*this, true, false);
}

void RendererD3D9::drawIndexed(RenderIndexBuffer* pIB, RenderVertexBuffer* pVB, RenderBufferPart* pIBPart, RenderBufferPart* pVBPart) {

	RendererD3D9_VertexBuffer& vb = RendererD3D9_convert(pVB);
	RendererD3D9_IndexBuffer& ib = RendererD3D9_convert(pIB);

	if (pVBPart == NULL) {

		pVBPart = &(vb.mPart);
	}
	if (pIBPart == NULL) {

		pIBPart = &(ib.mPart);
	}


	bool setVB = (mRO_VertexBuffer != pVB);
	bool setIB = (mRO_IndexBuffer != pIB);

	if (setVB) vb.set(*this, false, false);
	if (setIB) ib.set(*this, false, false);

	MCheckD3DStmt(mpDevice->DrawIndexedPrimitive(ib.mPrimType, 0, 0, pVBPart->primitiveCount, 
									pIBPart->startIndex, pIBPart->primitiveCount));

	if (setVB && mRO_VertexBuffer) mRO_VertexBuffer->set(*this, false, false);
	if (setIB && mRO_IndexBuffer) mRO_IndexBuffer->set(*this, false, false);
}
*/

}