#include "WERendererD3D9DrawObject.h"

#include "WERendererD3D9.h"
#include "WERendererD3D9Convert.h"

namespace WE {

static WORD RendererD3D9_DrawObject_boxSrcIndices[] = {0,1,2,0,3,2,6,3,7,6,5,2,1,5,0,4,5,7,4,3};
static DWORD RendererD3D9_DrawObject_boxSrcIndexCount = sizeof(RendererD3D9_DrawObject_boxSrcIndices) / sizeof(WORD);

static WORD RendererD3D9_DrawObject_boxSrcIndicesNoDiagList[] = {0,1,1,2,2,3,3,0,0,4,4,5,5,6,6,7,7,4,1,5,2,6,3,7};
static DWORD RendererD3D9_DrawObject_boxSrcIndexCountNoDiagList = sizeof(RendererD3D9_DrawObject_boxSrcIndicesNoDiagList) / sizeof(WORD);

static WORD RendererD3D9_DrawObject_sphereSrcIndices[] = {	0,1,2,3,4,5,6,7,0, 
									8,9,10,11,12,13,14,15,8, 16,17,18,19,20,21,22,23,16, 
									24,20,12,4,
									29,30,31,32,25,26,27,28,29, 37,38,39,40,33,34,35,36,37,
									41,33,25,0,

									1,9,17,24,21,13,5,30,38,41,34,26,1,
									2,10,18,24,22,14,6,31,39,41,35,27,2,
									3,11,19,23,15,7,32,40,41,36,28,3
								};
static DWORD RendererD3D9_DrawObject_sphereSrcIndexCount = sizeof(RendererD3D9_DrawObject_sphereSrcIndices) / sizeof(WORD);

static WORD RendererD3D9_DrawObject_matrixSrcIndices[] = {0,1,0,2,0,3};
static DWORD RendererD3D9_DrawObject_matrixSrcIndexCount = sizeof(RendererD3D9_DrawObject_matrixSrcIndices) / sizeof(WORD);

struct RendererD3D9_DrawObject_Vertex : Point {
};

void RendererD3D9_DrawObject::onDeviceState(IDirect3DDevice9* pDevice, RendererD3D9_DeviceState state) {

	switch (state) {
		case RD3D9_DS_Lost:
			destroy();
			break;
		case RD3D9_DS_Reset:
			resetDevice(pDevice);
			break;
	}
}

RendererD3D9_DrawObject::RendererD3D9_DrawObject() {

	mpRenderer = NULL;

	mpVB = NULL;

	mpIB_Box = NULL;
	mpIB_BoxNoDiag = NULL;
	mpIB_Sphere = NULL;
	mpIB_Matrix = NULL;

	mFVF = D3DFVF_XYZ;
	mVertexSizeBytes = sizeof(RendererD3D9_DrawObject_Vertex);

	mVertexCount = 64;

	D3DCOLORVALUE val;
	val.r = 0;
	val.g = 0;
	val.b = 0;
	val.a = 0.25f;

	mat.Diffuse = mat.Ambient = mat.Specular = mat.Emissive = val;

}

RendererD3D9_DrawObject::~RendererD3D9_DrawObject() {

	destroy();
}

void RendererD3D9_DrawObject::setRenderer(RendererD3D9* pRenderer) {

	mpRenderer = pRenderer;
}

void RendererD3D9_DrawObject::destroy() {

	MSafeRelease(mpVB);

	MSafeRelease(mpIB_Box);
	MSafeRelease(mpIB_BoxNoDiag);
	MSafeRelease(mpIB_Sphere);
	MSafeRelease(mpIB_Matrix);
}

void RendererD3D9_DrawObject::resetDevice(IDirect3DDevice9* pDevice) {

   destroy();

	if (pDevice == NULL) {
		return;	
	}

	
	pDevice->CreateVertexBuffer(mVertexCount * mVertexSizeBytes, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
									mFVF, D3DPOOL_DEFAULT, &mpVB, NULL);



	pDevice->CreateIndexBuffer(sizeof(RendererD3D9_DrawObject_boxSrcIndices), 0/*D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY*/,
								D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mpIB_Box, NULL);

	if (mpIB_Box) {

		WORD* indices = NULL; 

		mpIB_Box->Lock(0, sizeof(RendererD3D9_DrawObject_boxSrcIndices), (void**) &indices, 0/*D3DLOCK_DISCARD*/);

		if (indices != NULL) {
			memcpy(indices, RendererD3D9_DrawObject_boxSrcIndices, sizeof(RendererD3D9_DrawObject_boxSrcIndices));
			mpIB_Box->Unlock();	
		}
	}

	pDevice->CreateIndexBuffer(sizeof(RendererD3D9_DrawObject_boxSrcIndicesNoDiagList), 0/*D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY*/,
								D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mpIB_BoxNoDiag, NULL);

	if (mpIB_BoxNoDiag) {

		WORD* indices = NULL; 

		mpIB_BoxNoDiag->Lock(0, sizeof(RendererD3D9_DrawObject_boxSrcIndicesNoDiagList), (void**) &indices, 0/*D3DLOCK_DISCARD*/);

		if (indices != NULL) {
			memcpy(indices, RendererD3D9_DrawObject_boxSrcIndicesNoDiagList, sizeof(RendererD3D9_DrawObject_boxSrcIndicesNoDiagList));
			mpIB_BoxNoDiag->Unlock();	
		}
	}
	

	pDevice->CreateIndexBuffer(sizeof(RendererD3D9_DrawObject_sphereSrcIndices), 0/*D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY*/,
								D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mpIB_Sphere, NULL);

	if (mpIB_Sphere) {

		WORD* indices = NULL; 

		mpIB_Sphere->Lock(0, sizeof(RendererD3D9_DrawObject_sphereSrcIndices), (void**) &indices, 0/*D3DLOCK_DISCARD*/);

		if (indices != NULL) {
			memcpy(indices, RendererD3D9_DrawObject_sphereSrcIndices, sizeof(RendererD3D9_DrawObject_sphereSrcIndices));
			mpIB_Sphere->Unlock();	
		}
	}
	


	pDevice->CreateIndexBuffer(sizeof(RendererD3D9_DrawObject_matrixSrcIndices), 0/*D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY*/,
								D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mpIB_Matrix, NULL);

	if (mpIB_Matrix) {

		WORD* indices = NULL; 

		mpIB_Matrix->Lock(0, sizeof(RendererD3D9_DrawObject_matrixSrcIndices), (void**) &indices, 0/*D3DLOCK_DISCARD*/);

		if (indices != NULL) {
			memcpy(indices, RendererD3D9_DrawObject_matrixSrcIndices, sizeof(RendererD3D9_DrawObject_matrixSrcIndices));
			mpIB_Matrix->Unlock();	
		}
	}

}

void RendererD3D9_DrawObject::_draw(IDirect3DDevice9* pDevice, 
							  const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor, 
							  D3DPRIMITIVETYPE primType, UINT primCount, 
							  bool indexed, IDirect3DIndexBuffer9*  mpIB, UINT numVertices, UINT startIndex,
							  bool cullNone, bool alpha) {

	//assrt(mpRenderer->mStackRS.blockCount() == 0);

	mpRenderer->setRenderState(RS_VertexBlendIndexed, 0);
	mpRenderer->setRenderState(RS_VertexBlendMatrixCount, 0);

	static D3DMATRIX m;

	if (pWorldMatrix) {
	
		RendererD3D9_convert(*pWorldMatrix, m);
		MCheckD3DStmt(pDevice->SetTransform(D3DTS_WORLDMATRIX(0), &m));
	}


	mpRenderer->checkSet_RS_EnableBlend(alpha);
	
	
	if (pColor) {

		mat.Emissive.r = pColor->el[RC_Red];
		mat.Emissive.g = pColor->el[RC_Green];
		mat.Emissive.b = pColor->el[RC_Blue];
		
	} else {

		mat.Emissive.r = 1.0f;
		mat.Emissive.g = 1.0f;
		mat.Emissive.b = 1.0f;
	}

	mpRenderer->checkSet_RS_CullMode(cullNone ? D3DCULL_NONE : D3DCULL_CCW);


	RenderStateValue backSoftVertProc = mpRenderer->mRS_SoftwareVertexProcessing;

	mpRenderer->checkSet_RS_SoftwareVertexProcessing(false);
	MCheckD3DStmt(pDevice->SetMaterial(&mat));
	MCheckD3DStmt(pDevice->SetTexture(0, NULL));
	
	mpRenderer->checkSet_RS_EnableLighting(true);
	mpRenderer->checkSet_Curr_Shader(mFVF);
	
 
	MCheckD3DStmt(pDevice->SetStreamSource(0, mpVB, 0, mVertexSizeBytes)); 

	if (indexed) {
		MCheckD3DStmt(pDevice->SetIndices(mpIB));
		MCheckD3DStmt(pDevice->DrawIndexedPrimitive(primType, 0, 0, numVertices, startIndex, primCount));
	} else {
		MCheckD3DStmt(pDevice->DrawPrimitive(primType, 0, primCount));
	}

	mpRenderer->checkSet_RS_SoftwareVertexProcessing(backSoftVertProc);


	/*
	XX
	if (mpRenderer->mCurr_Material) {

		mpRenderer->mCurr_Material->set(*mpRenderer);
	}
	*/	
}

void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, const OBB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor) {

	Vector srcPoints[8];
	short pointCount = 8;

	object.toPoints(srcPoints);

	{
		void* points = NULL;

		mpVB->Lock(0, sizeof(srcPoints), &points, D3DLOCK_DISCARD);

		if (points == NULL) {
			return;
		}

		memcpy(points, srcPoints, sizeof(srcPoints));
		mpVB->Unlock();
	}

	_draw(pDevice, pWorldMatrix, pColor, D3DPT_LINESTRIP, RendererD3D9_DrawObject_boxSrcIndexCount - 1, true, mpIB_Box, 8);

}

