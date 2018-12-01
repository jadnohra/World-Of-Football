#include "Player.h"

#include "inc/Scene.h"
#include "entity/camera/SceneCamera.h"
#include "entity/footballer/Footballer.h"
#include "Match.h"
#include "Team.h"

namespace WOF { namespace match {

Player::Player(Match& match, unsigned int switchPriority, float inputReactionTime, String* pID) {

	mIndexInTeam = 0;
	mMatch = &match;
	mTeam = Team_Invalid;

	mSwitchPriority = switchPriority;
	mInputReactionTime = inputReactionTime;
	
	if (pID)
		mID.assign(*pID);
}

unsigned int Player::getSwitchPriority() {

	return mSwitchPriority;
}

TeamEnum Player::getAttachedTeamEnum() {

	return mTeam;
}

Team& Player::getAttachedTeam() {

	assrt(mTeam != Team_Invalid);

	return mMatch->getTeam(mTeam);
}

void Player::attachTeam(TeamEnum team, unsigned int index) {

	assrt(mTeam == Team_Invalid);

	mTeam = team;
	mIndexInTeam = index;
}

void Player::detachTeam() {

	mTeam = Team_Invalid;
}

Input::Controller* Player::getAttachedController() {

	return mController.ptr();
}

bool Player::isSwitchLocked() {

	return mFootballer.isValid() && mFootballer->isSwitchLocked();
}

bool Player::isAskingForManualSwitch() {

	return mFootballer.isValid() && mFootballer->isAskingForManualSwitch();
}
	
void Player::attachController(Input::Controller& controller) {

	assrt(mController.isNull());

	mController = &controller;
}


void Player::detachController() {

	mController.destroy();
}

Footballer* Player::getAttachedFootballer() {

	return mFootballer;
}

void Player::attachFootballer(Footballer& footballer) {

	if (mFootballer.isValid()) {

		if (mFootballer.ptr() == &footballer)
			return;

		detachFootballer();
	}

	mFootballer = &footballer;
	mFootballer->attachPlayer(mController, this, mMatch->getClock().getTime());

	getAttachedTeam().onPlayerAttachedFootballer(&footballer);
}

void Player::detachFootballer() {

	if (mFootballer.isValid()) {

		getAttachedTeam().onPlayerDetachedFootballer(mFootballer);
		mFootballer->detachPlayer();
		mFootballer.destroy();
	}
}

void Player::frameMove(Match& match, const DiscreeteTime& time) {

	SoftPtr<Footballer> footballer = getAttachedFootballer();

	if (!footballer.isValid()) {

		Team& team = getAttachedTeam();

		if (team.getFootballerCount(FootballerIterator::F_AI)) {

			FootballerIndex iter;

			while((getAttachedFootballer() == NULL) && team.iterateFootballers(footballer.ptrRef(), iter, FootballerIterator::F_AI)) {

				if (footballer->canActivatePlayerMode())
					attachFootballer(footballer);
			}

		} else {

			return; 
		}
	}
}

void Player::setSkin(Match& match, Team& team, PlayerSkin* pSkin) {

	mSkin.setIfEmpty(pSkin, true, true, true);
	mSkin.scanner.setIfEmpty(team.getScannerSkin());
	mSkin.COC.setIfEmpty(match.getCOCSkin(mIndexInTeam, false));
	mSkin.COC.setIfEmpty(&match.mCOCSetup.defaultSkin);
	mSkin.activeCOC.setIfEmpty(match.getCOCSkin(mIndexInTeam, true));
	mSkin.activeCOC.setIfEmpty(&match.mCOCSetup.defaultSkin);
}

void Player::render(Match& match, Renderer& renderer, bool flagExtraRenders, RenderPassEnum pass) {

	{
		SoftPtr<Footballer> footballer = getAttachedFootballer();

		if (footballer.isValid()) {

			{
				Vector3 COCPos;

				footballer->mTransformWorld.transformPoint(footballer->getRelativeCOCOffset(), COCPos);

				{
					RenderSprite& coc = match.getCOCSprite();
					RenderSprite::SoftSkin& skin = footballer->isBallOwner() ? mSkin.activeCOC : mSkin.COC;

					coc.transform3D().setPosition(COCPos);
					coc.render3D(renderer, skin);
				}
			}

			{
				const Point& targetPos = footballer->mTransformWorld.getPosition();
				float targetX;
				float targetY;

				if (!match.getActiveCamera().projectOnScreen(targetPos, targetX, targetY)
						/*
						&& (fabs(fabs(targetX) - 1.0f) > 0.01f)
						&& (fabs(fabs(targetY) - 1.0f) > 0.01f)*/) {
				
				
					/*
						The best way is to really project the camera into the 4 points 
						where it hits the pitch and use those to determin the positions
						of the arrow
					*/

					float projectorWidth = match.getCoordSys().convUnit(5.0f);

					if (match.getActiveCamera().estimatePitchCameraWidth(projectorWidth)) {

						match.getActiveCamera().projectOrthoOnScreen(targetPos, targetX, targetY, projectorWidth);

						RenderSprite& sprite = match.getArrowSprite();

						targetX = tcap(targetX, -1.0f, 1.0f);
						targetY = tcap(targetY, -1.0f, 1.0f);

						if ((fabs(targetX) < 1.0f) && (fabs(targetY) < 1.0f)) {

							float& clip = (fabs(targetX) > fabs(targetY)) ? targetX : targetY;
							clip = (clip < 0.0f) ? -1.0f : 1.0f;
						}

						sprite.setOrientation2D(fabs(targetX) == 1.0f ? targetX : 0.0f, fabs(targetY) == 1.0f ? targetY : 0.0f, true);

						targetX = targetX * 0.5f + 0.5f;
						targetY = targetY * 0.5f + 0.5f;

						float viewHeight;
						float viewWidth = renderer.getViewportWidth(false, &viewHeight);

						sprite.setPosition2D(targetX * viewWidth, targetY * viewHeight);

						Vector3 orient2D;

						renderer.setViewTransform2D(NULL, true);
						sprite.render2D(renderer, mSkin.arrow);

					} else {

						//camera not pointing to pitch
					}
				}
			}
		}
	}
}

} }