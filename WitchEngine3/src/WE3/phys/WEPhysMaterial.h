#ifndef _WEPhysMaterial_h
#define _WEPhysMaterial_h

#include "WEPhysTypes.h"
#include "../coordSys/WECoordSysConv.h"

namespace WE {


	struct PhysMaterial {

		PhysScalar nRestCoeff; //bouncy
		PhysScalar tRestCoeff;

		bool hasFriction;

		PhysScalar frictionCoeff; //rough
		short frictionDirCount;

		PhysScalar errReductionParam;

		PhysScalar movePenalty; //
		PhysScalar moveDrag; //loss = v * drag

		PhysScalar pureRotFrictionCoeff;

		bool isFlexible;
		PhysScalar flexible;

		void convertUnits(const float& unitMulti);
	};

	void physMatCombine(const PhysMaterial& mat1, const PhysMaterial& mat2, PhysMaterial& comb);

}

#endif