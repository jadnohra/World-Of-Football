#ifndef _WEWorldVolumeCollider_h
#define _WEWorldVolumeCollider_h

#include "../../spatialManager/WESpatialObjects.h"
#include "WEWorldMaterialManager.h"

namespace WE {

	class VolumeColliderBase {
	public:

		void init(const WorldMaterialID& materialID) {

			mCollMaterialID = materialID;
		}

		inline const WorldMaterialID& getMaterial() { return mCollMaterialID; }

	public:

		WorldMaterialID mCollMaterialID;
	};

	class WorldVolumeCollider : public SpatialVolumeBinding, public VolumeColliderBase {
	public:
	};

	class WorldDynamicVolumeCollider : public SpatialVolumeBinding, public VolumeColliderBase {
	public:
	};
}

#endif