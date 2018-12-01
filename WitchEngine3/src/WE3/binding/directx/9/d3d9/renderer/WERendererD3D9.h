#ifndef _WERendererD3D9_h
#define _WERendererD3D9_h

#include "includeRenderer.h"
//#include "../loadContext/WEAssetManager.h"
#include "../includeD3D9.h"
#include "WERendererD3D9Enums.h"
#include "WERendererD3D9Convert.h"
//#include "WERendererD3D9Buffers.h"
//#include "WERendererD3D9Material.h"
#include "WERendererD3D9DrawObject.h"
//#include "WERendererD3D9TypeManager.h"

//temp text drawing
#include "../../d3dx9/includeD3DX9.h"
#include "../../dxut9/includeDXUT9.h"

namespace WE {

	class DataChunk;

	#define RD3D9_APPLY_CALL inline 
	#define RD3D9_SET_CALL __fastcall 
	#define RD3D9_CHECKSET_CALL inline 

	class RendererD3D9 : public Renderer {
	protected:
		
		friend class RendererD3D9_Texture;
		friend class RendererD3D9_Material;
		friend class RendererD3D9_VertexBuffer;
		friend class RendererD3D9_IndexBuffer;
		friend class RendererD3D9_DrawObject;

		IDirect3DDevice9* mpDevice;
		D3DCAPS9 mCaps;
		D3DSURFACE_DESC mBackBufferDesc;
		bool mWasReset;
		float mViewportAspectRatio;
		float mViewportWidthScreenAdjusted;
		float mViewportHeightScreenAdjusted;

		RendererD3D9_ResourceManager mResourceManager;
		RendererD3D9_DrawObject mDrawObject;

		CoordSys mCoordSys;

		D3DMATRIX mTempMatrix;
		HRESULT mTempResult;

	public:

		struct Settings {

			int CapsOverrides_MaxVertexBlendMatrices;
			int CapsOverrides_MaxVertexBlendMatrixIndex;

			Settings();
			void reset();
			void load(DataChunk* pChunk);
		};

	protected:

		void releaseDevice();

		void initStartupValues();
		void syncDeviceToValues();
	
		void reset_Material();
		void apply_Material();

		void apply_Light(const RenderLight& light, int index);

		Settings mSettings;

		RenderPassEnum mCurrPass;

		int mTexStageCount;
		D3DSAMPLERSTATETYPE mTexSamplerNative[3];
		D3DTEXTUREFILTERTYPE mTexFilterNative[4];
		RenderTextureFilterEnum mTexFilter[3][3]; //[sampler][stage]
		RenderTextureFilterEnum mTexFilterMax[3]; //[sampler]
		bool mTexFilterOverrideEnabled[3][3]; //[sampler][stage]
		RenderTextureFilterEnum mTexFilterOverride[3][3]; //[sampler][stage]
		bool mTexWrap[3]; //stage

		RenderTextureOp mTexStageColorOp[3];
		RenderTextureArg mTexStageColorArg1[3];
		RenderTextureArg mTexStageColorArg2[3];

		D3DMATERIAL9 mRS_Material;

		bool mShadowingAlphaBlend;
		bool mShadowingStencilTest;
		D3DMATERIAL9 mShadowMaterial;

		D3DCOLOR mWhiteColor;

		Vector3 mShadowPlanarLightDir;
		Plane mShadowPlanarPlane;

		RenderStateValue mRS_EnableLighting;
		RenderStateValue mRS_EnableSpecular;
		RenderStateValue mRS_VertexBlendMatrixCount;
		RenderStateValue mRS_AmbientColor;
		//RenderStateValue mRS_FVF;
		RenderStateValue mRS_SoftwareVertexProcessing;
		RenderStateValue mRS_CullMode;
		RenderStateValue mRS_EnableBlend;
		RenderStateValue mRS_BlendFactorSrc;
		RenderStateValue mRS_BlendFactorDest;
		RenderStateFloatValue mRSF_DepthBias;
		RenderStateValue mRS_VertexBlendIndexed;
		RenderStateValue mRS_FillMode;

		RenderStateValue mRS_EnableShadowing;

		RenderStateValue mCurr_Shader;
		/*
		RendererD3D9_Material* mCurr_Material;
		RendererD3D9_VertexBuffer* mCurr_VertexBuffer; 
		RendererD3D9_IndexBuffer* mCurr_IndexBuffer; 
		RenderBufferPart* mCurr_VertexBufferPart; 
		RenderBufferPart*  mCurr_IndexBufferPart; 
		*/
		
