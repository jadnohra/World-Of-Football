#include "WOFMatchFootballer_BodyDefs.h"

#include "WOFMatchFootballer_Body2.h"

namespace WOF { namespace match {


FootballerBodyDefs::CollisionResolveController FootballerBodyDefs::kStockCollisionResolveController;

FootballerBodyDefs::CollisionResolutionMode FootballerBodyDefs::CollisionResolveController::chooseCollResolveMode(Body& body, bool& allowSaveCurrTransformLocal) {

	bool penetrateFootballers = false;
	allowSaveCurrTransformLocal = true;

	CollRegistry::Index footballerRecordCount;
	CollRegistry::Index recordCount = body.getCollRecordCount(penetrateFootballers ? &footballerRecordCount : NULL);

	if (recordCount) {

		if (penetrateFootballers && (footballerRecordCount == recordCount))
			return CollResolve_GhostMode;

		return CollResolve_TrackBack;
	}

	return CollResolve_NoColls;
}



} }