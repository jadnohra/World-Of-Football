#include "Colliders.h"

namespace WOF { namespace match {

void ColliderBase::addSweptBallContacts(SpatialVolumeBinding* pThisVol, VolumeColliderBase* pThisCollBase, Object* pThisObj,
						WorldMaterialManager& matManager, 
						const WorldMaterialID& sphereMat, const float& radius, 
						const Vector3& pos, const Vector3& sweepVector, 
						SweptIntersectionArray& intersections, bool executeMaterials) {

	SpatialVolumeBinding& collider = dref(pThisVol);

	switch (collider.volume.type) {
		case V_OBB:
			{

				OBB& vol = collider.volume.toOBB();
				SweptIntersectionTesterOBB tester(vol, radius, pos, sweepVector);
				bool didIntersect;
				
				SweptIntersection* pInter = &(intersections.makeSpaceForOne());

				while (tester.next(didIntersect, pInter->contactPoint, *pInter, *pInter)) {

					if (didIntersect) {

						pInter->recheckGeometry = (pInter->sweptFraction == 0.0f);
						pInter->pMaterial = matManager.getCombinedMaterial(sphereMat, pThisCollBase->getMaterial());
						pInter->contactHasVelocity = false;
						pInter->staticObj = pThisObj;

						intersections.addOneReserved();
						pInter = &(intersections.makeSpaceForOne());
					}
				}
			}
			break;
	}
}

} }