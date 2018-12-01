#include "WOFMatchFootballer.h"

#include "../../WOFMatch.h"

namespace WOF { namespace match {

Footballer::Footballer() {

	mTeam = _TeamInvalid;
	mNumber = FootballerNumberInvalid;
}

void Footballer::_cancel_init_create() {

	mTeam = _TeamInvalid;
	mNumber = FootballerNumberInvalid;
}

bool Footballer::isInited() {

	return (mTeam != _TeamInvalid) && (mNumber!= FootballerNumberInvalid);
}

bool Footballer::_init_create(Match& match, TeamEnum team, FootballerNumber number) {

	mTeam = team;
	mNumber = number;

	return true;
}

bool Footballer::_init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	SoftRef<DataChunk> gameChunk = chunk.getChild(L"game");
	if (gameChunk.isNull()) {

		return false;
	}

	if (gameChunk->getAttribute(L"team", tempStr) == false) {

		return false;
	}

	TeamEnum team;
	FootballerNumber number;

	if (tempStr.equals(L"0")) {

		team = _TeamA;

	} else {

		team = _TeamB;
	}

	assrt(sizeof(FootballerNumber) == sizeof(int));
	if (gameChunk->scanAttribute(tempStr, L"number", L"%d", &number) == false) {

		return false;
	}

	return _init_create(match, team, number);
}

bool Footballer::init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;
	
	initSuccess = initSuccess && _init_create(match, tempStr, chunk, pConv);
#ifdef FOOTBALLER_BODY1
	initSuccess = initSuccess && Footballer_Scene::init_create(match, tempStr, chunk, pConv);
#endif
	initSuccess = initSuccess && Footballer_Body::init_create(match, tempStr, chunk, pConv);
	initSuccess = initSuccess && Footballer_Brain::init_create(match, tempStr, chunk, pConv);

	if (initSuccess == false) {

#ifdef FOOTBALLER_BODY1
		Footballer_Scene::cancel_init_create();
#endif
		Footballer_Body::cancel_init_create();
		Footballer_Brain::cancel_init_create();

		_cancel_init_create();
	}

	return initSuccess;
}

bool Footballer::init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	bool initSuccess = true;
	
	Match& match = mNodeMatch.dref();

#ifdef FOOTBALLER_BODY1
	initSuccess = initSuccess && Footballer_Scene::init_nodeAttached(match, tempStr, chunk, pConv);
#endif
	initSuccess = initSuccess && Footballer_Body::init_nodeAttached(match, tempStr, chunk, pConv);
	initSuccess = initSuccess && Footballer_Brain::init_nodeAttached(match, tempStr, chunk, pConv);

	if (initSuccess == false) {
		
#ifdef FOOTBALLER_BODY1
		Footballer_Scene::cancel_init_create();
#endif
		Footballer_Body::cancel_init_create();
		Footballer_Brain::cancel_init_create();

		_cancel_init_create();
	}

	return initSuccess;

}

void Footballer::init_prepareScene() {

	Match& match = mNodeMatch.dref();
	Team& team = match.getTeam(mTeam);
	Footballer& footballer = *this;

#ifdef FOOTBALLER_BODY1
	Footballer_Scene::init_prepareScene(match, team, footballer);
#endif
	Footballer_Body::init_prepareScene(match, team, footballer);
	Footballer_Brain::init_prepareScene(match, team, footballer);

}


void Footballer::frameMove(const DiscreeteTime& time) {

	for (TickCount tick = 0; tick < time.frameTicks; tick++) {

		Footballer_Brain::frameMoveTick(*this, time, tick, time.tickLength);
		Footballer_Body::frameMoveTick(*this, time, tick, time.tickLength);
	}

#ifdef FOOTBALLER_BODY3
	Footballer_Body::onFrameMoved();
#endif
#ifdef FOOTBALLER_BODY2
	Footballer_Body::onFrameMoved();
#endif
#ifdef FOOTBALLER_BODY1
	Footballer_Scene::onFrameMoved();
#endif
}

void Footballer::render(Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass) {

	Footballer_Brain::render(renderer, cleanIfNeeded, flagExtraRenders, pass);

#ifdef FOOTBALLER_BODY3
	Footballer_Body::render(renderer, cleanIfNeeded, flagExtraRenders, pass);
#endif
#ifdef FOOTBALLER_BODY2
	Footballer_Body::render(renderer, cleanIfNeeded, flagExtraRenders, pass);
#endif
#ifdef FOOTBALLER_BODY1
	Footballer_Scene::render(renderer, cleanIfNeeded, flagExtraRenders, pass);
#endif
}

} }