#ifndef _BallInterceptEstimate_h_WOF_match
#define _BallInterceptEstimate_h_WOF_match

#include "../entity/ball/simul/BallSimulation.h"
#include "../DataTypes.h"

namespace WOF { namespace match {

	class Footballer;

	class BallInterceptEstimate {
	public:

		enum AnalysisMode {

			Analyse_None = -1, 
			Analyse_Path, 
			Analyse_Colls
		};

		enum Type {

			Estimate_None = -1, 
			Estimate_SimulInterceptorWait, 
			Estimate_SimulBallWait,
			Estimate_SimulLastBallWait,
			Estimate_CurrBall,
			Estimate_RestingBall
		};

		enum UpdateResult {

			Update_None = -1,
			Update_NoChange,
			Update_NewEstimate,
			Update_Invalidated
		};

		struct FootballerInterceptState {

			//float playerReactionTime; //in seconds
			float distanceTolerance;
			
			float vel;
			float activationTime;
			float penaltyTime;

			FootballerInterceptState();
			void init(Footballer& footballer);
		};

	public:
		
		BallInterceptEstimate() : mType(Estimate_None) {}

		inline bool isValid() { return mType != Estimate_None; }
		inline float getInterceptTime() const { return mArrivalTime + mWaitTime;  } 

		inline const Type& getType() const { return mType; }

		bool validate(const Time& currTime, BallSimulation* pSimul, Ball* pBall);

		bool tryEstimateEarliestSimulIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged);
		bool tryEstimateLastSimulSampleIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged);
		bool tryEstimateRestingBall(Ball& ball, Footballer& footballer, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged);
		bool tryEstimateCurrBall(Ball& ball, Footballer& footballer, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged);
		bool tryEstimateEarliestCollSimulIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged);

		UpdateResult update(Ball& ball, Footballer& footballer, const Time& currTime, BallSimulation* pSimul, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos);

	public:

		bool prepareForAnalysis(BallSimulation& simul, const float& time);
		void prepareForAnalysis(const float& time);
		static Type analyzeSampleIntercept(Footballer& footballer, const FootballerInterceptState& footballerState, const Vector3& samplePos, const Time& sampleTime, const Time& currTime, float& outArrivalTime, float& outFootballerWaitTime);

		bool setSample(const BallSimulation::Index& sampleIndex);
		bool setSample(const BallSimulation::Index& fromIndex, const BallSimulation::Index& toIndex, const Vector3& pos, const float& u);
		bool setSample(Ball& ball);

	public:

		inline bool script_isValid() { return isValid(); }
		inline void script_invalidate() { mType = Estimate_None; }
		inline int script_getType() { return mType; }
		inline bool script_isSimulated() { return mType == Estimate_SimulInterceptorWait || mType == Estimate_SimulBallWait || mType == Estimate_SimulLastBallWait; }
		inline bool script_isResting() { return mType == Estimate_RestingBall; }
		inline bool script_isInstantaneous() { return mType == Estimate_CurrBall; }
		CtVector3* script_getPos();
		CtVector3* script_getVel();

		int script_update(Match* pMatch, ScriptedFootballer* pFootballer, Time currTime, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos);

	protected:

		struct Analysis {

			Type type;

			float analysisTime;
		
			float arrivalTime;
			float waitTime; 
		};

		struct PathAnalysis : Analysis {

			bool isSingleSample;

			BallSimulation::Index sampleIndex[2];
			Vector3 interceptPos;
			Vector3 interceptBallVel;

			unsigned int restingID;
		};

		struct CollAnalysis : Analysis {

			BallSimulation::Index sampleIndex;
		};

	protected:

		Type mType;
		
		SoftPtr<BallSimulation> mSimul;
		SimulationID mSimulID;

		float mAnalysisTime;
		
		float mArrivalTime;
		float mWaitTime; 

		bool mIsSingleSample;

		BallSimulation::Index mSampleIndex[2];
		Vector3 mInterceptPos;
		Vector3 mInterceptBallVel;

		unsigned int mRestingID;

		Type mCollType;
		BallSimulation::Index mCollSampleIndex;

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};
	
} }

#endif