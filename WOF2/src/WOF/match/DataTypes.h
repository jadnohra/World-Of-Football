#ifndef h_WOF_match_DataTypes
#define h_WOF_match_DataTypes

#include "WE3/scene/WESceneDataTypes.h"
#include "WE3/object/WEObject.h"
#include "WE3/math/WEVector.h"
#include "WE3/math/WEVector2.h"
#include "WE3/math/WEMatrix2.h"
#include "WE3/scene/WESceneDirection.h"
#include "WE3/scene/WESceneTransform.h"
#include "WE3/WETime.h"
#include "WE3/render/WERenderSprite.h"
using namespace WE;

#include "../script/ScriptEngine.h"

namespace WOF { namespace match {

	class ScriptedFootballer;
	class Footballer;
	class FootballerSavedSpatialState;
	class Ball;
	class Match;
	class PitchRegionShape;
	
	typedef int FootballerNumber;
	const FootballerNumber FootballerNumberInvalid = (FootballerNumber) -1;

	typedef unsigned int FootballerIndex;
	typedef unsigned int PlayerIndex;

	const FootballerIndex FootballerIndexInvalid = (FootballerIndex) -1;

	enum TeamEnum {

		Team_Invalid = -1, Team_A = 0, Team_B
	};

	enum TriggerIDEnum {

		TriggerID_None = -1, TriggerID_Goal
	};

	inline TeamEnum opponentTeam(TeamEnum team) { return (TeamEnum) ((team + 1) % 2); }

	enum SceneNodeClassID {

		NodeClassID_None = -1, NodeClassID_Node = 0, NodeClassID_EntMesh
	};


	enum SceneNodeType {

		Node_None = -1, Node_Node = 0, Node_Camera, Node_EntMesh, Node_Ball, Node_SimulBall, Node_Footballer, Node_Goal, Node_Pitch, Node_PitchLimit, Node_CamPoint, Node_Light, Node_ScriptedCharacter, Node_DynVol, Node_ScripedCharacterPositionProxy, Node_GoalProxy, Node_EntMeshNoScriptedCharacterColl, Node_SimulPitch, _Node_Count
	};

	inline bool typeIsProxy(const ObjectType& type) {

		return (type == Node_ScripedCharacterPositionProxy || type == Node_GoalProxy);
	}

	inline bool typeIsEntMesh(const ObjectType& type) {

		return (type == Node_EntMesh || type == Node_Footballer);
	}

	inline bool typeIsScriptedCharacter(const ObjectType& type) {

		return (type == Node_ScriptedCharacter || typeIsEntMesh(type));
	}

	inline bool objectIsProxy(const Object& object) {

		return typeIsProxy(object.objectType);
	}

	
	inline bool objectIsDynamic(const Object& object) {

		return (object.objectType == Node_Footballer);
	}
	
	class SceneDynVol;

	bool objectHasProxy(Object* pObject);
	SceneDynVol* objectGetProxy(Object* pObject);



	enum SceneNode2Type {

		MNT2_None = -1, MNT2_Node = 0, MNT2_NodeMovable, MNT2_NodeMesh
	};

	enum SceneComponent {
		SC_Loadable = 0, SC_WorldNode, SC_Renderable, 
	};

	enum FootballerSwitchMode {

		FSM_None = 0, FSM_Automatic, FSM_OnShootAction, FSM_Manual
	};

	enum FootballerSwitchTechnique {

		FST_Default = 0, FST_NextNumber, FST_BallDist, FST_ShotAnalysis
	};

	struct PitchInfo {

		bool isValid;

		float height;

		float width;
		float length;

		float halfWidth;
		float halfLength;

		float penaltyBoxWidth;
		float penaltyBoxLength;

		float penaltyBoxHalfWidth;
		float penaltyBoxHalfLength;

		float penaltyBoxFwdMax;

		float goalBoxWidth;
		float goalBoxLength;

		float goalBoxHalfWidth;
		float goalBoxHalfLength;

		float goalBoxFwdMax;

		float goalHeight;
		float goalWidth;
		float goalHalfWidth;

		Plane pitchPlane;

		HardPtr<PitchRegionShape> regionPitch;
		HardPtr<PitchRegionShape> regionPenaltyBack;
		HardPtr<PitchRegionShape> regionGoalBoxBack;
		HardPtr<PitchRegionShape> regionPenaltyFwd;
		HardPtr<PitchRegionShape> regionGoalBoxFwd;

		Vector3 goalCenterBack;
		Vector3 goalSideBarBackLeft;
		Vector3 goalSideBarBackRight;

		Vector3 goalCenterFwd;
		Vector3 goalSideBarFwdLeft;
		Vector3 goalSideBarFwdRight;

		Vector2 goalCenterBack2D;
		Vector2 goalSideBarBackLeft2D;
		Vector2 goalSideBarBackRight2D;

		Vector2 goalCenterFwd2D;
		Vector2 goalSideBarFwdLeft2D;
		Vector2 goalSideBarFwdRight2D;

		PitchInfo();

		void update(Match& match);

		inline bool isValidWidthLength() const { return width > 0.0f && length > 0.0f; }
		
		bool contains(CtVector2& from, Vector2& exitPoint, CtVector2* pTo = NULL, CoordSysDirection* pExitDirSide = NULL, CoordSysDirection* pExitDirFwd = NULL);
	};

	struct SwitchControlSetup {

		FootballerSwitchMode mode;

		bool veryDynamic;

		float footballerReachHeight;
		float playerReactionTime;
		float pathInterceptDistTolerance;

		float switchInfluenceTimeMultiplier;
		float playerControlEffectInterval;
		float pathInterceptTimeCompareTolerance;
		float pathInterceptDistCompareTolerance;
		float ballDistCompareTolerance;
		float minSwitchInterval;
		float lazyUpdateInterval;

