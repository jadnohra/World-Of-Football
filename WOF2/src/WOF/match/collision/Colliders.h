#ifndef h_WOF_match_Colliders_h
#define h_WOF_match_Colliders_h

#include "WE3/coll/WECollDataTypes.h"
#include "WE3/defaultImpl/world/WEWorldMaterialManager.h"
#include "WE3/defaultImpl/world/WEWorldPolygonalCollider.h"
#include "WE3/defaultImpl/world/WEWorldVolumeCollider.h"
using namespace WE;

#include "SweptDataTypes.h"

namespace WOF { namespace match {

	class ColliderBase {
	public:

		void addSweptBallContacts(
						SpatialVolumeBinding* pThisVol, VolumeColliderBase* pThisCollBase, Object* pThisObj,
						WorldMaterialManager& matManager, 
						const WorldMaterialID& sphereMat, const float& radius, 
						const Vector3& pos, const Vector3& sweepVector, 
						SweptIntersectionArray& intersections, bool executeMaterials);

	};

	class VolumeCollider : public ColliderBase, public WorldVolumeCollider {
	public:

		void init(const WorldMaterialID& materialID) {
			
			WorldVolumeCollider::init(materialID);
		}

		inline void addSweptBallContacts(
						Object* pThisObj,
						WorldMaterialManager& matManager, 
						const WorldMaterialID& sphereMat, const float& radius, 
						const Vector3& pos, const Vector3& sweepVector, 
						SweptIntersectionArray& intersections, bool executeMaterials) {

			ColliderBase::addSweptBallContacts(this, this, pThisObj,
						matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);
		}

	};

	class DynamicVolumeCollider : public ColliderBase, public WorldDynamicVolumeCollider {
	public:

		void init(const WorldMaterialID& materialID) {
			
			WorldDynamicVolumeCollider::init(materialID);
		}

		inline void addSweptBallContacts(
						Object* pThisObj,
						WorldMaterialManager& matManager, 
						const WorldMaterialID& sphereMat, const float& radius, 
						const Vector3& pos, const Vector3& sweepVector, 
						SweptIntersectionArray& intersections, bool executeMaterials) {

			ColliderBase::addSweptBallContacts(this, this, pThisObj,
						matManager, sphereMat, radius, pos, sweepVector, intersections, executeMaterials);
		}
	};

	class PolygonalCollider : public ColliderBase, public WorldPolygonalCollider {
	};


	//typedef PolygonalCollider MeshCollider;
	
} }

#endif