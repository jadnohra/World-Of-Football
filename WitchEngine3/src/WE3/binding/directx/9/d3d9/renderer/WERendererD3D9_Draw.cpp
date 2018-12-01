#include "WERendererD3D9.h"

namespace WE {


struct Quad2DVertex {

    Vector3 position; 
	FLOAT w;
	D3DCOLOR    color;   
};

struct TexturedQuad2DVertex {

    Vector3 position; 
	FLOAT w;
	D3DCOLOR    color;   
    FLOAT       tu, tv;   
};

inline float convInt(const float& val) {

	return (float) ((int) val + 0.5f);
}

void RendererD3D9::draw2DQuad(const RenderQuad& quad, Render2DScaling viewportScaling, const RenderColor* pColor, RenderTexture* pTexture, bool fixCoordOffsets, unsigned int* _indices) {

	const RenderColor& color = pColor ? dref(pColor) : RenderColor::kWhite;

	float mulX;
	float mulY;

	switch (viewportScaling) {

		case R2DS_Width: {

			mulX = (float) mBackBufferDesc.Width;
			mulY = (float) mBackBufferDesc.Width;

		} break;

		case R2DS_Height: {

			mulX = (float) mBackBufferDesc.Height;
			mulY = (float) mBackBufferDesc.Height;

		} break;

		case R2DS_WidthHeight: {
			
			mulX = (float) mBackBufferDesc.Width;
			mulY = (float) mBackBufferDesc.Height;

		} break;

		default: {

			mulX = 1.0f;
			mulY = 1.0f;

		} break;
	}

	float toffset;

	if (fixCoordOffsets) {

		toffset = -0.5f;

	} else {

		toffset = 0.0f;
	}
	
	static unsigned int identityIndices[] = {0, 1, 2, 3};
	unsigned int* indices = _indices ? _indices : identityIndices;

	this->checkSet_RS_EnableLighting(false);
	this->checkSet_RS_CullMode(D3DCULL_NONE);

	this->setTexture(pTexture, 0);

	if (pTexture) {

		TexturedQuad2DVertex vertices[4];

		if (fixCoordOffsets)
			vertices[0].position.set(convInt(quad.points[indices[0]].x * mulX) + toffset, convInt(quad.points[indices[0]].y * mulY) + toffset, 0.0f);
		else
			vertices[0].position.set(quad.points[indices[0]].x * mulX + toffset, quad.points[indices[0]].y * mulY + toffset, 0.0f);
		vertices[0].w = 1.0f;
		RendererD3D9_convert(color, vertices[0].color);
		vertices[0].tu = 0.0f;
		vertices[0].tv = 0.0f;

		if (fixCoordOffsets)
			vertices[1].position.set(convInt(quad.points[indices[1]].x * mulX) + toffset, convInt(quad.points[indices[1]].y * mulY) + toffset, 0.0f);
		else
			vertices[1].position.set(quad.points[indices[1]].x * mulX + toffset, quad.points[indices[1]].y * mulY + toffset, 0.0f);
		vertices[1].w = 1.0f;
		vertices[1].color = vertices[0].color;
		vertices[1].tu = 1.0f;
		vertices[1].tv = 0.0f;

		if (fixCoordOffsets)
			vertices[2].position.set(convInt(quad.points[indices[2]].x * mulX) + toffset, convInt(quad.points[indices[2]].y * mulY) + toffset, 0.0f);
		else 
			vertices[2].position.set(quad.points[indices[2]].x * mulX + toffset, quad.points[indices[2]].y * mulY + toffset, 0.0f);
		vertices[2].w = 1.0f;
		vertices[2].color = vertices[0].color;
		vertices[2].tu = 1.0f;
		vertices[2].tv = 1.0f;

		if (fixCoordOffsets)
			vertices[3].position.set(convInt(quad.points[indices[3]].x * mulX) + toffset, convInt(quad.points[indices[3]].y * mulY) + toffset, 0.0f);
		else 
			vertices[3].position.set(quad.points[indices[3]].x * mulX + toffset, quad.points[indices[3]].y * mulY + toffset, 0.0f);
		vertices[3].w = 1.0f;
		vertices[3].color = vertices[0].color;
		vertices[3].tu = 0.0f;
		vertices[3].tv = 1.0f;

		this->checkSet_Curr_Shader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 |  D3DFVF_TEXCOORDSIZE2(0));
		MCheckD3DStmt(mpDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(TexturedQuad2DVertex)));

	} else {

		Quad2DVertex vertices[4];

		vertices[0].position.set(quad.points[indices[0]].x * mulX + toffset, quad.points[indices[0]].y * mulY + toffset, 0.0f);
		vertices[0].w = 1.0f;
		RendererD3D9_convert(color, vertices[0].color);
		
		vertices[1].position.set(quad.points[indices[1]].x * mulX + toffset, quad.points[indices[1]].y * mulY + toffset, 0.0f);
		vertices[1].w = 1.0f;
		vertices[1].color = vertices[0].color;
		
		vertices[2].position.set(quad.points[indices[2]].x * mulX + toffset, quad.points[indices[2]].y * mulY + toffset, 0.0f);
		vertices[2].w = 1.0f;
		vertices[2].color = vertices[0].color;

		vertices[3].position.set(quad.points[indices[3]].x * mulX + toffset, quad.points[indices[3]].y * mulY + toffset, 0.0f);
		vertices[3].w = 1.0f;
		vertices[3].color = vertices[0].color;
		

		this->checkSet_Curr_Shader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		MCheckD3DStmt(mpDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Quad2DVertex)));
	}
}

