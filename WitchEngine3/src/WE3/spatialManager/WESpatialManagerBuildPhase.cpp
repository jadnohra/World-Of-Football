#include "WESpatialManagerBuildPhase.h"

namespace WE {

SpatialManagerBuildPhase::SpatialManagerBuildPhase() 
	: mpConfig(NULL) {
}

SpatialManagerBuildPhase::~SpatialManagerBuildPhase() {

	if (mpConfig) {

		mBuilder->destroyConfiguration(mpConfig);
	}

	mManager.destroy();
	mBuilder.destroy();
}

bool SpatialManagerBuildPhase::init(SpatialManager& manager, bool tryLoadFromCache) {

	isCacheValid = false;

	mManager = &manager;
	mBuilder = mManager->createBuilder();

	BufferString tempStr;

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

bool SpatialManagerBuildPhase::finalize(void* pConfig) {

	if (mManager.isValid()) {

		if (mBuilder.isValid()) {

			if (!isCacheValid) {

				if (!mBuilder->process(pConfig ? pConfig : mpConfig, false)) {

					return false;
				}

				if (!mBuilder->saveToCache()) {

					//assrt(false);
				}
			}

			if (!mBuilder->build(mManager)) {

				return false;
			}
			
		} else {

			return true;
		}

		if (mpConfig) {

			mBuilder->destroyConfiguration(mpConfig);
		}

		mManager.destroy();
		mBuilder.destroy();

		return true;
	}

	return false;
}

bool SpatialManagerBuildPhase::add(SpatialTriContainer* pSpatial, ObjectBinding* pBinding, bool destroyWhenDone) {

	bool ret;

	if (mBuilder.isValid()) {

		if (!isCacheValid) {

			ret = mBuilder->add(pSpatial, pBinding, destroyWhenDone);

		} else {

			ret = true;
		}

	
	} else {

		ret = mManager->add(pSpatial, pBinding);
	}

	return ret;
}

bool SpatialManagerBuildPhase::add(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) {

	bool ret;

	if (mBuilder.isValid()) {

		if (!isCacheValid) {

			ret = mBuilder->add(spatial, pBinding);

		} else {

			ret = true;
		}

	} else {

		ret = mManager->add(spatial, pBinding);
	}

	return ret;
}

bool SpatialManagerBuildPhase::addDynamicVolume(VolumeStruct& spatial, SpatialVolumeBinding* pBinding) {

	bool ret;

	if (mBuilder.isValid()) {

		if (!isCacheValid) {

			ret = mBuilder->addDynamicVolume(spatial, pBinding);

		} else {

			ret = true;
		}

	} else {

		ret = mManager->addDynamicVolume(spatial, pBinding);
	}

	return ret;
}

void* SpatialManagerBuildPhase::loadConfiguration(BufferString& tempStr, DataChunk* pChunk, CoordSysConv* pConv, bool saveConfiguration) {

	void* pConfig = mBuilder->loadConfiguration(tempStr, pChunk, pConv);

	if (saveConfiguration) {

		destroyConfiguration(mpConfig);
		mpConfig = pConfig;
	}

	return pConfig;
}

void SpatialManagerBuildPhase::destroyConfiguration(void*& pConfig) {

	 mBuilder->destroyConfiguration(pConfig);
}

}