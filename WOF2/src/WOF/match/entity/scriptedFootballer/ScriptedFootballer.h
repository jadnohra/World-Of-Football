#ifndef h_WOF_match_ScriptedFootballer
#define h_WOF_match_ScriptedFootballer

#include "WE3/script/WEScriptArgByRef.h"
#include "WE3/math/WEIntersection.h"
using namespace WE;

#include "../scriptedCharacter/SceneScriptedCharacter.h"
#include "../../switchControl/ControlSwitchStates.h"
#include "../ball/BallListener.h"
#include "../../spatialQuery/BallInterceptEstimate.h"
#include "../../spatialQuery/SpatialMatchState.h"
#include "../../spatialQuery/PitchRegionShape.h"

namespace WOF { namespace match {

	class Ball;
	class Player;
	class Team;

	class ScriptedFootballer : public SceneScriptedCharacter, public BallOwner {
	public:

		static inline ScriptedFootballer* footballerScriptedFromObject(Object* pObject) {
			return (ScriptedFootballer*) pObject;
		}

		static inline ScriptedFootballer* footballerScriptedFromNode(SceneNode* pNode) {
			return (ScriptedFootballer*) pNode;
		}

	public:

		enum SimulEstimationState {

			SimulEst_None = -1, SimulEst_Wait, SimulEst_True, SimulEst_False
		};

		struct TriggerVolume {

			unsigned int ID;
			Point localPos;
			Point worldPos;
			float radiusSq;
			SoftPtr<SceneTransform> coordFrame;

			void init(const unsigned int& ID, SceneTransform* pTransform, CtVector3& localPos, const float& radius);

			inline void update() { coordFrame->transformPoint(localPos, worldPos); }
			inline bool contains(const Point& pt) { return (sphereSquaredContains(worldPos, radiusSq, pt)); }
			inline bool containsSq(const Point& pt, const float& ptRadiusSq) { return (sphereSquaredContains(worldPos, radiusSq + ptRadiusSq, pt)); }
		};

		
	public:

		ScriptedFootballer();

		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType = Node_Footballer);
		void init_prepareScene();

		void frameMove(const Time& time, const Time& dt);

		BallInteraction& getBallInteraction() { return mBallInteraction; }
		inline bool isBallOwner() { return mBallInteraction.ownership.type != BallOwnership_None; }
		inline bool isBallSoleOwner() { return mBallInteraction.ownership.type == BallOwnership_Solo; }
		inline bool isBallSharedOwner() { return mBallInteraction.ownership.type == BallOwnership_Shared; }
		bool isLastBallOwner();
		float getDefaultRunSpeed();

		bool attachPlayer(Input::Controller* pController, Player* pPlayer, const Time& attachTime);
		bool detachPlayer();

		inline Player* getPlayer() { return mPlayer; }
		inline const Time& getPlayerAttachTime() { return mPlayerAttachTime; }

		inline void setRelativeCOCOffset(CtVector3& offset) { mRelativeCOCOffset = offset; }
		inline const Vector3& getRelativeCOCOffset() { return mRelativeCOCOffset; }
		inline CtVector3& getRelativeCOCOffsetCt() { return mRelativeCOCOffset; }
		inline CtVector3* getRelativeCOCOffsetCtPtr() { return &mRelativeCOCOffset; }

		void executeTriggerVolumesOnBall();

		bool isInVOC(const Vector3& pos, const float& radius);
		bool isInVOC(Ball& ball);

		bool isBallAfterTouchOwner();
		bool isSwitchLocked();
		bool isAskingForManualSwitch();
		
		static float calcBallRelHorizDistSq(CtVector3& footballerPos, CtVector3& ballPos, const float& ballRadius);
		static float calcBallRelHeight(CtVector3& footballerPos, CtVector3& ballPos, const float& ballRadius);

		virtual void addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, bool executeMaterials);

		virtual void render(Renderer& renderer, const bool& cleanIfNeeded, const bool& flagExtraRenders, RenderPassEnum pass);

		virtual bool loadTransform(BufferString& tempStr, Match& match, DataChunk* pChunk, CoordSysConv* pConv);

		virtual void onBallCollision(Ball& ball, Object& object, const unsigned int& objectPartID);
		virtual void onBallCommand(BallCommand& command);
		virtual void onBallAcquiredAfterTouch(Ball& ball);
		virtual void onBallLostAfterTouch(Ball& ball);

		inline void onLostBall(Ball& ball) { scriptedOnLostBall(ball); }
		inline void onAcquiredBall(Ball& ball, bool isSolo) { scriptedOnAcquiredBall(ball, isSolo); }

		void onBallCommandRejected(Ball& ball, const unsigned int& commandID);

