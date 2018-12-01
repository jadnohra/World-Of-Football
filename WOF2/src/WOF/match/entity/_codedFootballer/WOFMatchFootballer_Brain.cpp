#include "WOFMatchFootballer_Brain.h"

#include "../../WOFMatch.h"
#include "WOFMatchFootballer.h"
#include "../../WOFMatchTeam.h"
#include "../../WOFMatchPlayer.h"

namespace WOF { namespace match {

Footballer_Brain::Footballer_Brain() {

	brain_mIsAI = true;
	brain_mAttachTime = 0.0f;
	brain_mAttachedLastBallInteraction = FBI_None;
}

void Footballer_Brain::cancel_init_create() {

	Footballer_Brain_AI::cancel_init_create();
	Footballer_Brain_Player::cancel_init_create();
}

bool Footballer_Brain::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;

	initSuccess = initSuccess && Footballer_Brain_AI::init_create(match, *this, tempStr, chunk, pConv);
	initSuccess = initSuccess && Footballer_Brain_Player::init_create(match, *this, tempStr, chunk, pConv);

	return initSuccess;
}

bool Footballer_Brain::init_nodeAttached(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;

	initSuccess = initSuccess && Footballer_Brain_AI::init_nodeAttached(match, *this, tempStr, chunk, pConv);
	initSuccess = initSuccess && Footballer_Brain_Player::init_nodeAttached(match, *this, tempStr, chunk, pConv);

	return initSuccess;
}

void Footballer_Brain::init_prepareScene(Match& match, Team& team, Footballer& footballer) {

	Footballer_Brain_AI::init_prepareScene(match, team, footballer);
	Footballer_Brain_Player::init_prepareScene(match, team, footballer);
}

void Footballer_Brain::render(Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass) {

	Match& match = mNodeMatch.dref();

	if (match.getFlagRenderEntityStates() == false) {

		return;
	}

	Team& team = match.getTeam(mTeam);
	Footballer_Brain& brain = *this;

	if (brain_mIsAI) {

		Footballer_Brain_AI::render(match, team, brain, renderer, cleanIfNeeded, flagExtraRenders, pass);
	} else {

		Footballer_Brain_Player::render(match, team, brain, renderer, cleanIfNeeded, flagExtraRenders, pass);
	}
}

void Footballer_Brain::frameMoveTick(Footballer& footballer, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

	Match& match = mNodeMatch.dref();
	Team& team = match.getTeam(mTeam);
	Footballer_Brain& brain = *this;

	if (brain_mIsAI) {

		Footballer_Brain_AI::frameMove(match, team, brain, time, tickIndex, tickLength);

	} else {

		Footballer_Brain_Player::frameMove(match, team, brain, time, tickIndex, tickLength);
	}

#ifdef FOOTBALLER_BODY3
	Footballer::BallInteractionType interaction = footballer.getBallInteractionType();
#endif
#ifdef FOOTBALLER_BODY2
	Footballer::BallInteraction interaction = footballer.getBallInteraction();
#endif
#ifdef FOOTBALLER_BODY1
	Footballer::BallInteraction interaction = footballer.body_getBallInteraction();
#endif

	if (interaction != FBI_None) {

		brain_mAttachedLastBallInteraction = interaction;
		brain_mAttachedLastBallInteractionTime = time.t_discreet;
	}

}

Time Footballer_Brain::brain_getAttachTime() {

	return brain_mAttachTime;
}

Footballer_Body::BallInteractionType Footballer_Brain::brain_getAttachedLastBallInteraction(Time& interactionTime) {

	interactionTime = brain_mAttachedLastBallInteractionTime;

	return brain_mAttachedLastBallInteraction;
}

bool Footballer_Brain::brain_isAI() {

	return brain_mIsAI;
}

void Footballer_Brain::brain_playerSwitchFootballer() {

	if (brain_mIsAI) {

		return;
	}

	/*
	Player* pPlayer = Footballer_Brain_Player::getAttachedInputController()->getPlayer();

	if (pPlayer) {

		mNodeMatch->getTeam(mTeam).switchFootballer(mNodeMatch.dref(), *pPlayer, true);

	} else {

		assrt(false);
	}
	*/
}

void Footballer_Brain::brain_attachAI() {

	if (brain_mIsAI == false) {

		Match& match = mNodeMatch.dref();
		Team& team = match.getTeam(mTeam);
		Footballer_Brain& brain = *this;

		Footballer_Brain_Player::detach(match, team, brain);
		Footballer_Brain_AI::attach(match, team, brain);

		brain_mIsAI = true;

		brain_mAttachTime = match.getTime().getTime();
		brain_mAttachedLastBallInteraction = FBI_None;
	}
}

Player* Footballer_Brain::brain_getAttachedPlayer() {

	return brain_mIsAI ? NULL : Footballer_Brain_Player::getAttachedPlayer();
}

bool Footballer_Brain::brain_receivedAnyPlayerCommands() {

	return brain_mIsAI ? false : Footballer_Brain_Player::receivedAnyPlayerCommands();
}

bool Footballer_Brain::brain_attachPlayer(InputController& controller) {

	if (brain_mIsAI == false) {

		if (Footballer_Brain_Player::getAttachedInputController() == &controller) {

			return true;
		}

		assrt(false);
		return false;
	}

	

	Match& match = mNodeMatch.dref();
	Team& team = match.getTeam(mTeam);
	Footballer_Brain& brain = *this;

	Footballer_Brain_AI::detach(match, team, brain);
	Footballer_Brain_Player::attach(match, team, brain, controller);

	brain_mIsAI = false;
	brain_mAttachTime = match.getTime().getTime();
	brain_mAttachedLastBallInteraction = FBI_None;

	return true;
}

} }