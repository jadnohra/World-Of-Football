#ifndef h_WOF_match_SceneScriptedCharacter
#define h_WOF_match_SceneScriptedCharacter

#include "WE3/math/script/WEScriptVector3.h"
#include "WE3/script/WEScriptDataChunk.h"
#include "WE3/script/WEScriptArgByRef.h"
#include "WE3/coordSys/script/WEScriptCoordSysConv.h"
#include "WE3/input/script/Controller.h"
using namespace WE;

#include "../SceneEntityMesh.h"
#include "../../../script/ScriptEngine.h"

#include "../../inc/CollisionDef.h"
#include "../../collision/CollRegistry.h"


namespace WOF { namespace match {

	class SceneScriptedCharacter : public SceneEntityMesh {
	public:

		static inline SceneScriptedCharacter* entMeshScriptedFromObject(Object* pObject) {
			return (SceneScriptedCharacter*) pObject;
		}

		static inline SceneScriptedCharacter* entMeshScriptedFromNode(SceneNode* pNode) {
			return (SceneScriptedCharacter*) pNode;
		}

	public:

		enum CollState {

			Coll_None = -1, Coll_NoColls, Coll_Constrained, Coll_Blocked, Coll_Ghosted, Coll_Unresolved
		};

	public:

		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType = Node_ScriptedCharacter);
		void init_prepareScene();

		template<typename OverloadingClassT>
		bool init_create(OverloadingClassT* pThis, Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType = Node_ScriptedCharacter);

		bool isActiveCamVisible();
		inline bool isInAIMode() { return scriptedIsInAIMode(); }
		inline bool activateAIMode() { return scriptedActivateAIMode(); }
		inline bool activatePlayerMode(Input::Controller* pController) { return scriptedActivatePlayerMode(pController); }

		inline bool canActivateAIMode() { return scriptedCanActivateAIMode(); }
		inline bool canActivatePlayerMode() { return scriptedCanActivatePlayerMode(); }

		inline bool doClean() { return scriptClean(); }

	public:

		bool scriptedHasAIMode();
		bool scriptedHasPlayerMode();
		bool scriptedActivatePlayerMode(Input::Controller* pController);
		bool scriptedActivateAIMode();

		bool scriptedCanActivatePlayerMode();
		bool scriptedCanActivateAIMode();

		bool scriptedIsInAIMode();

		int scriptedGetTypeInt();
		const TCHAR* scriptedGetType();

	protected:

		void scriptedSetEntity();
		void scriptedPrepareScene();

	protected:

		bool scriptClean();

	public:

		SceneScriptedCharacter() : mCollState(Coll_None) {}
		SceneScriptedCharacter(Match& match);
		~SceneScriptedCharacter();

		ScriptObject& getScriptObject() { return mScriptObject; }

		void sanityCheck();

		void frameMove(const Time& time, const Time& dt);

	protected:

		struct CollisionProcessing {

			CollisionProcessing() : savedTransformValid(false), mCollGhostedDynamic(true), mCollGhostedStatic(true) {}

			CollRegistry collRegistry;
			CollRegistry collProxyRegistry;
			
			bool savedTransformValid;
			SceneTransform savedTransform;

			bool mCollGhostedDynamic;
			bool mCollGhostedStatic;

			inline void invalidateTransform() { savedTransformValid = false; }
			inline void saveTransform(const SceneTransform& source) { savedTransform = source; savedTransformValid = true; }
			inline bool loadTransform(SceneTransform& target) { if (savedTransformValid) target = savedTransform; return savedTransformValid; }

			inline CollRegistry& getPosCollRegistry(SceneScriptedCharacter* pThis) { return pThis->hasProxyVol() ? collProxyRegistry : collRegistry; }
			inline DynamicVolumeCollider& getPosCollider(SceneScriptedCharacter* pThis) { return pThis->hasProxyVol() ? pThis->getProxyVol()->getCollider() : pThis->mVolumeColliders->dynamicColl.dref(); }
		};

		bool collResolve_Trackback(CollisionProcessing& state);
		bool collResolve_Slide(CollisionProcessing& state, bool considerDynamicColls, bool considerStaticColls);

