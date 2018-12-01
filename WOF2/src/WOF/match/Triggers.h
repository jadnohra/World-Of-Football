#ifndef h_WOF_match_Triggers
#define h_WOF_match_Triggers

#include "WE3/defaultImpl/world/WEWorldMaterial.h"
using namespace WE;

#include "DataTypes.h"

namespace WOF { namespace match {

	class Match;

	class MatchTrigger_Goal : public MaterialTrigger {
	public:

		MatchTrigger_Goal();
		void init(Match& match, TeamEnum team);
		
		virtual unsigned int getID() const { return TriggerID_Goal; }

		virtual void execute(const Vector3* pPos);

		inline const TeamEnum& getTeam() const { return mTeam; }

	protected:

		SoftPtr<Match> mMatch;
		TeamEnum mTeam;
		Time mLastTriggerTime;
	};

} }

#endif