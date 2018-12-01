#ifndef h_WOF_match_SpecializedCollisionLaws
#define h_WOF_match_SpecializedCollisionLaws

#include "WE3/WitchEngine.h"
#include "WE3/phys/WEPhysCollisionLaw.h"
using namespace WE;

namespace WOF { namespace match {

	struct SpecializedCollisionLaw : PhysCollisionLaw {
	
		static void applyImpulseToBall(PhysRigidBody& body, const Vector3& impulse, Point& contactNormal, Vector3& contactPoint, float dt, bool isBodyI);

		static void computeBallImpulse(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, PhysMaterial* pMaterial, 
														Vector3& normal, Point& contactPoint, Vector& relVel, PhysScalar relVelNormalMag, Vector3& JV, Vector3& JW);
	};

} }


#endif