void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, const AAB& aab, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor, bool noDiag) {

	Vector srcPoints[8];
	short pointCount = 8;

	aab.toPoints(srcPoints);

	{
		void* points = NULL;

		mpVB->Lock(0, sizeof(srcPoints), &points, D3DLOCK_DISCARD);

		if (points == NULL) {
			return;
		}

		memcpy(points, srcPoints, sizeof(srcPoints));
		mpVB->Unlock();
	}

	if (noDiag) {

		_draw(pDevice, pWorldMatrix, pColor, D3DPT_LINELIST, RendererD3D9_DrawObject_boxSrcIndexCountNoDiagList / 2, true, mpIB_BoxNoDiag, 8);

	} else {

		_draw(pDevice, pWorldMatrix, pColor, D3DPT_LINESTRIP, RendererD3D9_DrawObject_boxSrcIndexCount - 1, true, mpIB_Box, 8);
	}
	
}

void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, 
										const Point& p0, const Point& p1, const Point& p2, 
										const WE::Matrix4x3Base* pWorldMatrix,
										bool bumpDepthBias, bool wireFrame, const RenderColor* pColor) {

	{
		Point* points = NULL;

		mpVB->Lock(0, 3 * sizeof(WE::Point), (void**) (&points), D3DLOCK_DISCARD);

		if (points == NULL) {
			return;
		}

		points[0] = p0;
		points[1] = p1;
		points[2] = p2;
		
		mpVB->Unlock();
	}

	DWORD bias;
	if (bumpDepthBias) {
		pDevice->GetRenderState(D3DRS_DEPTHBIAS, &bias);
		pDevice->SetRenderState(D3DRS_DEPTHBIAS, bias + 1);
	}
	DWORD fillMode;
	if (wireFrame) {
		pDevice->GetRenderState(D3DRS_FILLMODE, &fillMode);
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	_draw(pDevice, pWorldMatrix, pColor, D3DPT_TRIANGLELIST, 1, false, NULL, 0, 0, true);


	if (bumpDepthBias) {
		pDevice->SetRenderState(D3DRS_DEPTHBIAS, bias);
	}
	if (wireFrame) {
		pDevice->SetRenderState(D3DRS_FILLMODE, fillMode);
	}
}

void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, 
										const Point& p0, const Point& p1, 
										const WE::Matrix4x3Base* pWorldMatrix,
										bool bumpDepthBias, const RenderColor* pColor) {

	{
		Point* points = NULL;

		mpVB->Lock(0, 2 * sizeof(WE::Point), (void**) (&points), D3DLOCK_DISCARD);

		if (points == NULL) {
			return;
		}

		points[0] = p0;
		points[1] = p1;
		
		mpVB->Unlock();
	}

	DWORD bias;
	if (bumpDepthBias) {
		pDevice->GetRenderState(D3DRS_DEPTHBIAS, &bias);
		pDevice->SetRenderState(D3DRS_DEPTHBIAS, bias + 1);
	}

	_draw(pDevice, pWorldMatrix, pColor, D3DPT_LINELIST, 1, false, NULL, 0, 0, true);


	if (bumpDepthBias) {
		pDevice->SetRenderState(D3DRS_DEPTHBIAS, bias);
	}
}

