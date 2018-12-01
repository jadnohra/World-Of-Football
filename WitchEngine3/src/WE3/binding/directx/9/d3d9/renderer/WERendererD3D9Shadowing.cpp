#include "WERendererD3D9.h"

#include "../../d3dx9/includeD3DX9.h"

namespace WE {


void RendererD3D9::switchPassState(RenderPassEnum currPass, RenderPassEnum newPass) {

	switch (currPass) {
		case RP_Normal:
			//applyPassNormal(false);
			break;
		case RP_Shadowing:
			applyPassShadowing(false);
			break;
	}

	switch (newPass) {
		case RP_Normal:
			//applyPassNormal(true);
			break;
		case RP_Shadowing:
			applyPassShadowing(true);
			break;
	}
}

void RendererD3D9::setupShadowingMaterial(const RenderColor& shadowColor, bool alphaBlend, bool stencilTest) {

	mShadowingAlphaBlend = alphaBlend;
	mShadowingStencilTest = stencilTest;

	//memset(&mShadowMaterial, 0, sizeof(D3DMATERIAL9));
	mShadowMaterial.Diffuse.a = shadowColor.alpha;
	mShadowMaterial.Ambient.r = shadowColor[RC_Red];
	mShadowMaterial.Ambient.g = shadowColor[RC_Green];
	mShadowMaterial.Ambient.b = shadowColor[RC_Blue];
}

void RendererD3D9::setupShadowingPlanar(const Vector3& lightDir, const Plane& plane) {

	mShadowPlanarLightDir = lightDir;
	mShadowPlanarPlane = plane;
}

void RendererD3D9::applyPassShadowing(bool start) {

	if (mRS_EnableShadowing == false) {

		assrt(false);
	}

	static D3DXMATRIX v;

	if (start) {

		D3DXVECTOR4 light(mShadowPlanarLightDir.x, mShadowPlanarLightDir.y, mShadowPlanarLightDir.z, 0.0f);
		float a,b,c,d;
		mShadowPlanarPlane.toPlaneEquation(a, b, c, d);
		D3DXPLANE plane(a, b, c, d);

		D3DXMATRIX matShadow, worldShadow[4], viewShadow;

		D3DXMatrixShadow(&matShadow, &light, &plane);

		//disable all textures in all stages
		setTexture(NULL, 0);
		//IDirect3DBaseTexture9* pTex;
		//mpDevice->GetTexture(0, &pTex);
		//mpDevice->SetTexture(0, NULL);

		MCheckD3DStmt(mpDevice->SetMaterial(&mShadowMaterial));

		D3DXMATRIX w[4];
		
		mpDevice->GetTransform(D3DTS_VIEW, &v);
		/*
		mpDevice->GetTransform(D3DTS_WORLDMATRIX(0), &w[0]);
		mpDevice->GetTransform(D3DTS_WORLDMATRIX(1), &w[1]);
		mpDevice->GetTransform(D3DTS_WORLDMATRIX(2), &w[2]);
		mpDevice->GetTransform(D3DTS_WORLDMATRIX(3), &w[3]);
		*/

		D3DXMatrixMultiply(&viewShadow, &matShadow, &v);
		/*
		D3DXMatrixMultiply(&worldShadow[0], &w[0], &matShadow);
		D3DXMatrixMultiply(&worldShadow[1], &w[1], &matShadow);
		D3DXMatrixMultiply(&worldShadow[2], &w[2], &matShadow);
		D3DXMatrixMultiply(&worldShadow[3], &w[3], &matShadow);
		*/


		MCheckD3DStmt(mpDevice->SetTransform(D3DTS_VIEW, &viewShadow));
		/*
		mpDevice->SetTransform(D3DTS_WORLDMATRIX(0), &worldShadow[0]);
		mpDevice->SetTransform(D3DTS_WORLDMATRIX(1), &worldShadow[1]);
		mpDevice->SetTransform(D3DTS_WORLDMATRIX(2), &worldShadow[2]);
		mpDevice->SetTransform(D3DTS_WORLDMATRIX(3), &worldShadow[3]);
		*/

		//disable all lights but enable lighting
		checkSet_RS_EnableLighting(TRUE);
		checkSet_RS_AmbientColor(D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f));

		for (LightEnables::Index i = 0; i < mLightEnables.count; ++i) {

			if (mLightEnables[i])
				mpDevice->LightEnable(i, FALSE);
		}

		
		if (mShadowingAlphaBlend) {
		
			checkSet_RS_EnableBlend(true);
			//MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));

			//checkSet_RS_SrcBlend(D3DBLEND_SRCALPHA);
			//checkSet_RS_DestBlend(D3DBLEND_INVSRCALPHA);

			checkSet_RS_BlendFactorSrc(D3DBLEND_ZERO);
			checkSet_RS_BlendFactorDest(D3DBLEND_SRCCOLOR);

			//MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
			//MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

		} else {

			checkSet_RS_EnableBlend(false);
		}
		
		if (mShadowingStencilTest) {

			MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE));
			MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL));
			MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_STENCILREF, 0));
			MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_STENCILMASK, 1));
			MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP));
			MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT));

		} else {

			MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE));
		}

	} else {

		for (LightEnables::Index i = 0; i < mLightEnables.count; ++i) {

			if (mLightEnables[i])
				mpDevice->LightEnable(i, TRUE);
		}


		if (mShadowingStencilTest) {
			
			MCheckD3DStmt(mpDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE));
		}
		
		if (mShadowingAlphaBlend) {

			//not necessary
			//checkSet_RS_EnableAlpha(false);
			//MCheckD3DStmt(mpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE));
		}
		
		//MCheckD3DStmt(mpDevice->LightEnable(0, TRUE));

		MCheckD3DStmt(mpDevice->SetTransform(D3DTS_VIEW, &v));
		/*
		mpDevice->SetTransform(D3DTS_WORLDMATRIX(0), &w[0]);
		mpDevice->SetTransform(D3DTS_WORLDMATRIX(1), &w[1]);
		mpDevice->SetTransform(D3DTS_WORLDMATRIX(2), &w[2]);
		mpDevice->SetTransform(D3DTS_WORLDMATRIX(3), &w[3]);
		*/
				
		//MCheckD3DStmt(mpDevice->SetMaterial(&mRS_Material));
		//mpDevice->SetTexture(0, pTex);
		//pTex->Release();
	} 
}

}