#ifdef FOOTBALLER_BODY3

#ifndef _WOFMatchFootballer_Body3_h
#define _WOFMatchFootballer_Body3_h

#include "WE3/WEPtr.h"
#include "WE3/mesh/WEMeshInstance.h"
using namespace WE;

#include "WOFMatchFootballer_Base.h"
#include "../../WOFMeshPropertyManager.h"
#include "../../scene/WOFMatchSceneNode.h"

#include "../../collision/WOFMatchColliders.h"
#include "../../collision/WOFMatchCollRegistry.h"

#include "WOFMatchFootballerDataTypes.h"
#include "WOFMatchFootballer_BodyDefs.h"

#include "WOFMatchFootballer_BodyAction_Idle.h"
#include "WOFMatchFootballer_BodyAction_Running.h"
#include "WOFMatchFootballer_BodyAction_Tackling.h"
#include "WOFMatchFootballer_BodyAction_Falling.h"
#include "WOFMatchFootballer_BodyAction_Jumping.h"

namespace WOF { namespace match {

	class Footballer;
	class Team;

	class Footballer_Body : public FootballerBodyDefs, public Footballer_Base, public SceneNode {
	public:
		
		Footballer_Body();
		~Footballer_Body();

	public:

		ActionType getActionType() { return mAction->getType(); }
		ActionImpl* getAction() { return mAction; }

		Footballer_BodyAction_Idle& getActionIdle() { return mActionIdle; }
		Footballer_BodyAction_Running& getActionRunning() { return mActionRunning; }
		Footballer_BodyAction_Tackling& getActionTackling() { return mActionTackling; }
		Footballer_BodyAction_Falling& getActionFalling() { return mActionFalling; }
		Footballer_BodyAction_Jumping& getActionJumping() { return mActionJumping; }

	protected:


		Footballer_BodyAction_Falling& getActionFallingAndSetupFromTackle(const Time& time, Footballer_Body* pTackler);

	public:

		BallInteraction mBallInteraction;

		inline const BallInteraction& getBallInteraction() { return mBallInteraction; }
		inline const BallInteractionType& getBallInteractionType() { return mBallInteraction.interactionType; }
		inline const BallRelativeInfo& getBallRelativeInfo() { return mBallInteraction.relativeInfo; }
		inline BallOwnership& getBallOwnership() { return mBallInteraction.ownership; }

	public:

		inline const bool& isActiveCamVisible() { return mActiveCamVisible; }

		inline const Vector3& getPosition() { return mTransformLocal.getPosition(); }
		inline const Vector3& getDirection(const SceneDirectionEnum& dir) { return mTransformLocal.getDirection(dir); }

		inline CtVector3& getCurrFacing() { return mTransformLocal.row[Scene_Forward]; }
	

	protected:

		void setActiveAction(ActionImpl* pNewActive) { mAction = pNewActive; }

		inline const BodyCommEvents::Index& getBodyCommEventCount() { return mBodyCommEvents.count; }
		inline BodyCommEvent& getBodyCommEvent(const BodyCommEvents::Index& i) { return mBodyCommEvents[i]; }
		inline BodyCommEvent& addBodyCommEvent() { return mBodyCommEvents.addOne(); }

		void sendTackleEvents(const Time& time);

	protected:

		inline const AnimationIndex::Type& getAnimIndex(const AnimEnum& anim) { return mAnimMap->animIndex(anim); }

		inline SkeletonInstance& getSkelInst() { return mSkelInst; }

		inline void orientLookAt(const Vector3& lookAt, const Vector3& up) { mTransformLocal.setOrientation(mTransformWorld.getPosition(), lookAt, up); }
		inline void orientDirection(const Vector3& direction, const Vector3& up) { mTransformLocal.setOrientation(direction, up); }
		void orientLookAt(const Vector3& lookAt);
		void orientDirection(const Vector3& direction);

		inline void invalidateLocalTransform() { mSavedTransformLocalValid = false; }
		inline void saveLocalTransform() { mSavedTransformLocal = mTransformLocal; mSavedTransformLocalValid = true; }
		inline bool loadLocalTransform() { if (mSavedTransformLocalValid) mTransformLocal = mSavedTransformLocal; return mSavedTransformLocalValid; }
		
	public:

		bool _init_mesh(Mesh& mesh, SkeletalMeshPropertyManager& skelMeshProps);
		bool _init_collider(const WorldMaterialID& collMaterial);

		void cancel_init_create();
		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene(Match& match, Team& team, Footballer& footballer);

		void clean();

		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass);
		void frameMoveTick(Footballer& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);

		void onFrameMoved();

	protected:

		static SkeletonAnimMap* _init_animMap(SkeletalMeshPropertyManager& skelMeshProps, Mesh* pMesh);
		bool initSettings(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

	protected:

		void updateCollisions(BodyCollisionListener* pFootballerCollListener = NULL, void* pContext = NULL);
		CollisionResolutionMode getDefaultCollResolveMode(bool penetrateFootballers = false);
		CollisionResolutionMode resolveCollisions(const CollisionResolutionMode& resolutionMode, const bool& allowSaveCurrTransformLocal, bool& wasChanged, const Vector3& posDiffHint = Vector3::kZero);
		bool resolveCollisions_Resolve(const Vector3& posDiffHint, bool& wasChanged);

	protected:

		CollRegistry::Index getCollRecordCount(CollRegistry::Index* pFootballerRecordCount, BodyCollisionListener* pFootballerCollListener, void* pContext);

	protected:
	
		friend class Footballer_BodyAction_Idle;
		friend class Footballer_BodyAction_Running;
		friend class Footballer_BodyAction_Tackling;
		friend class Footballer_BodyAction_Falling;
		friend class Footballer_BodyAction_Jumping;

	protected:

		SoftPtr<ActionImpl> mAction;

		Footballer_BodyAction_Idle mActionIdle;
		Footballer_BodyAction_Running mActionRunning;
		Footballer_BodyAction_Tackling mActionTackling;
		Footballer_BodyAction_Falling mActionFalling;
		Footballer_BodyAction_Jumping mActionJumping;

	protected:

		MeshInstance mMeshInst;
		SkeletonInstance mSkelInst;
		SoftPtr<SkeletonAnimMap> mAnimMap;

		DynamicVolumeCollider mCollider;
		CollRegistry mCollRegistry;
		
		CollRegistry::Index mCollRecordCount;
		CollRegistry::Index mFootballerCollRecordCount;

		CollisionResolutionMode mCollResolutionMode;
		
		bool mSavedTransformLocalValid;
		SceneTransform mSavedTransformLocal;

		bool mActiveCamVisible;

		BodyCommEvents mBodyCommEvents;
	};

} }

#endif

#endif