#ifndef _WERenderer_h
#define _WERenderer_h

#include "WERenderObjects.h"
#include "WERenderElementProfile.h"
#include "../WEMath.h"
#include "../WERef.h"
#include "../WETL.h"
#include "../WEString.h"
#include "../WECoordSys.h"


namespace WE {

	class Renderer;

	typedef unsigned long RenderStateType;
	typedef unsigned long RenderStateValue;
	typedef float RenderStateFloatValue;

	typedef RenderStateValue RenderShader;

	enum RenderTransformEnum {

		RT_World = 0, RT_View, RT_Projection
	};

	enum RenderStateEnum {

		RS_AmbientColor = 0, 
		RSCOL_MaterialAmbient, RSCOL_MaterialDiffuse, RSCOL_MaterialSpecular, RSF_MaterialSpecularPower, 
		RSCOL_MaterialEmissive,

		RS_EnableLighting, RS_EnableSpecular, RS_EnableBlend, RS_DisableCull,
		RS_FillMode, RS_EnableShadowing,
		
		RS_BlendFactorSrc, RS_BlendFactorDest, 
		RSF_DepthBias, 

		RS_VertexBlendMatrixCount, RS_VertexBlendIndexed, 

		_RS_ReservedLast, _RS_ReservedForceSize = 0x7fffffff
	};

	
	enum RenderBufferEnum {

		RB_VertexBuffer = 0, RB_IndexBuffer
	};

	enum RenderTextureSamplerEnum {

		RTS_Min = 0, RTS_Mag, RTS_Mip
	};

	enum RenderTextureFilterEnum {

		RTF_None = 0, RTF_Point, RTF_Linear, RTF_Anisotropic
	};

	enum RenderTextureOp {

		RTOP_None = 0, RTOP_Disable, RTOP_Modulate, RTOP_Add
	};

	enum RenderTextureArg {

		RTArg_None = 0, RTArg_Texture, RTArg_Diffuse, RTArg_Current
	};

	enum RenderPrimitiveEnum {

		RP_PointList = 0, RP_LineList, RP_TriangleList
	};

	enum RenderFillModeEnum {

		RFM_Solid = 0, RFM_WireFrame, 
	};

	enum RenderBlendFactorEnum {

		RBF_SrcAlpha = 0, RBF_InvSrcAlpha, RBF_Zero, RBF_One, RBF_SrcColor, RBF_DestColor
	};

	struct RenderBufferUsage {

		bool dynamic;
		bool writeOnly;

		RenderBufferUsage(bool isDynamic = false, bool isWriteOnly = true);
		bool equals(const RenderBufferUsage& comp) const;
	};

	enum RenderPassEnum {

		RP_Start = -1, RP_Normal = 0, RP_Shadowing, RP_End
	};

	enum Render2DScaling {

		R2DS_None = -1, R2DS_Width, R2DS_Height, R2DS_WidthHeight
	};

	/*
	enum RenderPool {
		RP_Managed = 0
	};
	*/

	enum RenderColorElement {

		RC_Red = 0, RC_Green = 1, RC_Blue = 2
	};

	struct RenderColor : Vector {

		float alpha;

		RenderColor();
		RenderColor(float r, float g, float b, float alpha = 1.0f);
		RenderColor(int r, int g, int b, int alpha = 255);
		RenderColor(int scale, int alpha = 255);

		void set(float r, float g, float b, float alpha = 1.0f);
		void set(int r, int g, int b, int alpha = 255);
		void set(float scale, float alpha = 1.0f);
		void set(int scale, int alpha = 255);

		inline void set2(float r, float g, float b, float alpha) { set(r, g, b, alpha); }

		inline float getR() { return el[RC_Red]; }
		inline float getG() { return el[RC_Green]; }
		inline float getB() { return el[RC_Blue]; }
		inline float getAlpha() { return alpha; }

		void toColorComplement();

		static const RenderColor kBlack;
		static const RenderColor kWhite;
		static const RenderColor kRed;
		static const RenderColor kGreen;
		static const RenderColor kBlue;
		static const RenderColor kYellow;
		static const RenderColor kMagenta;
		static const RenderColor kBluish1;
		static const RenderColor kHotPink;
		static const RenderColor kDeepPink;
		static const RenderColor kLightPink;
	};

	enum RenderLightType {

		RL_None = -1, RL_Directional, RL_Point
	};

	struct RenderLight : RenderObject {

		RenderLightType type;

		RenderColor diffuse;
		RenderColor specular;
		RenderColor ambient;
		
		Vector3 direction;

		//point only
		Vector3 position;
		float range;
		float attenuation;

		inline void setType(int type) { this->type = (RenderLightType) type; }

		inline RenderColor* script_getDiffuse() { return &diffuse; } 
		inline RenderColor* script_getSpecular() { return &specular; } 
		inline RenderColor* script_getAmbient() { return &ambient; } 

