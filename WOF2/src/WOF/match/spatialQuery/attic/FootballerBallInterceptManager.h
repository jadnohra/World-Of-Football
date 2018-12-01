#ifndef _FootballerBallInterceptManager_h_WOF_match
#define _FootballerBallInterceptManager_h_WOF_match

#include "../WOFMatchObjectDefines.h"
#include "../entity/ball/ballSimul/Ballsimulation2.h"

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

		inline BallSimulation2* getValidSimul() { return mSimul.isValid() ? &mSimul : NULL; }
		inline BallSimulation2& getSimul() { return mSimul; }

		bool requestUpdate(Match& match, Footballer& footballer, const Time& time); //returns true if directly updated
		bool update(Match& match, Footballer& footballer, const Time& time); //returns true if footballer has a valid intercept
		bool isStillIntercepting(Match& match, Footballer& footballer, const Time& time); //checks if current intercept if still ok

		inline bool isValid() { return mSimul.isValid(); }
		inline bool isProcessing() { return mSimul.isProcessing(); }
		inline const SimulationID& getSimulID() { return mSimul.getID(); }

	protected:

		SoftPtr<Match> mMatch;

		BallSimulation2 mSimul;
	};

} }

#endif