#ifndef h_WOF_match_Player
#define h_WOF_match_Player

#include "WE3/WEPTr.h"
#include "WE3/input/include.h"
#include "WE3/render/WERenderer.h"
using namespace WE;

#include "DataTypes.h"
#include "inc/SceneDef.h"
#include "switchControl/ControlSwitchStates.h"

namespace WOF { namespace match {

	class Footballer;
	class Team;

	class Player {
	public:

		Player(Match& match, unsigned int switchPriority, float inputReactionTime, String* pID = NULL);

		TeamEnum getAttachedTeamEnum();
		Team& getAttachedTeam();
		void attachTeam(TeamEnum team, unsigned int index);
		void detachTeam();
	
		Input::Controller* getAttachedController(); 
		void attachController(Input::Controller& controller);
		void detachController();
		
		Footballer* getAttachedFootballer(); 
		void attachFootballer(Footballer& footballer); //can be used safely even if another or same footballer attached
		void detachFootballer();

		unsigned int getSwitchPriority();
		bool isSwitchLocked();
		bool isAskingForManualSwitch();

		const String& getID() { return mID; }

		void setSkin(Match& match, Team& team, PlayerSkin* pSkin);
		inline SoftPlayerSkin& getSkin() { return mSkin; }
		
		void frameMove(Match& match, const DiscreeteTime& time);
		void render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass);

		inline const float& getInputReactionTime() { return mInputReactionTime; }

	public:

		SwitchControl::PlayerState hSwitchState;

	protected:

		String mID;
		unsigned int mSwitchPriority;
		unsigned int mIndexInTeam;
		float mInputReactionTime;

		SoftPtr<Match> mMatch;
		TeamEnum mTeam;
		SoftPtr<Input::Controller> mController;
		SoftPtr<Footballer> mFootballer;

		SoftPlayerSkin mSkin;
	};

} }

#endif