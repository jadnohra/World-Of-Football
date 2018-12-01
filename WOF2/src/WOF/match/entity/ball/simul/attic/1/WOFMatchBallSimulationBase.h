#ifndef _WOFMatchBallSimulationBase_h
#define _WOFMatchBallSimulationBase_h

#include "WE3/object/WEObject.h"
#include "WE3/WETL/WETLSizeAlloc.h"
using namespace WE;

#include "WOFMatchBallTrajectory.h"

namespace WOF { namespace match {

	class IBallSimulation {
	public:

		virtual bool simulEnded() { return false; }
		virtual bool ignoreCollision(Object* pObject) { return false; }
		virtual void signalCollision(Object* pObject, const Point& pos, const Vector3& v, const Vector3& w, const Time& time) {}

		virtual bool getDuration(Time& time) { return false; }

		virtual bool isDummy() { return true; }

		virtual void signalBallResting(const Time& time) {}
		virtual void signalBallTouchingState(bool touching, const Time& time) {}
		virtual void signalBallState(const Point& pos, const Vector3& v, const Vector3& w, const Time& time) {}
		virtual void signalFinalBallState(const Point& pos, const Vector3& v, const Vector3& w, const Time& time) {}
	};

	class BallSimulation_Filtered : public IBallSimulation {
	public:

		struct BallState : BallTrajectory::Sample {

			Vector w;
		};

		enum EndReason {

			ER_None = 0, ER_Duration, ER_Distance, ER_Resting, ER_BreakType
		};

		typedef unsigned int Index;

		struct CollSample : BallTrajectory::Sample {

			SoftPtr<Object> collider;
			Index prevSampleIndex;

			inline void set(Object* pObject, const Point& pos, const Vector3& vel, const Time& time, const Index& prevSamplIdx) {

				BallTrajectory::Sample::set(pos, vel, time);
				collider = pObject;
				prevSampleIndex = prevSamplIdx;
			}
		};

	public:

		BallSimulation_Filtered();

		virtual bool isDummy() { return false; }

		void reset(bool resetEnded, bool resetIgnoreTypes, bool resetBreakTypes);
		void resetTrajectory(const Time& startTime, const bool& clearTrajectory);

		void addIgnoreType(const ObjectType& type);
		void addBreakType(const ObjectType& type);
		
		void setEnableEndAtResting(bool enable);
		void setRegisterCollisions(bool value) { mRegisterCollisions = value; }

		void setDuration(Time time = 0); //Zero means no break time
		virtual bool getDuration(Time& time);

		virtual bool simulEnded();
		virtual bool ignoreCollision(Object* pObject);
		void signalCollision(const Index& prevSampleIndex, Object* pObject, const Point& pos, const Vector3& v, const Vector3& w, const Time& time);

		virtual void signalBallResting(const Time& time);
		virtual void signalBallTouchingState(bool touching, const Time& time);

		inline const EndReason& getEndReason() { return mEndReason; }
		inline const BallState& getEndState() { return mEndState; }

		inline const Index& getCollSampleCount() { return mCollisionSampleCount; }
		inline const CollSample& getCollSample(const Index& index) { return mCollisionSamples[index]; }

	protected:

		typedef WETL::SizeAllocT<ObjectType, Index, false> Types;
		typedef WETL::SizeAllocT<CollSample, Index, false> CollSamples;

		Types mIgnores;
		Types mBreaks;

		bool mBallIsTouching;
		
		bool mEnabledEndAtResting;

		Time mStartTime;

		bool mRegisterCollisions;
		CollSamples mCollisionSamples;
		CollSamples::Index mCollisionSampleCount;
	
		bool mHasDuration;
		Time mDuration;

		bool mEnded;
		EndReason mEndReason;
		BallState mEndState;
		
	};

	class BallSimulation_Filtered_Trajectory 
		: public BallSimulation_Filtered
		, public BallTrajectory
		, public BallTrajectory::Sampler {
	public:

		void reset(const Time& startTime, bool clearTrajectory, bool resetEnded, bool resetIgnoreTypes, bool resetBreakTypes);

		virtual void signalCollision(Object* pObject, const Point& pos, const Vector3& v, const Vector3& w, const Time& time);

		virtual void signalBallState(const Point& pos, const Vector3& v, const Vector3& w, const Time& time);
		virtual void signalFinalBallState(const Point& pos, const Vector3& v, const Vector3& w, const Time& time);

		void render(Renderer& renderer, const float& ballRadiusStart, RenderListener* pListener = NULL, const float* pBallRadiusEnd = NULL, const RenderColor* pLineStart = NULL, const RenderColor* pLineEnd = NULL, const RenderColor* pSphereStart = NULL, const RenderColor* pSphereEnd = NULL);

		void addEndStateAsSample();
	};
} }

#endif