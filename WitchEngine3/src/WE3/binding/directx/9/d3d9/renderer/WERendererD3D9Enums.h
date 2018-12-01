#ifndef _WERendererD3D9Enums_h
#define _WERendererD3D9Enums_h

#include "includeRenderer.h"
#include "../includeD3D9.h"


namespace WE {

#ifdef _DEBUG
	#define MCheckD3DStmt(statement) mTempResult = statement; assrt(mTempResult == D3D_OK)
	#define MWarnD3DStmt(statement) statement
#else 
	#define MCheckD3DStmt(statement) statement
	#define MWarnD3DStmt(statement) statement
#endif


	enum RendererD3D9_DeviceState {

		RD3D9_DS_Lost = 0, RD3D9_DS_Reset, 
	};

	enum RendererD3D9_RenderStateType {

		RD3D9_RS_FVF = _RS_ReservedLast, RD3D9_RS_SoftwareProcessing,  RD3D9_RS_IndexedVertexBlending,
		RD3D9_RS_CullMode, RD3D9_RS_EnableAlpha, RD3D9_RS_SrcBlend, RD3D9_RS_DestBlend, RD3D9_RS_DepthBias, 

		//RD3D9_RO_Texture, 
		
		//RD3D9_NRO_Texture, RD3D9_NRO_Material, 

		_RD3D9_RS_ReservedForceSize = 0x7fffffff
	};	

	/*
	enum RendererD3D9_RenderObjectType {

		RD3D9_RO_Texture = _RO_ReservedLast,

		_RD3D9_RO_ReservedLast, _RD3D9_RO_ReservedForceSize = 0x7fffffff
	};	

	enum RendererD3D9_NativeRenderObjectType {

		RD3D9_NRO_Texture = _RD3D9_RO_ReservedLast, RD3D9_NRO_Material, 

		_RD3D9_NRO_ReservedForceSize = 0x7fffffff
	};
	*/

	enum RendererD3D9_RenderStateValueEnum {

		RD3D9_RSV_CullNone = D3DCULL_NONE, RD3D9_RSV_CullClock = D3DCULL_CW, RD3D9_RSV_CullCounterClock = D3DCULL_CCW, 
		
		RD3D9_RSV_SamplerFilter_None = D3DTEXF_NONE, RD3D9_RSV_SamplerFilter_Point = D3DTEXF_POINT,
		RD3D9_RSV_SamplerFilter_Linear = D3DTEXF_LINEAR, RD3D9_RSV_SamplerFilter_Anisotropic = D3DTEXF_ANISOTROPIC,
		
		_RD3D9_RSV_ReservedForceSize = 0x7fffffff
	};

	enum RendererD3D9_RenderVertexBlendingQuery_Reserved {

		Res_UseSoftware = 0, Res_UseHardware = 1
	};

	/*
	enum RendererD3D9_SharedObjectType {

		SOT_Texture = 0, SOT_Material, SOT_VertexBuffer, SOT_IndexBuffer, SOT_TypeCount
	};
	*/
}

#endif