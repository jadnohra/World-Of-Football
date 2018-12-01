#ifdef FOOTBALLER_BODY1

#ifndef _WOFMatchFootballer_Body1_h
#define _WOFMatchFootballer_Body1_h


#include "WOFMatchFootballer_Scene.h"
#include "WOFMatchFootballerDataTypes.h"

namespace WOF { namespace match {

	class Footballer;
	class Team;

	class Footballer_Body : public Footballer_Scene {
	public:

		enum State {

			BS_None = -1, BS_Idle = 0, BS_Running, BS_Tackling, BS_Shooting, BS_Falling
		};

		enum BallInteraction {

			FBI_None = -1, FBI_ActiveShot, FBI_PassiveShot, FBI_Controlled, FBI_UncontrolledCollision
		};

		enum RunType {

			BR_Jog, BR_Normal, BR_Sprint
		};

		enum ShotType {

			ST_ShotNone = 0, ST_ShotLow, ST_ShotHigh, ST_ShotPass, ST_ShotExtra
		};

		typedef FootballerBallOwnership BallOwnership;

		enum EventType {

			ET_None = -1, ET_MoveBlockStarted, ET_MoveBlockEnded, ET_TackleEnded, ET_ShootEnded, ET_FallStarted, ET_FallEnded
		};

		struct Event {

			EventType type;
		};

		enum InterFootballerEventType {

			IFET_None = -1, IFET_Tackled, IEFT_FallingToTackle
		};

		struct InterFootballerEvent {

			InterFootballerEventType type;
			SoftPtr<Footballer_Body> source;
		};

		typedef unsigned int EventIndex;

	protected:

		//CollRegistry body_mCollRegistry;

		State body_mCurrState;
		BallInteraction body_mBallInteraction;
		bool body_mMoveBlocked;
		bool body_mIsInGhostMode;

		Vector body_mRunning_dir;
		RunType body_mRunning_type;
		Vector3 body_mRunning_velocity;

		Vector3 body_mTackling_veolcity;
		float body_mTackling_acceleration;

		bool body_mMoveTest_hasStartedColls;
		Vector3 body_mVelConstrainContext;

		Vector3 body_mSweetSpot;
		Vector3 body_mRelBallPos; //relative to sweet spot
		float body_mRelBallDist;
		float body_mRelBallAngle;

		BallOwnership body_mBallOwnership;
		
		typedef FootballerCOCState COCState;

		COCState mCOC;
		bool body_mBallActionDoing;
		bool body_mBallActionDone;
		bool body_mBallFlagPassiveCollided;

		ShotType m_ShotType;

		typedef WETL::CountedArray<Event, false, EventIndex> Events;
		typedef WETL::CountedArray<InterFootballerEvent, false, EventIndex> InterFootballerEvents;

		Events body_mEvents;
		InterFootballerEvents body_mInterFootballerEvents;

	protected:

		inline void body_toAnimSpeed_Idle(const FastUnitCoordSys& coordSys, float& animSpeed) {

			animSpeed = 1.0f;
		}

		inline void body_toAnimSpeed_Tackle(const FastUnitCoordSys& coordSys, float& animSpeed) {

			animSpeed = 1.0f;
		}

		inline void body_toAnimSpeed_Shoot(const FastUnitCoordSys& coordSys, float& animSpeed) {

			animSpeed = 1.0f;
		}

		inline void body_toAnimSpeed_Fall(const FastUnitCoordSys& coordSys, float& animSpeed) {

			animSpeed = 1.0f;
		}

		inline void body_toAnimSpeed_Running(const FastUnitCoordSys& coordSys, const float& vel, float& animSpeed) {

			coordSys.invConvUnit(vel, animSpeed);
		}

		void body_blindApply_setIdle(const FastUnitCoordSys& coordSys, float& temp) {

			mCOC.idleBallHandled = false;

			body_toAnimSpeed_Idle(coordSys, temp);	
			scene_setAnim(FA_Idle, temp);
		}

		void body_blindApply_setTackling(const FastUnitCoordSys& coordSys, float& temp, bool wasRunning) {

			body_mBallActionDone = false;
			body_mBallActionDoing = true;

			body_toAnimSpeed_Tackle(coordSys, temp);	
			scene_setAnim(FA_Tackle, temp);

			body_initTackling(wasRunning ? body_mRunning_velocity : Vector3::kZero);
		}

