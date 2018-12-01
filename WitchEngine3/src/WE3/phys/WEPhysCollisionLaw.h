#ifndef _WEPhysCollisionLaw_h
#define _WEPhysCollisionLaw_h

#include "WEPhysTypes.h"
#include "WEPhysConstraintContact.h"

namespace WE {

	struct PhysCollisionLaw {

		static void computeCollisionMatrix(const PhysScalar& invMassA, InertiaTensor& tensorA, const Vector3& ra, 
											const PhysScalar& invMassB, InertiaTensor& tensorB, const Vector3& rb,
											Matrix33& K); 

		static void applyImpulse(PhysRigidBody& body, const Vector3& impulse, Vector3& contactPoint, bool isBodyI);
		static void applyImpulse(PhysRigidBody* pBody, const Vector3& r, const Vector3& J);
		static void applyImpulse(PhysConstraintContact& contact, const Vector3& J);
	};

	struct PhysCollisionLawFrictionlessNewton : PhysCollisionLaw {
	
		static void computeImpulse(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, PhysMaterial* pMaterial, 
									Vector3& normal, Point& contactPoint, Vector& relVel, PhysScalar relVelNormalMag, Vector3& J);
	};

	struct PhysCollisionLawFrictionNewton : PhysCollisionLaw {
	
		static void computeImpulse(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, PhysMaterial* pMaterial, 
									Vector3& normal, Point& contactPoint, Vector& relVel, PhysScalar relVelNormalMag, Vector3& J);
	};

	struct PhysCollisionLawChatterjeeRuina : PhysCollisionLaw {
	
		static void computeImpulse(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, PhysMaterial* pMaterial, 
									Vector3& normal, Point& contactPoint, Vector& relVel, PhysScalar relVelNormalMag, Vector3& J);
	};

}


#endif