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
			Analyse_Coll,
			Valid_Both
		};

		enum EstimateType {

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
			Update_Invalidated,
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
		
		BallInterceptEstimate(bool enablePathAnalysis = false, bool enableCollAnalysis = false);

		void enableAnalysis(bool pathAnalysis, bool collAnalysis);
		void enableBallOutOfPlayEstimates(bool enable);

		inline bool isPathValid() { return mPathAnalysis->isValid(); }
		inline bool isCollValid() { return mCollAnalysis->isValid(); }
		inline bool isValid() { return mPathAnalysis.isValid() ? isPathValid() : isCollValid(); }
		inline bool isValid(AnalysisMode mode) { return mode == Analyse_Path ? isPathValid() : isCollValid(); }
		AnalysisMode analyseValidity();

		inline void invalidatePath() { mPathAnalysis->invalidate(); }
		inline void invalidateColl() { mCollAnalysis->invalidate(); }
		inline void invalidate() { if (mPathAnalysis.isValid()) invalidatePath(); else invalidateColl(); }
		inline void invalidate(AnalysisMode mode) { if (mode == Analyse_Path) invalidatePath(); else invalidateColl(); }

		inline float getPathInterceptTime() const { return mPathAnalysis->getInterceptTime(); } 
		inline float getPathArrivalTime() const { return mPathAnalysis->getArrivalTime(); } 
		inline float getCollInterceptTime() const { return mCollAnalysis->getInterceptTime(); } 
		inline float getInterceptTime() const { return mPathAnalysis.isValid() ? getPathInterceptTime() : getCollInterceptTime(); } 
		inline float getInterceptTime(AnalysisMode mode) const { return mode == Analyse_Path ? getPathInterceptTime() : getCollInterceptTime(); } 

		inline const EstimateType& getPathType() const { return mPathAnalysis->estimateType; }
		inline const EstimateType& getCollType() const { return mCollAnalysis->estimateType; }
		inline const EstimateType& getType() const { return mPathAnalysis.isValid() ? getPathType() : getCollType(); }
		inline const EstimateType& getType(AnalysisMode mode) const { return mode == Analyse_Path ? getPathType() : getCollType(); }

		inline bool isPathSimulated() { return mPathAnalysis->isSimulated(); }
		inline bool isPathResting() { return mPathAnalysis->isResting(); }
		inline bool isPathInstantaneous(){ return mPathAnalysis->isInstantaneous(); }

		inline bool isSimulated() { return mPathAnalysis.isValid() ? isPathSimulated() : true; }
		inline bool isResting() { return mPathAnalysis.isValid() ? isPathResting() : false; }
		inline bool isInstantaneous() { return mPathAnalysis.isValid() ? isPathInstantaneous() : false; }

		inline bool isSimulated(AnalysisMode mode) { return mode == Analyse_Path ? isPathSimulated() : true; }
		inline bool isResting(AnalysisMode mode) { return mode == Analyse_Path ? isPathResting() : false; }
		inline bool isInstantaneous(AnalysisMode mode) { return mode == Analyse_Path ? isPathInstantaneous() : false; }

		bool validatePath(const Time& currTime, BallSimulation* pSimul, Ball* pBall, bool setEstimateType, bool& outTryEstimates);
		bool validateColl(const Time& currTime, BallSimulation* pSimul, Ball* pBall, bool setEstimateType, bool& outTryEstimates);
		inline bool validate(const Time& currTime, BallSimulation* pSimul, Ball* pBall, bool setEstimateType, bool& outTryEstimates) { return mPathAnalysis.isValid() ? validatePath(currTime, pSimul, pBall, setEstimateType, outTryEstimates) : validateColl(currTime, pSimul, pBall, setEstimateType, outTryEstimates); }
		inline bool validate(AnalysisMode mode, const Time& currTime, BallSimulation* pSimul, Ball* pBall, bool setEstimateType, bool& outTryEstimates) { return mode == Analyse_Path ? validatePath(currTime, pSimul, pBall, setEstimateType, outTryEstimates) : validateColl(currTime, pSimul, pBall, setEstimateType, outTryEstimates); }
		
		inline CtVector3* getPathPos() { return mPathAnalysis->getPos(mSimul); }
		inline CtVector3* getPathVel() { return mPathAnalysis->getVel(mSimul); }
		inline CtVector3* getCollPos() { return mCollAnalysis->getPos(mSimul); }
		inline CtVector3* getCollVel() { return mCollAnalysis->getVel(mSimul); }
		inline CtVector3* getPos() { return mPathAnalysis.isValid() ? getPathPos() : getCollPos(); }
		inline CtVector3* getVel() { return mPathAnalysis.isValid() ? getPathVel() : getCollVel(); }
		inline CtVector3* getPos(AnalysisMode mode) { return mode == Analyse_Path ? getPathPos() : getCollPos(); }
		inline CtVector3* getVel(AnalysisMode mode) { return mode == Analyse_Path ? getPathVel() : getCollVel(); }

		UpdateResult updatePath(Ball& ball, Footballer& footballer, const Time& currTime, BallSimulation* pSimul, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos);
		UpdateResult updateColl(Ball& ball, Footballer& footballer, const Time& currTime, BallSimulation* pSimul);

		float estimateTimeUntilValidity(Footballer& footballer);

	public:

		bool setSimulation(BallSimulation& simul, const float& time);

	public:
		
		inline int script_getPathType() { return getPathType(); }
		inline int script_getCollType() { return getCollType(); }
		inline int script_getType() { return getType(); }
		inline int script_getModeType(int mode) { return getType((AnalysisMode) mode); }

		inline bool script_isValid() { return isValid(); }
		inline bool script_isValidMode(int mode) { return isValid((AnalysisMode) mode); }

		inline int script_analyseValidity() { return analyseValidity(); } 

		inline void script_invalidate() { return invalidate(); }
		inline void script_invalidateMode(int mode) { return invalidate((AnalysisMode) mode); }

		inline float script_getPathInterceptTime() { return getPathInterceptTime(); }
		inline float script_getPathArrivalTime() { return getPathArrivalTime(); }
		inline float script_getCollInterceptTime() { return getCollInterceptTime(); }
		inline float script_getInterceptTime() { return getInterceptTime(); }
		inline float script_getModeInterceptTime(int mode) { return getInterceptTime((AnalysisMode) mode); }

		inline bool script_isSimulated() { return isSimulated(); }
		inline bool script_isResting() { return isResting(); }
		inline bool script_isInstantaneous() { return isInstantaneous(); }

		inline bool script_isModeSimulated(int mode) { return isSimulated((AnalysisMode) mode); }
		inline bool script_isModeResting(int mode) { return isResting((AnalysisMode) mode); }
		inline bool script_isModeInstantaneous(int mode) { return isInstantaneous((AnalysisMode) mode); }

		inline CtVector3* script_getPos() { return getPos(); }
		inline CtVector3* script_getVel() { return getVel(); }
		inline CtVector3* script_getModePos(int mode) { return getPos((AnalysisMode) mode); }
		inline CtVector3* script_getModeVel(int mode) { return getVel((AnalysisMode) mode); }

		int script_updatePath(ScriptedFootballer* pFootballer, Time currTime, bool fallbackLastSimulSample, bool fallbackRestingPos, bool fallbackCurrPos);
		int script_updateColl(ScriptedFootballer* pFootballer, Time currTime);

		float script_estimateTimeUntilValidity(ScriptedFootballer* pFootballer);

	protected:

		struct Analysis {

			EstimateType estimateType;

			float analysisTime;
		
			float arrivalTime;
			float waitTime; 

			Analysis() : estimateType(Estimate_None) {}

			inline void invalidate() { estimateType = Estimate_None; }

			inline bool isValid() { return estimateType != Estimate_None; }
			inline float getInterceptTime() const { return arrivalTime + waitTime;  } 
			inline const float& getArrivalTime() const { return arrivalTime;  } 
	
			inline bool isSimulated() { return estimateType == Estimate_SimulInterceptorWait || estimateType == Estimate_SimulBallWait || estimateType == Estimate_SimulLastBallWait; }
			inline bool isResting() { return estimateType == Estimate_RestingBall; }
			inline bool isInstantaneous() { return estimateType == Estimate_CurrBall; }

			void prepare(const float& time);
			inline void setAnalysisTime(const float& time) { analysisTime = time; }
			static EstimateType analyzeSampleIntercept(Footballer& footballer, const FootballerInterceptState& footballerState, const Vector3& samplePos, const Time& sampleTime, const Time& currTime, float& outArrivalTime, float& outFootballerWaitTime);
		};

		struct PathAnalysis : Analysis {

			bool isSingleSample;

			BallSimulation::Index sampleIndex[2];
			Vector3 interceptPos;
			Vector3 interceptBallVel;

			unsigned int restingID;

			bool setSample(BallSimulation& simul, const BallSimulation::Index& sampleIndex);
			bool setSample(BallSimulation& simul, const BallSimulation::Index& fromIndex, const BallSimulation::Index& toIndex, const Vector3& pos, const float& u);
			bool setSample(Ball& ball);

			EstimateType tryEstimateRestingBall(Ball& ball, Footballer& footballer, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType);
			EstimateType tryEstimateEarliestSimulIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType);
			EstimateType tryEstimateLastSimulSampleIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType);
			EstimateType tryEstimateCurrBall(Ball& ball, Footballer& footballer, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType);

			CtVector3* getPos(BallSimulation* pSimul);
			CtVector3* getVel(BallSimulation* pSimul);
		};

		struct CollAnalysis : Analysis {

			BallSimulation::Index sampleIndex;

			EstimateType tryEstimateEarliestCollSimulIntercept(Ball& ball, Footballer& footballer, BallSimulation& simul, const Time& currTime, FootballerInterceptState* pFootballerState, bool& estimateChanged, bool setEstimateType);

			CtVector3* getPos(BallSimulation* pSimul);
			CtVector3* getVel(BallSimulation* pSimul);
		};

	protected:

		bool mEnableBallOutOfPlayEstimates;

		SoftPtr<BallSimulation> mSimul;
		SimulationID mSimulID;

		HardPtr<PathAnalysis> mPathAnalysis;
		HardPtr<CollAnalysis> mCollAnalysis;

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};
	
} }

#endif