struct Quad3DVertex {

    Vector3 position; 
	D3DCOLOR    color;   
};

struct TexturedQuad3DVertex {

    Vector3 position; 
	D3DCOLOR    color;   
    FLOAT       tu, tv;   
};

void RendererD3D9::draw(RenderQuad& quad, const RenderColor* pColor, RenderTexture* pTexture) {

	const RenderColor& color = pColor ? dref(pColor) : RenderColor::kWhite;

	/*
	bool enableAlpha = color.alpha != 1.0f;

	if (enableAlpha) {

		this->setRenderState(RS_EnableBlend, 1);

		this->setRenderState(RS_BlendFactorSrc, RBF_SrcAlpha);
		this->setRenderState(RS_BlendFactorDest, RBF_InvSrcAlpha);

	} else {

		this->setRenderState(RS_EnableBlend, 0);
	}
	*/
	
	


	this->checkSet_RS_EnableLighting(false);
	this->checkSet_RS_CullMode(D3DCULL_NONE);

	this->setTexture(pTexture, 0);

	if (pTexture) {

		TexturedQuad3DVertex vertices[4];

		vertices[0].position = quad.points[0];
		RendererD3D9_convert(color, vertices[0].color);
		vertices[0].tu = 0.0f;
		vertices[0].tv = 0.0f;

		vertices[1].position = quad.points[1];
		vertices[1].color = vertices[0].color;
		vertices[1].tu = 1.0f;
		vertices[1].tv = 0.0f;

		vertices[2].position = quad.points[2];
		vertices[2].color = vertices[0].color;
		vertices[2].tu = 1.0f;
		vertices[2].tv = 1.0f;

		vertices[3].position = quad.points[3];
		vertices[3].color = vertices[0].color;
		vertices[3].tu = 0.0f;
		vertices[3].tv = 1.0f;

		this->checkSet_Curr_Shader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
		MCheckD3DStmt(mpDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(TexturedQuad3DVertex)));

	} else {

		Quad3DVertex vertices[4];

		vertices[0].position = quad.points[0];
		RendererD3D9_convert(color, vertices[0].color);
		
		vertices[1].position = quad.points[1];
		vertices[1].color = vertices[0].color;
		
		vertices[2].position = quad.points[2];
		vertices[2].color = vertices[0].color;

		vertices[3].position = quad.points[3];
		vertices[3].color = vertices[0].color;
		

		this->checkSet_Curr_Shader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		MCheckD3DStmt(mpDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Quad3DVertex)));
	}

	/*
	if (enableAlpha) {

		this->checkSet_TexBlendOp(RBOP_Modulate, 0);
	}
	*/
}

}