		void body_blindApply_setShooting(const FastUnitCoordSys& coordSys, float& temp) {

			mCOC.needChangeDirToActivateDribble = false;
			body_mBallActionDone = false;
			body_mBallActionDoing = true;

			body_toAnimSpeed_Shoot(coordSys, temp);	
			scene_setAnim(FA_Shoot, temp);
		}

		void body_blindApply_setFalling(const FastUnitCoordSys& coordSys, float& temp) {

			body_mBallActionDone = false;
			body_mBallActionDoing = false;

			body_toAnimSpeed_Fall(coordSys, temp);	
			scene_setAnim(FA_Fall, temp);
		}

		inline void body_blindApply_setRunning_velocity(const Vector3& runDir, const float& runVel) {

			scene_orientDirection(runDir);
			runDir.mul(runVel, body_mRunning_velocity);
		}

		inline void body_blindApply_setRunning_anim(const FastUnitCoordSys& coordSys, float& temp, const float& runVel) {

			body_toAnimSpeed_Running(coordSys, runVel, temp);
			scene_setAnim(FA_Run, temp);
		}

		bool body_trySwitchTo(const State& state);

		void body_constrainVelocity(const Vector3& posDiff, Vector3& vel, Vector& correction);
		bool body_testMove(const DiscreeteTime& time, const Vector3& testPos);
		
		void body_getSweetSpot(Match& match, Vector3& spot);
		inline void body_updateSweetSpot(Match& match);

		void body_doBallAction(Footballer& footballer);

		inline Event& body_addEvent() { return body_mEvents.addOne(); }
		inline InterFootballerEvent& body_addInterFootballerEvent() { return body_mInterFootballerEvents.addOne(); }

		void body_initTackling(const Vector3& startVel);
		const Vector3& body_updateTacklingVelocity(const float& dt);
		const Vector3& body_getTacklingVelocity();

	public:

		void body_toVelocity_Running(const FastUnitCoordSys& coordSys, const RunType& type, float& vel);
		float body_toVelocity_Running(const RunType& type);

		static const TCHAR* toString(const State& state);

		inline const State& body_getCurrState() {
			return body_mCurrState;
		}

		inline float body_getStateSwitchTime(const State& state) {
			return 0.0f; //improve this
		}

		inline const BallInteraction& body_getBallInteraction() {
			return body_mBallInteraction;
		}

		inline EventIndex& body_getEventCount() { return body_mEvents.count; }
		inline Event& body_getEvent(const EventIndex& i) { return body_mEvents[i]; }

		inline void body_clearEvents() { body_mEvents.count = 0; }

		void body_setIdle();
		
		void body_setRunning();
		void body_setRunning_dir(const Vector3& dir, bool forceUpdate = false);
		void body_setRunning_type(const RunType& type);

		void body_setTackling(ShotType shotType = ST_ShotNone);
		void body_setFalling();

		void body_getIdealShotVel(Vector3& v, Vector3& w);

		inline const Vector3& getRelBallPos() { return body_mRelBallPos; }
		inline const float& getRelBallDist() { return body_mRelBallDist; }
		inline const float& getRelBallAngle() { return body_mRelBallAngle; }
		inline BallOwnership& getBallOwnership() { return body_mBallOwnership; }
		
		void setBallPos(const Vector3& pos, const Time& t);

	public:

		bool readBahviour_isGivingBallAway(); 


	public:

		/*
		virtual bool getOBBContact(const OBB& obb, Vector& normal, float& penetration);
		virtual void addSweptBallContacts(WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections);
		*/
		virtual bool isCameraIdle();

		/*
		void collBroadPhaseOnStart(Object* pObject, CollRecordPool& collPool);
		void collBroadPhaseOnEnd(Object* pObject, CollRecordPool& collPool);
		*/

	public:

		Footballer_Body();
		~Footballer_Body();

		void cancel_init_create();
		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene(Match& match, Team& team, Footballer& footballer);

		void frameMoveTick(Footballer& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);

	protected:

		enum BallVelocityType {

			BVT_Slow = 0, BVT_Normal, BVT_Fast
		};

		struct BallVelocityTypeRange {

			float maxSlow;
			float maxNormal;
		};

		BallVelocityType actBall_getVelocityType(const Ball& ball, const BallVelocityTypeRange& range);

		CollRegistry::Index getCollRecordCount(CollRegistry::Index* pFootballerRecordCount);

		void actBall_Idle(const DiscreeteTime& time, const float& tickLength);
		void processTacklingCollisions();
		void fallToTackle(Footballer_Body* pSource);

	protected:

		
	};
	
} }

#endif

#endif