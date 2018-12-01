#ifndef h_WOF_match_SweptDataTypes
#define h_WOF_match_SweptDataTypes

#include "WE3/math/WETriangle.h"
#include "WE3/math/WESphere.h"
#include "WE3/math/dynamicIntersect/WEDynamicIntersect.h"
#include "WE3/WETL/WETLArray.h"
#include "WE3/phys/WEPhysTypes.h"
#include "WE3/phys/WEPhysRigidBody.h"
#include "WE3/phys/WEPhysMaterial.h"
#include "WE3/defaultImpl/world/WEWorldMaterialManager.h"
#include "WE3/object/WEObject.h"
using namespace WE;

#include "../inc/Index.h"

namespace WOF { namespace match {
	
	enum SweptFractionType {
		SFT_Zero = 0, SFT_ZeroEpsilon = 1, SFT_Delta
	};
	
	struct SweptIntersection : WE::SweptIntersection, PlanarFaceIntersectionEx4 {

		Point contactPoint;

		bool recheckGeometry;
		WorldMaterialCombo* pMaterial;

		//ObjectType staticObjType;
		SoftPtr<Object> staticObj;
		unsigned int objectPartID;

		bool contactHasVelocity;
		Vector3 contactVelocity;

		//if contactHasVelocity
		float contactMass;
		float contactBodyRadiusSq;
		Vector3 contactBodyPos;
	};

	enum SweptContactDynamicType {
		SCDT_Resting = 0, SCDT_Colliding = 1, SCDT_Separating, SCDT_Trigger
	};

	struct SweptContactDynamics {

		Vector3 sweptVectorAtContact;
		Point posAtContact;
		
		Vector3 normal;
		
		PhysScalar penetration;

		Vector3 relVel;
		PhysScalar relVelNMag;

		SweptContactDynamicType type;
	};

	struct SweptIntersectionArray : WETL::CountedArray<SweptIntersection, false, ContactIndexType> {
	};

	struct SweptIntersectionPtrArray : WETL::CountedArray<SweptIntersection*, false, ContactIndexType> {
	};

	
} }


#endif