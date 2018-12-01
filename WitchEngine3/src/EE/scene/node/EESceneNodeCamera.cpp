#include "EESceneNodeCamera.h"

#include "../EEScene.h"

#include "WE3/binding/directx/9/d3d9/renderer/WERendererD3D9Convert.h"

namespace EE {


SceneNodeCamera::SceneNodeCamera() : mMode(Cam_None) {

	mCamIsInited = false;
}


void SceneNodeCamera::setCameraMode(SceneCameraMode mode) {

	if (mode == mMode) {

		return;
	}

	switch(mMode) {
		case Cam_FPS:
			mCameraFPS.destroy();
			break;
		case Cam_Model:
			mCameraModel.destroy();
			break;
	}

	mMode = mode;

	switch(mMode) {
		case Cam_FPS:
			MMemNew(mCameraFPS.ptrRef(), CFirstPersonCamera());
			break;
		case Cam_Model:
			MMemNew(mCameraModel.ptrRef(), CModelViewerCamera());
			break;
	}

	mCamIsInited = false;
}

CBaseCamera* SceneNodeCamera::getBaseCamera() {

	switch(mMode) {
		case Cam_FPS:
			return mCameraFPS.ptr();
			break;
		case Cam_Model:
			return mCameraModel.ptr();
			break;
	}

	return NULL;
}

bool SceneNodeCamera::getLookDir(Vector3& lookDir) {

	CBaseCamera* pCam = getBaseCamera();

	if (pCam == NULL) {

		return false;
	}

	const D3DXVECTOR3* pPt;
	Point eye;

	pPt = pCam->GetEyePt();

	eye.set(pPt->x, pPt->y, pPt->z);

	pPt = pCam->GetLookAtPt();
	lookDir.set(pPt->x, pPt->y, pPt->z);
	lookDir.subtract(eye, lookDir);
	lookDir.normalize();

	return true;
}

void SceneNodeCamera::initFromScene() {

	switch (mMode) {
		case Cam_FPS:
			{
				Renderer& renderer = mNodeScene->renderer();

				setupCamera(mCameraFPS.dref(), renderer, mFOV, mRangeMin, mRangeMax, mEye, mLookAt);

				mCameraFPS->SetEnablePositionMovement(true);
				mCameraFPS->SetEnableYAxisMovement(true);
				mCameraFPS->SetScalers(0.003f * mRotation, mVelocity);

				mCamIsInited = true;
			}
			break;
		case Cam_Model:
			{
				assrt(false);
			}
			break;
	}
}


void SceneNodeCamera::frameMove(const ShortTime& dt) {

	switch (mMode) {
		case Cam_FPS:
			{
				mCameraFPS->FrameMove(dt);

				signalCameraChanged();
			}
			break;
		case Cam_Model:
			{
				assrt(false);
			}
			break;
	}
}


bool SceneNodeCamera::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (mMode) {
		case Cam_FPS:
			{
				if (mCameraFPS->HandleMessages(0, uMsg, wParam, lParam)) {
			
					//signalCameraChanged();
					return true;
				}
			}
			break;
		case Cam_Model:
			{
				if (mCameraModel->HandleMessages(0, uMsg, wParam, lParam)) {

					//signalCameraChanged();
					return true;
				}
			}
			break;
	}

	return false;
}


void SceneNodeCamera::signalCameraChanged() {
	
	nodeMarkDirty();

	const D3DXVECTOR3* pEye = NULL; 

	switch (mMode) {
		case Cam_FPS:
			{
				
				pEye = mCameraFPS->GetEyePt();
			}
			break;
		case Cam_Model:
			{
				pEye = mCameraModel->GetEyePt();
			}
			break;
	}

	if (pEye) {

		Vector3 pos(MDXExpandVect(*pEye));
		mTransformLocal.setPosition(pos);
	}
}


void SceneNodeCamera::nodeProcessSceneEvt(SceneBaseEvent& sceneBaseEvt, const bool& nodeWasDirty) {

	if (sceneBaseEvt.isGameEvt == false) {

		SceneEvent& sceneEvt = (SceneEvent&) sceneBaseEvt;
		bool setMatrices = true; 

		if (mCamIsInited) {

			switch (sceneEvt.type) {
				case SE_Render:

					if (setMatrices) {

						switch (mMode) {
						case Cam_FPS:
							{
								Renderer& renderer = dref<Renderer>(sceneEvt.pRenderer);

								RendererD3D9_convert(*mCameraFPS->GetProjMatrix(), mMatProj);
								renderer.setTransform(mMatProj, RT_Projection);

								RendererD3D9_convert(*mCameraFPS->GetViewMatrix(), mMatView);
								renderer.setTransform(mMatView, RT_View);

							}
							break;
						case Cam_Model:
							{
								assrt(false);
							}
							break;
						}
					}
					break;
			}
		}
	}
}

}