		inline Vector3* script_getDirection() { return &direction; } 
		inline Vector3* script_getPosition() { return &position; } 
		
	};

	
	class RenderVertexBlendingQuery {
	public:
		Renderer* pRenderer;
		void* reserved;

	public:

		bool inHasNormals;

		bool inVertexBlend;
		bool inUseIndexed;
		int inTotalMatrixCount; 
		int inMaxBlendMatrixCount; 
		bool inAllowIndexedSplit;
		int inIndexedSplitMinMatrixCount;

		bool outIsSupported;
		bool outVertexBlend;
		bool outUseIndexed;
		int outMaxBlendMatrixCount;
		bool outIndexedSplit;
		int outIndexedSplitMaxMatrixCount;
	
	public:

		RenderVertexBlendingQuery();
		~RenderVertexBlendingQuery();
	};

	
	struct RenderMaterialOptions {

		bool dynamic;	//used for later, if dynamic will not be shared probably

		RenderMaterialOptions();
	};

	struct RenderViewportInfo {

		int width;
		int height;
		
		float getAspectRatio();
	};

	class Renderer : public Ref {
	public:

		Renderer();
		virtual ~Renderer();

		virtual const TCHAR* getName() = 0;

		virtual bool isReady() = 0;
		virtual void cleanObjectCache() = 0;

	//public:

	//	CoordSysConv* pConverter;
	//	void setConverter(CoordSysConv* pConvert);
		
	public:

		virtual bool getViewportInfo(RenderViewportInfo& info) = 0;
		virtual float getViewportAspectRatio() = 0; //width / height
		virtual float getViewportWidth(bool screenAspectRatioAdjusted = false, float* pOutHeight = NULL) = 0;

		virtual unsigned int getTexStageCount() = 0;

		virtual const CoordSys& getCoordSys() = 0;

		virtual void setTransform(const Matrix4x3Base& trans, RenderTransformEnum type, int index = 0) = 0;
		virtual void setTransform(const Matrix4x4& trans, RenderTransformEnum type, int index = 0) = 0;

		virtual void toRenderStateValue(const RenderColor& color, RenderStateValue& value) = 0;
		
		//this is for now a hack for representing Direct3D materials with simple render states
		//after render states are set using setRenderState, setRenderStateMaterial should be called
		virtual void setRenderStateMaterial() = 0;

		virtual void setRenderState(const RenderStateType& state, const RenderStateValue& value) = 0;
		virtual void setRenderStateF(const RenderStateType& state, const RenderStateFloatValue& value) = 0;
		virtual void setRenderStateCol(const RenderStateType& state, const RenderColor& value) = 0;

		virtual void setShader(RenderShader shader) = 0;
		
		virtual void setTextureFilter(RenderTextureSamplerEnum sampler, RenderTextureFilterEnum filter, int stage) = 0;
		virtual void setTextureWrapping(bool enabled, int stage) = 0;

		virtual void setTextureFilterOverride(bool enabled, RenderTextureSamplerEnum sampler, RenderTextureFilterEnum filter, int stage) = 0;

		virtual void setupTextureStageColor(const int& stage, const RenderTextureOp& op, const RenderTextureArg& arg1, const RenderTextureArg& arg2) = 0; 

		virtual void setLight(RenderLight* pLight, int index = 0) = 0;
		virtual void enableLight(bool enable, int index = 0, bool* pWasEnabled = NULL) = 0;

	public:

		virtual void processQuery(RenderVertexBlendingQuery& query) = 0;
		virtual void freeQuery(RenderVertexBlendingQuery& query) = 0;

		/*
		virtual void setMaterial(RenderMaterial* pMaterial) = 0;
		virtual void setVertexBuffer(RenderVertexBuffer* pVB, bool setPart) = 0;
		virtual void setIndexBuffer(RenderIndexBuffer* pIB, bool setPart) = 0;
		virtual void setVertexBufferPart(RenderBufferPart* pPart) = 0;
		virtual void setIndexBufferPart(RenderBufferPart* pPart) = 0;
		*/

		virtual void setTexture(RenderTexture* pTexture, int stage) = 0;

		virtual bool createVB(const RenderElementProfile& profile, RenderBufferUsage& usage, RenderUINT elementCount, RenderBuffer** ppBuffer) = 0;
		virtual void lockVB(RenderBuffer* pBuffer, RenderUINT offset, RenderUINT sizeBytes, void** ppData) = 0;
		virtual void unlockVB(RenderBuffer* pBuffer) = 0;

		virtual bool createIB(bool bit32, RenderBufferUsage& usage, RenderUINT bufferSizeBytes, RenderBuffer** ppBuffer) = 0;
		virtual void lockIB(RenderBuffer* pBuffer, RenderUINT offset, RenderUINT sizeBytes, void** ppData) = 0;
		virtual void unlockIB(RenderBuffer* pBuffer) = 0;
		

		virtual void setSourceVB(RenderUINT stream, RenderBuffer* pBuffer, RenderUINT stride) = 0;
		virtual void setSourceIB(RenderBuffer* pBuffer) = 0;

