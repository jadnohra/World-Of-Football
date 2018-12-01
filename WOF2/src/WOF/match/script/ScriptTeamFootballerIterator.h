#ifndef _ScriptTeamFootballerIterator_h_WOF_match
#define _ScriptTeamFootballerIterator_h_WOF_match

#include "../DataTypes.h"
#include "../FootballerIterator.h"
#include "../../script/ScriptEngine.h"

namespace WOF { namespace match { 

	class Team;
	class ScriptedFootballer;

	class ScriptTeamFootballerIterator {
	public:

		ScriptTeamFootballerIterator();

		void setTeam(Match& match, TeamEnum team);
		void setMaxConsecutiveNextCount(int count);
		void reset();
		ScriptedFootballer* next();

	protected:

		void resetConsecutiveCycle();

	protected:

		SoftPtr<Team> mTeam;
		FootballerIndex mMaxConsecutiveNextCount;
		FootballerIndex mConsecutiveNextCount;
		FootballerIndex mConsecutiveCycleStartIndex;
		FootballerIndex mNextIndex;

	public:

		inline void script_setTeam(Match& match, int team) { return setTeam(match, (TeamEnum) team); }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

} }

#endif