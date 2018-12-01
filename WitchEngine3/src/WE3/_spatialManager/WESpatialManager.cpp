#include "WESpatialManager.h"

namespace WE {

SpatialManagerBuilder::SubdivisionSettings::SubdivisionSettings(const Vector3* pMinCellSize, 
								const UINT32* pMaxDepth, const Index* pMaxCellElementCount,
								const Index* pSplitPreferenceFactor, const float* pFreeSpaceLoad,
								const float* pLargeVolumeCoeff) {

	if (pMinCellSize) {

		minCellSize = *pMinCellSize;

	} else {

		minCellSize.zero();
	}

	if (pMaxDepth) {

		maxDepth = *pMaxDepth;

	} else {

		maxDepth = UINT32_MAX;
	}

	if (pMaxCellElementCount) {

		maxCellElementCount = *pMaxCellElementCount;

	} else {

		maxCellElementCount = (Index) -1;
	}

	if (pSplitPreferenceFactor) {

		splitPreferenceFactor = *pSplitPreferenceFactor;

	} else {

		splitPreferenceFactor = 1;
	}

	if (pFreeSpaceLoad) {

		freeSpaceLoad = *pFreeSpaceLoad;

	} else {

		freeSpaceLoad = 0.0f;
	}

	if (pLargeVolumeCoeff) {

		largeVolumeCoeff = *pLargeVolumeCoeff;

	} else {

		largeVolumeCoeff = 0.2f;
	}
}

bool SpatialManagerBuildPhase::init(SpatialManager& manager, bool tryLoadFromCache) {

	isCacheValid = false;

	mManager = &manager;
	mBuilder = mManager->createBuilder();

	if (mBuilder.isValid()) {

		if (tryLoadFromCache) {

			isCacheValid = mBuilder->isCacheValid() && mBuilder->loadFromCache();

		} else {

			isCacheValid = false;
		}

	} else {

		if (!mManager->canBuildDynamically()) {

			return false;
		}
	}

	return true;
}

bool SpatialManagerBuildPhase::finalize(const SpatialManagerBuilder::SubdivisionSettings& settings) {

	if (mManager.isValid()) {

		if (mBuilder.isValid()) {

			if (!isCacheValid) {

				if (!mBuilder->process(settings)) {

					return false;
				}

				if (!mBuilder->saveToCache()) {

					assrt(false);
				}
			}

			if (!mBuilder->build(mManager)) {

				return false;
			}

			if (!addKept(true)) {

				return false;
			}
			
		} else {

			return true;
		}

		mManager.destroy();

		return true;
	}

	return false;
}

void SpatialManagerBuildPhase::keep(SpatialTriContainer& object) {

	mKeptTriObjects.addOne(&object);
}

void SpatialManagerBuildPhase::keep(SpatialVolume& object) {

	mKeptVolumes.addOne(&object);
}

void SpatialManagerBuildPhase::keep(SpatialDynamicVolume& object) {

	mKeptDynamicVolumes.addOne(&object);
}

bool SpatialManagerBuildPhase::addKept(bool clearKept) {

	for (Index i = 0; i < mKeptTriObjects.count; ++i) {

		if (!mManager->add(*mKeptTriObjects.el[i]))
			return false;
	}

	for (Index i = 0; i < mKeptVolumes.count; ++i) {

		if (!mManager->add(*mKeptVolumes.el[i]))
			return false;
	}

	for (Index i = 0; i < mKeptDynamicVolumes.count; ++i) {

		if (!mManager->add(*mKeptDynamicVolumes.el[i]))
			return false;
	}

	if (clearKept) {

		mKeptTriObjects.destroy();
		mKeptVolumes.destroy();
		mKeptDynamicVolumes.destroy();
	}

	return true;
}

bool SpatialManagerBuildPhase::add(SpatialTriContainer& object, bool shouldKeep) {

	bool ret;

	if (mBuilder.isValid()) {

		if (!isCacheValid) {

			ret = mBuilder->add(object);

		} else {

			ret = true;
		}

		if (shouldKeep) {

			keep(object);
		}

	} else {

		ret = mManager->add(object);
	}

	return ret;
}

bool SpatialManagerBuildPhase::add(SpatialVolume& object, bool shouldKeep) {

	bool ret;

	if (mBuilder.isValid()) {

		if (!isCacheValid) {

			ret = mBuilder->add(object);

		} else {

			ret = true;
		}

		if (shouldKeep) {

			keep(object);
		}

	} else {

		ret = mManager->add(object);
	}

	return ret;
}

bool SpatialManagerBuildPhase::add(SpatialDynamicVolume& object) {

	keep(object);

	return true;
}

}