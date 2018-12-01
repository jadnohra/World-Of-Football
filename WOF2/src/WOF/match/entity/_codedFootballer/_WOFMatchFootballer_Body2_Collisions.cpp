#include "WOFMatchFootballer_Body2.h"

namespace WOF { namespace match {


CollRegistry::Index Footballer_Body::getCollRecordCount(CollRegistry::Index* pFootballerRecordCount) {

	if (pFootballerRecordCount) {

		(*pFootballerRecordCount) = 0;
		CollRegistry::Index dynRecordCount = mCollRegistry.getDynVolRecordCount();

		for (CollRegistry::Index i = 0; i < dynRecordCount; ++i) {

			SoftPtr<CollRecordVol> record =  mCollRegistry.getDynVolRecord(i);

			if (record->getObjectType() == MNT_Footballer) {

				++(*pFootballerRecordCount);
			}
		}
	}

	return mCollRegistry.getTotalRecordCount();
}

void Footballer_Body::processCollisions(CollisionResolveController& controller, bool& wasChanged) {

	wasChanged = false;
	
	clean();

	bool allowSaveCurrTransformLocal;
	bool trySaveTransformLocal = false;
	CollisionResolutionMode mode = controller.chooseCollResolveMode(*this, allowSaveCurrTransformLocal);

	mCollIsInGhostMode = false;

	switch (mode) {

		case CollResolve_NoColls: {

			trySaveTransformLocal = true;

		} break;

		case CollResolve_GhostMode: {

			trySaveTransformLocal = true;
			mCollIsInGhostMode = true;

		} break;

		case CollResolve_TrackBack: {

			if (loadLocalTransform()) {

				wasChanged = true;
				nodeMarkDirty();

			} else {

				mCollIsInGhostMode = true;
			}

		} break;

		case CollResolve_Resolve: {

			assrt(false);

		} break;
	}

	if (trySaveTransformLocal) {

		if (allowSaveCurrTransformLocal)
			saveLocalTransform();
		else 
			invalidateLocalTransform();
	}
}

} }