		ScriptObject mScriptObject;
		HardPtr<CollisionProcessing> mCollProcessing;
		CollState mCollState;
		
	public:

		inline void script_test() {}

		ScriptObject script_getScriptObject() { return mScriptObject; }
		inline Match* script_getMatch() { return mNodeMatch; }

		inline const TCHAR* script_getNodeName() { return nodeName(); }

		inline void script_nodeMarkDirty() { nodeMarkDirty(); }
		inline bool script_nodeIsDirty() { return nodeIsDirty(); }

		inline CtVector3* script_getPosition() { return &mTransformLocal.row[3]; }
		inline Vector3* script_position() { return &mTransformLocal.row[3]; }
		inline void script_shiftPosition(Vector3* val) { return mTransformLocal.movePosition(dref(val)); }
		inline void script_shiftPositionByVel(CtVector3* vel, float dt) { return mTransformLocal.movePositionByVel(dref(vel), dt); }

		inline CtVector3* script_getFacing() { return &mTransformLocal.row[Scene_Forward]; }
		inline CtVector3* script_getRight() { return &mTransformLocal.row[Scene_Right]; }

		inline void orientLookAt(const Vector3& lookAt, const Vector3& up) { mTransformLocal.setOrientation(mTransformWorld.getPosition(), lookAt, up); }
		inline void orientDirection(const Vector3& direction, const Vector3& up) { mTransformLocal.setOrientation(direction, up); }
		inline bool isOrientDirection(CtVector3& direction, const Vector3& up) { return mTransformLocal.isOrientation(direction, up); }
		void script_orientLookAt(Vector3* lookAt);
		void script_orientDirection(Vector3* direction);
		void script_camRelativizeInPlace(Vector3* direction, bool constrainUp, bool discretize);
		bool script_isOrientDirection(CtVector3* direction);

		inline SceneTransform* script_getLocalTransform() { return &mTransformLocal; }
		inline SceneTransform* script_getWorldTransform() { return &mTransformWorld; }

		AnimationIndex::Type script_getAnimationIndex(const TCHAR* name);
		AnimationIndex::Type script_getCurrAnimationIndex();
		bool script_setAnimationByIndex(AnimationIndex::Type index, bool reset);
		bool script_setAnimation(const TCHAR* name, bool reset);
		void script_setAnimationWeight(float weight);
		void script_setAnimationSpeed(float val);
		void script_setAnimationLooping(bool looping);
		void script_addAnimationTime(float dt);
		bool script_isAnimationFinished();
		bool script_addBlendAnimation(const TCHAR* name, float weight);

		bool script_collEnablePorcessing(bool enable);
		void script_collUpdate();
		int script_collResolve(bool enableGhostDynamic, bool enableGhostStatic);
		ScriptObject script_collFindDynVolColliderInt(ScriptInt* searchStartIndex, int type);
		ScriptObject script_collFindDynVolCollider(ScriptInt* searchStartIndex, const TCHAR* type);

		inline int script_getCollState() { return mCollState; }

		bool script_createPositionProxy(float horizeRadiusCoeff);

		void script_playSound(const TCHAR* name, CtVector3* pPos, float mag);
		void script_playSound2(const TCHAR* name, CtVector3* pPos, float mag, float pitch);

		inline bool script_extractBoneMeshLocalTransformAt(const TCHAR* boneName, const TCHAR* animName, float time, SceneTransform& result) {

			return extractBoneMeshLocalTransformAt(boneName, animName, time, result);
		}

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

} }