		typedef WETL::CountedArray<bool, true, unsigned int, WETL::ResizeExact> LightEnables;
		typedef WETL::CountedArray<RenderLight, true, unsigned int, WETL::ResizeExact> Lights;

		LightEnables mLightEnables;
		Lights mLights;

	protected:

		//temp. text drawing
		SoftRef<ID3DXFont> mTextFont;
		SoftRef<ID3DXSprite> mTextSprite;
		HardPtr<CDXUTTextHelper> mTextHelper;

	protected:
				
		/*
		bool extractObjectId(const AssetTypeId& typeId, void* creationStruct, AssetId& objectId);
		Ref* createObject(AssetManager* pManager, const AssetTypeId& typeId, void* creationStruct, const AssetId& objectId);
		Ref* createObject_Texture(AssetManager* pManager, RendererD3D9_CreateTexture* creationStruct, const AssetId& objectId);
		Ref* createObject_Material(AssetManager* pManager, RendererD3D9_CreateMaterial* creationStruct, const AssetId& objectId);
		Ref* createObject_VertexBuffer(AssetManager* pManager, RendererD3D9_CreateVertexBuffer* creationStruct, const AssetId& objectId);
		Ref* createObject_IndexBuffer(AssetManager* pManager, RendererD3D9_CreateIndexBuffer* creationStruct, const AssetId& objectId);
		*/

		void switchPassState(RenderPassEnum currPass, RenderPassEnum newPass);
		void applyPassShadowing(bool start);



	public:

		static const TCHAR* kName;

		RendererD3D9(DataChunk* pSettingsChunk = NULL);
		virtual ~RendererD3D9();

		virtual const TCHAR* getName();

		void setDevice(IDirect3DDevice9* pDevice);
		IDirect3DDevice9* getDevice(); //RefCount is not Bumped so dont release

		void onLostDevice();

		virtual bool isReady();
		virtual void cleanObjectCache();

		RenderTexture* createTextureFromNative(IDirect3DTexture9* pNativeTex, unsigned int* pLoadWidth, unsigned int* pLoadHeight);

	public:

		virtual bool getViewportInfo(RenderViewportInfo& info);
		virtual float getViewportAspectRatio() { return mViewportAspectRatio; }
		virtual float getViewportWidth(bool screenAspectRatioAdjusted, float* pOutHeight = NULL);
		
		virtual unsigned int getTexStageCount();

		virtual const CoordSys& getCoordSys();
		
		virtual void setTransform(const Matrix4x3Base& trans, RenderTransformEnum type, int index = 0) ;
		virtual void setTransform(const Matrix4x4& trans, RenderTransformEnum type, int index = 0) ;

		virtual void toRenderStateValue(const RenderColor& color, RenderStateValue& value) ;

		virtual void setRenderStateMaterial();

		virtual void setRenderState(const RenderStateType& state, const RenderStateValue& value) ;
		virtual void setRenderStateF(const RenderStateType& state, const RenderStateFloatValue& value) ;
		virtual void setRenderStateCol(const RenderStateType& state, const RenderColor& value);
		
		virtual void setShader(RenderShader shader);

		virtual void setTextureFilter(RenderTextureSamplerEnum sampler, RenderTextureFilterEnum filter, int stage);
		virtual void setTextureFilterMax(RenderTextureSamplerEnum sampler, RenderTextureFilterEnum filter);

		virtual void setTextureFilterOverride(bool enabled, RenderTextureSamplerEnum sampler, RenderTextureFilterEnum filter, int stage);

		virtual void setTextureWrapping(bool enabled, int stage);

		virtual void setupTextureStageColor(const int& stage, const RenderTextureOp& op, const RenderTextureArg& arg1, const RenderTextureArg& arg2); 

		virtual void setLight(RenderLight* pLight, int index ) ;
		virtual void enableLight(bool enable, int index = 0, bool* pWasEnabled = NULL) ;

	public:

		virtual void processQuery(RenderVertexBlendingQuery& query) ;
		virtual void freeQuery(RenderVertexBlendingQuery& query) ;

