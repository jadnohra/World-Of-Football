#ifndef _EESceneNodeCamera_h
#define _EESceneNodeCamera_h

#include "../EESceneNode.h"
#include "WE3/binding/directx/9/dxut9/includeDXUT9.h"
#include "WE3/WETime.h"

namespace EE {

	enum SceneCameraMode {

		Cam_None = 0, Cam_FPS, Cam_Model
	};

	class SceneNodeCamera : public SceneNode {
	public:

		SceneNodeCamera();

		void setCameraMode(SceneCameraMode mode);
		void initFromScene();

		void frameMove(const ShortTime& dt);
		bool handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		CBaseCamera* getBaseCamera();
		void signalCameraChanged();

		bool getLookDir(Vector3& lookDir);

		virtual void nodeProcessSceneEvt(SceneBaseEvent& sceneEvt, const bool& nodeWasDirty);

	public:

		SceneCameraMode mMode;
		bool mCamIsInited;

		float mRangeMin;
		float mRangeMax;
		float mFOV;

		float mVelocity;
		float mRotation;

		Vector3 mEye;
		Vector3 mLookAt;
		Vector3 mUp;

		Matrix4x4 mMatProj;
		Matrix4x3 mMatView;

		union {
			struct {
				HardPtr<CModelViewerCamera> mCameraModel;
			};
			struct {
				HardPtr<CFirstPersonCamera> mCameraFPS;
			};
		};
	};

}

#endif