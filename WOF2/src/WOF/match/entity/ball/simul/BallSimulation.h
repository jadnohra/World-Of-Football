#ifndef h_WOF_match_BallSimulation
#define h_WOF_match_BallSimulation

#include "WE3/object/WEObject.h"
#include "WE3/math/WEVector.h"
#include "WE3/math/WEPlane.h"
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
		virtual bool signalPreTrigger(WorldMaterialCombo* pMaterial, const Time& time, Ball& ball, const SimulationID& ballMoveTickID);
		virtual bool signalPostTrigger(WorldMaterialCombo* pMaterial, const Time& time, Ball& ball);

		//return false to end simul
		virtual bool signalSimulTickEnd(const Time& time, const unsigned int& simulTickIndex, Ball& ball);

	public:

		virtual void onBallSyncCommand(Ball& ball, const BallCommand* pCommand);
		virtual void onBallSyncSimulableStateChange(Ball& ball);
		virtual void onBallSyncPostCollision(Ball& ball, Object& object);
		virtual void onBallSyncBallState(Ball& ball, const Time& time);

	public:

		virtual void addSweptBallContacts(Ball& ball, WorldMaterialManager& matManager, 
											const WorldMaterialID& sphereMat, const float& radius, 
											const Vector3& pos, const Vector3& sweepVector, 
											SweptIntersectionArray& intersections, BallSimulation* pSimul2, bool executeMaterials);


	public:

		typedef unsigned int Index;

		enum SimulState {

			Simul_None = -1, Simul_Simulating, Simul_Simulated
		};

		enum SyncState {

			Sync_None = -1, Sync_Syncing, Sync_Synced, Sync_Expired
		};

		enum ApproximateType {

			Approximate_None = -1, Approximate_AfterTouch
		};


		typedef WETL::IndexSpan<Index> SampleInterval;

		struct Sample;
		struct TrajSample;
		struct CollSample;
		struct TriggerSample;
		struct SimulData;

	protected:

		struct SyncData {

			SyncState state;
			SoftPtr<Ball> ball;
		
			unsigned int flowTrajSampleIndex;
			unsigned int flowCollSampleIndex;
			unsigned int flowFootballerReachableIntervalIndex;

			SyncData();
		};

	public:

		BallSimulation();

		void setupUsingMatch(Match& match);
		void loadConfig(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		void enableSyncMode(bool enable);
		void enableSimul(bool enable);
		void setSimulData(SimulData* pSimulData);
		SimulData* cloneSimulData();
		inline SimulData* getSimulData() { return mSimulData; }

		void setSimulMaxFrameCount(unsigned int value);
		void setEnableEarlySimulEnd(bool value);
		void setSimulTickSkipCount(unsigned int value);
		void setSimulMaxFrameDuration(float value);
		void setSimulFrameDuration(float value);
		void setSimulEndAtRest(bool value);
		void setTolerance(float value);
		void setEnableExtraTrajPrecision(bool value);
		void setEnableFootballerReachableAnalysis(bool value);

		inline const unsigned int& getID() { return mID; }

	
		void rewindSync();
		SyncState updateSync(Ball& ball);
		inline bool isProcessingSync(bool allowApproximate = false) { return (mSyncData->state == Sync_Syncing) && (mSimulData->mApproximateType == Approximate_None || allowApproximate); }
		inline bool isValidSync(bool allowApproximate = false) { return (mSyncData->state == Sync_Synced) && (mSimulData->mApproximateType == Approximate_None || allowApproximate); }
		inline bool isSyncExpired() const { return mSyncData->state == Sync_Expired; }

		float getMaxRemainingSimulTime(Ball& ball);

		inline const unsigned int& getSyncFlowTrajSampleIndex() { return mSyncData->flowTrajSampleIndex; }
		inline const unsigned int& getSyncFlowCollSampleIndex() { return mSyncData->flowCollSampleIndex; }
		inline const unsigned int& getSyncFlowFootballerReachableIntervalIndex () { return mSyncData->flowFootballerReachableIntervalIndex; }
		

		inline const BallCommand& getCommand() { return mBallCommand; }
		inline void setCommand(BallCommand* pCommand) { if (pCommand) mBallCommand = dref(pCommand); else mBallCommand.invalidate(); }

		inline const unsigned int& getSampleCount() { return mTrajSamples.count; }
		inline const unsigned int& getCollSampleCount() { return mCollSamples.count; }
		inline const unsigned int& getTriggerSampleCount() { return mTriggerSamples.count; }
		inline const unsigned int& getFootballerReachableIntervalCount() { return mFootballerReachableIntervals.getCount(); }

		
		inline const TrajSample& getSample(const Index& i) { return mTrajSamples[i]; }
		inline const CollSample& getCollSample(const Index& i) { return mCollSamples[i]; }
		inline const TriggerSample& getTriggerSample(const Index& i) { return mTriggerSamples[i]; }
		inline const SampleInterval& getFootballerReachableInterval(const Index& i) { return mFootballerReachableIntervals.get(i); }

		inline const TrajSample* getLastSample() { return mTrajSamples.count > 0 ? &mTrajSamples[mTrajSamples.count - 1] : NULL; }
		inline const CollSample* getCollLastSample() { return mCollSamples.count > 0 ? &mCollSamples[mCollSamples.count - 1] : NULL; }
		inline const TriggerSample* getTriggerLastSample() { return mTriggerSamples.count > 0 ? &mTriggerSamples[mTriggerSamples.count - 1] : NULL; }

		inline TrajSample& getSampleRef(const Index& i) { return mTrajSamples[i]; }
		inline CollSample& getCollSampleRef(const Index& i) { return mCollSamples[i]; }
		inline TriggerSample& getTriggerSampleRef(const Index& i) { return mTriggerSamples[i]; }

		void render(Renderer& renderer, bool useSyncFlow, 
							 const float* pBallRadiusStart = NULL, const float* pBallRadiusEnd = NULL, 
							 const RenderColor* pLineStart = &RenderColor::kWhite, const RenderColor* pLineEnd = &RenderColor::kRed, 
							 const RenderColor* pSphereStart = &RenderColor::kGreen, const RenderColor* pSphereEnd = &RenderColor::kRed,
							 const RenderColor* pSphereColl = &RenderColor::kBlack);


		bool estimateNextBallState(Ball& ball, TrajSample& result);
		bool estimateStateAtTime(Time t, bool useSyncFlow, TrajSample& result, bool* pIsLastSamplFallback, Time epsilon = 0.1f, bool allowLastSampleFallback = true);
		bool estimateStateAtSpeedSq(float speed, bool useSyncFlow, TrajSample& result, bool* pIsLastSamplFallback, float epsilonSq, bool allowLastSampleFallback = true);
		bool estimateStateAtPosDim(int posDim, float posValue, bool useSyncFlow, TrajSample& result, bool* pIsLastSamplFallback, float epsilon, bool allowLastSampleFallback = true);
		bool estimateStateAtPlane(const AAPlane& plane, bool useSyncFlow, TrajSample& result, bool isNoObstacleSimul, bool* pIsLastSamplFallback, bool allowLastSampleFallback = true);
		bool estimateStateAtPlane(const Plane& plane, bool useSyncFlow, TrajSample& result, bool isNoObstacleSimul, bool* pIsLastSamplFallback, bool allowLastSampleFallback = true);

		bool tuneApproximatedEstimate(Ball& ball, TrajSample& sample);

	public:

		struct Sample {

			Time time; 
			Point pos;
			Vector3 vel;

			void set(const Time& time, const Point& pos, const Vector3& vel);
			inline void interpolatePos(const Time& _time, Vector3& result) { pos.addMultiplication(vel, _time - time, result); }

			bool isFootballerReachable(Match& match, const float& ballRadius) const;
			static bool isBallFootballerReachable(Match& match, Ball& ball);

			void interpolateByFactor(const Sample& nextSample, float factor, Sample& result);
			void interpolateByTime(const Sample& nextSample, Time t, Sample& result);
			void interpolateBySpeedSq(const Sample& nextSample, float speedSq, Sample& result);
			void interpolateByPosDim(const Sample& nextSample, int posDim, float posValue, Sample& result);
			void interpolateByPlane(const Sample& nextSample, const AAPlane& plane, Sample& result);
			void interpolateByPlane(const Sample& nextSample, const Plane& plane, Sample& result);
		};

		struct TrajSample : Sample {

			bool isOnPitch;

			void set(const Time& time, const Point& pos, const Vector3& vel, const bool& isOnPitch, bool isResting = false);
			
			void interpolateByFactor(const TrajSample& nextSample, float factor, TrajSample& result);
			void interpolateByTime(const TrajSample& nextSample, Time t, TrajSample& result);
			void interpolateBySpeedSq(const TrajSample& nextSample, float speedSq, TrajSample& result);
			void interpolateByPosDim(const TrajSample& nextSample, int posDim, float posValue, TrajSample& result);

			void script_set(TrajSample& ref) { (*this) = ref; }
			inline bool script_isOnPitch() { return isOnPitch; }
			inline float script_getTime() { return time; }
			Vector3* script_pos() { return &pos; }
			CtVector3* script_getPos() { return &pos; }
			CtVector3* script_getVel() { return &vel; }

			static const TCHAR* ScriptClassName;
			static void declareInVM(SquirrelVM& target);
		};

		struct CollSample : Sample {

			Vector3 postVel;
			SoftPtr<Object> collider;
		};

		struct TriggerSample : Sample {

			SimulationID ballMoveTickID;
			SoftPtr<MaterialTrigger> trigger;
		};

		struct SimulContinueSample : Sample {

			unsigned int tick;
			Vector3 w;
			bool isResting;
		};

		enum EndConditionType {

			EndCondition_None = -1, EndCondition_Apogee, EndCondition_Coll
		};

		struct SimulData {

			Time mSimulMaxFrameDuration;
			Time mSimulFrameDuration;
			unsigned int mSimulSimulMaxFrameCount;
			unsigned int mSimulTickSkipCount;
			float mToleranceSq;
			bool mSimulEndAtRest;
			bool mEnableEarlySimulEnd;
			bool mEnableExtraTrajPrecision;
			bool mEnableFootballerReachableAnalysis;

			SimulState mSimulState;
			unsigned int mNextSimulProcessingFrameIndex;
			Time mNextRecordSimulTick;
			TrajSample mLastInspectedTickEndSample;
			bool mLastInspectedTickEndSampleAdded;
			bool mLastInspectedTickEndSampleReachable;
			bool mCollBetweenSamples;
			SimulContinueSample mLastSimulSample;
			ApproximateType mApproximateType;

			bool mRecordSamples;
			EndConditionType mEndConditionType;

			float mApogeeHeight;
			Time mApogeeTime;

			SimulData();
		};

	public:

		void resetSimul();
		inline const SimulState& getSimulState() { return mSimulData->mSimulState; }
		SimulState updateSimulate(Ball& ball, Time simulStartTime = 0.0f);
		inline bool isValidSimul(bool allowApproximate = false) { return (mSimulData->mSimulState == Simul_Simulated) && (mSimulData->mApproximateType == Approximate_None || allowApproximate); }
		inline bool isProcessingSimul(bool allowApproximate = false) { return (mSimulData->mSimulState == Simul_Simulating) && (mSimulData->mApproximateType == Approximate_None || allowApproximate); }

		inline bool isSimulApproximate() { return mSimulData->mApproximateType != Approximate_None; }
		inline const ApproximateType& getSimulApproximateType() const { return mSimulData->mApproximateType; }

	protected:

		typedef WETL::CountedArray<TriggerSample, false, Index> TriggerSamples;
		typedef WETL::CountedArray<CollSample, false, Index> CollSamples;
		typedef WETL::CountedArray<TrajSample, false, Index> TrajSamples;
		typedef WETL::IndexSpanBatch<Index, SampleInterval> SampleIntervals;

		TriggerSamples mTriggerSamples;
		CollSamples mCollSamples;
		TrajSamples mTrajSamples;
		SampleIntervals mFootballerReachableIntervals;

	protected:

		SimulState continueSimulProcessing(Ball& ball, bool restart, Time simulStartTime = 0.0f);

		void startSyncBall(Ball& ball);
		void stopSyncBall();

	protected:

		unsigned int mID;

		BallCommand mBallCommand;
		
		Time mSimulStartTime;

		HardPtr<SyncData> mSyncData;
		FlexiblePtr<SimulData> mSimulData;
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