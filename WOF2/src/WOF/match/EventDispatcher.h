#ifndef h_WOF_match_EventDispatcher
#define h_WOF_match_EventDispatcher

#include "WE3/WEPtr.h"
using namespace WE;

#include "DataTypes.h"

namespace WOF { namespace match {

	class Match;
	class Ball;

	class EventDispatcher {
	public:

		void init(Match& match);

		void onBallSwitchController(Ball& ball);
		void onBallCommand(Ball& ball);

		void onBallOwnershipChange(Ball& ball);

		void onActiveFootballersChange();

	protected:

		SoftPtr<Match> mMatch;
	};

} }

#endif