void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, const AAPlane& plane, float size, 
									const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor) {
	
	Vector srcPoints[4];
	short pointCount = 4;

	plane.toPoints(srcPoints, size);

	{
		Point* points = NULL;

		mpVB->Lock(0, 4 * sizeof(WE::Point), (void**) (&points), D3DLOCK_DISCARD);

		if (points == NULL) {
			return;
		}

		points[0] = srcPoints[0];
		points[1] = srcPoints[1];
		points[2] = srcPoints[2];
		points[3] = srcPoints[3];
		
		mpVB->Unlock();
	}

	_draw(pDevice, pWorldMatrix, pColor, D3DPT_TRIANGLELIST, 2, true, mpIB_Box, 4, 0, true, true);

}


void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, const Plane& plane, float size, 
									const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor) {
	
	Vector srcPoints[4];
	short pointCount = 4;

	plane.toPoints(srcPoints, size);

	{
		Point* points = NULL;

		mpVB->Lock(0, 4 * sizeof(WE::Point), (void**) (&points), D3DLOCK_DISCARD);

		if (points == NULL) {
			return;
		}

		points[0] = srcPoints[0];
		points[1] = srcPoints[1];
		points[2] = srcPoints[2];
		points[3] = srcPoints[3];
		
		mpVB->Unlock();
	}

	_draw(pDevice, pWorldMatrix, pColor, D3DPT_TRIANGLELIST, 2, true, mpIB_Box, 4, 0, true, true);

}