#define MDeclareMatchSceneScriptedCharacter_ScriptFuncs\
	inline void script_test() { SceneScriptedCharacter::script_test(); }\
	inline ScriptObject script_getScriptObject() { return SceneScriptedCharacter::script_getScriptObject(); }\
	inline Match* script_getMatch() { return SceneScriptedCharacter::script_getMatch(); }\
	inline const TCHAR* script_getNodeName() { return SceneScriptedCharacter::script_getNodeName(); }\
	inline void script_nodeMarkDirty() { SceneScriptedCharacter::script_nodeMarkDirty(); }\
	inline bool script_nodeIsDirty() { return SceneScriptedCharacter::script_nodeIsDirty(); }\
	inline CtVector3* script_getPosition() { return SceneScriptedCharacter::script_getPosition(); }\
	inline Vector3* script_position() { return SceneScriptedCharacter::script_position(); }\
	inline void script_shiftPosition(Vector3* val) { return SceneScriptedCharacter::script_shiftPosition(val); }\
	inline void script_shiftPositionByVel(CtVector3* vel, float dt) { return SceneScriptedCharacter::script_shiftPositionByVel(vel, dt); }\
	inline CtVector3* script_getFacing() { return SceneScriptedCharacter::script_getFacing(); }\
	inline CtVector3* script_getRight() { return SceneScriptedCharacter::script_getRight(); }\
	inline void orientLookAt(const Vector3& lookAt, const Vector3& up) { SceneScriptedCharacter::orientLookAt(lookAt, up); }\
	inline void orientDirection(const Vector3& direction, const Vector3& up) { SceneScriptedCharacter::orientDirection(direction, up); }\
	inline void script_orientLookAt(Vector3* lookAt) { SceneScriptedCharacter::script_orientLookAt(lookAt); }\
	inline void script_orientDirection(Vector3* direction) { SceneScriptedCharacter::script_orientDirection(direction); }\
	inline bool script_isOrientDirection(Vector3* direction) { return SceneScriptedCharacter::script_isOrientDirection(direction); }\
	inline SceneTransform* script_getLocalTransform() { return SceneScriptedCharacter::script_getLocalTransform(); }\
	inline SceneTransform* script_getWorldTransform() { return SceneScriptedCharacter::script_getWorldTransform(); }\
	inline void script_camRelativizeInPlace(Vector3* direction, bool constrainUp, bool discretize) { SceneScriptedCharacter::script_camRelativizeInPlace(direction, constrainUp, discretize); }\
	inline AnimationIndex::Type script_getAnimationIndex(const TCHAR* name) { return SceneScriptedCharacter::script_getAnimationIndex(name); }\
	inline AnimationIndex::Type script_getCurrAnimationIndex() { return SceneScriptedCharacter::script_getCurrAnimationIndex(); }\
	inline bool script_setAnimationByIndex(AnimationIndex::Type index, bool reset) { return SceneScriptedCharacter::script_setAnimationByIndex(index, reset); }\
	inline bool script_setAnimation(const TCHAR* name, bool reset) { return SceneScriptedCharacter::script_setAnimation(name, reset); }\
	inline void script_setAnimationWeight(float val) { SceneScriptedCharacter::script_setAnimationWeight(val); }\
	inline void script_setAnimationSpeed(float val) { SceneScriptedCharacter::script_setAnimationSpeed(val); }\
	inline void script_setAnimationLooping(bool looping) { SceneScriptedCharacter::script_setAnimationLooping(looping); }\
	inline void script_addAnimationTime(float dt) { SceneScriptedCharacter::script_addAnimationTime(dt); }\
	inline bool script_isAnimationFinished() { return SceneScriptedCharacter::script_isAnimationFinished(); }\
	inline bool script_addBlendAnimation(const TCHAR* name, float weight) { return SceneScriptedCharacter::script_addBlendAnimation(name, weight); }\
	inline bool script_collEnablePorcessing(bool enable) { return SceneScriptedCharacter::script_collEnablePorcessing(enable); }\
	inline void script_collUpdate() { SceneScriptedCharacter::script_collUpdate(); }\
	inline int script_collResolve(bool enableGhostDynamic, bool enableGhostStatic) { return SceneScriptedCharacter::script_collResolve(enableGhostDynamic, enableGhostStatic); }\
	inline int script_getCollState() { return SceneScriptedCharacter::script_getCollState(); }\
	inline ScriptObject script_collFindDynVolColliderInt(ScriptInt* searchStartIndex, int type) { return SceneScriptedCharacter::script_collFindDynVolColliderInt(searchStartIndex, type); }\
	inline ScriptObject script_collFindDynVolCollider(ScriptInt* searchStartIndex, const TCHAR* type) { return SceneScriptedCharacter::script_collFindDynVolCollider(searchStartIndex, type); }\
	inline bool script_createPositionProxy(float horizeRadiusCoeff) { return SceneScriptedCharacter::script_createPositionProxy(horizeRadiusCoeff); }\
	inline void script_playSound(const TCHAR* name, CtVector3* pPos, float mag) { SceneScriptedCharacter::script_playSound(name, pPos, mag); }\
	inline void script_playSound2(const TCHAR* name, CtVector3* pPos, float mag, float pitch) { SceneScriptedCharacter::script_playSound2(name, pPos, mag, pitch); }\
	inline bool script_extractBoneMeshLocalTransformAt(const TCHAR* boneName, const TCHAR* animName, float time, SceneTransform& result) { return SceneScriptedCharacter::script_extractBoneMeshLocalTransformAt(boneName, animName, time, result); }
	