		/*
		virtual void setMaterial(RenderMaterial* pMaterial);
		virtual void setVertexBuffer(RenderVertexBuffer* pVB, bool setPart);
		virtual void setIndexBuffer(RenderIndexBuffer* pIB, bool setPart);
		virtual void setVertexBufferPart(RenderBufferPart* pPart);
		virtual void setIndexBufferPart(RenderBufferPart* pPart);
		*/

		virtual void setTexture(RenderTexture* pTexture, int stage);

		virtual bool createVB(const RenderElementProfile& profile, RenderBufferUsage& usage, RenderUINT elementCount, RenderBuffer** ppBuffer);
		virtual void lockVB(RenderBuffer* pBuffer, RenderUINT offset, RenderUINT sizeBytes, void** ppData);
		virtual void unlockVB(RenderBuffer* pBuffer);

		virtual bool createIB(bool bit32, RenderBufferUsage& usage, RenderUINT bufferSizeBytes, RenderBuffer** ppBuffer);
		virtual void lockIB(RenderBuffer* pBuffer, RenderUINT offset, RenderUINT sizeBytes, void** ppData);
		virtual void unlockIB(RenderBuffer* pBuffer);

		virtual void setSourceVB(RenderUINT stream, RenderBuffer* pBuffer, RenderUINT stride);
		virtual void setSourceIB(RenderBuffer* pBuffer);
		/*
		virtual bool createMaterial(String& materialId, MaterialKit& kit, RenderMaterialOptions& options, 
										RenderMaterial** ppMaterial) ;

		virtual bool createVertexBuffer(const VertexProfile& profile, const RenderVertexBlendingQuery* pVertexBlending,
											RenderBufferUsage& usage, UINT vertexCount, RenderPool pool, 
											RenderVertexBuffer** ppBuffer, void* sourceData = NULL) ;

		virtual bool createIndexBuffer(const FaceProfile& profile, const VertexProfile& vertexProfile,
											const RenderVertexBlendingQuery* pVertexBlending, 
											RenderBufferUsage& usage, UINT indexCount, RenderPool pool, 
											RenderIndexBuffer** ppBuffer, void* sourceData = NULL, void* sourceVertexData = NULL, bool viewSpaceSorted = false) ;
		*/

	public:

		virtual void drawIndexed(RenderPrimitiveEnum primitive, RenderUINT indexBufferOffset, RenderUINT primitiveCount, RenderUINT vertexBufferOffset, RenderUINT vertexCount);
		virtual void draw(RenderPrimitiveEnum primitive, RenderUINT vertexBufferOffset, RenderUINT vertexCount);
	
	public:

		virtual RenderPassEnum nextPass(RenderPassEnum currPass = RP_Start);
		virtual RenderPassEnum getPass();

		virtual void setupShadowingMaterial(const RenderColor& shadowColor, bool alphaBlend, bool stencilTest);
		virtual void setupShadowingPlanar(const Vector3& lightDir, const Plane& plane);

	public:

		virtual void draw(const OBB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL) ;
		virtual void draw(const AAB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL, bool noDiag = false) ;
		virtual void draw(const Sphere& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL) ;
		virtual void draw(const Matrix3x3Base& matrix, const Vector& position, const float& size) ;
		virtual void draw(const WE::RigidMatrix4x3& matrix, const float& size) ;
		virtual void draw(const Triangle& triangle, const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL, 
							bool bumpDepthBias = false, bool wireFrame = false) ;
		virtual void draw(const TriangleEdgeNormals& triangle, float size, const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL, 
							bool bumpDepthBias = false) ;
		virtual void draw(const Point& from, const Point& to, const WE::Matrix4x3Base* pWorldMatrix, 
							const RenderColor* pColor = NULL) ;
		virtual void draw(const AAPlane& plane, float size, const WE::Matrix4x3Base* pWorldMatrix, 
							const RenderColor* pColor = NULL) ;
		virtual void draw(const Plane& plane, float size, const WE::Matrix4x3Base* pWorldMatrix, 
							const RenderColor* pColor = NULL) ;
		virtual void drawText(const TCHAR* text, int x, int y, const RenderColor* pColor = NULL);
		
		//uses the set world matrix
		virtual void draw(RenderQuad& quad, const RenderColor* pColor = NULL, RenderTexture* pTexture = NULL);

	public:

		virtual void draw2DQuad(const RenderQuad& quad, Render2DScaling viewportScaling, const RenderColor* pColor = NULL, RenderTexture* pTexture = NULL, bool fixCoordOffsets = true, unsigned int* indices = NULL);

	public:

		virtual bool wasReset();
		virtual void setWasReset(bool value);
		virtual bool begin();
		virtual void end();

	public:

		inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

		void RD3D9_APPLY_CALL apply_TexStageColorSetup(IDirect3DDevice9* pDevice, const RenderTextureOp& op, const RenderTextureArg& arg1, const RenderTextureArg& arg2, const int& stage) {

			MCheckD3DStmt(pDevice->SetTextureStageState(stage,  D3DTSS_COLOROP, RendererD3D9_convRenderTextureOp(op)));
			MCheckD3DStmt(pDevice->SetTextureStageState(stage, D3DTSS_COLORARG1, RendererD3D9_convRenderTextureArg(arg1)));
			MCheckD3DStmt(pDevice->SetTextureStageState(stage, D3DTSS_COLORARG2, RendererD3D9_convRenderTextureArg(arg2)));
		}

		void RD3D9_SET_CALL set_TexStageColorSetup(const RenderTextureOp& op, const RenderTextureArg& arg1, const RenderTextureArg& arg2, const int& stage) {

			assrt(stage <= 3);

			mTexStageColorOp[stage] = op;
			mTexStageColorArg1[stage] = arg1;
			mTexStageColorArg2[stage] = arg2;

			apply_TexStageColorSetup(mpDevice, op, arg1, arg2, stage);
		}

		void RD3D9_CHECKSET_CALL checkSet_TexStageColorSetup(const RenderTextureOp& op, const RenderTextureArg& arg1, const RenderTextureArg& arg2, const int& stage) {

			assrt(stage <= 3);

			if (mTexStageColorOp[stage] != op
					|| mTexStageColorArg1[stage] != arg1
					|| mTexStageColorArg2[stage] != arg2) {

				set_TexStageColorSetup(op, arg1, arg2, stage);
			}
		}

		void RD3D9_APPLY_CALL apply_TexWrap(IDirect3DDevice9* pDevice, const bool& value, const int& stage) {

			//hope retail version chooses next best if filter is not supported
			MWarnD3DStmt(pDevice->SetSamplerState(stage, D3DSAMP_ADDRESSU, value ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP));
			MWarnD3DStmt(pDevice->SetSamplerState(stage, D3DSAMP_ADDRESSV, value ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP));
			MWarnD3DStmt(pDevice->SetSamplerState(stage, D3DSAMP_ADDRESSW, value ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP));
		}

		void RD3D9_SET_CALL set_TexWrap(const bool& value, const int& stage) {

			mTexWrap[stage] = value;
			apply_TexWrap(mpDevice, value, stage);
		}

		void RD3D9_CHECKSET_CALL checkSet_TexWrap(const bool& value, const int& stage) {

			assrt(stage <= 3);

			if (mTexWrap[stage] != value) set_TexWrap(value, stage);
		}

		void RD3D9_APPLY_CALL apply_TexFilter(IDirect3DDevice9* pDevice, const RenderTextureSamplerEnum& sampler, const RenderTextureFilterEnum& value, const int& stage) {

			//hope retail version chooses next best if filter is not supported
			MWarnD3DStmt(pDevice->SetSamplerState(stage, mTexSamplerNative[sampler], mTexFilterNative[value]));
		}

		void RD3D9_SET_CALL set_TexFilter(const RenderTextureSamplerEnum& sampler, const RenderTextureFilterEnum& value, const int& stage) {

			if (mTexFilterOverrideEnabled[sampler][stage]) {

				mTexFilter[sampler][stage] = mTexFilterOverride[sampler][stage];

			} else {

				mTexFilter[sampler][stage] = value;
			}

			apply_TexFilter(mpDevice, sampler, mTexFilter[sampler][stage], stage);
		}

		void RD3D9_CHECKSET_CALL checkSet_TexFilter(const RenderTextureSamplerEnum& sampler, const RenderTextureFilterEnum& value, const int& stage) {

			assrt(stage <= 3);

			if (mTexFilter[sampler][stage] != value) set_TexFilter(sampler, value, stage);
		}


