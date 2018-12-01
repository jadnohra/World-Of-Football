#include "WERendererD3D9Convert.h"

#include "WERendererD3D9Enums.h"

namespace WE {


void RD3D9_CONVERT_CALL RendererD3D9_convert(const D3DMATRIX& src, Matrix4x3Base& m) {

	m.m11 = src._11; m.m12 = src._12; m.m13 = src._13; 
	m.m21 = src._21; m.m22 = src._22; m.m23 = src._23; 
	m.m31 = src._31; m.m32 = src._32; m.m33 = src._33;
	m.tx = src._41; m.ty = src._42; m.tz = src._43;
}

void RD3D9_CONVERT_CALL RendererD3D9_convert(const Matrix4x3Base& src, D3DMATRIX& m) {

	m._11 = src.m11; m._12 = src.m12; m._13 = src.m13; m._14 = 0.0f;
	m._21 = src.m21; m._22 = src.m22; m._23 = src.m23; m._24 = 0.0f;
	m._31 = src.m31; m._32 = src.m32; m._33 = src.m33; m._34 = 0.0f;
	m._41 = src.tx; m._42 = src.ty; m._43 = src.tz; m._44 = 1.0f;
}

void RD3D9_CONVERT_CALL RendererD3D9_convert(const D3DMATRIX& src, Matrix4x4& m) {

	m.m11 = src._11; m.m12 = src._12; m.m13 = src._13; m.el_14 = src._14;
	m.m21 = src._21; m.m22 = src._22; m.m23 = src._23; m.el_24 = src._24; 
	m.m31 = src._31; m.m32 = src._32; m.m33 = src._33; m.el_34 = src._34;
	m.m41 = src._41; m.m42 = src._42; m.m43 = src._43; m.el_44 = src._44;
}

void RD3D9_CONVERT_CALL RendererD3D9_convert(const Matrix4x4& src, D3DMATRIX& m) {

	m._11 = src.m11; m._12 = src.m12; m._13 = src.m13; m._14 = src.el_14;
	m._21 = src.m21; m._22 = src.m22; m._23 = src.m23; m._24 = src.el_24;
	m._31 = src.m31; m._32 = src.m32; m._33 = src.m33; m._34 = src.el_34;
	m._41 = src.m41; m._42 = src.m42; m._43 = src.m43; m._44 = src.el_44;
}



void RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderLight& from, D3DLIGHT9& to) {

	ZeroMemory(&to, sizeof(D3DLIGHT9) );

	switch (from.type) {

		case RL_Point: 
			to.Type = D3DLIGHT_POINT;
		break;

		default:
			to.Type = D3DLIGHT_DIRECTIONAL;
		break;
	}
	

	RendererD3D9_convert(from.diffuse, to.Diffuse);
	RendererD3D9_convert(from.specular, to.Specular);
	RendererD3D9_convert(from.ambient, to.Ambient);
	
	if (to.Type == D3DLIGHT_DIRECTIONAL) {

		RendererD3D9_convert(from.direction, to.Direction);
	}

	if (to.Type == D3DLIGHT_POINT) {

		RendererD3D9_convert(from.position, to.Position);
		to.Range = from.range;
		to.Attenuation1 = from.attenuation;

		to.Attenuation0 = 0.0f;
		to.Attenuation2 = 0.0f;
	}
}

/*
void RD3D9_CONVERT_CALL RendererD3D9_convertColor(TiXmlElement* pEl, float& r, float& g, float& b, float& a) {

	if (pEl == NULL) {
	}

	const char* val;
	
	if((val = pEl->Attribute("intensity")) || (val = pEl->Attribute("value"))) {

		float v = 0.0f;

		sscanf(val, "%f", &v);
		
		r = v;
		g = v;
		b = v;

		a = 0.0f;

	} else if((val = pEl->Attribute("rgb"))) {

		sscanf(val, "%f,%f,%f", &r, &g, &b);

		a = 0.0f;

	} else if((val = pEl->Attribute("rgba"))) {

		sscanf(val, "%f,%f,%f", &r, &g, &b, &a);

	} else {

		r = 0.5f;
		g = 0.5f;
		b = 0.5f;
		a = 0.5f;
	}
}
*/

void RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderVertexBlendingQuery* pVertexBlending, 
							bool& illegal, bool& needsSoftwareProcessing, bool& needsVertexBlending) {
	
	if (pVertexBlending) {

		if (pVertexBlending->inVertexBlend != pVertexBlending->outVertexBlend) {
			illegal = true;
			return;
		}

		needsVertexBlending = pVertexBlending->outVertexBlend;

		if (pVertexBlending->outVertexBlend) {
			void* temp = pVertexBlending->reserved;

			//probably not a big deal anyway
			assrt(sizeof(void*) >= sizeof(DWORD));

			switch((RendererD3D9_RenderVertexBlendingQuery_Reserved) (DWORD) temp) {
				case Res_UseSoftware:
					needsSoftwareProcessing = true;
					break;
				case Res_UseHardware:
					needsSoftwareProcessing = false;
					break;
			}
		}
	}
}

void RD3D9_CONVERT_CALL RendererD3D9_convert(RenderBufferUsage& usage, bool needsSoftwareProcessing, DWORD& d3d_usage) {

	d3d_usage = 0;

	if (usage.dynamic) {

		d3d_usage |= D3DUSAGE_DYNAMIC;
	}
	if (usage.writeOnly) {

		d3d_usage |= D3DUSAGE_WRITEONLY;
	}
	if (needsSoftwareProcessing) {

		d3d_usage |= D3DUSAGE_SOFTWAREPROCESSING;
	}
}

/*
void RD3D9_CONVERT_CALL RendererD3D9_convert(RenderPool& pool, D3DPOOL& d3d_pool) {

	d3d_pool = D3DPOOL_MANAGED;
}
*/

bool RD3D9_CONVERT_CALL RendererD3D9_hasAlpha(D3DFORMAT format) {

	switch (format) {

		case D3DFMT_R8G8B8: //24-bit RGB pixel format with 8 bits per channel.
		case D3DFMT_X8R8G8B8: // 22 32-bit RGB pixel format, where 8 bits are reserved for each color. 
		case D3DFMT_R5G6B5: // 23 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
		case D3DFMT_X1R5G5B5: // 24 16-bit pixel format where 5 bits are reserved for each color. 
		case D3DFMT_X4R4G4B4: // 30 16-bit RGB pixel format using 4 bits for each color. 
		case D3DFMT_X8B8G8R8: // 33 32-bit RGB pixel format, where 8 bits are reserved for each color. 
		case D3DFMT_G16R16: // 34 32-bit pixel format using 16 bits each for green and red. 
		case D3DFMT_P8: // 41 8-bit color indexed. 
		case D3DFMT_L8: // 50 8-bit luminance only. 
		case D3DFMT_L16: // 81 16-bit luminance only. 
			return false;
		
		case D3DFMT_A8R8G8B8: // 21 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
		case D3DFMT_A1R5G5B5: // 25 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
		case D3DFMT_A4R4G4B4: // 26 16-bit ARGB pixel format with 4 bits for each channel. 
		case D3DFMT_R3G3B2: // 27 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 
		case D3DFMT_A8: // 28 8-bit alpha only. 
		case D3DFMT_A8R3G3B2: // 29 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
		case D3DFMT_A2B10G10R10: // 31 32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
		case D3DFMT_A8B8G8R8: // 32 32-bit ARGB pixel format with alpha, using 8 bits per channel. 
		case D3DFMT_A2R10G10B10: // 35 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha. 
		case D3DFMT_A16B16G16R16: // 36 64-bit pixel format using 16 bits for each component. 
		case D3DFMT_A8P8: // 40 8-bit color indexed with 8 bits of alpha. 
		case D3DFMT_A8L8: // 51 16-bit using 8 bits each for alpha and luminance. 
		case D3DFMT_A4L4: //8-bit using 4 bits each for alpha and luminance. 
			return true;

		default:
			assrt(false);
			return false;
	}

	return false;
}


