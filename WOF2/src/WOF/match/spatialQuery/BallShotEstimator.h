#ifndef _BallShotEstimator_h_WOF_match
#define _BallShotEstimator_h_WOF_match

#include "WE3/script/WEScriptArgByRef.h"
#include "WE3/script/WEScriptEngine.h"
using namespace WE;

#include "../entity/ball/simul/BallSimulation.h"
#include "../DataTypes.h"

namespace WOF { namespace match {

	class BallShotEstimator {
	public:

		typedef unsigned int Index;

		struct EstimResult {

			bool isConstraintSatisfied;
			float minWaitTime;
			float speed;

			static const TCHAR* ScriptClassName;
			static void declareInVM(SquirrelVM& target);
		};

		class Group {
		public:

			struct Record {

				Record();
				void onSimulUpdated();

				inline const float& getSpeed() { return speed; }
				inline const float& getMaxFwdDist() { return maxFwdDist; }

				HardPtr<BallSimulation> simulation;
				float speed;
				float maxFwdDist;
				float maxT;
			};

		public:

			Group(const TCHAR* name);

			inline const Index& getRecordCount() { return mRecords.count; } 
			inline Record& getRecord(const Index& i) { return dref(mRecords[i]); }

			inline void addRecord(Record* pRecord) { mRecords.addOne(pRecord); }
			
			bool estimateNeededShotSpeedByTime(BallShotEstimator& estimator, CtVector3& dir, CtVector3& from, CtVector3& to, const float& posEpsilon, float t, const float& timeEpsilon, EstimResult& result);
			bool estimateNeededShotSpeedBySpeed(BallShotEstimator& estimator, CtVector3& dir, CtVector3& from, CtVector3& to, const float& posEpsilon, float speed, const float& timeEpsilon, EstimResult& result);

			bool toLocalPos(BallShotEstimator& estimator, CtVector3& dir, CtVector3& from, CtVector3& to, Vector3& outLocalTargetPos);

		protected:

			bool estimateNeededShotSpeedLocalByTime(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& localTargetPos, const float& posEpsilon, const float& t, const float& timeEpsilon, EstimResult& result);
			bool estimateNeededShotSpeedLocalBySpeed(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& posEpsilon, const float& speed, const float& speedEpsilon, EstimResult& result);

			bool interpolateByTime(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& t, 
											Record& record, BallSimulation::TrajSample& trajSample, Record* pLastRecord, BallSimulation::TrajSample* pLastTrajSample, bool lastTrajSampleIsLastFallback, float& outSpeed);
			bool interpolateBySpeedSq(BallShotEstimator& estimator, const float& startHeightDiff, const Vector3& targetPos, const float& speedSq, 
											Record& record, BallSimulation::TrajSample& trajSample, Record* pLastRecord, BallSimulation::TrajSample* pLastTrajSample, bool lastTrajSampleIsLastFallback, float& outSpeed);


		protected:

			typedef WETL::CountedPtrArrayEx<Record, Index, WETL::ResizeExact> Records;

			String mName;
			StringHash mNameHash;
			Records mRecords;
		};

	public:

		BallShotEstimator();
		BallShotEstimator(Match& match);

		void load(Match& match, BufferString& tempStr, DataChunk& mainChunk, CoordSysConv* pConv);

		bool addRecord(const TCHAR* groupName, BallCommand& command, bool autoCreateGroup = false);

		bool estimateNeededShotSpeedByTime(const TCHAR* groupName, CtVector3& dir, CtVector3& from, CtVector3& to, const float& posEpsilon, float t, const float& timeEpsilon, EstimResult& result);
		bool estimateNeededShotSpeedBySpeed(const TCHAR* groupName, CtVector3& dir, CtVector3& from, CtVector3& to, const float& posEpsilon, float speed, const float& speedEpsilon, EstimResult& result);

		void render(Renderer& renderer, 
							 const float* pBallRadiusStart = NULL, const float* pBallRadiusEnd = NULL, 
							 const RenderColor* pLineStart = &RenderColor::kWhite, const RenderColor* pLineEnd = &RenderColor::kRed, 
							 const RenderColor* pSphereStart = &RenderColor::kGreen, const RenderColor* pSphereEnd = &RenderColor::kRed,
							 const RenderColor* pSphereColl = &RenderColor::kBlack);


		/*
		bool simulateApogee(const Vector3& velocity, float& outApogeeHeight, Time& outApogeeTime);
		bool simulatePitchColl(const float& height, BallSimulation::CollSample& outColl);
		*/

		inline BallSimulation_SimulPitch& getTempSimulPitchSimulation() { return mSimulPitchSimulation; }
		bool simulateSingleVerticalBounce(const Vector3& startVelocity, BallSimulation& inOutSimul, bool recordSamples);

	protected:

		const Vector3& getStartPos();

	protected:

		typedef WETL::PtrHashMap<StringHash, Group*, Index> GroupMap;

		SoftPtr<Match> mMatch;
		HardPtr<BallSimulation::SimulData> mSimulData;
		GroupMap mGroupMap;

		bool mStartPosSet;
		Vector3 mStartPos;

		BallSimulation_SimulPitch mSimulPitchSimulation;

	public:

		inline bool script_addRecord(const TCHAR* groupName, BallCommand* pCommand, bool autoCreateGroup) { return addRecord(groupName, dref(pCommand), autoCreateGroup); }
		
		inline bool script_estimateNeededShotSpeedByTime(const TCHAR* groupName, CtVector3* dir, CtVector3* from, CtVector3* to, float posEpsilon, float t, float tEpsilon, EstimResult* result) {

			return estimateNeededShotSpeedByTime(groupName, dref(dir), dref(from), dref(to), posEpsilon, t, tEpsilon, dref(result));
		}

		inline bool script_estimateNeededShotSpeedBySpeed(const TCHAR* groupName, CtVector3* dir, CtVector3* from, CtVector3* to, float posEpsilon, float speed, float speedEpsilon, EstimResult* result) {

			return estimateNeededShotSpeedBySpeed(groupName, dref(dir), dref(from), dref(to), posEpsilon, speed, speedEpsilon, dref(result));
		}

	public:

		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);
	};

} }

#endif