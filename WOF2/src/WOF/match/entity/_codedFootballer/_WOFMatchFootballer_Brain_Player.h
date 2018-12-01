#ifndef _WOFMatchFootballer_Brain_Player_h
#define _WOFMatchFootballer_Brain_Player_h

#include "WE3/string/WEBufferString.h"
#include "WE3/data/WEDataSourceChunk.h"
#include "WE3/coordSys/WECoordSysConv.h"
#include "WE3/WEPtr.h"
#include "WE3/render/WERenderer.h"
using namespace WE;

#include "../../WOFMatchTime.h"
#include "../../inputController/WOFMatchInputController.h"
#include "WOFMatchFootballer_Body.h"

namespace WOF { namespace match {

	class Match;
	class Footballer_Brain;
	class Footballer;
	class Team;
	class Player;

	class Footballer_Brain_Player {
	public:

		Footballer_Brain_Player();

		void cancel_init_create();
		bool init_create(Match& match, Footballer_Brain& brain, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(Match& match, Footballer_Brain& brain, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene(Match& match, Team& team, Footballer& footballer);

		Player* getAttachedPlayer();
		InputController* getAttachedInputController();
		void attach(Match& match, Team& team, Footballer_Brain& brain, InputController& inputController);
		void detach(Match& match, Team& team, Footballer_Brain& brain);

		void frameMove(Match& match, Team& team, Footballer_Brain& brain, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);
		void render(Match& match, Team& team, Footballer_Brain& brain, Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass);

		bool receivedAnyPlayerCommands(); //since player attached

	protected:

		#ifdef FOOTBALLER_BODY3
		void syncRunSpeed(Match& match, Footballer_Brain& brain, const Time& time, InputControllerState& inputState, const FootballerBodyDefs::RunSpeedType& speedType);
		void syncDir(Match& match, Footballer_Brain& brain, const Time& time, InputControllerState& inputState, const bool& bodyDir, const bool& runDir);
		void syncHard(Match& match, Team& team, Footballer_Brain& brain);
		void syncIncremental(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputControllerState& inputState);
		#endif

		#ifdef FOOTBALLER_BODY2
		void syncRunSpeed(Match& match, Footballer_Brain& brain, InputControllerState& inputState, const FootballerBodyDefs::RunType& runType);
		void syncDir(Match& match, Footballer_Brain& brain, InputControllerState& inputState, const bool& bodyDir, const bool& runDir);
		void syncHard(Match& match, Team& team, Footballer_Brain& brain);
		void syncIncremental(Match& match, Team& team, Footballer_Brain& brain, InputControllerState& inputState);
		#endif
		

	protected:

		SoftPtr<InputController> mInputController;
		bool mReceivedAnyPlayerCommands;

		bool mForceUpdateRunParams;
	};
	
} }

#endif