#include "select.h"
#ifdef FOOTBALLER_BODY2

#ifndef _WOFMatchFootballer_Body2_h
#define _WOFMatchFootballer_Body2_h

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

#ifndef _DEBUG
#include "WOFMatchFootballer_BodyActions.h"
#endif


namespace WOF { namespace match {

	class Footballer;
	class Team;

	class Footballer_BodyActions;
	class Footballer_Body;

	class Footballer_Body : public FootballerBodyDefs, public Footballer_Base, public SceneNode {
	public:
		
		Footballer_Body();
		~Footballer_Body();

		inline void setStamina(const float& stamina) { mStamina = stamina; }

		void requestHeadFacing(const Vector3& val);
		inline const Facing& getHeadFacing() { return mHeadFacing; }

		void requestBodyFacing(const Vector3& val);
		inline const Facing& getBodyFacing() { return mBodyFacing; }
		
		inline const Action& getAction() { return mAction; }
		inline const ActionType& getActionType() { return mAction.value; }
		
		void requestActionIdle();
		void requestActionRunning();
		void requestActionShooting();
		void requestActionTackling();

		inline void queueActionNone() { mQueuedAction = Action_None; }
		inline void queueActionIdle() { mQueuedAction = Action_Idle; }
		inline void queueActionRunning() { mQueuedAction = Action_Running; }
		inline void queueActionShooting() { mQueuedAction = Action_Shooting; }
		inline void queueActionTackling() { mQueuedAction = Action_Tackling; }

		inline const ActionType& getQueuedAction() { return mQueuedAction; }

		//returns true if a request was actually made
		bool requestActionIdleIfNeeded();
		bool requestActionRunningIfNeeded();
		bool requestActionShootingIfNeeded();
		bool requestActionTacklingIfNeeded();

		void requestRunDir(const Vector3& val);
		void requestRunSpeed(const float& val);

		inline const Variable_Vector3& getRunDir() { return mRunDir; }
		inline const Variable_float& getRunSpeed() { return mRunSpeed; }

		float estimateActionSwitchTime();
		float estimateActionSwitchTimeTo(const ActionType& target);

		//inline bool& hasBallInRange();
		//inline float& getBallControlCoeff();

		inline const Index& getBodyEventCount() { return mEvents.count; }
		inline const Event& getBodyEvent(const Index& index) { return mEvents[index]; }
		inline void clearBodyEvents() { mEvents.count = 0; }

	public:

		inline const float& getRunTypeSpeed(const RunType& type) { return mRunTypeSpeeds[type]; }

	public:

		BallOwnership mBallOwnership;
		BallRelativeInfo mBallRelativeInfo;
		BallInteraction mBallInteraction;

		inline const BallInteraction& getBallInteraction() { return mBallInteraction; }
		inline const BallRelativeInfo& getBallRelativeInfo() { return mBallRelativeInfo; }
		inline BallOwnership& getBallOwnership() { return mBallOwnership; }

	public:

		inline const bool& isActiveCamVisible() { return mActiveCamVisible; }

		inline const Vector3& getPosition() { return mTransformLocal.getPosition(); }
		inline const Vector3& getDirection(const SceneDirectionEnum& dir) { return mTransformLocal.getDirection(dir); }

		inline const AnimationIndex::Type& getAnimIndex(const AnimEnum& anim) { return mAnimMap->animIndex(anim); }
		inline SkeletonInstance& getSkelInst() { return mSkelInst; }

		/*
		inline bool animReachedEnd() { return mSkelInst.reachedAnimationEnd(); }
		inline void setAnim(AnimEnum anim, const float& speed) { mSkelInst.setAnimation(mAnimMap->animIndex(anim), speed); }
		inline void setCurrAnimSpeed(const float& speed) { mSkelInst.setSpeedFactor(speed); }
		inline float getAnimLength(AnimEnum anim) { return mSkelInst.getAnimationLength(mAnimMap->animIndex(anim)); }
		*/

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
		void initSettings(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

	protected:

		inline void addPendingRequest(const VariableType& type) { mPendingRequests.addOne(type); }
		
		inline const Index& getPendingRequestCount() { return mPendingRequests.count; }
		inline const VariableType& getPendingRequest(const Index& index) { return mPendingRequests[index]; }
		inline void clearPendingRequests() { mPendingRequests.count = 0; }

		inline Event& addEvent() { return mEvents.addOne(); }

		Event& addEvent(const EventType& _type, const CauseType& _cause) { 

			Event& evt = addEvent(); evt.set(_type, _cause); return evt; 
		}

		Event& addEvent(const EventType& _type, const VariableType& _request, CauseType _cause = Cause_None) {

			Event& evt = addEvent(); evt.set(_type, _request, _cause); return evt; 
		}

	public:

		CollRegistry::Index getCollRecordCount(CollRegistry::Index* pFootballerRecordCount);

	protected:

		void processCollisions(CollisionResolveController& controller, bool& wasChanged);

	protected:
	
		friend class Footballer_BodyActions;
		friend class Footballer_BodyAction_Idle;
		friend class Footballer_BodyAction_Running;
		friend class Footballer_BodyAction_Tackling;

	protected:

		typedef WETL::CountedArray<Event, false, Index> Events;
		typedef WETL::CountedArray<VariableType, false, Index> VariableTypes;

		Events mEvents;
		VariableTypes mPendingRequests;

		float mStamina;

		Facing mHeadFacing;
		Facing mBodyFacing;
		
		Action mAction;
		ActionType mQueuedAction;

		Variable_Vector3 mRunDir;
		Variable_float mRunSpeed;
	
		float mRunTypeSpeeds[RunType_Count];

		#ifdef _DEBUG
		HardPtr<Footballer_BodyActions> mActions;
		#else
		TransparentPtr<Footballer_BodyActions> mActions;
		#endif

	protected:

		MeshInstance mMeshInst;
		SkeletonInstance mSkelInst;
		SoftPtr<SkeletonAnimMap> mAnimMap;

		DynamicVolumeCollider mCollider;
		CollRegistry mCollRegistry;
		bool mCollIsInGhostMode;
		
		bool mSavedTransformLocalValid;
		SceneTransform mSavedTransformLocal;

		bool mActiveCamVisible;
	};

} }

#endif

#endif