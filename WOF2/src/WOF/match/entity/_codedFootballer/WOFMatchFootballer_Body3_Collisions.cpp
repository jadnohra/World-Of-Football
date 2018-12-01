#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY3

namespace WOF { namespace match {

FootballerBodyDefs::CollisionResolutionMode Footballer_Body::getDefaultCollResolveMode(bool penetrateFootballers) {

	if (mCollRecordCount) {

		if (penetrateFootballers && (mFootballerCollRecordCount == mCollRecordCount))
			return CollResolve_GhostMode;

		return CollResolve_TrackBack;
	}

	return CollResolve_NoColls;
}

CollRegistry::Index Footballer_Body::getCollRecordCount(CollRegistry::Index* pFootballerRecordCount, BodyCollisionListener* pFootballerCollListener, void* pContext) {

	if (pFootballerRecordCount || pFootballerCollListener) {

		(*pFootballerRecordCount) = 0;
		CollRegistry::Index dynRecordCount = mCollRegistry.getDynVolRecordCount();

		for (CollRegistry::Index i = 0; i < dynRecordCount; ++i) {

			SoftPtr<CollRecordVol> record =  mCollRegistry.getDynVolRecord(i);

			if (record->getObjectType() == MNT_Footballer) {

				if (pFootballerRecordCount)
					++(*pFootballerRecordCount);

				if (pFootballerCollListener)
					pFootballerCollListener->onBodyCollision(*this, record, pContext);
			}
		}
	}

	return mCollRegistry.getTotalRecordCount();
}

void Footballer_Body::updateCollisions(BodyCollisionListener* pFootballerCollListener, void* pContext) {

	clean();

	mCollRecordCount = getCollRecordCount(&mFootballerCollRecordCount, pFootballerCollListener, pContext);
}

bool Footballer_Body::resolveCollisions_Resolve(const Vector3& posDiffHint, bool& wasChanged) {

	Vector3 correction;
	CollRegistry::Index recordCount = mCollRegistry.getTriRecordCount();

	//TODO we need this
	const Vector3& inVel = posDiffHint;
	Vector3 outVel;
	bool isTouching;
	bool allResolved = false;
	Vector3 velConstrainContext(0.0f);

	const int maxPasses = 3;
	int pass = 0;

	const float guideAngleSinSpread = sinf(degToRad(55.0f));
	
	while (recordCount && !allResolved && (pass < maxPasses)) {

		allResolved = true;

		for (CollRegistry::Index i = 0; i < recordCount; ++i) {

			if (mCollRegistry.constrainByTriRecord(i, mCollider.volume.toOBB(), velConstrainContext, inVel, outVel, correction, Scene_Up, false, 0.0f, isTouching, guideAngleSinSpread)) {

				if (!isTouching) {

					wasChanged = true;

					nodeMarkDirty();

					Vector3 temp = mTransformLocal.getPosition();
					mTransformLocal.movePosition(correction);

					/*
					static int ct = 0;

					if (correction.magSquared() > framePosDiff.magSquared()) {

						String str;
						str.assignEx(L"%d: %g, %g, %g", ct, correction.el[0], correction.el[1], correction.el[2]);

						mNodeMatch->getConsole().show(true, true);
						mNodeMatch->getConsole().print(str.c_tstr());

						++ct;
					}
					*/

					//needed becuase some corrections might leave position
					//the same because of floating point issues 
					if (!temp.equals(mTransformLocal.getPosition())) {

						allResolved = false;
						break;
					}
				} 
			}
		}

		++pass;

		if (!allResolved) {
		
			clean();
			CollRegistry::Index newRecordCount = mCollRegistry.getTriRecordCount();

			if (newRecordCount < recordCount)
				--pass;

			recordCount = newRecordCount;
		}
	}

	return allResolved;
}

Footballer_Body::CollisionResolutionMode Footballer_Body::resolveCollisions(const CollisionResolutionMode& resolutionMode, const bool& allowSaveCurrTransformLocal, bool& wasChanged, const Vector3& posDiffHint) {

	wasChanged = false;
	
	clean();

	bool trySaveTransformLocal = false;
	mCollResolutionMode = resolutionMode;

	bool repeat = true;

	while (repeat) {

		repeat = false;

		switch (mCollResolutionMode) {

			case CollResolve_NoColls: {

				trySaveTransformLocal = true;

			} break;

			case CollResolve_GhostMode: {

				trySaveTransformLocal = true;

			} break;

			case CollResolve_TrackBack: {

				if (loadLocalTransform()) {

					wasChanged = true;
					nodeMarkDirty();

				} else {

					mCollResolutionMode = CollResolve_GhostMode;
				}

			} break;

			case CollResolve_Resolve: {

				if (!resolveCollisions_Resolve(posDiffHint, wasChanged)) {

					mCollResolutionMode = CollResolve_TrackBack;
					repeat = true;
				}

			} break;
		}
	}

	if (trySaveTransformLocal) {

		if (allowSaveCurrTransformLocal)
			saveLocalTransform();
		else 
			invalidateLocalTransform();
	}

	return mCollResolutionMode;
}

} }

#endif