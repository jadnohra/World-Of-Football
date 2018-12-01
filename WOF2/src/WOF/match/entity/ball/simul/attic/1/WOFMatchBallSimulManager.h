#ifndef _WOFMatchBallSimulManager_h
#define _WOFMatchBallSimulManager_h

/*
#include "WOFMatchDataTypes.h"
#include "WOFMatchBallSimulation.h"
#include "WOFMatchObjectDefines.h"
*/

namespace WOF { namespace match {

	class Match;

	class BallSimulManager {
	/*
	public:

		typedef BallSimulation Simul;

	public:

		void init(Match& match);
		void enable(bool enabled);
		void load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		void frameMove(Match& match, const DiscreeteTime& time);
		void render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass);

		//footballer collisions are NOT taken into account
		//when simulation is no longer valid, ID is changed
		Simul* getCurrSimul();
		inline const SimulationID& getCurrSimulID() { return mCurrSimul.getID(); }
		inline SimulationID getNextSimulID() { return getCurrSimulID() + 1; }

	public:

		void onBallSwitchController(Ball& ball);
		void onBallCommand(Ball& ball);
		void onBallOwnershipChange(Ball& ball);

	public:

		void invalidateSimul();
		bool simulIsDesynced(Match& match, const DiscreeteTime& time);
		void reSimul(bool newSimul);
		void setEmptySimul(bool valid);

	protected:

		SoftPtr<Match> mMatch;
		
		bool mEnabled;
		bool mActive;
		bool mLogPerformance;

		bool mBallSimulDirty;

		int mFrameCount;
		TickCount mFPS;
		float mDurationPerFrame;
		int mProcessingFrameCount;


		Simul mCurrSimul;
	*/
	};

} }

#endif