void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, const WE::RigidMatrix4x3& matrix, const float& size) {

	draw(pDevice, matrix.castToCtMatrix3x3Base(), matrix.row[3], size);
}

void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, const Matrix3x3Base& matrix, const Vector& position, const float& size) {

	Vector srcPoints[4];
	short pointCount = 4;
	
	srcPoints[0] = position;

	WE::Vector3 temp;

	matrix.row[0].mul(size, temp);
	position.add(temp, srcPoints[1]);

	matrix.row[1].mul(size, temp);
	position.add(temp, srcPoints[2]);

	matrix.row[2].mul(size, temp);
	position.add(temp, srcPoints[3]);
	
	{
		void* points = NULL;

		mpVB->Lock(0, sizeof(srcPoints), &points, D3DLOCK_DISCARD);

		if (points == NULL) {
			return;
		}

		memcpy(points, srcPoints, sizeof(srcPoints));
		mpVB->Unlock();
	}

	static RenderColor color1(1.0f, 0.0f, 0.0f);
	static RenderColor color2(0.0f, 1.0f, 0.0f);
	static RenderColor color3(0.0f, 0.0f, 1.0f);


	_draw(pDevice, &Matrix4x3Base::kIdentity, &color1, D3DPT_LINESTRIP, 1, true, mpIB_Matrix, 1, 0);
	_draw(pDevice, &Matrix4x3Base::kIdentity, &color2, D3DPT_LINESTRIP, 1, true, mpIB_Matrix, 1, 2);
	_draw(pDevice, &Matrix4x3Base::kIdentity, &color3, D3DPT_LINESTRIP, 1, true, mpIB_Matrix, 1, 4);
	
}