#define MAppendSQClassDefMatchSceneScriptedCharacter_ScriptFuncs(ClassName)\
	enumInt(ClassName::Coll_None, L"Coll_None").\
	enumInt(ClassName::Coll_NoColls, L"Coll_NoColls").\
	enumInt(ClassName::Coll_Constrained, L"Coll_Constrained").\
	enumInt(ClassName::Coll_Blocked, L"Coll_Blocked").\
	enumInt(ClassName::Coll_Ghosted, L"Coll_Ghosted").\
	enumInt(ClassName::Coll_Unresolved, L"Coll_Unresolved").\
	func(&ClassName::script_test, L"test").\
	func(&ClassName::script_getScriptObject, L"getScriptObject").\
	func(&ClassName::script_getMatch, L"getMatch").\
	func(&ClassName::script_getNodeName, L"getNodeName").\
	func(&ClassName::script_playSound, L"playSound").\
	func(&ClassName::script_playSound2, L"playSound2").\
	func(&ClassName::script_nodeMarkDirty, L"nodeMarkDirty").\
	func(&ClassName::script_nodeIsDirty, L"nodeIsDirty").\
	func(&ClassName::script_getPosition, L"getPos").\
	func(&ClassName::script_shiftPosition, L"shiftPos").\
	func(&ClassName::script_shiftPositionByVel, L"shiftPosByVel").\
	func(&ClassName::script_position, L"pos").\
	func(&ClassName::script_getFacing, L"getFacing").\
	func(&ClassName::script_getRight, L"getRight").\
	func(&ClassName::script_orientLookAt, L"orientLookAt").\
	func(&ClassName::script_orientDirection, L"orientDir").\
	func(&ClassName::script_orientDirection, L"setFacing").\
	func(&ClassName::script_isOrientDirection, L"isFacing").\
	func(&ClassName::script_getLocalTransform, L"getLocalTransform").\
	func(&ClassName::script_getWorldTransform, L"getWorldTransform").\
	func(&ClassName::script_camRelativizeInPlace, L"camRelativizeInPlace").\
	func(&ClassName::script_getAnimationIndex, L"getAnimIndex").\
	func(&ClassName::script_getCurrAnimationIndex, L"getCurrAnimIndex").\
	func(&ClassName::script_setAnimationByIndex, L"setAnimByIndex").\
	func(&ClassName::script_setAnimation, L"setAnim").\
	func(&ClassName::script_setAnimationWeight, L"setAnimWeight").\
	func(&ClassName::script_setAnimationSpeed, L"setAnimSpeed").\
	func(&ClassName::script_setAnimationLooping, L"setAnimLoop").\
	func(&ClassName::script_addAnimationTime, L"addAnimTime").\
	func(&ClassName::script_isAnimationFinished, L"isAnimFinished").\
	func(&ClassName::script_addBlendAnimation, L"addBlendAnim").\
	func(&ClassName::script_collEnablePorcessing, L"collEnable").\
	func(&ClassName::script_collUpdate, L"collUpdate").\
	func(&ClassName::script_collResolve, L"collResolve").\
	func(&ClassName::script_getCollState, L"getCollState").\
	func(&ClassName::script_collFindDynVolColliderInt, L"collFindDynVolColliderInt").\
	func(&ClassName::script_collFindDynVolCollider, L"collFindDynVolCollider").\
	func(&ClassName::script_createPositionProxy, L"createPositionProxy").\
	func(&ClassName::script_extractBoneMeshLocalTransformAt, L"extractBoneMeshLocalTransformAt")


#include "SceneScriptedCharacter_TImpl.h"

#endif