		/*
		virtual bool createMaterial(String& materialId, MaterialKit& kit, RenderMaterialOptions& options, 
										RenderMaterial** ppMaterial) = 0;

		virtual bool createVertexBuffer(const VertexProfile& profile, const RenderVertexBlendingQuery* pVertexBlending,
											RenderBufferUsage& usage, UINT vertexCount, RenderPool pool, 
											RenderVertexBuffer** ppBuffer, void* sourceData = NULL) = 0;

		virtual bool createIndexBuffer(const FaceProfile& profile, const VertexProfile& vertexProfile,
											const RenderVertexBlendingQuery* pVertexBlending, 
											RenderBufferUsage& usage, UINT indexCount, RenderPool pool, 
											RenderIndexBuffer** ppBuffer, void* sourceData = NULL, void* sourceVertexData = NULL, bool viewSpaceSorted = false) = 0;
		*/

	public:

		virtual void drawIndexed(RenderPrimitiveEnum primitive, RenderUINT indexBufferOffset, RenderUINT primitiveCount, RenderUINT vertexBufferOffset, RenderUINT vertexCount) = 0;
		virtual void draw(RenderPrimitiveEnum primitive, RenderUINT vertexBufferOffset, RenderUINT vertexCount) = 0;
		
	public:

		virtual RenderPassEnum nextPass(RenderPassEnum currPass = RP_Start) = 0;
		virtual RenderPassEnum getPass() = 0;

		virtual void setupShadowingMaterial(const RenderColor& shadowColor, bool alphaBlend, bool stencilTest) = 0;
		virtual void setupShadowingPlanar(const Vector3& lightDir, const Plane& plane) = 0;

	public:

		virtual void draw(const OBB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL) = 0;
		virtual void draw(const AAB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL, bool noDiag = false) = 0;
		virtual void draw(const Sphere& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL) = 0;
		virtual void draw(const Matrix3x3Base& matrix, const Vector& position, const float& size) = 0;
		virtual void draw(const WE::RigidMatrix4x3& matrix, const float& size) = 0;
		virtual void draw(const Triangle& triangle, const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL, 
							bool bumpDepthBias = false, bool wireFrame = false) = 0;
		virtual void draw(const TriangleEdgeNormals& triangle, float size, const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL, 
							bool bumpDepthBias = false) = 0;
		virtual void draw(const Point& from, const Point& to, const WE::Matrix4x3Base* pWorldMatrix, 
							const RenderColor* pColor = NULL) = 0;
		virtual void draw(const AAPlane& plane, float size, const WE::Matrix4x3Base* pWorldMatrix, 
							const RenderColor* pColor = NULL) = 0;
		virtual void draw(const Plane& plane, float size, const WE::Matrix4x3Base* pWorldMatrix, 
							const RenderColor* pColor = NULL) = 0;
		virtual void drawText(const TCHAR* text, int x, int y, const RenderColor* pColor = NULL) = 0;

		//uses the set world matrix
		virtual void draw(RenderQuad& quad, const RenderColor* pColor = NULL, RenderTexture* pTexture = NULL) = 0;

	public:

		/*
			for 2D there is a fixed coord sys with 
				x -> right
				y -> down
			origin being at top, left
		*/
		

		//will not be affected by 2D Transforms
		virtual void draw2DQuad(const RenderQuad& quad, Render2DScaling viewportScaling, const RenderColor* pColor = NULL, RenderTexture* pTexture = NULL, bool fixCoordOffsets = true, unsigned int* indices = NULL) = 0;

		//setting pTrans to NULL is allowed (similar to identity but faster)
		virtual void setViewTransform2D(const Matrix4x3Base* pTrans, bool flipY);

		//will be affected by 2D Transforms
		virtual void draw2D(const RenderQuad& quad, const Matrix4x3Base* pWorldTrans, Render2DScaling viewportScaling, const RenderColor* pColor = NULL, RenderTexture* pTexture = NULL, bool fixCoordOffsets = true);

	public:

		RenderStateValue toRenderStateValue(const RenderColor& color);
		void drawVol(VolumeStruct& vol, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL);
		
		void enableQueuedVols(bool enable);
		void queueVol(VolumeStruct& vol, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor = NULL);
		void drawQueuedVols();

	public:

		virtual bool wasReset() = 0;
		virtual void setWasReset(bool value) = 0;
		virtual bool begin() = 0;
		virtual void end() = 0;


	protected:

		//void setDefaultConverter(CoordSys& semRenderer);

	protected:

		bool mIsValidViewTransform2D;
		Matrix4x3Base mViewTransform2D;
		bool mTransform2DFlipY;

		struct QueuedVolume {

			Volume volume;
			bool hasMatrix;
			Matrix4x3Base matrix;
			bool hasColor;
			RenderColor color;
		};

		typedef WETL::CountedPtrArrayEx<QueuedVolume, unsigned int> QueuedVolumes;
		HardPtr<QueuedVolumes> mQueuedVolumes;
	};
	
};

#endif