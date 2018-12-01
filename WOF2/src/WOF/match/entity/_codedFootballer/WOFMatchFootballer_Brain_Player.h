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

		class InputStateInterpreter {
		public:

			InputStateInterpreter() {}

			void attachController(InputController& controller, const DiscreeteTime& time);
			void detachController();

			inline bool isValid() { return mController.isValid(); }

			inline InputController* getController() { return mController; }
			inline InputControllerState& getInputState() { return mController->mState; }
			
			bool frameMove(Match& match, const DiscreeteTime& time);

			Footballer_Body::ShotType getShootType();
			Footballer_Body::RunType getRunType();
			Footballer_Body::RunSpeedType getRunSpeedType();
			Footballer_Body::JumpType getJumpType();

			inline bool getModifier() { return getInputState().modifier.state; } 
			
			void syncRunParams(Match& match, Footballer_Brain& brain, const Time& time, Footballer_BodyAction_Running& action, bool force = false);
			void syncTackleParams(Match& match, Footballer_Brain& brain, const Time& time, Footballer_BodyAction_Tackling& action, bool force = false);
			void syncJumpParams(Match& match, Footballer_Brain& brain, const Time& time, Footballer_BodyAction_Jumping& action, bool force = false);

			inline const Vector3& getMoveDir() { return getInputState().moveDir; } 
			inline bool hasMoveDir() { return getInputState().move.state; }

			//relativizes if needed
			const Vector3& getMoveDir(Match& match, Vector3& temp);

			const float& getReactionTime();

		protected:

			SoftPtr<InputController> mController;
		};

		class ActionHandler {
		public:

			//virtual Footballer_Body::ActionType getType() = 0;

			virtual void attach(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) {}
			virtual void detach(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) {}

			virtual void frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr) = 0;
		};

		class ActionHandler_Default : public ActionHandler {
		public:

			ActionHandler_Default();

			//virtual Footballer_Body::ActionType getType();

			virtual void attach(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr);

			virtual void frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr);

		protected:

			Footballer_Body::ActionType mTrackingAction;
			Time mTrackingActionStartTime;
		};

		class ActionHandler_Running : public ActionHandler {
		public:

			//virtual Footballer_Body::ActionType getType();

			virtual void frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr);
		};

		class ActionHandler_Tackling : public ActionHandler {
		public:

			//virtual Footballer_Body::ActionType getType();

			virtual void frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr);
		};

		/*
		class ActionHandler_Falling : public ActionHandler {
		public:

			//virtual Footballer_Body::ActionType getType();

			virtual void frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr);
		};

		class ActionHandler_Jumping : public ActionHandler {
		public:

			//virtual Footballer_Body::ActionType getType();

			virtual void frameMove(Match& match, Team& team, Footballer_Brain& brain, const Time& time, InputStateInterpreter& inputState, Footballer_Body::ActionImpl& curr);
		};
		*/

	protected:

		ActionHandler* getHandlerFor(const Footballer_Body::ActionType& actionType);

	protected:

		InputStateInterpreter mInputInterpreter;
		bool mReceivedAnyPlayerCommands;

		SoftPtr<Footballer_Body::ActionImpl> mLastAction;

		SoftPtr<ActionHandler> mHandler;
		
		ActionHandler_Default mHandler_Default;
		ActionHandler_Running mHandler_Running;
		ActionHandler_Tackling mHandler_Tackling;
		//ActionHandler_Tackling mHandler_Falling;
		//ActionHandler_Jumping mHandler_Jumping;
	};
	
} }

#endif