		bool addTriggerVolume(const unsigned int& ID, const TCHAR* boneName, CtVector3& position, const float& radius);
		void removeTriggerVolume(unsigned int ID);
		void clearTriggerVolumes();

		inline bool getShowBar() { return mShowBar; }
		inline float getBarValue() { return mBarValue; }
		inline float getBarCancelValue() { return mBarCancelValue; }

		bool getFormationPos(Vector3& outPos);
		CtVector3& getTeamForwardDir();
		bool isTeamWithForwardDir();

		/*
		void tacticalToWorldPos(CtVector2& inPos, Vector3& outPos);
		void tacticalToWorldDir(CtVector2& inDir, Vector3& outDir);
		void worldToTacticalPos(CtVector3& inPos, Vector2& outPos);
		void worldToTacticalDir(CtVector3& inDir, Vector2& outDir);
		void tacticalToWorldPosEx(CtVector2& inPos, Vector3& outPos, int refTeam);
		void tacticalToWorldDirEx(CtVector2& inDir, Vector3& outDir, int refTeam);
		void worldToTacticalPosEx(CtVector3& inPos, Vector2& outPos, int refTeam);
		void worldToTacticalDirEx(CtVector3& inDir, Vector2& outDir, int refTeam);
		*/

	public:


		void scriptedOnLostBall(Ball& ball);
		void scriptedOnAcquiredBall(Ball& ball, bool isSolo);

	public:

		MDeclareMatchSceneScriptedCharacter_ScriptFuncs
		
		inline int script_getNumber() { return mNumber; }

		inline BallInteraction* script_getBallInteraction() { return &mBallInteraction; }
		//void script_setBallCollGhosting(bool ghostUpperBody, bool ghostLowerBody);

		//applies only to current frame
		inline void script_setBallTransparent(bool value) { mBallTransparent = value; }
		inline bool script_isBallTransparent() { return mBallTransparent; }

		inline bool script_isBallOwner() { return isBallOwner(); }
		inline bool script_isBallSoleOwner() { return isBallSoleOwner(); }
		inline bool script_isBallSharedOwner() { return isBallSharedOwner(); }
		inline bool script_isBallAfterTouchOwner() { return isBallAfterTouchOwner(); }
		void script_giveUpBallAfterTouch();
		bool script_requestBallAfterTouch(unsigned int commandID, CtVector3& acceleration, float sideMag, float upMag, float downMag, float rotSyncRatio, bool transformFwdToUp);

		inline bool script_addTriggerVolume(unsigned int ID, const TCHAR* boneName, CtVector3& position, float radius) { return addTriggerVolume(ID, boneName, position, radius); }
		//void script_updateBallControlVolumeIntersections();

	
		//idealBallActionPosOffset: ideal ball offset from player position
		//footballerPosDiff: the posDiff the footballer already moved in this frame
		bool script_needsShootBall(CtVector3& idealBallActionPosOffset, CtVector3& idealBallDir, CtVector3& footballerPosDiff, bool footballerDirChanged, float epsilon);
		bool script_shouldTriggerBallDribbleAction(SceneTransform& currTransform, SceneTransform& nextTransform, CtVector3& ballSweetSpot, CtVector3& wantedBallDir, float ballPosEpsilon, float runDirEpsilon);
		
		/*
		CtVector3* script_getBallPos();
		Vector3 script_getRelativeBallPos();
		Vector3 script_getNextBallPos();
		Vector3 script_getRelativeNextBallPos();
		*/
		
		float script_getCOCFwdReach();

		float script_calcBallCorrectionDist(CtVector3& refPos, CtVector3& refDir, CtVector3& pos);
		void script_correctBallShotVelocity(float correction, Vector3& inOutVelocity);

		/*
		float script_calcBallRelHorizDistSq(Ball* pBall);
		float script_calcBallRelHeight(Ball* pBall);

		float script_calcBallPosRelHorizDistSq(Ball* pBall, CtVector3& ballPos);
		float script_calcBallPosRelHeight(Ball* pBall, CtVector3& ballPos);
		*/

		void script_controlBall(unsigned int commandID, float controlStrength, CtVector3& targetPoint, float totalTime, float timeLeft, bool rotate);
		void script_controlBall2(unsigned int commandID, float controlStrength, CtVector3& targetPoint, float totalTime, float timeLeft, bool rotate, Vector3* pOutNextBallPos);
		//void script_shootBall(unsigned int commandID, float controlStrength, CtVector3& vel, float rotSyncRatio, float blendCurrRotRatio, float switchControlInfluence, bool enableAfterTouch);

		//bool script_isBallAtRestNearTargetPoint(CtVector3& targetPoint, float epsilon);

