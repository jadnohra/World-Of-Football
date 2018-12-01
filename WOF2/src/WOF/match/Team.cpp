#include "entity/footballer/Footballer.h"

#include "Team.h"

#include "Match.h"
#include "Player.h"

namespace WOF { namespace match {

Team::Team() {

	mPlayerAttachedFootballerCount = 0;
	mRecievedGoalCount = 0;
	mSwitchedFormationFlag = false;
	mIsDeclared = false;
	mActiveFootballerCount = 12;
	mActiveFootballerCountLeft = mActiveFootballerCount;
	mActiveFootballerStartIndex = 0;
	mActiveFootballerStartIndexLeft = mActiveFootballerStartIndex;

	constructorTactical();
}

Team::~Team() {
}

bool Team::init(Match& match, TeamEnum team) {

	mMatch = &match;
	mTeam = team;

	mIsTeamWithFwdDir = match.getTeamWithWorldFwdDir() == mTeam;

	{
		mFwdDir.el[Scene_Right] = 0.0f;
		mFwdDir.el[Scene_Forward] = mIsTeamWithFwdDir ? 1.0f : -1.0f;
		mFwdDir.el[Scene_Up] = 0.0f;
	}

	return true;
}

bool Team::addFormation(TeamFormation* pFormation) {

	if (pFormation) {

		if (mFormations.insertFormation(pFormation)) {

			if (!mCurrFormationMode.isValid()) {

				mCurrFormation = pFormation;
				mCurrFormationMode = mCurrFormation->getMode(0);
			}

			return true;
		}
	}

	return false;
}

bool Team::switchFormation(const TCHAR* formationName, const TCHAR* modeName) {

	bool hasChanges = false;

	if (formationName) {

		SoftPtr<TeamFormation> formation = mFormations.findFormation(formationName);

		if (formation.isValid() && formation.ptr() != mCurrFormation.ptr()) {

			hasChanges = true;
			mCurrFormation = formation;
		}
	}

	if (mCurrFormation.isValid() && (hasChanges || modeName != NULL)) {

		const TCHAR* modeNameTarget = modeName;

		if (modeName == NULL && mCurrFormationMode.isValid()) {

			modeNameTarget = mCurrFormationMode->getName();
		}

		SoftPtr<TeamFormation::Mode> mode = mCurrFormation->findMode(modeNameTarget);

		if (mode.isValid() && mode.ptr() != mCurrFormationMode.ptr()) {

			hasChanges = true;
			mCurrFormationMode = mode;
		}

		if (!mode.isValid()) {

			if (mCurrFormation->getModeCount()) {

				mCurrFormationMode = mCurrFormation->getMode(0);
				hasChanges = true;
			}
		}
	}

	if (hasChanges)
		mSwitchedFormationFlag = true;

	return hasChanges;
}

const TeamFormation::Element* Team::getFormationInfo(StringHash code, FootballerNumber* pFallbackNumber) {

	if (mCurrFormationMode.isValid()) {

		return mCurrFormationMode->findElement(code, pFallbackNumber);
	}

	return NULL;
}

TeamFormation::Element* Team::getFormationInfoRef(StringHash code, FootballerNumber* pFallbackNumber) {

	if (mCurrFormationMode.isValid()) {

		return mCurrFormationMode->findElement(code, pFallbackNumber);
	}

	return NULL;
}

Player& Team::getPlayer(const PlayerIndex& index) { return dref(mPlayers[index]); }

bool Team::addFootballer(Footballer* pFootballer) {

	if (mFootballerMap.hasKey(pFootballer->mNumber)) {

		assrt(false);
		return false;
	}

	mFootballerMap.insert(pFootballer->mNumber, pFootballer);
	mFootballerArray.addOne(pFootballer);

	return true;
}

bool Team::removeFootballer(const FootballerNumber& num) {

	Footballer* pFootballer;

	if (getFootballer(num, pFootballer)) {

		return removeFootballer(pFootballer);
	}

	return false;
}

bool Team::removeFootballer(Footballer* pFootballer) {

	if (mFootballerMap.remove(pFootballer->mNumber)) {

		FootballerArray::Index idx;

		mFootballerArray.searchFor(pFootballer, 0, mFootballerArray.count, idx);
		mFootballerArray.removeSwapWithLast(idx, true);

		return true;
	}

	return false;
}

bool Team::hasFootballer(Footballer* pFootballer) {

	return hasFootballer(pFootballer->mNumber);
}

bool Team::hasFootballer(const FootballerNumber& num) {

	return (mFootballerMap.hasKey(num));
}

bool Team::getFootballer(const FootballerNumber& num, Footballer*& pFootballer) {

	return mFootballerMap.find(num, pFootballer);
}

void Team::onPlayerAttachedFootballer(Footballer* pFootballer) {

	if (pFootballer)
		++mPlayerAttachedFootballerCount;
}

void Team::onPlayerDetachedFootballer(Footballer* pFootballer) {

	if (pFootballer)
		--mPlayerAttachedFootballerCount;
}

FootballerIndex Team::getFootballerCount(Filter filter) {

	switch (filter) {

		case F_AI: 
			return mFootballerArray.count - mPlayerAttachedFootballerCount; 

		case F_Player:
			return mPlayerAttachedFootballerCount;
	}

	return mFootballerArray.count;
}

bool Team::iterateFootballers(Footballer*& pInOutFootballer, FootballerIndex& iter, Filter filter) {

	if (pInOutFootballer == NULL) {
		
		iter = 0;

		if (mFootballerArray.count) {

			pInOutFootballer = mFootballerArray.el[iter];

		} else {

			pInOutFootballer = NULL;
		}

	} else {

		++iter;

		if (iter >= mFootballerArray.count) {

			pInOutFootballer = NULL;

		} else {

			pInOutFootballer = mFootballerArray.el[iter];
		}
	}

	switch (filter) {

		case F_AI: {

			while (pInOutFootballer && pInOutFootballer->isInAIMode() == false) {

				++iter;

				if (iter >= mFootballerArray.count) {

					pInOutFootballer = NULL;
					break;
				}

				pInOutFootballer = mFootballerArray.el[iter];
			}

		} break;
			
		case F_Player: {

			while (pInOutFootballer && pInOutFootballer->isInAIMode() == true) {

				++iter;

				if (iter >= mFootballerArray.count) {

					pInOutFootballer = NULL;
					break;
				}

				pInOutFootballer = mFootballerArray.el[iter];
			}

		} break;

		default: {
		} break;
	}

	return (pInOutFootballer != NULL);
}


void Team::init_prepareScene() {

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	while (iterateFootballers(footballer.ptrRef(), iter)) {

		footballer->init_prepareScene();
	}
}


void Team::frameMove(const Clock& clock) {

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	while (iterateFootballers(footballer.ptrRef(), iter)) {

		footballer->frameMove(clock.getTime(), clock.getFrameMoveTime());
	}

	updateTactical(clock);

	mSwitchedFormationFlag = false;
}

void Team::render(Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass) {

	SoftPtr<Footballer> footballer;
	FootballerIndex iter;

	while (iterateFootballers(footballer.ptrRef(), iter)) {

		footballer->render(renderer, cleanIfNeeded, flagExtraRenders, pass);
	}
}

void Team::addPlayer(TeamEnum team, bool attachController, unsigned int switchPriority, float inputReactionTime, const TCHAR* pSelect, unsigned int instanceIndex, String* pID) {

	SoftRef<Input::Controller> controller;

	if (attachController) {

		controller = mMatch->getInputManager().findController(pSelect, instanceIndex, false, true);

		if (!controller.isValid()) {

			controller = mMatch->getInputManager().selectController(pSelect, instanceIndex, false, true);
		}

		if (!controller.isValid()) {

			mMatch->getLog().log(LOG_ERROR, true, true, L"Controller '%s'(%u) not found, player '%s' removed", pSelect ? pSelect : L"", instanceIndex, pID ? pID->c_tstr() : L"");
			assrt(false);
			return;
		}


		for (int ti = 0; ti < 2; ++ti) {

			Team& team = mMatch->getTeam((TeamEnum) ti);

			for (Players::Index i = 0; i < team.mPlayers.count; ++i) {

				if (team.mPlayers[i]->getAttachedController() == controller.ptr()) {

					mMatch->getLog().log(LOG_ERROR, true, true, L"Controller '%s' already attached, player '%s' removed", pSelect ? pSelect : L"", pID ? pID->c_tstr() : L"");
					assrt(false);
					return;
				}
			}
		}
	}


	SoftPtr<Player> player;

	MMemNew(player.ptrRef(), Player(mMatch, switchPriority, inputReactionTime, pID));

	player->attachTeam(team, mPlayers.count);

	mPlayers.addOne(player.ptr());

	if (attachController) {

		player->attachController(controller);

		mMatch->getLog().log(LOG_INFO, true, true, L"Controller '%s' attached to player '%s'", controller->getName(), pID ? pID->c_tstr() : L"");
	}
}

void Team::addPlayer(Input::Controller* pController, Time controllerAttachTime) {

	if (pController != NULL) {

		for (int ti = 0; ti < 2; ++ti) {

			Team& team = mMatch->getTeam((TeamEnum) ti);

			for (Players::Index i = 0; i < team.mPlayers.count; ++i) {

				if (team.mPlayers[i]->getAttachedController() == pController) {

					mMatch->getLog().log(LOG_ERROR, true, true, L"Controller already attached");

					assrt(false);
					return;
				}
			}
		}
	}


	SoftPtr<Player> player;

	MMemNew(player.ptrRef(), Player(mMatch, 0, 0.0f, NULL));

	player->attachTeam(mTeam, mPlayers.count);

	mPlayers.addOne(player.ptr());

	if (pController != NULL) {

		player->attachController(dref(pController));

		mMatch->getLog().log(LOG_INFO, true, true, L"Controller '%s' attached to player", pController->getName());
	}
}

void Team::removePlayerWithController(Input::Controller* pController) {

	if (pController != NULL) {

		for (Players::Index i = 0; i < mPlayers.count; ++i) {

			if (mPlayers[i]->getAttachedController() == pController) {

				mPlayers[i]->detachController();
				mPlayers.remove(i);
				break;
			}
		}
	}
}

void Team::frameMovePlayers(const DiscreeteTime& time) {

	for (Players::Index i = 0; i < mPlayers.count; ++i) {

		mPlayers[i]->frameMove(mMatch, time);
	}
}

void Team::renderPlayers(Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass) {

	for (Players::Index i = 0; i < mPlayers.count; ++i) {

		mPlayers[i]->render(mMatch, renderer, flagExtraRenders, pass);
	}
}

void Team::prepare(Match& match) {

	if (mSkin.isValid()) {

		Players::Index i;

		for (i = 0; i < mPlayers.count && i < mSkin->playerSkins.count; ++i) {

			mPlayers[i]->setSkin(match, dref(this), mSkin->playerSkins[i]);
		}

		if (mSkin->playerSkins.count) {

			SoftPtr<PlayerSkin> skin = (mSkin->playerSkins[mSkin->playerSkins.count - 1]);

			for (; i < mPlayers.count; ++i) {

				mPlayers[i]->setSkin(match, dref(this), skin);
			}
		}
	}
}

RenderSprite::Skin* Team::getScannerSkin() {

	if (mSkin.isValid()) {

		return &mSkin->scannerSkin;
	} 

	return NULL;
}

void Team::formationToWorldPos(const Vector2& inPos, Vector3& pos, float height) {

	const PitchInfo& pitchInfo = mMatch->getPitchInfoRef();

	assrt(pitchInfo.isValidWidthLength());

	pos.el[Scene_Right] = inPos.el[Scene2D_Right] * pitchInfo.halfWidth;
	pos.el[Scene_Forward] = inPos.el[Scene2D_Forward] * pitchInfo.halfLength;
	pos.el[Scene_Up] = height;
}

void Team::formationToWorldDir(const Vector2& inDir, Vector3& dir) {

	mMatch->toScene3D(inDir, dir, 0.0f);

	if (!mIsTeamWithFwdDir) {

		dir.negate();
	} 
}


void Team::teamLocalToWorld(const CtVector2& v, Vector2& vWorld) {

	if (isTeamWithFwdDir())
		vWorld = v;
	else
		v.negate(vWorld);
}

void Team::teamLocalToWorld(const CtVector3& v, Vector3& vWorld) {

	if (isTeamWithFwdDir()) {

		vWorld = v;

	} else {
		
		vWorld.el[Scene_Right] = -v.el[Scene_Right];
		vWorld.el[Scene_Forward] = -v.el[Scene_Forward];
		vWorld.el[Scene_Up] = v.el[Scene_Up];
	}
}


void Team::teamLocalToWorld(Vector3& v) {

	if (!isTeamWithFwdDir()) {
		
		v.el[Scene_Right] = -v.el[Scene_Right];
		v.el[Scene_Forward] = -v.el[Scene_Forward];
	}
}

} }