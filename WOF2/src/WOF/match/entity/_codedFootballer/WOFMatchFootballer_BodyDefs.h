#ifdef FOOTBALLER_BODY3

#ifndef _WOFMatchFootballer_BodyDefs_h
#define _WOFMatchFootballer_BodyDefs_h

#include "WE3/WEPtr.h"
#include "WE3/WETime.h"
using namespace WE;

#include "../../scene/WOFMatchSceneNode.h"
#include "../../collision/WOFMatchCollRegistry.h"

#include "WOFMatchFootballerDataTypes.h"
#include "WOFMatchFootballer_BodyCaps.h"
//#include "bodyAction/WOFMatchFootballerBody_ConfigurableAction.h"

namespace WOF { namespace match {

	class Footballer_Body;

	struct FootballerBodyDefs : FootballerBodyCaps {

		typedef Footballer_Body Body;

		enum ActionType {

			Action_None = -1, Action_Idle, Action_Running, Action_Shooting, Action_Tackling, 
				Action_Falling, Action_Jumping, Action_Injured, Action_FakeInjured, Action_Transition, ActionTypeCount
		};

		class ActionImpl {
		public:

			virtual ~ActionImpl() {}

			virtual ActionType getType() = 0;

			virtual bool load(Body& body, BufferString& tempStr, DataChunk* pActions, CoordSysConv* pConv) { return true; }

			virtual bool start(Body& body, const Time& time, ActionImpl* pCurrAction) = 0;
			virtual void stop(Body& body, const Time& time) {}

			virtual void frameMoveTick(Body& body, const Time& time, const TickCount& tickIndex, const float& tickLength) = 0;

			virtual bool switchActionIdle(Body& body, const Time& time) = 0;
			virtual bool switchActionRunning(Body& body, const Time& time) = 0;
			virtual bool switchActionTackling(Body& body, const Time& time) = 0;
			virtual bool switchActionJumping(Body& body, const Time& time) = 0;
			virtual void cancelSwitchAction() = 0;
		};

		enum AnimEnum {

			Anim_None = -1, Anim_Idle, Anim_Running, Anim_Tackling, Anim_Turning180, Anim_Shooting, 
			Anim_FallingFwd, Anim_FallingBack, Anim_FallingLeft, Anim_FallingRight, 
			Anim_JumpingUp, Anim_JumpingUpLeft, Anim_JumpingUpRight,
			Anim_StrafingFwd, Anim_StrafingLeft, Anim_StrafingRight, Anim_StrafingBack, AnimEnum_Count
		};

	
		enum BallInteractionType {

			FBI_None = -1, FBI_ActiveShot, FBI_PassiveShot, FBI_Controlled, FBI_UncontrolledCollision
		};

		typedef BallOwnership BallOwnership;
		typedef BallRelativeDist BallRelativeInfo;

		struct BallInteraction {

			BallOwnership ownership;
			BallRelativeInfo relativeInfo;
			BallInteractionType interactionType;
		};

		enum CollisionResolutionMode {

			CollResolve_NoColls, CollResolve_GhostMode, CollResolve_TrackBack, CollResolve_Resolve
		};
	
		template<typename T>
		struct Request {

			bool isValid;
			T value;

			Request() : isValid(false) {}

			inline void onSetValue() { isValid = true; }
			inline void onConsumedValue() { isValid = false; }
		};

		/*
			Timed Request is for parameters with non-zero response time,
			which is something we try to not use! (see comments in WOFMatchFootballer_BodyCaps.h)
		*/
		template<typename T>
		struct TimedRequest : Request<T> {

			Time time;
			bool isEnabledFlag;

			TimedRequest() : time(-1.0f) {}

			bool isEnabled(const Time& time, const float& delay) {

				if (isEnabledFlag)
					return true;

				isEnabledFlag = (time - this->time) >= delay;

				return isEnabledFlag;
			}

			void onSetValue(const float& time) {

				this->time = time;
				isEnabledFlag = false;
				Request<T>::onSetValue();
			}
		};

		typedef TimedRequest< SoftPtr<ActionImpl> > ActionImplTimedRequest;

		enum RunType {

			Run_None = -1, Run_Normal, Run_StrafeFwd, Run_StrafeLeft, Run_StrafeRight, Run_StrafeBack, RunTypeCount
		};

		enum RunSpeedType {

			RunSpeed_None = -1, RunSpeed_Jog, RunSpeed_Normal, RunSpeed_Sprint, RunSpeedTypeCount
		};

		enum ShotType {

			Shot_None = -1, Shot_Low, Shot_High, Shot_Pass, Shot_Extra, ShotTypeCount
		};

		enum FallType {

			Fall_None = -1, Fall_Fwd, Fall_Back, Fall_Left, Fall_Right
		};

		enum JumpType {

			Jump_None = -1, Jump_Up, Jump_UpLeft, Jump_UpRight
		};


		enum BodyCommEventType {

			BodyComm_None = -1, BodyComm_Tackled
		};

		struct BodyCommEvent {

			BodyCommEventType type;
			SoftPtr<Footballer_Body> source;

			void set(const BodyCommEventType& _type, Footballer_Body* pSource = NULL) {

				type = _type;
				source = pSource;
			}
		};

		typedef WETL::CountedArray<BodyCommEvent, false, unsigned int> BodyCommEvents;

		class BodyCollisionListener {
		protected:

			friend class Footballer_Body;

			virtual void onBodyCollision(Body& body, CollRecordVol& record, void* pContext = NULL) {}
			virtual void onBodyCollision(Body& body, CollRecordTri& record, void* pContext = NULL) {}
		};

		//typedef FootballerBody_ConfigurableAction ConfigurableAction;
	};

} }

#endif

#endif