		int script_simulEstBallWillTouchPitchInCOC();
		bool script_simulNextBallSample(Ball& ball, BallSimulation::TrajSample& sample);

		void script_cancelBallControl();
		void script_setBallControlImmediateTeleport(unsigned int commandID, float controlStrength, bool ghostBall, CtVector3& targetPos, bool isLocalTargetPos, bool syncRotation);
		void script_setBallControlSmoothTeleport(unsigned int commandID, float controlStrength, bool ghostBall, float speed, float maxDuration, CtVector3& targetPos, bool isLocalTargetPos, bool syncRotation);
		void script_setBallControlImmediateVel(unsigned int commandID, float controlStrength, bool ghostBall, CtVector3& vel, CtVector3& w, float switchControlInfluence, bool enableAfterTouch);
		void script_setBallControlImmediateGuard(unsigned int commandID, float controlStrength);
		void script_setBallControlGhostedShot(unsigned int commandID, float controlStrength, CtVector3& vel, CtVector3& w, float switchControlInfluence, bool enableAfterTouch);
		bool script_isShootingBall();

		unsigned int script_getFrameBallCommandCount();

		inline bool script_isSelf(ScriptGameObject* pObject) { return dref(pObject).getObject() == (Object*) this; }

		unsigned int script_genBallCommandID();

		void script_showBar(float value, float cancelInterval);
		void script_hideBar();

		inline int script_getTeamEnum() { return mTeam; }
		inline int script_getOppTeamEnum() { return opponentTeam(mTeam); }
		Team* script_getTeam();
		Team* script_getOppTeam();
		inline void script_requestManualSwitch() { mIsAskingForManualSwitch = true; }

		const TCHAR* script_getFormationPositionCode();
		ScriptDataChunk script_getFormationPositionChunk();
		

		PitchRegionShape* script_getRegion(const TCHAR* name);
		PitchRegionShape* script_getRegionPitch();
		PitchRegionShape* script_getRegionPenalty();
		PitchRegionShape* script_getRegionOppPenalty();

		unsigned int script_getSpatialStateBallDistIndex() { return mForeignSpatialState.indexBallDist; }

	protected:

		void cancelBallControls(bool notify);

		void ballControlFrameMove(const Time& time, const Time& dt);
		void ballControlFrameMoveSmoothTeleport(const Time& time, const Time& dt);
		void ballControlFrameMoveGhostedShot(const Time& time, const Time& dt);

		//void getNextBallPos(Vector3& pos);

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);

	public:

		inline SpatialMatchState::FootballerState& getSpatialState() { return mForeignSpatialState; }
		inline SwitchControl::FootballerState getSwitchControlState() { return mForeignSwitchControlState; }

	public:

		typedef WETL::CountedArray<StringHash, false, unsigned int, WETL::ResizeExact> BoneGroups;
				

		TeamEnum mTeam;
		FootballerNumber mNumber;
		StringHash mTeamFormationCode;

		Vector3 mRelativeCOCOffset;

		BallInteraction mBallInteraction;
		SpatialMatchState::FootballerState mForeignSpatialState;
		SwitchControl::FootballerState mForeignSwitchControlState;

		SoftPtr<Player> mPlayer;
		Time mPlayerAttachTime;

		bool mBallTransparent;
		bool mExecutingScriptBallSelfCollision;
		bool mIsAskingForManualSwitch;

	protected:

		typedef WETL::CountedArray<TriggerVolume, false, unsigned int> TriggerVolumes;


		TriggerVolumes mTriggerVolumes;

	protected:

		enum BallControlMode { BallControl_None = -1, BallControl_SmoothTeleport, BallControl_GhostedShot };

		BallControlMode	mBallControlMode;

		unsigned int mBallControlFrameCommandCount;

		Vector3 mBallControlTargetPos;
		unsigned int mBallControlCommandID;
		Time mBallControlDuration;
		Time mBallControlTimeLeft;
		bool mBallControlSyncRotation;
		bool mBallControlGhostBall;
		float mBallControlStrength;

		bool mBallControlSmoothTeleportStabilizeEnd;
		bool mBallControlSmoothTeleportIsStabilizingEnd;

		bool mShowBar;
		float mBarValue;
		float mBarCancelValue;
	};

	inline ScriptedFootballer* scriptArg(ScriptedFootballer* pFootballer) { return pFootballer; }

	class FootballerSavedSpatialState {
	public:
		
		void init(ScriptedFootballer& footballer);

		inline SceneTransform* getWorldTransformPtr() { return &mWorldTransform; }

	public:

		SceneTransform mWorldTransform;

	public:
		
		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);
	};

} }

#endif