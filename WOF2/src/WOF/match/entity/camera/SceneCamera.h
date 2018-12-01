#ifndef h_WOF_match_SceneCamera_h
#define h_WOF_match_SceneCamera_h

#include "WE3/binding/directx/9/dxut9/includeDXUT9.h"
#include "WE3/WETime.h"
#include "WE3/data/WEDataSourceChunk.h"
#include "WE3/math/WEFrustum.h"
using namespace WE;

#include "../../scene/SceneNode.h"
#include "SceneCamera_State.h"

#include "SceneCamera_Controller.h"
#include "SceneCamera_ControllerBufferZone.h"

namespace WOF { namespace match {

	class Player;

	class SceneCamera : public SceneNode {
	public:

		static inline SceneCamera* camFromObject(Object* pObject) {
			return (SceneCamera*) pObject;
		}

		static inline SceneCamera* camFromNode(SceneNode* pNode) {
			return (SceneCamera*) pNode;
		}

	public:

		SceneCamera();
		virtual ~SceneCamera();

		bool _init_settings(BufferString& tempStr, DataChunk& camChunk, CoordSysConv* pConv);
		bool _init_lookAt(BufferString& tempStr, DataChunk& camChunk, CoordSysConv* pConv);
		bool _init_camera();

		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene();
		
		void frameMove(const Time& t, const Time& dt);
		void signalCameraChanged();
		
		void cancelTrack();
		void track(SceneNode* pNode, float* pConstrainUpValue = NULL);
		void track(Player* pPlayer, float* pConstrainUpValue = NULL);
		
		inline Player* getTrackedPlayer() { return mTrackedPlayer; }

		SceneNode* getTrackedNode();
		const Vector3& getConstrainedTrackPoint();

		void clean();

		void setMatrices(Renderer& renderer, bool cleanIfNeeded, RenderPassEnum pass);

		void syncToTracked(); //ignore buffer and syncs exactly to tracked object
		void setEnableBuffer(bool enable);
		void setBufferTrackSpeed(float speed);

		void setCameraTechnique(int tech);

		void relativiseDirection(const Vector3& dir, Vector3& relDir, bool constrainUp, bool discreeteDirections, bool inverse = false);
		bool projectOnScreen(const Vector3& point, float& x, float& y);
		bool projectOrthoOnScreen(const Vector3& point, float& x, float& y, float width);
		bool estimatePitchCameraWidth(float& width);
		void getDirectionTransform(bool constrainUp, bool discreeteDirections, SceneTransform& outTransform);
		void getMainDirection2D(Match& match, SceneDirectionEnum* pOutDir, bool* pOutNeg, Vector3* pOutDirAxis);

		Frustum* getFrustum();
		const Vector3* getFrustTerrainLookAtPoint();
		const Vector3* getFrustTerrainIntersectionPoints(bool*& pointValidity); // 4 points, also pointValidity[4]

		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders);

		bool processWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void notifySwitchedFrom(SceneCamera* pOldCam);

		void updateAsPitchCamera(Match& match, SceneCamera& refCam);

	protected:

		struct Settings {

			float rangeMin;
			float rangeMax;
			float FOVw;
			float FOVh;

			bool FOVwIsRef;

			float velocity;
			float rotation;

			void updateFOVs(const RenderViewportInfo& viewport);
		};

		/*
		struct CamPointCollider {

			Object_Collider collider;
			float radius;

			CollRegistry collRegistry;
			Vector3 correction;

			Vector3 correctedLookAtPos;
		};
		*/

	protected:

		void resetProjParams();

		void getCamViewParams(Vector3* pEye, Vector3* pLookAt);

		void setCamViewParams(const Vector3& eyePos, const Vector3& lookAt);
		void syncToTrackedNode();

		void moveCamera(const Point& lookAtPos);

		void updateFrustum();

	protected:

#ifdef _DEBUG
		bool mIsInitedSettings;
#endif
		Settings mSettings;
		
		float mVelocity;
		float mRotation;
		
		Vector3 mEye;
		Vector3 mLookAt;
		Vector3 mUp;
		
		Matrix4x4 mMatProj;
		SceneTransform mMatView;

		bool mFrustumIsDirty;
		Frustum mFrustum;

		bool mIsValidFrustTerrainLookAtPoint;
		Vector3 mFrustTerrainLookAtPoint;
		bool mIsValidFrustTerrainIntersectionPoints;
		Vector3 mFrustTerrainIntersectionPoints[4];
		bool mFrustTerrainIntersectionPointValid[4];


		CFirstPersonCamera mCamera;
		
		Vector3 mOrigTrackOffset;
		Vector3 mTrackOffset;
		Vector3 mLookAtOffset;
		SoftPtr<SceneNode> mTrackedNode;
		SoftPtr<Player> mTrackedPlayer;

		sceneCamera::CamState mState;

		int mManipulLastPosX;
		int mManipulLastPosY;
		Vector3 mManipulTrackOffset;
		/*
		HardPtr<CamPointCollider> mCamPointCollider;
		*/

		Plane debugPlanes[Frustum::PlaneCount];

	protected:

		HardRef<sceneCamera::ControllerBase> mBoundController;
		HardRef<sceneCamera::ControllerBase> mBoundPostController;
		HardRef<sceneCamera::Controller_BufferZone> mBoundBufferZone;
	};

} }

#endif