#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY3

#include "WOFMatchFootballer.h"

namespace WOF { namespace match {

Footballer_BodyAction_Falling& Footballer_Body::getActionFallingAndSetupFromTackle(const Time& time, Footballer_Body* pTackler) {

	SoftPtr<Footballer_BodyAction_Falling> actionFalling = &getActionFalling();
	
	if (!actionFalling->setFallTypeFromTackle(*this, time, pTackler)) {

		assrt(false);
	}
	
	return actionFalling;
}

void Footballer_Body::sendTackleEvents(const Time& time) {

	CollRegistry::Index footballerCount = 0;

	for (CollRegistry::Index i = 0; i < mCollRecordCount; ++i) {

		if (footballerCount >= mFootballerCollRecordCount)
			break;

		SoftPtr<CollRecordVol> record =  mCollRegistry.getDynVolRecord(i);

		if (record->getObjectType() == MNT_Footballer) {

			++footballerCount;

			SoftPtr<Footballer> footballer = Footballer::footballerFromObject(record->getOwner());

			footballer->addBodyCommEvent().set(BodyComm_Tackled, this);
		}
	}
}


} }

#endif