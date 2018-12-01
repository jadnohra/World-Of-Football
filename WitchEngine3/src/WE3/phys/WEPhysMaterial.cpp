#include "WEPhysMaterial.h"

namespace WE {


void PhysMaterial::convertUnits(const float& unitMulti) {

	movePenalty *= unitMulti;
}

void physMatCombine(const PhysMaterial& mat1, const PhysMaterial& mat2, PhysMaterial& comb) {

	comb.nRestCoeff = MMin(mat1.nRestCoeff, mat2.nRestCoeff); 
	comb.tRestCoeff = MMin(mat1.tRestCoeff, mat2.tRestCoeff); 

	comb.hasFriction = mat1.hasFriction & mat2.hasFriction;

	comb.frictionCoeff = MMin(mat1.frictionCoeff, mat2.frictionCoeff); 
	comb.frictionDirCount = MMin(mat1.frictionDirCount, mat2.frictionDirCount);

	comb.errReductionParam = (mat1.errReductionParam + mat2.errReductionParam) * 0.5f;

	comb.movePenalty = (mat1.movePenalty + mat2.movePenalty) * 0.5f; //m./sec.
	comb.moveDrag = (mat1.moveDrag + mat2.moveDrag) * 0.5f; //loss = v * drag
	comb.pureRotFrictionCoeff = (mat1.pureRotFrictionCoeff + mat2.pureRotFrictionCoeff) * 0.5f; 

	comb.flexible = MMax(mat1.flexible, mat2.flexible); 
	comb.isFlexible = (comb.flexible != 0.0f);
}

}