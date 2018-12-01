#ifndef _WEPhysContraintGroup_h
#define _WEPhysContraintGroup_h

#include "WEPhysTypes.h"
#include "../WETL.h"
#include "WEPhysConstraint.h"


namespace WE {


	class PhysBodyGroup {
	public:

		typedef WETL::CountedArray<PhysConstraint*, false, PhysIndex> ConstraintPtrArray;
		typedef WETL::CountedArray<PhysRigidBody*, false, PhysIndex> BodyPtrArray;
		typedef WETL::CountedArray<PhysIndex, false, PhysIndex> TagArray;
		
		ConstraintPtrArray mConstraintPtrs;
		ConstraintPtrArray mVolatileConstraintPtrs;
		BodyPtrArray mBodyPtrs;

	protected:

		void evaluateConstraints(ConstraintPtrArray& constraintPtrs, int& jacRowIndex, PhysIndex& activeConstraintCount);
		void fillConstraintLCPElements(ConstraintPtrArray& constraintPtrs, PhysConstraintLCPElements& lcpElements);

	public:

		void resizeContainers() const;
		void fillGeneralizedVelocity(PhysScalar* u) const;
		void fillGeneralizedPosition(PhysScalar* s) const;
		void fillGeneralizedExternalForce(PhysScalar* fext, bool includeVelocityForces) const;
		void fillInverseMass(PhysScalar* invM, PhysScalar* invI) const;
		void fillGeneralizedInverseMass(PhysBigMatrix& invM) const;

		void evaluateConstraints(PhysIndex& constraintCount);
		void fillConstraintLCPElements(PhysConstraintLCPElements& lcpElements);
		

		void putGeneralizedVelocity(PhysScalar* u) const;
		void doPositionUpdate(PhysScalar dt) const;
		static void doPositionUpdate(PhysRigidBody& body, PhysScalar dt);
		//void doPositionUpdate(PhysScalar* s, PhysScalar* u, PhysScalar dt, PhysScalar* sNew) const;

	public:

		PhysBodyGroup();
		~PhysBodyGroup();

		void destroy();

		void init(PhysIndex estBodyCount, PhysIndex estFixConstraintCount, PhysIndex estVolatileConstraintCount);

		void addBody(PhysRigidBody* pBody);
		void addConstraint(PhysConstraint* pConstraint, bool isVolatile);

		void resetVolatileConstraints();

	};

}

#endif