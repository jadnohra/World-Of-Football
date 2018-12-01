#ifndef _WEPhysConstraint_h
#define _WEPhysConstraint_h

#include "WEPhysTypes.h"
#include "WEPhysRigidBody.h"

namespace WE {


	struct PhysConstraintLCPElements {
		
		PhysBigMatrix* pJacobian;
		int* jacobianBodyIndices;
		PhysScalar* limitLows;
		PhysScalar* limitHighs;
		int* dependIndices;
		PhysScalar* dependFactors;
		PhysScalar* errorTerm;
		PhysScalar fps;

		int fillJacobianIndex;
		int fillJacobianBodyIndicesIndex;
		int fillJacobianIndexBodyI;
		int fillJacobianIndexBodyJ;
	};
	

	class PhysConstraint {
	public:

		enum BodyIndex {
			BodyI = 0, BodyJ
		};

		PhysRigidBody* mpBody[2];

		bool mIsActive;
		int mJacobianIndex;

		PhysScalar* mSavedSolution;

		bool mForceInactive;
		PhysScalar mErrorReductionParam;

	public:

		PhysScalar getErrorReductionParam();
		void setErrorReductionParam(PhysScalar val);

		void forceInactive(bool inactive);

		void setJacobianIndex(int index);
		int getJacobianIndex();

		void saveSolution(PhysScalar* vector);
		PhysScalar* getSavedSolution();

		bool isActive();

		virtual void evaluate(bool& isActive, int& jacobianRowCount) = 0; 
		
		virtual void fillConstraintLCPElements(PhysConstraintLCPElements& lcpElements) = 0;
				
	};

}

#endif