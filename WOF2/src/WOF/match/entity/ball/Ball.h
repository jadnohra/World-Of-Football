#ifndef h_WOF_match_Ball
#define h_WOF_match_Ball

#include "WE3/WEPtr.h"
#include "WE3/phys/WEPhysRigidBody.h"
#include "WE3/math/WESphere.h"
#include "WE3/mesh/WEMeshInstance.h"
#include "WE3/WETL.h"
using namespace WE;

#include "../../scene/SceneNode.h"
//#include "../components/WOFMatchSceneNodeComp_Collider.h"

#include "controller/BallControllerPhysImpl.h"
#include "controller/BallControllerFPS.h"
#include "controller/BallControllerBoneAttach.h"

#include "../../collision/CollRegistry.h"
#include "../../collision/Colliders.h"

#include "BallPhysEnv.h"
#include "../../DataTypes.h"
#include "BallListener.h"
#include "BallCommandManager.h"
#include "../../../script/ScriptEngine.h"


namespace WOF { namespace match {

	class Match;
	class Footballer;
	class BallSimulation;

	class Ball : public SceneNode {
	public:

		static inline Ball* ballFromObject(Object* pObject) {
			return (Ball*) pObject;
		}

		static inline Ball* ballFromNode(SceneNode* pNode) {
			return (Ball*) pNode;
		}

	public:

		enum SimulableState {

			Simulable_None = -1, Simulable_Phys, Simulable_PhysAfterTouch
		};

		struct MatchStats {

			struct TeamOwnership {

				TeamEnum team;

				Time timeStamp;
				Time totalTime;
			};

			TeamOwnership ownership[2]; //index by team enum
		};

		enum ImmediateCollisionResponseType {

			ImmCollResp_None = -1, ImmCollResp_Continue, ImmCollResp_IgnoreCollAndContinute, ImmCollResp_ExitFrameMove
		};

	public:

		Ball();
		virtual ~Ball();

		bool _init_mesh(Mesh& mesh);
		bool _init_collider(const WorldMaterialID& collMaterial);

		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene();

		void clean();

		BallControllerPhysImpl* getActiveControllerPhys();
		BallControllerFPS* getActiveControllerFPS();
		BallControllerPhysImpl* getControllerPhys();

		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass);
		virtual bool isCameraIdle() { return isResting(); }
	
		void bindController(BallController* pController);
		
		void frameMove(const Clock& time);

		inline Time& getSimulStateTime() { return mSimulStateTime; }

		inline BallPhysEnv& getPhysEnv() { return mPhysEnv; }

		Ball* clone(const TCHAR* cloneName, ObjectType objType = Node_SimulBall, bool doClonePrepareScene = true);

		void syncCamera();

		float getRadius(bool meters);
		inline const float& getRadius() const { return mSphereLocal.radius; }
		inline const bool& isResting() const { return mIsResting; } //resting is only momentaneous, the ball could start moving afterwards
		inline const unsigned int& getRestingID() const { return mRestingID; }

		inline const bool& isOnPitch() const { return mIsOnPitch; } 

		void setVisible(bool visible) { mIsVisible = visible; }

		inline void setIsInPlay(bool isInPlay) { mIsInPlay = isInPlay; }
		inline bool isInPlay() { return mIsInPlay && !mIsAlienControlled; }
		bool isSimulable();
		const SimulableState& getSimulableState() { return mSimulableState; }

		inline BallCommandManager& getCommandManager() { return mCommandManager; }
		inline bool hasAfterTouchOwner() { return mAfterTouchOwner.isValid(); }
		inline bool isAfterTouchOwner(BallOwner* pObject) { return mAfterTouchOwner.isValid() && (mAfterTouchOwner == pObject); }
		bool isAfterTouchOwner(const BallCommand* pCommand);
		void giveUpAfterTouch(BallOwner* pCurrOwner); 
		
		inline Vector3& accessAfterTouchVel() { return mAfterTouchVel; }
		inline Vector3& accessAfterTouchPosDiff() { return mAfterTouchPosDiff; }
		
		inline const bool& hasAfterTouchPosDiff() const { return mHasAfterTouchPosDiff; }
		inline const Vector3& getAfterTouchPosDiff() const { return mAfterTouchPosDiff; }
		inline const Time& getAfterTouchPosDiffStartTime() const { return mAfterTouchPosDiffStartTime; }

		void calcVelocitySyncedW(const CtVector3& vel, const CtVector3& crossAxis, Vector3& w);

		bool isAttachedTo(SceneNode* pNode);

		bool processWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:

		void addListener(BallSimulSyncListener& listener) { mSimulSyncListeners.addOne(&listener); }
		void removeListener(BallSimulSyncListener& listener);
		void notifySimulListeners(const BallCommand* pCommand);
		void notifySimulListeners();

	public:

		void updateFootballerOwnerships(Match& match, const Clock& time);
		inline const FootballerIndex& getOwnerCount() { return mOwners.count; }
		inline Footballer* getOwner(const FootballerIndex& index) { return mOwners[index]; }

		inline const FootballerIndex& getTeamOwnerCount(TeamEnum team) { return mTeamOwnerCounts[team]; }

	protected:

		bool init_nodeAttached();
		