		void RD3D9_SET_CALL checkSet_TexFilters(const RenderTextureFilterEnum& min, const RenderTextureFilterEnum& mag, const RenderTextureFilterEnum& mip, const int& stage) {

			assrt(stage <= 3);

			if (mTexFilter[RTS_Min][stage] != min) {
				
				mTexFilter[RTS_Min][stage] = min;
				apply_TexFilter(mpDevice, RTS_Min, min, stage);
			}
			if (mTexFilter[RTS_Mag][stage] != mag) {
				
				mTexFilter[RTS_Mag][stage] = mag;
				apply_TexFilter(mpDevice, RTS_Mag, mag, stage);
			}
			if (mTexFilter[RTS_Mip][stage] != mip) {
				
				mTexFilter[RTS_Mip][stage] = mip;
				apply_TexFilter(mpDevice, RTS_Mip, mip, stage);
			}
		}

		void RD3D9_APPLY_CALL apply_RS_EnableLighting(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_LIGHTING, value));
		}

		void RD3D9_SET_CALL set_RS_EnableLighting(const RenderStateValue& value) {

			mRS_EnableLighting = value;
			apply_RS_EnableLighting(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_EnableLighting(const RenderStateValue& value) {

			if (mRS_EnableLighting != value) set_RS_EnableLighting(value);
		}


		void RD3D9_APPLY_CALL apply_RS_EnableSpecular(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			if (value <= mRS_EnableSpecular) {

				MCheckD3DStmt(pDevice->SetRenderState(D3DRS_SPECULARENABLE, value));
			} else {

				MCheckD3DStmt(pDevice->SetRenderState(D3DRS_SPECULARENABLE, mRS_EnableSpecular));
			}
		}

		void RD3D9_SET_CALL set_RS_EnableSpecular(const RenderStateValue& value) {

			mRS_EnableSpecular = value;
			apply_RS_EnableSpecular(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_EnableSpecular(const RenderStateValue& value) {

			if (mRS_EnableSpecular != value) set_RS_EnableSpecular(value);
		}


		void RD3D9_APPLY_CALL apply_RS_VertexBlendMatrixCount(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_VERTEXBLEND, RendererD3D9_vertexBlendMatrixCountToRenderState(value)));
		}

		void RD3D9_SET_CALL set_RS_VertexBlendMatrixCount(const RenderStateValue& value) {

			mRS_VertexBlendMatrixCount = value;
			apply_RS_VertexBlendMatrixCount(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_VertexBlendMatrixCount(const RenderStateValue& value) {

			if (mRS_VertexBlendMatrixCount != value) set_RS_VertexBlendMatrixCount(value);
		}


		void RD3D9_APPLY_CALL apply_RS_AmbientColor(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_AMBIENT, value));
		}

		void RD3D9_SET_CALL set_RS_AmbientColor(const RenderStateValue& value) {

			mRS_AmbientColor = value;
			apply_RS_AmbientColor(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_AmbientColor(const RenderStateValue& value) {

			if (mRS_AmbientColor != value) set_RS_AmbientColor(value);
		}


		/*
		void RD3D9_APPLY_CALL apply_RS_FVF(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetFVF(value));
		}

		void RD3D9_SET_CALL set_RS_FVF(const RenderStateValue& value) {

			mRS_FVF = value;
			apply_RS_FVF(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_FVF(const RenderStateValue& value) {

			if (mRS_FVF != value) set_RS_FVF(value);
		}
		*/

		void RD3D9_APPLY_CALL apply_RS_SoftwareVertexProcessing(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetSoftwareVertexProcessing(value));
		}

		void RD3D9_SET_CALL set_RS_SoftwareVertexProcessing(const RenderStateValue& value) {

			mRS_SoftwareVertexProcessing = value;
			apply_RS_SoftwareVertexProcessing(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_SoftwareVertexProcessing(const RenderStateValue& value) {

			if (mRS_SoftwareVertexProcessing != value) set_RS_SoftwareVertexProcessing(value);
		}


		void RD3D9_APPLY_CALL apply_RS_CullMode(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_CULLMODE, value));
		}

		void RD3D9_SET_CALL set_RS_CullMode(const RenderStateValue& value) {

			mRS_CullMode = value;
			apply_RS_CullMode(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_CullMode(const RenderStateValue& value) {

			if (mRS_CullMode != value) set_RS_CullMode(value);
		}


		void RD3D9_APPLY_CALL apply_RS_EnableBlend(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, value));
		}

		void RD3D9_SET_CALL set_RS_EnableBlend(const RenderStateValue& value) {

			mRS_EnableBlend = value;
			apply_RS_EnableBlend(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_EnableBlend(const RenderStateValue& value) {

			if (mRS_EnableBlend != value) set_RS_EnableBlend(value);
		}


		void RD3D9_APPLY_CALL apply_RS_BlendFactorSrc(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_SRCBLEND, value));
		}

		void RD3D9_SET_CALL set_RS_BlendFactorSrc(const RenderStateValue& value) {

			mRS_BlendFactorSrc = value;
			apply_RS_BlendFactorSrc(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_BlendFactorSrc(const RenderStateValue& value) {

			if (mRS_BlendFactorSrc != value) set_RS_BlendFactorSrc(value);
		}


		void RD3D9_APPLY_CALL apply_RS_BlendFactorDest(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_DESTBLEND, value));
		}

		void RD3D9_SET_CALL set_RS_BlendFactorDest(const RenderStateValue& value) {

			mRS_BlendFactorDest = value;
			apply_RS_BlendFactorDest(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_BlendFactorDest(const RenderStateValue& value) {

			if (mRS_BlendFactorDest != value) set_RS_BlendFactorDest(value);
		}


		void RD3D9_APPLY_CALL apply_RSF_DepthBias(IDirect3DDevice9* pDevice, const RenderStateFloatValue& value) {

			//http://softwarecommunity.intel.com/articles/eng/1688.htm

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_DEPTHBIAS, F2DW(value)));

			if (value == 0.0f) {

				float slopeScaledValue = 0.0f;

				MCheckD3DStmt(pDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(slopeScaledValue)));
				MCheckD3DStmt(pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));

			} else {

				float slopeScaledValue = 1.0f;

				MCheckD3DStmt(pDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(slopeScaledValue)));
				MCheckD3DStmt(pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
			}
		}

		void RD3D9_SET_CALL set_RSF_DepthBias(const RenderStateFloatValue& value) {

			mRSF_DepthBias = value;
			apply_RSF_DepthBias(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RSF_DepthBias(const RenderStateFloatValue& value) {

			if (mRSF_DepthBias != value) set_RSF_DepthBias(value);
		}


		void RD3D9_APPLY_CALL apply_RS_VertexBlendIndexed(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, value));
		}

		void RD3D9_SET_CALL set_RS_VertexBlendIndexed(const RenderStateValue& value) {

			mRS_VertexBlendIndexed = value;
			apply_RS_VertexBlendIndexed(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_VertexBlendIndexed(const RenderStateValue& value) {

			if (mRS_VertexBlendIndexed != value) set_RS_VertexBlendIndexed(value);
		}


		void RD3D9_APPLY_CALL apply_Shader(IDirect3DDevice9* pDevice, const RenderShader& value) {

			MCheckD3DStmt(pDevice->SetFVF(value));
		}

		void RD3D9_SET_CALL set_Curr_Shader(const RenderShader& value) {

			mCurr_Shader = value;
			apply_Shader(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_Curr_Shader(const RenderShader& value) {

			if (mCurr_Shader != value) set_Curr_Shader(value);
		}

		void RD3D9_APPLY_CALL apply_RS_FillMode(IDirect3DDevice9* pDevice, const RenderStateValue& value) {

			MCheckD3DStmt(pDevice->SetRenderState(D3DRS_FILLMODE , RendererD3D9_convertFillMode(value)));
		}

		void RD3D9_SET_CALL set_RS_FillMode(const RenderStateValue& value) {

			mRS_FillMode = value;
			apply_RS_FillMode(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_FillMode(const RenderStateValue& value) {

			if (mRS_FillMode != value) set_RS_FillMode(value);
		}

		void RD3D9_APPLY_CALL apply_EnableShadowing(IDirect3DDevice9* pDevice, const RenderStateValue& value) {
		}

		void RD3D9_SET_CALL set_RS_EnableShadowing(const RenderStateValue& value) {

			mRS_EnableShadowing = value;
			apply_EnableShadowing(mpDevice, value);
		}

		void RD3D9_CHECKSET_CALL checkSet_RS_EnableShadowing(const RenderStateValue& value) {

			if (mRS_EnableShadowing != value) set_RS_EnableShadowing(value);
		}
	};

	
}


#endif