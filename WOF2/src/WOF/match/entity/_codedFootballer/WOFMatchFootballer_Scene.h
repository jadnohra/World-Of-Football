#ifndef _WOFMatchFootballer_Scene_h
#define _WOFMatchFootballer_Scene_h


#include "WE3/WEPtr.h"
#include "WE3/mesh/WEMeshInstance.h"
using namespace WE;

#include "WOFMatchFootballer_Base.h"
#include "../../WOFMeshPropertyManager.h"
#include "../../scene/WOFMatchSceneNode.h"

#include "../../collision/WOFMatchColliders.h"
#include "../../collision/WOFMatchCollRegistry.h"

namespace WOF { namespace match {

	class Match;
	class Footballer;
	class Team;

	enum FootballerAnimEnum {

		FA_None = -1, FA_Idle, FA_Run, FA_Tackle, FA_Turn180, FA_Shoot, FA_Fall, FA_ReservedSize
	};

	class Footballer_Scene : public Footballer_Base, public SceneNode {
	public:
	
		inline const Vector3& scene_getPosition() {return mTransformLocal.getPosition();}
		inline const Vector3& scene_getDirection(const SceneDirectionEnum& dir) {return mTransformLocal.getDirection(dir);}

		inline bool scene_animReachedEnd() {

			return scene_mSkelInst.reachedAnimationEnd();
		}

		inline void scene_setAnim(FootballerAnimEnum anim, const float& speed) {

			scene_mSkelInst.setAnimation(scene_mAnimMap->animIndex(anim), speed);
		}

		inline void scene_setCurrAnimSpeed(const float& speed) {

			scene_mSkelInst.setSpeedFactor(speed);
		}


		inline void scene_orientLookAt(const Vector3& lookAt, const Vector3& up) {

			mTransformLocal.setOrientation(mTransformWorld.getPosition(), lookAt, up);
		}

		inline void scene_orientDirection(const Vector3& direction, const Vector3& up) {

			mTransformLocal.setOrientation(direction, up);
		}

		void scene_orientLookAt(const Vector3& lookAt);
		void scene_orientDirection(const Vector3& direction);

		inline bool& isActiveCamVisible() { return scene_mActiveCamVisible; }

		inline void scene_invalidateLocalTransform() { scene_mSavedTransformLocalValid = false; }
		inline void scene_saveLocalTransform() { scene_mSavedTransformLocal = mTransformLocal; scene_mSavedTransformLocalValid = true; }
		inline bool scene_loadLocalTransform() { if (scene_mSavedTransformLocalValid) mTransformLocal = scene_mSavedTransformLocal; return scene_mSavedTransformLocalValid; }

		inline float scene_getAnimLength(FootballerAnimEnum anim) {
		
			return scene_mSkelInst.getAnimationLength(scene_mAnimMap->animIndex(anim));
		}
		/*
		inline void scene_move(const Vector3& direction, const float& velocity) {

			direction.mul(velocity, scene_mVelocity);
		}

		inline void scene_moveStop() {

			scene_mVelocity.zero();
		}
		
		inline Vector3& scene_moveVelocityRef() { return scene_mVelocity;}
		*/
	
	public:

		Footballer_Scene();
		virtual ~Footballer_Scene();

		bool _init_mesh(Mesh& mesh, SkeletalMeshPropertyManager& skelMeshProps);
		bool _init_collider(const WorldMaterialID& collMaterial);

		void cancel_init_create();
		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene(Match& match, Team& team, Footballer& footballer);

		inline void scene_addAnimTime(const Time& dt) {
			scene_mSkelInst.addTime(dt);
		}

		void scene_clean();
		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass);

		void onFrameMoved();

		//inline Volume& getTestVolume() { return scene_mCollider.mVolumeWorld; }
	
	protected:

		void updateCollisions();

		static SkeletonAnimMap* _init_animMap(SkeletalMeshPropertyManager& skelMeshProps, Mesh* pMesh);

	protected:

		MeshInstance scene_mMeshInst;
		SkeletonInstance scene_mSkelInst;
		SoftPtr<SkeletonAnimMap> scene_mAnimMap;

		DynamicVolumeCollider scene_mCollider;
		CollRegistry scene_mCollRegistry;
		
		bool scene_mSavedTransformLocalValid;
		SceneTransform scene_mSavedTransformLocal;
		//Vector3 scene_mVelocity;

		bool scene_mActiveCamVisible;
	};

} }

#endif