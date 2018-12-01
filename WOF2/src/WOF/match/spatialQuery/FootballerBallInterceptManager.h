#ifndef _FootballerBallInterceptManager_h_WOF_match
#define _FootballerBallInterceptManager_h_WOF_match

#include "../DataTypes.h"

#include "../entity/ball/simul/BallSimulation.h"
#include "../inc/SceneDef.h"

namespace WOF { namespace match {

	class Match;

	class FootballerBallInterceptManager {
	public:

		enum State {

			State_None = -1, State_Invalid, State_Processing, State_Valid
		};

	public:

		FootballerBallInterceptManager();

		void init(Match& match);

		void frameMove(Match& match, const Time& time);
		void render(Renderer& renderer, const bool& flagExtraRenders, RenderPassEnum pass);

		void load(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		inline BallSimulation* getValidSimul() { return mSimul.isValidSync() ? &mSimul : NULL; }
		inline BallSimulation& getSimul() { return mSimul; }

		//to be implemented
		//bool requestUpdate(Match& match, BallInterceptEstimate& estimate, const Time& time); //returns true if directly updated, otherwize estimate must not be destoyed until updated
		
		inline bool isValid() { return mSimul.isValidSync(); }
		inline bool isProcessing() { return mSimul.isProcessingSync(); }
		inline const SimulationID& getSimulID() { return mSimul.getID(); }

	protected:

		SoftPtr<Match> mMatch;

		BallSimulation mSimul;
	};

} }

#endif