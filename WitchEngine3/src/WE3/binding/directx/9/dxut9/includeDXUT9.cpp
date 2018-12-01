#include "includeDXUT9.h"

namespace WE {

void DXUT::setupCamera(CModelViewerCamera& cam, Renderer& renderer, const Sphere& volumeSphere) {

	RenderViewportInfo viewInfo;

	if (renderer.getViewportInfo(viewInfo) == false) {

		assrt(false);
	}

	//Sphere volumeSphere;
	//volumeSphere.init(volumeBox, true);

	cam.Reset();
	cam.SetWindow(viewInfo.width, viewInfo.height);
	cam.SetModelCenter(D3DXVECTOR3(MExpand3(volumeSphere.center)));
	cam.SetRadius(volumeSphere.radius * 2.5f, volumeSphere.radius * 0.1f, volumeSphere.radius * 10.0f);
	cam.SetButtonMasks();


	D3DXMATRIX projection_matrix;
	float aspect = viewInfo.getAspectRatio();
	float fov = D3DX_PI/4;
	D3DXMatrixPerspectiveFovLH(&projection_matrix, fov, aspect, 1.0f, 1000.0f );
	float fovw = atan(1.0f / projection_matrix._11) * 2.0f;
	float d = (volumeSphere.radius / sin(fovw / 2.0f)) * 1.2f;
	float dclipn = volumeSphere.radius * 0.001f;//d - (gMeshSphere.radius * 10.0f);
	float dclipf = d + (volumeSphere.radius * 100.0f);

	cam.SetProjParams(fov, aspect, dclipn, dclipf);
	//pd3dDevice->SetTransform(D3DTS_PROJECTION, cam.GetProjMatrix());

	
	D3DXVECTOR3 eye_vector; 
	D3DXVECTOR3 up_vector;
	D3DXVECTOR3 lookat_vector;
	Vector3 temp;
	float val;
	
	const CoordSys& coordSys = renderer.getCoordSys();

	temp = volumeSphere.center;
	coordSys.getValue(CSD_Forward, temp.el, val);
	val += d;
	coordSys.setValue(CSD_Forward, val, temp.el);
	MDXVectFromPtr(eye_vector, temp.el);

	coordSys.getUnitVector(CSD_Up, temp.el);
	MDXVectFromPtr(up_vector, temp.el);

	//eye_vector = D3DXVECTOR3(volumeSphere.center[0], volumeSphere.center[1], volumeSphere.center[2] + d);
	//up_vector = D3DXVECTOR3(0.0f,1.0f,0.0f);
	
	
	MDXVectFromPtr(lookat_vector, volumeSphere.center);
	
	D3DXMATRIX world_matrix;
	D3DXMatrixIdentity(&world_matrix);


	cam.SetViewParams(&eye_vector, &lookat_vector);
	cam.SetWorldMatrix(world_matrix);
}

void DXUT::setupCamera(CFirstPersonCamera& cam, Renderer& renderer, float fov, float minDist, float maxDist, const Vector3& eye, const Vector3& lookAt) {
	
	
	RenderViewportInfo viewInfo;

	if (renderer.getViewportInfo(viewInfo) == false) {

		assrt(false);
	}

	float fovY = fov;
	float dclipn = minDist;
	float dclipf = maxDist;

	D3DXMATRIX projection_matrix;
	D3DXMATRIX view_matrix;

	//D3DXVECTOR3 up_vector = D3DXVECTOR3(MExpand3(up));
	D3DXVECTOR3 lookat_vector = D3DXVECTOR3(MExpand3(lookAt));
	D3DXVECTOR3 eye_vector = D3DXVECTOR3(MExpand3(eye));

	//D3DXMatrixPerspectiveFovLH(&projection_matrix, fovY, viewInfo.getAspectRatio(), dclipn, dclipf);
	//D3DXMatrixLookAtLH(&view_matrix, &eye_vector, &lookat_vector, &up_vector);
	
	cam.SetProjParams(fovY, viewInfo.getAspectRatio(), dclipn, dclipf);
	cam.SetViewParams(&eye_vector, &lookat_vector);
	

	/*
	float aspect = renderer.getViewportAspectRatio();
		
		float fovY = D3DX_PI / 4.0f;
		float dclipn = match.getMinVisibleDistance();
		float dclipf = match.getMaxVisibleDistance();

		mProjection.setupPerspectiveFov(fovY, aspect, dclipn, dclipf);
		//D3DXMatrixPerspectiveFovLH(&projection_matrix, fov, aspect, dclipn, dclipf);


		Vector up;
		Vector at;
		Vector eye;

		
		at.zero();
		at.el[D_Forward] = fromMeters(60.0f);
		at.el[D_Up] = fromMeters(1.8f);
		at.el[D_Right] = 0.0f;

		eye.el[D_Forward] = fromMeters(50.0f);
		eye.el[D_Up] = fromMeters(7.0f);
		eye.el[D_Right] = 0.0f;

		if (fabs(eye.el[D_Up] - at.el[D_Up]) > 0.001f) {
			up = kUnit[D_Up];
		} else {
			//if looking almost straight down set Up vector to be forward otherwize problems will occur
			//because of cross products
			up = kUnit[D_Forward];
		}
		//D3DXVECTOR3 up_vector = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		//D3DXVECTOR3 lookat_vector = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		//D3DXVECTOR3 eye_vector = D3DXVECTOR3(0.0f, globals.toRenderUnits(2.0f), globals.toRenderUnits(-1.0f));
		
		//D3DXMatrixLookAtLH(&view_matrix, &eye_vector, &lookat_vector, &up_vector);
		mView.setupLookAt(eye, at, up);

		D3DXVECTOR3 eye_vector = D3DXVECTOR3(MExpand3(eye.el));
		D3DXVECTOR3 lookat_vector = D3DXVECTOR3(MExpand3(at.el));
		mCamera.SetProjParams(fovY, aspect, dclipn, dclipf);
		mCamera.SetViewParams(&eye_vector, &lookat_vector);
		mCamera.SetEnablePositionMovement(true);
		mCamera.SetEnableYAxisMovement(true);
		//mCamera.SetScalers(0.003f, 200.0f);
		mCamera.SetScalers(0.003f, 150.0f);
	
	
		renderer.setTransform(mProjection, RT_Projection);
	*/
}

}