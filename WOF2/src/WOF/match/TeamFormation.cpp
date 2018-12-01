#include "TeamFormation.h"

#include "WE3/helper/load/WEMathDataLoadHelper.h"
#include "Team.h"

namespace WOF { namespace match {

TeamFormation::TeamFormation() {
}

TeamFormation::~TeamFormation() {
}

void TeamFormation::destroy() {

	mModeMap.destroy();
}

TeamFormation::Mode* TeamFormation::findMode(const TCHAR* name) {

	SoftPtr<Mode> ret;

	mModeMap.find(String::hash(name), ret.ptrRef());

	return ret;
}

unsigned int TeamFormation::getModeCount() {

	return mModeMap.getCount();
}

TeamFormation::Mode* TeamFormation::getMode(unsigned int index) {

	ModeMap::Iterator iter = mModeMap.iterator();

	SoftPtr<Mode> ret;

	while (mModeMap.next(iter, ret.ptrRef())) {

		if (index == 0)
			return ret;

		--index;
	}

	return NULL;
}

PitchRegionShape* TeamFormation::Mode::findRegion(StringHash name) {

	SoftPtr<PitchRegionShape> ret;

	mRegionMap.find(name, ret.ptrRef());

	return ret;
}

TeamFormation::Element* TeamFormation::Mode::findElement(const TCHAR* code, FootballerNumber* pFallbackNumber) {

	return findElement(String::hash(code), pFallbackNumber);
}

TeamFormation::Element* TeamFormation::Mode::findElement(StringHash code, FootballerNumber* pFallbackNumber) {

	SoftPtr<Element> ret;

	mElementMap.find(code, ret.ptrRef());

	if (pFallbackNumber && !ret.isValid()) {

		mElementMapByNumber.find(*pFallbackNumber, ret.ptrRef());
	}

	return ret;
}

TeamFormation::Element::Element() : mNumber(FootballerNumberInvalid) {
}

bool TeamFormation::Element::load(BufferString& tempStr, TeamFormation::Mode& parent, DataChunk& chunk, Team* pTeam) {

	if (!chunk.getAttribute(L"code", mCode))
		return false;

	chunk.scanAttribute(tempStr, L"number", L"%d", &mNumber);
	
	Vector3 temp;

	if (!MathDataLoadHelper::extract(tempStr, chunk, temp, true, true, true, L"pos", NULL))
		return false;

	mPos.el[Scene2D_Right] = temp.el[0];
	mPos.el[Scene2D_Forward] = temp.el[1];

	if (pTeam)
		pTeam->teamLocalToWorld(mPos);

	mChunk = &chunk;

	return true;
}

bool TeamFormation::Mode::load(BufferString& tempStr, TeamFormation& parent, DataChunk& chunk, Team* pTeam) {

	if (!chunk.getAttribute(L"name", mName))
		return false;

	SoftRef<DataChunk> childEl = chunk.getFirstChild();

	while (childEl.isValid()) {

		if (childEl->equals(L"position")) {

			if (childEl->getAttribute(L"code", tempStr)) {

				if (mElementMap.hasKey(tempStr.hash())) {

					assrt(false);
					log(L"duplicate TeamFormationMode Position %s-%s", mName.c_tstr(), tempStr.c_tstr());
					return false;
					
				} else {

					SoftPtr<Element> element;

					WE_MMemNew(element.ptrRef(), Element());

					if (element->load(tempStr, *this, childEl, pTeam)) {
						
						mElementMap.insert(element->getCode().hash(), element);

						if (element->hasNumber()) {

							if (!mElementMapByNumber.insert(element->getNumber(), element)) {

								assrt(false);
								log(L"duplicate TeamFormationMode Position Number %s", element->getCode());
								return false;
							}
						}
						
					} else {

						HardPtr<Element> destroyElement = element.ptr();
						return false;
					}
				}
			}

		} else if (childEl->equals(L"region")) {

			if (childEl->getAttribute(L"name", tempStr)) {

				if (mRegionMap.hasKey(tempStr.hash())) {

					assrt(false);
					log(L"duplicate TeamFormationMode Region %s-%s", mName.c_tstr(), tempStr.c_tstr());
					return false;
					
				} else {

					SoftPtr<PitchRegionShape> region = PitchRegionFactory::createRegion(tempStr, childEl, pTeam);

					if (region.isValid()) {

						mRegionMap.insert(region->getName().hash(), region);
					}
				}
			}
		}

		toNextSibling(childEl);
	}

	return true;
}

bool TeamFormation::load(BufferString& tempStr, Match& match, DataChunk* pChunk, Team* pTeam) {

	destroy();

	if (pChunk) {

		DataSourceTranslationContext transCtx;
		transCtx.start(pChunk, true);

		if (!pChunk->getAttribute(L"name", mName))
			return false;

		SoftRef<DataChunk> childMode = pChunk->getFirstChild();

		while (childMode.isValid()) {

			if (childMode->equals(L"mode")) {

				if (childMode->getAttribute(L"name", tempStr)) {

					if (mModeMap.hasKey(tempStr.hash())) {

						assrt(false);
						log(L"duplicate TeamFormation Mode %s-%s", mName.c_tstr(), tempStr.c_tstr());
						return false;

					} else {

						SoftPtr<Mode> mode;

						WE_MMemNew(mode.ptrRef(), Mode());

						if (mode->load(tempStr, *this, childMode, pTeam)) {
						
							mModeMap.insert(String::hash(mode->getName()), mode);
							
						} else {

							HardPtr<Mode> destroyMode = mode.ptr();
							return false;
						}
					}
				}
			}

			toNextSibling(childMode);
		}


		return true;
	}

	return false;
}


TeamFormations::TeamFormations() {
}

TeamFormations::~TeamFormations() {
}

void TeamFormations::destroy() {

	mFormationMap.destroy();
}

TeamFormation* TeamFormations::findFormation(const TCHAR* name) {

	SoftPtr<TeamFormation> ret;

	mFormationMap.find(String::hash(name), ret.ptrRef());

	return ret;
}

unsigned int TeamFormations::getFormationCount() {

	return mFormationMap.getCount();
}

TeamFormation* TeamFormations::getFormation(unsigned int index) {

	FormationMap::Iterator iter = mFormationMap.iterator();

	SoftPtr<TeamFormation> ret;

	while (mFormationMap.next(iter, ret.ptrRef())) {

		if (index == 0)
			return ret;

		--index;
	}

	return NULL;
}

bool TeamFormations::insertFormation(TeamFormation* formation) {

	SoftPtr<TeamFormation> existing = findFormation(formation->getName());

	if (existing.isValid()) {

		assrt(false);
		return false;
	} 
	
	mFormationMap.insert(String::hash(formation->getName()), formation);
	return true;
}

bool TeamFormations::load(BufferString& tempStr, Match& match, DataChunk* pChunk, Team* pTeam) {

	destroy();

	if (pChunk) {

		SoftRef<DataChunk> childMode = pChunk->getFirstChild();

		while (childMode.isValid()) {

			if (childMode->equals(L"formation")) {

				SoftPtr<TeamFormation> formation;
				
				WE_MMemNew(formation.ptrRef(), TeamFormation());

				if (formation->load(tempStr, match, childMode, pTeam)) {

					SoftPtr<TeamFormation> existing = findFormation(formation->getName());
				
					if (existing.isValid()) {

						assrt(false);
						log(L"duplicate TeamFormation %s", formation->getName());

						HardPtr<TeamFormation> destroyFormation = formation.ptr();
						return false;

					} else {

						mFormationMap.insert(String::hash(formation->getName()), formation);
					}
					
					
				} else {

					HardPtr<TeamFormation> destroyFormation = formation.ptr();
					return false;
				}
			}

			toNextSibling(childMode);
		}


		return true;
	}

	return false;
}

} }