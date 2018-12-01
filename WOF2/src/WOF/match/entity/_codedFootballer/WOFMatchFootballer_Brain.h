#ifndef _WOFMatchFootballer_Brain_h
#define _WOFMatchFootballer_Brain_h

#include "WOFMatchFootballer_Body.h"

#include "WOFMatchFootballer_Brain_AI.h"
#include "WOFMatchFootballer_Brain_Player.h"

namespace WOF { namespace match {

	class Footballer;
	class Team;
	class InputController;
	class Player;
	
	class Footballer_Brain : public Footballer_Body, 
			public Footballer_Brain_AI, public Footballer_Brain_Player {

	public:

		bool brain_isAI();
		void brain_playerSwitchFootballer();

		void brain_attachAI();
		bool brain_attachPlayer(InputController& controller);

		Time brain_getAttachTime();

		Player* brain_getAttachedPlayer();
		bool brain_receivedAnyPlayerCommands(); 
		Footballer_Body::BallInteractionType brain_getAttachedLastBallInteraction(Time& interactionTime);

	protected:

		bool brain_mIsAI;
		Time brain_mAttachTime; 
		Time brain_mAttachedLastBallInteractionTime;
		Footballer_Body::BallInteractionType brain_mAttachedLastBallInteraction;

	public:

		Footballer_Brain();

		void cancel_init_create();
		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene(Match& match, Team& team, Footballer& footballer);

		void frameMoveTick(Footballer& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);
		void render(Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass);
	};
	
} }

#endif