void RendererD3D9_DrawObject::draw(IDirect3DDevice9* pDevice, const WE::Sphere& sphere, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor) {

	
	int ringVertexCount = 8;
	int ringFromCenterCount = 2;
	int vertexCount = 2 + (((ringFromCenterCount * 2) + 1) * ringVertexCount);

	{
		void* points = NULL;

		MCheckD3DStmt(mpVB->Lock(0, vertexCount, &points, D3DLOCK_DISCARD));

		if (points == NULL) {
			return;
		}

		int srcIndex = 0;
		Vector* srcPoints = (Vector*) points;

		//middle ring
		Point refPoint;

		refPoint.zero();
		refPoint[Ro] = sphere.radius;

		Point spherePoint;
		
		float thetaAngleInc = WE::k2Pi / (float) ringVertexCount;

		refPoint[Phi] = WE::kPiOver2;
		for (int rv = 0; rv < ringVertexCount; ++rv) {
			refPoint[The] = ((float) rv) * thetaAngleInc;

			refPoint.sphericalToCartesian(spherePoint);
			spherePoint += sphere.center;

			srcPoints[srcIndex++] = spherePoint;
		}

		float phiAngleInc = WE::kPiOver2 / (float) (ringFromCenterCount + 1);

		for (int up = 0; up < ringFromCenterCount; ++up) {
			
			refPoint[Phi] = WE::kPiOver2 - (float) (up + 1) * phiAngleInc;
			for (int rv = 0; rv < ringVertexCount; ++rv) {
				refPoint[The] = ((float) rv) * thetaAngleInc;

				refPoint.sphericalToCartesian(spherePoint);
				spherePoint += sphere.center;

				srcPoints[srcIndex++] = spherePoint;
			}
		}

		refPoint[Phi] = 0.0f;
		refPoint.sphericalToCartesian(spherePoint);
		spherePoint += sphere.center;

		srcPoints[srcIndex++] = spherePoint;

		for (int down = 0; down < ringFromCenterCount; ++down) {
			
			refPoint[Phi] = WE::kPiOver2 + (float) (down + 1) * phiAngleInc;
			for (int rv = 0; rv < ringVertexCount; ++rv) {
				refPoint[The] = ((float) rv) * thetaAngleInc;

				refPoint.sphericalToCartesian(spherePoint);
				spherePoint += sphere.center;

				srcPoints[srcIndex++] = spherePoint;
			}
		}

		refPoint[Phi] = WE::kPi;
		refPoint.sphericalToCartesian(spherePoint);
		spherePoint += sphere.center;

		srcPoints[srcIndex++] = spherePoint;

		MCheckD3DStmt(mpVB->Unlock());
	}


	_draw(pDevice, pWorldMatrix, pColor, D3DPT_LINESTRIP, RendererD3D9_DrawObject_sphereSrcIndexCount - 1, true, mpIB_Sphere, vertexCount);
}

