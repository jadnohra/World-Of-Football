#ifndef h_WOF_match_BallSimulation
#define h_WOF_match_BallSimulation

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

	class BallSimulation : public BallSimulSyncListener {
	public:

		virtual bool ignoreCollision(Object* pObject);

		//return false to end simul
		virtual bool signalPreCollision(Object* pObject, const Time& time, Ball& ball);
		virtual bool signalPostCollision(Object* pObject, const Time& time, Ball& ball);

		//return false to end simul
		virtual bool signalSimulTickEnd(const Time& time, const unsigned int& simulTickIndex, Ball& ball);

	public:

		virtual void onBallSyncResetSimul(Ball& ball, const BallCommand* pCommand);
		virtual void onBallSyncPostCollision(Object& object);
		virtual void onBallSyncBallState(Ball& ball, const Time& time);

	public:

		virtual void addSweptBallContacts(Ball& ball, WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials);


	public:

		typedef unsigned int Index;

		enum State {

			State_None = -1, State_Simulating, State_Simulated, State_Syncing, State_Synced, State_Expired
		};

		typedef WETL::IndexSpan<Index> SampleInterval;

		struct Sample;
		struct TrajSample;
		struct CollSample;

	public:

		BallSimulation();

		void loadConfig(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		void setSimulMaxFrameCount(unsigned int value);
		void setEnableEarlySimulEnd(bool value);
		void setSimulTickSkipCount(unsigned int value);
		void setSimulMaxFrameDuration(float value);
		void setSimulFrameDuration(float value);
		void setSimulEndAtRest(bool value);
		void setTolerance(float value);
		void setEnableExtraTrajPrecision(bool value);
		void setEnableFootballerReachableAnalysis(bool value);

		void reset();
		inline const State& getState() { return mState; }
		inline const unsigned int& getID() { return mID; }
		State updateSimulate(Ball& ball, Time simulStartTime = 0.0f);
		State updateSync(Ball& ball);

		inline bool isValid() { return mState == State_Simulated || mState == State_Synced; }
		inline bool isValidSimul() { return mState == State_Simulated; }
		inline bool isValidSync() { return mState == State_Synced; }
		inline bool isProcessing() { return mState == State_Simulating || mState == State_Syncing; }

		const BallCommand& getCommand() { return mBallCommand; }

		inline const unsigned int& getSampleCount() { return mTrajSamples.count; }
		inline const unsigned int& getCollSampleCount() { return mCollSamples.count; }
		inline const unsigned int& getFootballerReachableIntervalCount() { return mFootballerReachableIntervals.getCount(); }

		inline const unsigned int& getSyncFlowTrajSampleIndex() { return mSyncFlowTrajSampleIndex; }
		inline const unsigned int& getSyncFlowCollSampleIndex() { return mSyncFlowCollSampleIndex; }
		inline const unsigned int& getSyncFlowFootballerReachableIntervalIndex () { return mSyncFlowFootballerReachableIntervalIndex; }
		
		inline const TrajSample& getSample(const Index& i) { return mTrajSamples[i]; }
		inline const CollSample& getCollSample(const Index& i) { return mCollSamples[i]; }
		inline const SampleInterval& getFootballerReachableInterval(const Index& i) { return mFootballerReachableIntervals.get(i); }

		inline TrajSample& getSampleRef(const Index& i) { return mTrajSamples[i]; }
		inline CollSample& getCollSampleRef(const Index& i) { return mCollSamples[i]; }

		void render(Renderer& renderer, bool useSyncFlow, 
							 const float* pBallRadiusStart = NULL, const float* pBallRadiusEnd = NULL, 
							 const RenderColor* pLineStart = &RenderColor::kWhite, const RenderColor* pLineEnd = &RenderColor::kRed, 
							 const RenderColor* pSphereStart = &RenderColor::kGreen, const RenderColor* pSphereEnd = &RenderColor::kRed,
							 const RenderColor* pSphereColl = &RenderColor::kBlack);


		bool estimateNextBallState(Ball& ball, TrajSample& result);


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

		State continueSimulProcessing(Ball& ball, bool restart, Time simulStartTime = 0.0f);

		void startSyncBall(Ball& ball);
		void stopSyncBall();

	protected:

		Time mSimulMaxFrameDuration;
		Time mSimulFrameDuration;
		unsigned int mSimulSimulMaxFrameCount;
		unsigned int mSimulTickSkipCount;
		float mToleranceSq;
		bool mSimulEndAtRest;
		bool mEnableEarlySimulEnd;
		bool mEnableExtraTrajPrecision;
		bool mEnableFootballerReachableAnalysis;

		State mState;
		unsigned int mID;
		SoftPtr<Ball> mSyncBall;
		BallCommand mBallCommand;

		unsigned int mSyncFlowTrajSampleIndex;
		unsigned int mSyncFlowCollSampleIndex;
		unsigned int mSyncFlowFootballerReachableIntervalIndex;

		unsigned int mNextSimulProcessingFrameIndex;
		Time mSimulStartTime;
		Time mNextRecordSimulTick;
		TrajSample mLastInspectedTickEndSample;
		bool mLastInspectedTickEndSampleAdded;
		bool mLastInspectedTickEndSampleReachable;
		bool mCollBetweenSamples;
		
		SimulContinueSample mLastSimulSample;
	};


	class BallSimulation_SimulPitch : public BallSimulation {
	public:

		virtual void addSweptBallContacts(Ball& ball, WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials);
	};
} }

#endif