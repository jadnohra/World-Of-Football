#include "Match.h"
#include "Team.h"

#include "WE3/helper/load/WERenderDataLoadHelper.h"
#include "scene/SceneLoader_v1.h"

namespace WOF { namespace match {

bool Team::loadMatchSetup(TeamEnum thisTeam, Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	int teamID = -1;
	String teamKey;
	String teamKeyValue;
	
	if (chunk.scanAttribute(tempStr, L"id", L"%d", &teamID)
		&& teamID >= 0 && teamID <= 1
		&& teamID == thisTeam
		&& chunk.getAttribute(L"path", mLoadPathKey)) {

		chunk.getAttribute(L"skin", mSelectedSkinName);
			
		mTeam = (TeamEnum) teamID;
		this->setIsDeclared(true);

		chunk.scanAttribute(tempStr, L"activeFootballerCount", L"%u", &mActiveFootballerCount);
		mActiveFootballerCountLeft = mActiveFootballerCount;

		chunk.scanAttribute(tempStr, L"activeFootballerStartIndex", L"%u", &mActiveFootballerStartIndex);
		mActiveFootballerStartIndexLeft = mActiveFootballerStartIndex;
		
	} else {

		assrt(false);
		return false;
	}

	{
		SoftRef<DataChunk> formationsChunk = chunk.getChild(L"formations");

		if (formationsChunk.isValid())
			loadFormations(match, tempStr, chunk, pConv);
	}

	return true;
}

bool Team::loadScene(Match& match, BufferString& tempStr, CoordSysConv* pConv, SceneNodeFactory& nodeFactory, int& errCount) {

	if (!isDeclared())
		return true;

	String teamChunkPath(match.mTeamsPath);

	PathResolver::appendPath(teamChunkPath, mLoadPathKey.c_tstr(), true, false);

	SoftRef<DataChunk> teamChunk = match.getLoadContext().dataSourcePool().getChunk(teamChunkPath.c_tstr(), true, NULL, true);

	if (teamChunk.isValid()) {

		if (loadTeam(match, tempStr, teamChunk, pConv)) {

			SoftRef<DataChunk> parentChunk = teamChunk->getParent();
			SoftRef<DataChunk> entitiesChunk = parentChunk->getChild(L"entities");
			
			if (entitiesChunk.isValid()) {

				{
					String teamKeyValue;
					teamKeyValue.assignEx(L"%d", (int) mTeam);
					dref(match.getLoadContext().varTablePtr()).setValue(L"team", teamKeyValue.c_tstr());
				}

				{
					dref(match.getLoadContext().varTablePtr()).setValue(L"kitPath", mSkin->kitPath.c_tstr());
				}

				return SceneLoader_v1::loadEntities(tempStr, match, entitiesChunk, nodeFactory, pConv, errCount);
			}

			return true;
		}
	}

	assrt(false);
	log(L"Loading Team Failed [%s]", teamChunkPath.c_tstr());

	return false;
}

bool Team::loadFormations(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	SoftRef<DataChunk> formationChunk = chunk.getFirstChild();

	while (formationChunk.isValid()) {

		if (formationChunk->equals(L"formation")) {

			if (formationChunk->getAttribute(L"name", tempStr)) {

				SoftPtr<TeamFormation> formation = match.loadFormation(tempStr.c_tstr(), this);

				if (formation.isValid()) {

					if (!this->addFormation(formation)) {

						assrt(false);
						HardPtr<TeamFormation> destr = formation.ptr();
					}
				}
			}
		}

		toNextSibling(formationChunk);
	}

	return true;
}

bool Team::loadTeam(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {
	
	{
		SoftRef<DataChunk> skins = chunk.getChild(L"skins");
		SoftPtr<DataChunk> foundSkin;
		SoftRef<DataChunk> skin;
		SoftRef<DataChunk> firstSkin;

		if (skins.isValid()) {

			firstSkin = skins->getFirstChild();
			skin = skins->getFirstChild();

			while (skin.isValid()) {

				if (mSelectedSkinName.isEmpty()) {

					foundSkin = skin;
					break;
				}

				if (skin->getAttribute(L"name", tempStr)
					&& tempStr.equals(mSelectedSkinName)) {

					foundSkin = skin;
					break;
				}

				toNextSibling(skin);
			}
		}

		if (!foundSkin.isValid()) {
		
			foundSkin = firstSkin;
		}

		if (foundSkin.isValid()) {

			WE_MMemNew(mSkin.ptrRef(), Skin());

			mSkin->load(match , dref(this), tempStr, foundSkin);

		} else {

			WE_MMemNew(mSkin.ptrRef(), Skin());
			mSkin->randomize(match);
		}
	}

	if (mFormations.getFormationCount() == 0) {

		SoftRef<DataChunk> formationsChunk = chunk.getChild(L"formations");

		if (formationsChunk.isValid())
			loadFormations(match, tempStr, formationsChunk, pConv);
	}
		
	return true;
}

Team::Skin::Skin() : kitPath(L"default"){
}

void Team::Skin::randomize(Match& match) {

	scannerSkin.enableColor(true);

	scannerSkin.color().el[0] = trand(1.0f);
	scannerSkin.color().el[1] = trand(1.0f);
	scannerSkin.color().el[2] = trand(1.0f);
	scannerSkin.color().alpha = match.getScanner().getDefaultFootballerAlpha();
}

void Team::Skin::load(Match& match, Team& team, BufferString& tempStr, DataChunk& chunk) {

	chunk.getAttribute(L"name", name);
	chunk.getAttribute(L"kitPath", kitPath);

	float defaultScannerAlpha = match.getScanner().getDefaultFootballerAlpha();
	float defaultArrowAlpha = 0.99f;
	
	if (match.getArrowSprite().getSkin() && match.getArrowSprite().skin().getColor())
		defaultArrowAlpha = match.getArrowSprite().color().alpha;

	scannerSkin.enableColor(true);
	RenderDataLoadHelper::extract(tempStr, chunk, scannerSkin.color(), true, L"scannerColor", defaultScannerAlpha, false);

	BufferString texturePath;

	{
		playerSkins.reserve(chunk.countChildren(tempStr));

		SoftRef<DataChunk> playerSkinsChunk = chunk.getFirstChild();

		while (playerSkinsChunk.isValid()) {

			PlayerSkin& skin = dref(playerSkins.addNewOne());

			SoftRef<DataChunk> playerSkinChunk = playerSkinsChunk->getFirstChild();

			while (playerSkinChunk.isValid()) {

				if (playerSkinChunk->equals(L"COC")) {

					WE_MMemNew(skin.COC.ptrRef(), RenderSprite::Skin());

					skin.COC->load(tempStr, playerSkinChunk, match.getLoadContext().render(), match.mCOCSetup.defaultAlpha, &match.mCOCSetup.texturesPath);

				} else if (playerSkinChunk->equals(L"activeCOC")) {

					WE_MMemNew(skin.activeCOC.ptrRef(), RenderSprite::Skin());

					skin.activeCOC->load(tempStr, playerSkinChunk, match.getLoadContext().render(), match.mCOCSetup.defaultAlpha, &match.mCOCSetup.texturesPath);

				} else if (playerSkinChunk->equals(L"scanner")) {

					WE_MMemNew(skin.scanner.ptrRef(), RenderSprite::Skin());

					skin.scanner->load(tempStr, playerSkinChunk, match.getLoadContext().render(), defaultScannerAlpha, &match.mCOCSetup.texturesPath);

				} else if (playerSkinChunk->equals(L"arrow")) {

					WE_MMemNew(skin.arrow.ptrRef(), RenderSprite::Skin());

					skin.arrow->load(tempStr, playerSkinChunk, match.getLoadContext().render(), defaultArrowAlpha, &match.mCOCSetup.texturesPath);
				}

				toNextSibling(playerSkinChunk);
			}

			toNextSibling(playerSkinsChunk);
		}
	}

}

} }