#ifndef _WEPhysConstraintContact_h
#define _WEPhysConstraintContact_h

#include "WEPhysConstraint.h"
#include "WEPhysMaterial.h"
//#include "../WETL.h"

namespace WE {


	class PhysConstraintContact : public PhysConstraint {
	public:

		PhysMaterial* mpMaterial;

		short mTangFrictionDirCount;
		bool mIncludeCollisionLawInErrorTerm;

		PhysScalar mDistance; //separation (pos) / penetration (neg)

		Vector3 mRelPointVel;
		PhysScalar mRelNormalVelMag; ///< The relative contact velocity in the normal direction.

		Vector3 p;
		
		Vector3 n;
		Vector3 t1;
		Vector3 t2;

		Vector3 r[2];

		Vector3 rxn[2];

		Vector3 rxt1[2];
		Vector3 rxt2[2];
		
		//typedef WETL::Array<Vector3, false, short> FrictionPyramidBase;
		//FrictionPyramidBase D;

	protected:

		inline void fillJacobianLinear(PhysBigMatrix& matrix, PhysBigMatrixOffset& sub, int& col, PhysScalar mul);
		inline void fillJacobianAngular(PhysBigMatrix& matrix, PhysBigMatrixOffset& sub, int& col, BodyIndex body, PhysScalar mul);

		inline void fillErrorTerm(PhysScalar* vector, PhysScalar& fps);
		inline void fillLimits(PhysScalar* lows, PhysScalar* highs);
		inline void fillDependencies(int* indices, PhysScalar* factors, int offset);

		inline void initFrictionTangentDirections();
		inline void initContactVariables();

	public:

		PhysConstraintContact();
		~PhysConstraintContact();
		
		static void orderBodies(PhysRigidBody& body1, PhysRigidBody& body2, PhysRigidBody*& pBodyI, PhysRigidBody*& pBodyJ, bool& isInverseOrder);
		static void getContactVelocity(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, Vector3& normal, Vector3& point, Vector3& relVel, PhysScalar& relVelN);
		void initContact(PhysRigidBody* pBody1, PhysRigidBody* pBody2, PhysMaterial* pMaterial, 
							Vector3& normal, Vector3& point, PhysScalar distance);
		
		//returns correct result after initContact is called, even if evaluate was not called
		PhysScalar getRelativeNormalVelocityMag();

		void cloneInitedContact(PhysConstraintContact& clone);

	public:

		virtual void evaluate(bool& isActive, int& jacobianRowCount); 
		virtual void fillConstraintLCPElements(PhysConstraintLCPElements& lcpElements);
		
	};

}

#endif