D3DPRIMITIVETYPE RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderPrimitiveEnum& prim) {

	switch (prim) {
	case RP_PointList:
		return D3DPT_POINTLIST;
	case RP_LineList:
		return D3DPT_LINELIST;
	case RP_TriangleList:
		return D3DPT_TRIANGLELIST;
	default:
		assrt(false);
	}

	return D3DPT_FORCE_DWORD;
}

DWORD RD3D9_CONVERT_CALL RendererD3D9_vertexBlendMatrixCountToRenderState(const RenderStateValue& count) {

	switch (count) {
	case 2:
		return D3DVBF_1WEIGHTS;
	case 3:
		return D3DVBF_2WEIGHTS;
	case 4:
		return D3DVBF_3WEIGHTS;
	}

	return D3DVBF_DISABLE;
}

DWORD RD3D9_CONVERT_CALL RendererD3D9_convertCull(const RenderStateValue& disable) {

	if (disable) {

		return D3DCULL_NONE;
	}

	return D3DCULL_CCW;
}

void RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderColor& from, D3DCOLOR& to) {

	to = D3DCOLOR_COLORVALUE(from.el[RC_Red], from.el[RC_Green], from.el[RC_Blue], from.alpha);
}

void RD3D9_CONVERT_CALL RendererD3D9_convert(const RenderColor& from, D3DCOLORVALUE& to) {

	assrt(sizeof(RenderColor) == sizeof(D3DCOLORVALUE));

	memcpy(&to, &from, sizeof(D3DCOLORVALUE));
}

void RD3D9_CONVERT_CALL RendererD3D9_convert(const Vector3& from, D3DVECTOR& to) {

	memcpy(&to, &from, sizeof(D3DVECTOR));
}

D3DFILLMODE RD3D9_CONVERT_CALL RendererD3D9_convertFillMode(const RenderStateValue& from) {

	if (from == 0) {

		return D3DFILL_SOLID;
	}

	return D3DFILL_WIREFRAME;
}

RenderStateValue RD3D9_CONVERT_CALL RendererD3D9_convertBlendFactor(const RenderStateValue& from) {

	switch (from) {
		case RBF_SrcAlpha:
			return D3DBLEND_SRCALPHA;
		case RBF_InvSrcAlpha:
			return D3DBLEND_INVSRCALPHA;
		case RBF_Zero:
			return D3DBLEND_ZERO;
		case RBF_One: 
			return D3DBLEND_ONE;
		case RBF_SrcColor: 
			return D3DBLEND_SRCCOLOR;
		case RBF_DestColor:
			return D3DBLEND_DESTCOLOR;
	}

	assrt(false);
	return D3DBLEND_ONE;
}

/*
D3DTEXTUREOP RD3D9_CONVERT_CALL RendererD3D9_convertBlendOp(const RenderBlendOpEnum& from) {

	switch (from) {
		case RBOP_Modulate:
			return D3DTOP_MODULATE;
		case RBOP_Modulate2X:
			return D3DTOP_MODULATE2X;
		case RBOP_Modulate4X:
			return D3DTOP_MODULATE4X;
		case RBOP_Add: 
			return D3DTOP_MODULATEALPHA_ADDCOLOR;
		case RBOP_AddSmooth: 
			return D3DTOP_ADDSMOOTH;
		case RBOP_Subtract:
			return D3DTOP_SUBTRACT;
	}

	assrt(false);
	return D3DTOP_MODULATE;
}
*/

DWORD RD3D9_CONVERT_CALL RendererD3D9_convRenderTextureOp(const RenderTextureOp& op) {

	switch(op) {

		case RTOP_Disable: return D3DTOP_DISABLE; 
		case RTOP_Modulate: return D3DTOP_MODULATE;
		case RTOP_Add: return D3DTOP_ADD; 
	}

	assrt(false);
	return D3DTOP_DISABLE;
}

DWORD RD3D9_CONVERT_CALL RendererD3D9_convRenderTextureArg(const RenderTextureArg& arg) {

	switch (arg) {

		case RTArg_Texture: return D3DTA_TEXTURE;
		case RTArg_Diffuse: return D3DTA_DIFFUSE;
		case RTArg_Current: return D3DTA_CURRENT;
	}
 
	assrt(false);
	return D3DTA_TEXTURE;
}

}