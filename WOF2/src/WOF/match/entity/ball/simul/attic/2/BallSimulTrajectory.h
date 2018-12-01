#ifndef h_WOF_match_BallSimulationTrajectory
#define h_WOF_match_BallSimulationTrajectory

#include "WE3/object/WEObject.h"
#include "WE3/math/WEVector.h"
#include "WE3/WETime.h"
#include "WE3/WEPtr.h"
#include "WE3/render/WERenderer.h"
#include "WE3/WETL/WETLArray.h"
#include "WE3/WETL/WETLSpanArray.h"
#include "WE3/data/WEDataSource.h"
using namespace WE;

#include "../BallListener.h"
#include "../BallCommand.h"
#include "../../../../script/ScriptEngine.h"
#include "../../../collision/CollRegistry.h"

namespace WOF { namespace match {

	class Match;
	class Ball;

	enum BallSimulSyncState {

		BallSimulSync_None = -1, BallSimulSync_Syncing, BallSimulSync_Synced, BallSimulSync_Expired
	};

	class BallSimulTrajectorySync;

	class BallSimulTrajectory {
	public:

		virtual bool ignoreCollision(Object* pObject);

		//return false to end simul
		virtual bool signalPreCollision(Object* pObject, const Time& time, Ball& ball);
		virtual bool signalPostCollision(Object* pObject, const Time& time, Ball& ball);

		//return false to end simul
		virtual bool signalSimulTickEnd(const Time& time, const unsigned int& simulTickIndex, Ball& ball);

	public:

		void onBallSyncBallState(Ball& ball, const Time& time);

	public:

		typedef unsigned int Index;

		typedef WETL::IndexSpan<Index> SampleInterval;

		struct Sample;
		struct TrajSample;
		struct CollSample;

	public:

		BallSimulTrajectory();

		void loadConfig(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		void setSimulTickSkipCount(unsigned int value);
		void setTolerance(float value);
		void setEnableExtraTrajPrecision(bool value);
		void setEnableFootballerReachableAnalysis(bool value);

		inline const unsigned int& getID() { return mID; }
		const BallCommand& getCommand() { return mBallCommand; }

		inline const unsigned int& getSampleCount() { return mTrajSamples.count; }
		inline const unsigned int& getCollSampleCount() { return mCollSamples.count; }
		inline const unsigned int& getFootballerReachableIntervalCount() { return mFootballerReachableIntervals.getCount(); }
	
		inline const TrajSample& getSample(const Index& i) { return mTrajSamples[i]; }
		inline const CollSample& getCollSample(const Index& i) { return mCollSamples[i]; }
		inline const SampleInterval& getFootballerReachableInterval(const Index& i) { return mFootballerReachableIntervals.get(i); }

		inline TrajSample& getSampleRef(const Index& i) { return mTrajSamples[i]; }
		inline CollSample& getCollSampleRef(const Index& i) { return mCollSamples[i]; }

		void render(Renderer& renderer, Ball* pBall, BallSimulTrajectorySync* pSync, 
							 const float* pBallRadiusStart = NULL, const float* pBallRadiusEnd = NULL, 
							 const RenderColor* pLineStart = &RenderColor::kWhite, const RenderColor* pLineEnd = &RenderColor::kRed, 
							 const RenderColor* pSphereStart = &RenderColor::kGreen, const RenderColor* pSphereEnd = &RenderColor::kRed,
							 const RenderColor* pSphereColl = &RenderColor::kBlack);

	public:

		struct Sample {

			Time time; 
			Point pos;
			Vector3 vel;

			void set(const Time& time, const Point& pos, const Vector3& vel);
			inline void extrapolatePos(const Time& _time, Vector3& result) { pos.addMultiplication(vel, _time - time, result); }
		};

		struct TrajSample : Sample {

			bool isOnPitch;

			void set(const Time& time, const Point& pos, const Vector3& vel, const bool& isOnPitch);
			bool isFootballerReachable(Match& match, const float& ballRadius) const;
			static bool isBallFootballerReachable(Match& match, Ball& ball);

			inline bool script_isOnPitch() { return isOnPitch; }
			CtVector3* script_getPos() { return &pos; }
			CtVector3* script_getVel() { return &vel; }

			static const TCHAR* ScriptClassName;
			static void declareInVM(SquirrelVM& target);
		};

		struct CollSample : Sample {

			Vector3 postVel;
			SoftPtr<Object> collider;
		};

		struct SimulContinueSample : Sample {

			unsigned int tick;
			Vector3 w;
			bool isResting;
		};

	protected:

		typedef WETL::CountedArray<CollSample, false, Index> CollSamples;
		typedef WETL::CountedArray<TrajSample, false, Index> TrajSamples;
		typedef WETL::IndexSpanBatch<Index, SampleInterval> SampleIntervals;

		CollSamples mCollSamples;
		TrajSamples mTrajSamples;
		SampleIntervals mFootballerReachableIntervals;

	protected:

		float mToleranceSq;
		bool mEnableExtraTrajPrecision;
		bool mEnableFootballerReachableAnalysis;

		unsigned int mID;
		BallCommand mBallCommand;

		unsigned int mSimulTickSkipCount;

		Time mSimulStartTime;
		Time mNextRecordSimulTick;
		TrajSample mLastInspectedTickEndSample;
		bool mLastInspectedTickEndSampleAdded;
		bool mLastInspectedTickEndSampleReachable;
		bool mCollBetweenSamples;
		
		SimulContinueSample mLastSimulSample;
	};

	class BallSimulTrajectorySync {
	public:

		typedef BallSimulSyncState SyncState;

	public:

		BallSimulTrajectorySync();

		void reset();
		inline const SyncState& getState() { return mState; }
		SyncState updateSync(Ball& ball);

		inline bool isValid() { return mState == BallSimulSync_Synced; }
		inline bool isProcessing() { return mState == BallSimulSync_Syncing; }

		
		inline const unsigned int& getSyncFlowTrajSampleIndex() { return mSyncFlowTrajSampleIndex; }
		inline const unsigned int& getSyncFlowCollSampleIndex() { return mSyncFlowCollSampleIndex; }
		inline const unsigned int& getSyncFlowFootballerReachableIntervalIndex () { return mSyncFlowFootballerReachableIntervalIndex; }
	

		bool estimateNextBallState(Ball& ball, BallSimulTrajectory::TrajSample& result);

	protected:

		void startSyncBall(Ball& ball);
		void stopSyncBall();

	protected:

		SyncState mState;
		SoftPtr<Ball> mSyncBall;

		unsigned int mSyncFlowTrajSampleIndex;
		unsigned int mSyncFlowCollSampleIndex;
		unsigned int mSyncFlowFootballerReachableIntervalIndex;
	};

} }

#endif