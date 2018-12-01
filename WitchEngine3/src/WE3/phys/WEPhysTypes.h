#ifndef _WEPhysTypes_h
#define _WEPhysTypes_h


#include "../math/WEMathUtil.h"
#include "../math/WEBigMatrix.h"
#include "../math/WEMatrix.h"
#include "../math/WEQuaternion.h"
#include "../WETime.h"

#include "float.h"

namespace WE {

	typedef float PhysScalar;
	#define _Phys(val) ((PhysScalar) val)

	const PhysScalar PhysZero = _Phys(0);
	const PhysScalar PhysInf = FLT_MAX;

	typedef int PhysIndex;
		
	typedef BigMatrix<PhysScalar> PhysBigMatrix;
	typedef BigMatrixOffset<PhysScalar> PhysBigMatrixOffset;
	
	

	const float physPi = kPif;
	const float phys2Pi = k2Pif;
	const float physHalfPi = kPiOver2f;
	const float physPiInv = k1OverPif;
	const float physHalfPiInv = k1Over2Pif;
	const float physPiOver180 = kPiOver180f;
	const float phys180OverPi = k180OverPif;
	const float physPiSq = kPi2f;

	
	#define physAbs(val) fabs(val)
	#define physCos(rad) cosf(rad)
	#define physSin(rad) sinf(rad)
	#define physSqrt(val) sqrtf(val)

	
		 
};

#endif
