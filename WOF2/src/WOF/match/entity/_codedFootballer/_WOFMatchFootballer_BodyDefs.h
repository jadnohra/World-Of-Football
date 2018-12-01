#include "select.h"
#ifdef FOOTBALLER_BODY2

#ifndef _WOFMatchFootballer_BodyDefs_h
#define _WOFMatchFootballer_BodyDefs_h

#include "WE3/WEPtr.h"
using namespace WE;

#include "../../scene/WOFMatchSceneNode.h"
#include "../../collision/WOFMatchCollRegistry.h"

#include "WOFMatchFootballerDataTypes.h"

namespace WOF { namespace match {

	class Footballer_BodyActions;
	class Footballer_Body;

	struct FootballerBodyDefs {

		typedef Footballer_Body Body;
		typedef Footballer_BodyActions BodyActions;

		enum VariableType {
		
			Var_None = -1, 
			Var_Action, Var_HeadFacing, Var_BodyFacing, 
			Var_RunDir, Var_RunSpeed,

			Vars_BodyPhysics
		};

		enum ActionType {

			Action_None = -1, Action_Idle, Action_Running, Action_Shooting, Action_Tackling, 
				Action_Falling, Action_Injured, Action_FakeInjured, ActionTypeCount
		};

		enum EventType {

			Evt_None = -1, 
			
			Evt_ReqRejected, Evt_ReqExecuted, Evt_ReqExecuting, Evt_ReqRedundant, Evt_ReqCancelled,

			Evt_ForcedChange, Evt_ForcedTemporaryChange
		};

		enum CauseType {

			Cause_None = -1, 
			Cause_WorldCollide, Cause_FootballerCollide, Cause_Tackled, Cause_LowStamina, Cause_ActionEnded,
			Cause_ActionInProgress
		};

		struct Event {

			EventType type;
			VariableType var;
			CauseType cause;

			SoftPtr<SceneNode> node;

			void set(const EventType& _type, const CauseType& _cause) { type = _type; cause = _cause; }
			void set(const EventType& _type, const VariableType& _var, CauseType _cause = Cause_None) {
				type = _type; var = _var; cause = _cause; }
		};

		typedef unsigned int Index;

		template<typename _T>
		struct Variable {

			_T value;
			
			_T start;
			_T target;
			bool isChanging; //value is changing from start to target
			
			_T request;
			bool isRequestPending;

			Variable() : isChanging(false), isRequestPending(false) {}

			void initWithValue(const _T& val) { value = val; isChanging = false; isRequestPending = false; }

			void setRequest(const _T& val) { request = val; isRequestPending = true; }
			void resetRequest() { isRequestPending = false; }
			
			void setTargetFromRequest() { start = value; target = request; isChanging = true; isRequestPending = false; }
			void setValueFromTarget() { value = target; isChanging = false; }
			void setValueFromRequest() { value = request; isRequestPending = false; }

			inline bool isChangingWithNoRequests() { return isChanging && !isRequestPending; }

			/*
			
			
			
			void setValueFromTarget(const bool& _isTargetReached) { value = target; isTargetReached = _isTargetReached; }

			inline bool isIdle() { return isTargetReached && !isRequestPending; }
			
			inline bool isChangingWithRequests() { return !isTargetReached && isRequestPending; }
			inline bool isNotChangingWithNoRequests() { return isTargetReached && !isRequestPending; }
			inline bool isNotChangingWithRequests() { return isTargetReached && isRequestPending; }
			*/
		};

		typedef Variable<float> Variable_float;
		typedef Variable<Vector3> Variable_Vector3;
		typedef Variable_Vector3 Facing;
		typedef Variable<ActionType> Action;

		class ActionImpl {
		public:

			virtual ~ActionImpl() {}

			virtual ActionType getType() = 0;

			virtual bool switchTo(BodyActions& actions, Body& body, const ActionType& nextAction, ActionImpl& nextActionImpl, Event& outResult) = 0;
			virtual bool start(BodyActions& actions, Body& body, Event& outFailureResult) = 0;
			virtual void frameMoveTick(BodyActions& actions, Body& body, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) = 0;
		};

		enum AnimEnum {

			Anim_None = -1, Anim_Idle, Anim_Running, Anim_Tackling, Anim_Turning180, Anim_Shooting, Anim_Falling, AnimEnum_Count
		};

		enum RunType {

			Run_None = -1, Run_Jog, Run_Normal, Run_Sprint, Run_Modifier, RunType_Count
		};

		enum ShotType {

			Shot_None = -1, Shot_Low, Shot_High, Shot_Pass, Shot_Extra, ShotType_Count
		};

		enum BallInteraction {

			FBI_None = -1, FBI_ActiveShot, FBI_PassiveShot, FBI_Controlled, FBI_UncontrolledCollision
		};

		typedef FootballerBallOwnership BallOwnership;
		typedef BallFootballerRelativeInfo BallRelativeInfo;

		enum CollisionResolutionMode {

			CollResolve_NoColls, CollResolve_GhostMode, CollResolve_TrackBack, CollResolve_Resolve
		};

		class CollisionResolveController {
		public:

			virtual CollisionResolutionMode chooseCollResolveMode(Body& body, bool& allowSaveCurrTransformLocal);
		};

		static CollisionResolveController kStockCollisionResolveController;
	};

} }

#endif

#endif