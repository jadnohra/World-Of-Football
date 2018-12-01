#ifndef _WERendererD3D9Convert_h
#define _WERendererD3D9Convert_h

#include "../includeD3D9.h"
#include "includeRenderer.h"


namespace WE {

	#define RD3D9_CONVERT_CALL __fastcall 

	//void RD3D9_CONVERT_CALL RendererD3D9_convert(RenderPool& pool, D3DPOOL& d3d_pool);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const D3DMATRIX& src, Matrix4x3Base& m);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const Matrix4x3Base& src, D3DMATRIX& m);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const D3DMATRIX& src, Matrix4x4& m);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const Matrix4x4& src, D3DMATRIX& m);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderLight& from, D3DLIGHT9& to);
	//void RD3D9_CONVERT_CALL RendererD3D9_convert(TiXmlElement* pEl, RenderColor& to);
	//void RD3D9_CONVERT_CALL RendererD3D9_convertColor(TiXmlElement* pEl, float& r, float& g, float& b, float& a);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderVertexBlendingQuery* pVertexBlending, 
													bool& illegal, bool& needsSoftwareProcessing, bool& needsVertexBlending);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(RenderBufferUsage& usage, bool needsSoftwareProcessing, DWORD& d3d_usage);

	bool RD3D9_CONVERT_CALL RendererD3D9_hasAlpha(D3DFORMAT format);

	DWORD RD3D9_CONVERT_CALL RendererD3D9_convRenderTextureOp(const RenderTextureOp& op);
	DWORD RD3D9_CONVERT_CALL RendererD3D9_convRenderTextureArg(const RenderTextureArg& arg);


	D3DPRIMITIVETYPE RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderPrimitiveEnum& prim);
	DWORD RD3D9_CONVERT_CALL RendererD3D9_vertexBlendMatrixCountToRenderState(const RenderStateValue& count);
	DWORD RD3D9_CONVERT_CALL RendererD3D9_convertCull(const RenderStateValue& disable);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderColor& from, D3DCOLOR& to);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderColor& from, D3DCOLORVALUE& to);
	void RD3D9_CONVERT_CALL RendererD3D9_convert(const Vector3& from, D3DVECTOR& to);
	D3DFILLMODE RD3D9_CONVERT_CALL RendererD3D9_convertFillMode(const RenderStateValue& from);
	RenderStateValue RD3D9_CONVERT_CALL RendererD3D9_convertBlendFactor(const RenderStateValue& from);
	//D3DTEXTUREOP RD3D9_CONVERT_CALL RendererD3D9_convertBlendOp(const RenderBlendOpEnum& from);
	
}


#endif