		bool initClone_create(Ball& clone, const TCHAR* cloneName, ObjectType objType);
		bool _initClone_mesh(Ball& clone);
		bool _initClone_collider(Ball& clone);
		
		bool initClone_nodeAttached(Ball& clone);
		bool initClone_nodeAttach(Ball& clone);

		bool initClone_prepareScene(Ball& clone);

	public:

		void physMarkDesynced();

		void physSyncNodeFromPhysBody();
		void physSyncPhysBodyFromNode();

		void physGetSweepMoveIntersections(const Vector3& sweepVector, SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials);

		void physSetResting(bool resting);
		void physSetIsOnPitch(bool value) { mIsOnPitch = value; }

		inline const PhysRigidBody& getPhysBody() const { return mPhysBody; }
		
		/*
		inline void onPhysCollision(Object& object, const unsigned int& objectPartID) { 
	
			if (objectIsDynamic(object))
				notifySimulListeners(NULL);

			if (mOwners.count || mAfterTouchOwner.isValid()) 
				notifyPhysCollisionImpl(object, objectPartID); 
			else {

				if (object.objectType == Node_Footballer) {

					notifyFootballerBallColision(object, objectPartID);
				}
			}
		}
		*/

		inline ImmediateCollisionResponseType onPrePhysCollision(Object& object, const unsigned int& objectPartID) { return ImmCollResp_Continue; }
		ImmediateCollisionResponseType onPostPhysCollision(Object& object, const unsigned int& objectPartID);
		
		inline BallCommand& getImmediateCollResponseCommandRef() { return mImmediateCollResponseCommand; }

		inline const Vector3& getPos() {  return mControllerPhys.isAttached() ? mPhysBody.pos : (mTransformWorld.row[3]);  }
		inline CtVector3* getVel() {  return mControllerPhys.isAttached() ? &mPhysBody.v : &Vector3::kZero;  }
		inline CtVector3* getW() {  return mControllerPhys.isAttached() ? &mPhysBody.w : &Vector3::kZero;  }

	protected:

		bool executeCommand(BallCommand* pCommand);
		void postExecuteCommand(BallCommand* pCommand);

		void notifyFootballerBallColision(Object& object, const unsigned int& objectPartID);
		void notifyPhysCollisionImpl(Object& object, const unsigned int& objectPartID);
		void notifyCommand(BallCommand& command);
		
		void setAfterTouchOwner(BallOwner* pObject);

	public:

		bool mPhysIsDesynced;
		PhysRigidBody mPhysBody;
		BallPhysEnv mPhysEnv;

		bool mIsVisible;

		MeshInstance mMeshInst;
		DynamicVolumeCollider mCollider;
		Sphere mSphereLocal;

		unsigned int mRestingID;
		bool mIsResting;
		//bool mIsSimulable;
		SimulableState mSimulableState;
		bool mIsInPlay;
		bool mIsAlienControlled;

		bool mIsOnPitch;

		SoftPtr<BallController> mController;

		BallControllerFPS mControllerFPS;
		BallControllerPhysImpl mControllerPhys;
		BallControllerBoneAttach mControllerBoneAttach;

		Vector3 mStartPos;
		Vector3 mReplayPos;
		Vector3 mReplayDir;
		Vector3 mReplayV;
		Vector3 mReplayW;

		Time mSimulStateTime;

		float mMaxV;

		CollRegistry mCollRegistry;

		BallCommand mImmediateCollResponseCommand;

		SceneTransform mLastTransformWorld;

	protected:

		typedef WETL::CountedArray<BallSimulSyncListener*, false, unsigned int> SimulSyncListeners;
		typedef WETL::CountedArray<Footballer*, false, unsigned int> Footballers;
		typedef Footballers Owners;

		SimulSyncListeners mSimulSyncListeners;
		Footballers mOwners;
		FootballerIndex mTeamOwnerCounts[2];

		HardPtr<BallCommandManager> mCommandManager;
		SoftPtr<BallOwner> mAfterTouchOwner;
		bool mHasAfterTouchPosDiff;
		Vector3 mAfterTouchVel;
		Vector3 mAfterTouchPosDiff;
		Time mAfterTouchPosDiffStartTime;

	protected:

		bool _init_volumeLocal();

	public:

		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);

	public:

		inline void script_test() {}
		inline float script_getRadius() { return getRadius(); }
		inline bool script_isResting() { return isResting(); }
		inline bool script_isOnPitch() { return isOnPitch(); }
		CtVector3* script_getPos();
		inline CtVector3* script_getVel() { return getVel(); }
		inline CtVector3* script_getW() { return getW(); }

		inline bool script_hasAfterTouchPosDiff() { return mHasAfterTouchPosDiff; }
		inline CtVector3* script_getAfterTouchPosDiff() { return &mAfterTouchPosDiff; }

		inline void script_calcVelocitySyncedW(CtVector3& vel, CtVector3& crossAxis, Vector3& w) { calcVelocitySyncedW(vel, crossAxis, w); }

		inline unsigned int script_getOwnerCount() { return getOwnerCount(); }
		inline unsigned int script_getTeamOwnerCount(int team) { return getTeamOwnerCount((TeamEnum) team); }
		ScriptObject script_getOwner(unsigned int index);
	};

	inline Ball* scriptArg(Ball& ball) { return &ball; }

} }

#endif