		FootballerSwitchTechnique technique;
		//bool fallbackToBallDist;
	};


	typedef IndexT<ObjectIndex> SceneNodeIndex;
	typedef int CollRecordIndex;

	typedef unsigned int SimulationID;



	struct BallRelativeDist {

		Vector3 COCOffset; 
		Vector3 footballerOffset; 

		float COCHorizDist; 
		float COCDist; 

		bool inFrontOfFootballer;
		float footballerDist;
		float footballerHorizDist;
		float footballerHorizAngle;

		bool isInVOC;
	
		inline const Vector3& getCOCOffset() { return COCOffset; } 
		inline const float& getCOCDist() { return COCDist; } 
		inline const float& getCOCHorizDist() { return COCHorizDist; } 
		inline const float& getHeight() { return COCOffset.el[Scene_Up]; } 

		inline const Vector3& getFootballerOffset() { return footballerOffset; } 
		inline const float& getFootballerDist() { return footballerDist; } 
		inline const float& getFootballerHorizAngle() { return footballerHorizAngle; } 
		inline const bool& isInFrontOfFootballer() { return inFrontOfFootballer; } 
				
		inline float script_getCOCDist() { return getCOCDist(); } 
		inline float script_getBallHeight() { return footballerOffset.el[Scene_Up]; } 
		inline float script_getFootballerHorizDist() { return footballerHorizDist; } 

		void script_toBallRelativeRist(Ball& ball, CtVector3& ballPos, ScriptedFootballer& footballer);
		
		bool isInCOC(Match& match);
		static bool calcIsInCOC(Match& match, SceneTransform& footballerTransform, CtVector3& footballerRelCOCOffset, CtVector3& ballPos, float ballRadius);
		static bool calcIsInCOC2(Match& match, SceneTransform& footballerTransform, CtVector3& footballerRelCOCOffset, CtVector3& ballPos, float ballRadius, bool& isInFront);
	
		void update(Match& match, SceneTransform& footballerTransform, CtVector3& footballerRelCOCOffset, CtVector3& ballPos, float ballRadius);

		static float calcFootballerBallHorizDistSq(CtVector3& footballerPos, CtVector3& _ballPos, float ballRadius);
		static float calcFootballerBallHeight(CtVector3& footballerPos, CtVector3& _ballPos, float ballRadius);

		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);
	};

	enum BallOwnershipType {

		BallOwnership_None = -1, BallOwnership_Shared, BallOwnership_Solo
	};

	struct BallOwnership {

		Time ownershipStartTime;
		BallOwnershipType type;

		BallOwnership() : type(BallOwnership_None) {}

		inline int getType() { return type;  }
		inline void setType(int newType) { type = (BallOwnershipType) newType;  }

		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);
	};

	struct COCState {

		bool needChangeDirToActivateDribble;

		float lastBallToSpotDistSq;
		bool ballCollided;

		bool idleBallHandled;
	};

	enum BallInteractionType {

		BallInteract_None = -1, BallInteract_ActiveShot, BallInteract_PassiveShot, BallInteract_Controlled, BallInteract_UncontrolledCollision
	};

	struct BallInteraction {

		BallOwnership ownership;
		BallRelativeDist relativeDist;

		BallInteractionType lastInteraction;
		Time lastInteractionTime;

		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);
	};

	class ScriptGameObject {
	public:

		ScriptGameObject() {}
		ScriptGameObject(Object& object) : mObject(&object) {}
		ScriptGameObject(Object* pObject) : mObject(pObject) {}
		inline ScriptGameObject& operator=(const ScriptGameObject& ref) { mObject.set(ref.mObject.ptr()); return *this; }
	
		inline bool equals(ScriptGameObject& comp) { return mObject.ptr() == comp.mObject.ptr(); }
		inline Object* getObject() { return mObject; }
		inline bool isPitch() { return (mObject->objectType == Node_Pitch); }
		inline bool isNull() { return !(mObject.isValid()); }
		inline bool isFootballer() { return mObject.isValid() && mObject->objectType == Node_Footballer; }

		ScriptObject script_toFootballer();
		
	public:

		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);

	protected:

		SoftPtr<Object> mObject;
	};

	inline ScriptGameObject scriptArg(Object& val) { return ScriptGameObject(val); }
	inline ScriptGameObject scriptArg(Object* pVal) { return ScriptGameObject(pVal); }

	class COCSkin {
	public:

		HardPtr<RenderSprite::Skin> COC;
		HardPtr<RenderSprite::Skin> activeCOC;
	};

	class PlayerSkin : public COCSkin {
	public:

		HardPtr<RenderSprite::Skin> scanner;
		HardPtr<RenderSprite::Skin> arrow;
	};

	class COCSetup {
	public:

		float radius;
		float height;
		float maxAngle;

		float defaultAlpha;
		String texturesPath;

		RenderSprite::Skin defaultSkin;

		typedef WETL::CountedPtrArrayEx<COCSkin, unsigned int> Skins;

		Skins skins;
	};


	struct SoftCOCSkin {

		RenderSprite::SoftSkin COC;
		RenderSprite::SoftSkin activeCOC;

		void setIfEmpty(COCSkin* pSkin, bool fallbackActiveCOC = false);
	};

	struct SoftPlayerSkin : SoftCOCSkin {

		RenderSprite::SoftSkin scanner;
		RenderSprite::SoftSkin arrow;

		void setIfEmpty(PlayerSkin* pSkin, bool fallbackActiveCOC = false, bool fallbackScanner = false, bool fallbackArrow = false);
	};

} }

#endif