void RendererD3D9::draw(const OBB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor) {

	mDrawObject.draw(mpDevice, object, pWorldMatrix, pColor);
}

void RendererD3D9::draw(const AAB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor, bool noDiag) {

	mDrawObject.draw(mpDevice, object, pWorldMatrix, pColor, noDiag);
}

void RendererD3D9::draw(const Sphere& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor) {

	mDrawObject.draw(mpDevice, object, pWorldMatrix, pColor);
}

void RendererD3D9::draw(const Matrix3x3Base& matrix, const Vector& position, const float& size) {

	mDrawObject.draw(mpDevice, matrix, position, size);
}

void RendererD3D9::draw(const WE::RigidMatrix4x3& matrix, const float& size) {

	mDrawObject.draw(mpDevice, matrix, size);
}

void RendererD3D9::draw(const Triangle& triangle, const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor, bool bumpDepthBias, bool wireFrame) {

	mDrawObject.draw(mpDevice, triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], pWorldMatrix, bumpDepthBias, wireFrame, pColor);
}

void RendererD3D9::draw(const TriangleEdgeNormals& triangle, float size, const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor, bool bumpDepthBias) {

	mDrawObject.draw(mpDevice, triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], pWorldMatrix, bumpDepthBias, false, pColor);

	Point points[2];
	RenderColor color; 

	{

		triangle.vertices[0].add(triangle.vertices[1], points[0]);
		points[0] *= 0.5f;
		points[0].add(triangle.vertices[2], points[0]);
		points[0] *= 0.5f;

		points[1] = triangle.normal;
		points[1] *= size;
		points[1] += points[0];

		pColor->mul(0.5f, color);

		mDrawObject.draw(mpDevice, points[0], points[1], pWorldMatrix, bumpDepthBias, &color);

	}

	{
		triangle.vertices[0].add(triangle.vertices[1], points[0]);
		points[0] *= 0.5f;

		points[1] = triangle.normals[TA_Edge01];
		points[1] *= size;
		points[1] += points[0];

		pColor->mul(0.2f, color);

		mDrawObject.draw(mpDevice, points[0], points[1], pWorldMatrix, bumpDepthBias, &color);
	}


	{
		triangle.vertices[1].add(triangle.vertices[2], points[0]);
		points[0] *= 0.5f;

		points[1] = triangle.normals[TA_Edge12];
		points[1] *= size;
		points[1] += points[0];

		pColor->mul(0.8f, color);
		color.el[RC_Red] = color.el[RC_Blue];

		mDrawObject.draw(mpDevice, points[0], points[1], pWorldMatrix, bumpDepthBias, &color);
	}

	{
		triangle.vertices[0].add(triangle.vertices[2], points[0]);
		points[0] *= 0.5f;

		points[1] = triangle.normals[TA_Edge02];
		points[1] *= size;
		points[1] += points[0];

		pColor->mul(1.2f, color);
		color.el[RC_Green] = 0.2f * color.el[RC_Red];

		mDrawObject.draw(mpDevice, points[0], points[1], pWorldMatrix, bumpDepthBias, &color);
	}

}

void RendererD3D9::draw(const Point& from, const Point& to, const WE::Matrix4x3Base* pWorldMatrix, 
				  const RenderColor* pColor) {

	mDrawObject.draw(mpDevice, from, to, pWorldMatrix, false, pColor);
}

void RendererD3D9::draw(const AAPlane& plane, float size, const WE::Matrix4x3Base* pWorldMatrix, 
				  const RenderColor* pColor) {

	mDrawObject.draw(mpDevice, plane, size, pWorldMatrix, pColor);
}

void RendererD3D9::draw(const Plane& plane, float size, const WE::Matrix4x3Base* pWorldMatrix, 
				  const RenderColor* pColor) {

	mDrawObject.draw(mpDevice, plane, size, pWorldMatrix, pColor);
}

}