#include "WECoordSysConv_Swizzle.h"

#include "../WEMacros.h"

namespace WE {



CoordSysConv_Swizzle::CoordSysConv_Swizzle() {
}

CoordSysConv_Swizzle::~CoordSysConv_Swizzle() {
}

bool CoordSysConv_Swizzle::isConverting() const {

	return convDir || convUnit;
}

void CoordSysConv_Swizzle::init(const CoordSys& src, const CoordSys& dest) {

	bool doConvDir = false;

	invertAngles = (src.rotationLeftHanded != dest.rotationLeftHanded);

	if (invertAngles) {

		doConvDir = true; 
	}

	CoordSysExpanded _src(src);
	CoordSysExpanded _dest(dest);
	
	for(short i = 0; i < 3; ++i) {

		switch (_src.el[i]) {

			case CSD_Left:
			{
				for(short j = 0; j < 3; ++j) {
					if (_dest.el[j] == CSD_Left) {

						if (i != j) doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = false;

						swizzleTargetToSrc[i] = j;
						negSrcToTarget[i] = false;

						break;
					} else if (_dest.el[j] == CSD_Right) {

						doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = true;

						swizzleTargetToSrc[i] = j;
						negSrcToTarget[i] = true;

						break;
					} 
				}
			}
			break;
			case CSD_Right:
			{
				for(short j = 0; j < 3; ++j) {
					if (_dest.el[j] == CSD_Left) {

						doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = true;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = true;

						break;
					} else if (_dest.el[j] == CSD_Right) {

						if (i != j) doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = false;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = false;

						break;
					} 
				}
			}
			break;
			case CSD_Up:
			{
				for(short j = 0; j < 3; ++j) {
					if (_dest.el[j] == CSD_Up) {

						if (i != j) doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = false;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = false;

						break;
					} else if (_dest.el[j] == CSD_Down) {

						doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = true;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = true;

						break;
					} 
				}
			}
			break;
			case CSD_Down:
			{
				for(short j = 0; j < 3; ++j) {
					if (_dest.el[j] == CSD_Up) {

						doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = true;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = true;

						break;
					} else if (_dest.el[j] == CSD_Down) {

						if (i != j) doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = false;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = false;

						break;
					} 
				}
			}
			break;
			case CSD_Forward:
			{
				for(short j = 0; j < 3; ++j) {
					if (_dest.el[j] == CSD_Forward) {

						if (i != j) doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = false;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = false;

						break;
					} else if (_dest.el[j] == CSD_Back) {

						doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = true;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = true;

						break;
					} 
				}
			}
			break;
			case CSD_Back:
			{
				for(short j = 0; j < 3; ++j) {
					if (_dest.el[j] == CSD_Forward) {

						doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = true;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = true;

						break;
					} else if (_dest.el[j] == CSD_Back) {

						if (i != j) doConvDir = true; 

						swizzleSrcToTarget[j] = i;
						negSrcToTarget[j] = false;

						swizzleTargetToSrc[i] = j;
						negTargetToSrc[i] = false;

						break;
					} 
				}
			}	
			break;
		}
	}

	convDir = doConvDir;

	if (src.hasUnitsPerMeter() && dest.hasUnitsPerMeter()) {

		convUnit = src.unitsPerMeter != dest.unitsPerMeter;

		if (convUnit) {

			unitSourceToTarget = _dest.unitsPerMeter / _src.unitsPerMeter;
			unitTargetToSource = _src.unitsPerMeter / _dest.unitsPerMeter;
		}

	} else {

		convUnit = false;
	}
}

void CoordSysConv_Swizzle::scale(float* values, const float& scale) const {

	values[0] *= scale;
	values[1] *= scale;
	values[2] *= scale;
}


void CoordSysConv_Swizzle::toTargetSwizzle(const float* from, float* to) const {

	if (convDir) {

		to[0] = from[swizzleSrcToTarget[0]];
		to[1] = from[swizzleSrcToTarget[1]];
		to[2] = from[swizzleSrcToTarget[2]];

	} else {

		_toTargetPoint(from, to);
	}
}

void CoordSysConv_Swizzle::toTargetPoint(const float* from, float* to) const {

	if (convDir) {

		to[0] = negSrcToTarget[0] ? -from[swizzleSrcToTarget[0]] : from[swizzleSrcToTarget[0]];
		to[1] = negSrcToTarget[1] ? -from[swizzleSrcToTarget[1]] : from[swizzleSrcToTarget[1]];
		to[2] = negSrcToTarget[2] ? -from[swizzleSrcToTarget[2]] : from[swizzleSrcToTarget[2]];

	} else {

		_toTargetPoint(from, to);
	}

	if (convUnit) {

		scale(to, unitSourceToTarget);
	}
}


void CoordSysConv_Swizzle::toTargetVector(const float* from, float* to) const {

	if (convDir) {

		to[0] = negSrcToTarget[0] ? -from[swizzleSrcToTarget[0]] : from[swizzleSrcToTarget[0]];
		to[1] = negSrcToTarget[1] ? -from[swizzleSrcToTarget[1]] : from[swizzleSrcToTarget[1]];
		to[2] = negSrcToTarget[2] ? -from[swizzleSrcToTarget[2]] : from[swizzleSrcToTarget[2]];

	} else {

		_toTargetVector(from, to);
	}
}

void CoordSysConv_Swizzle::toTargetRotation(const float* from, float fromAngle, float* to, float& toAngle) const {

	if (convDir) {

		to[0] = negSrcToTarget[0] ? -from[swizzleSrcToTarget[0]] : from[swizzleSrcToTarget[0]];
		to[1] = negSrcToTarget[1] ? -from[swizzleSrcToTarget[1]] : from[swizzleSrcToTarget[1]];
		to[2] = negSrcToTarget[2] ? -from[swizzleSrcToTarget[2]] : from[swizzleSrcToTarget[2]];

		toAngle = invertAngles ? -fromAngle : fromAngle;

	} else {

		_toTargetRotation(from, fromAngle, to, toAngle);
	}
}

void CoordSysConv_Swizzle::toTargetFaceIndices(const RenderUINT* from, RenderUINT* to) const {

	if (convDir && invertAngles) {

		to[0] = from[0];
		to[1] = from[2];
		to[2] = from[1];
	
	} else {

		_toTargetFaceIndices(from, to);
	}
}


void CoordSysConv_Swizzle::toTargetSwizzle(float* to) const {

	if (convDir) {

		float from[3];

		from[0] = to[0];
		from[1] = to[1];
		from[2] = to[2];

		to[0] = from[swizzleSrcToTarget[0]];
		to[1] = from[swizzleSrcToTarget[1]];
		to[2] = from[swizzleSrcToTarget[2]];

	} else {

		_toTargetPoint(to);
	}
}

void CoordSysConv_Swizzle::toTargetPoint(float* to) const {

	if (convDir) {

		float from[3];

		from[0] = to[0];
		from[1] = to[1];
		from[2] = to[2];

		to[0] = negSrcToTarget[0] ? -from[swizzleSrcToTarget[0]] : from[swizzleSrcToTarget[0]];
		to[1] = negSrcToTarget[1] ? -from[swizzleSrcToTarget[1]] : from[swizzleSrcToTarget[1]];
		to[2] = negSrcToTarget[2] ? -from[swizzleSrcToTarget[2]] : from[swizzleSrcToTarget[2]];

	} else {

		_toTargetPoint(to);
	}

	if (convUnit) {

		scale(to, unitSourceToTarget);
	}
}

void CoordSysConv_Swizzle::toTargetVector(float* to) const {

	if (convDir) {

		float from[3];

		from[0] = to[0];
		from[1] = to[1];
		from[2] = to[2];

		to[0] = negSrcToTarget[0] ? -from[swizzleSrcToTarget[0]] : from[swizzleSrcToTarget[0]];
		to[1] = negSrcToTarget[1] ? -from[swizzleSrcToTarget[1]] : from[swizzleSrcToTarget[1]];
		to[2] = negSrcToTarget[2] ? -from[swizzleSrcToTarget[2]] : from[swizzleSrcToTarget[2]];

	} else {

		_toTargetVector(to);
	}
}

void CoordSysConv_Swizzle::toTargetRotation(float* to, float& angle) const {

	if (convDir) {

		float from[3];

		from[0] = to[0];
		from[1] = to[1];
		from[2] = to[2];

		to[0] = negSrcToTarget[0] ? -from[swizzleSrcToTarget[0]] : from[swizzleSrcToTarget[0]];
		to[1] = negSrcToTarget[1] ? -from[swizzleSrcToTarget[1]] : from[swizzleSrcToTarget[1]];
		to[2] = negSrcToTarget[2] ? -from[swizzleSrcToTarget[2]] : from[swizzleSrcToTarget[2]];

		if (invertAngles) {
			angle =  -angle;
		}

	} else {

		_toTargetRotation(to, angle);
	}
}

void CoordSysConv_Swizzle::toTargetFaceIndices(RenderUINT* indices) const {

	if (convDir && invertAngles) {

		RenderUINT t = indices[1];
		indices[1] = indices[2];
		indices[2] = t;

	} else {

		_toTargetFaceIndices(indices);
	}
}


void CoordSysConv_Swizzle::toSourceSwizzle(const float* from, float* to) const {

	if (convDir) {

		to[0] = from[swizzleTargetToSrc[0]];
		to[1] = from[swizzleTargetToSrc[1]];
		to[2] = from[swizzleTargetToSrc[2]];

	} else {

		_toSourcePoint(from, to);
	}

}

void CoordSysConv_Swizzle::toSourcePoint(const float* from, float* to) const {

	if (convDir) {

		to[0] = negTargetToSrc[0] ? -from[swizzleTargetToSrc[0]] : from[swizzleTargetToSrc[0]];
		to[1] = negTargetToSrc[1] ? -from[swizzleTargetToSrc[1]] : from[swizzleTargetToSrc[1]];
		to[2] = negTargetToSrc[2] ? -from[swizzleTargetToSrc[2]] : from[swizzleTargetToSrc[2]];

	} else {

		_toSourcePoint(from, to);
	}

	if (convUnit) {

		scale(to, unitTargetToSource);
	}
}

void CoordSysConv_Swizzle::toSourceVector(const float* from, float* to) const {

	if (convDir) {

		to[0] = negTargetToSrc[0] ? -from[swizzleTargetToSrc[0]] : from[swizzleTargetToSrc[0]];
		to[1] = negTargetToSrc[1] ? -from[swizzleTargetToSrc[1]] : from[swizzleTargetToSrc[1]];
		to[2] = negTargetToSrc[2] ? -from[swizzleTargetToSrc[2]] : from[swizzleTargetToSrc[2]];

	} else {

		_toSourceVector(from, to);
	}
}

void CoordSysConv_Swizzle::toSourceRotation(const float* from, float fromAngle, float* to, float& toAngle) const {

	if (convDir) {

		to[0] = negTargetToSrc[0] ? -from[swizzleTargetToSrc[0]] : from[swizzleTargetToSrc[0]];
		to[1] = negTargetToSrc[1] ? -from[swizzleTargetToSrc[1]] : from[swizzleTargetToSrc[1]];
		to[2] = negTargetToSrc[2] ? -from[swizzleTargetToSrc[2]] : from[swizzleTargetToSrc[2]];

		toAngle = invertAngles ? -fromAngle : fromAngle;

	} else {

		_toSourceRotation(from, fromAngle, to, toAngle);
	}
}

void CoordSysConv_Swizzle::toSourceFaceIndices(const RenderUINT* from, RenderUINT* to) const {

	if (convDir && invertAngles) {

		to[0] = from[0];
		to[1] = from[2];
		to[2] = from[1];
		
	} else {

		_toSourceFaceIndices(from, to);
	}
}

void CoordSysConv_Swizzle::toSourceSwizzle(float* to) const {

	if (convDir) {

		float from[3];

		from[0] = to[0];
		from[1] = to[1];
		from[2] = to[2];

		to[0] = from[swizzleTargetToSrc[0]];
		to[1] = from[swizzleTargetToSrc[1]];
		to[2] = from[swizzleTargetToSrc[2]];

	} else {

		_toSourcePoint(to);
	}
}

void CoordSysConv_Swizzle::toSourcePoint(float* to) const {

	if (convDir) {

		float from[3];

		from[0] = to[0];
		from[1] = to[1];
		from[2] = to[2];

		to[0] = negTargetToSrc[0] ? -from[swizzleTargetToSrc[0]] : from[swizzleTargetToSrc[0]];
		to[1] = negTargetToSrc[1] ? -from[swizzleTargetToSrc[1]] : from[swizzleTargetToSrc[1]];
		to[2] = negTargetToSrc[2] ? -from[swizzleTargetToSrc[2]] : from[swizzleTargetToSrc[2]];

	} else {

		_toSourcePoint(to);
	}

	if (convUnit) {

		scale(to, unitTargetToSource);
	}
}

void CoordSysConv_Swizzle::toSourceVector(float* to) const {

	if (convDir) {

		float from[3];

		from[0] = to[0];
		from[1] = to[1];
		from[2] = to[2];

		to[0] = negTargetToSrc[0] ? -from[swizzleTargetToSrc[0]] : from[swizzleTargetToSrc[0]];
		to[1] = negTargetToSrc[1] ? -from[swizzleTargetToSrc[1]] : from[swizzleTargetToSrc[1]];
		to[2] = negTargetToSrc[2] ? -from[swizzleTargetToSrc[2]] : from[swizzleTargetToSrc[2]];

	} else {

		_toSourceVector(to);
	}
}

void CoordSysConv_Swizzle::toSourceRotation(float* to, float& angle) const {

	if (convDir) {

		float from[3];

		from[0] = to[0];
		from[1] = to[1];
		from[2] = to[2];

		to[0] = negTargetToSrc[0] ? -from[swizzleTargetToSrc[0]] : from[swizzleTargetToSrc[0]];
		to[1] = negTargetToSrc[1] ? -from[swizzleTargetToSrc[1]] : from[swizzleTargetToSrc[1]];
		to[2] = negTargetToSrc[2] ? -from[swizzleTargetToSrc[2]] : from[swizzleTargetToSrc[2]];

		if (invertAngles) {
			angle =  -angle;
		}

	} else {

		_toSourceRotation(to, angle);
	}
}

void CoordSysConv_Swizzle::toSourceFaceIndices(UINT* indices) const {

	if (convDir && invertAngles) {

		UINT t = indices[1];
		indices[1] = indices[2];
		indices[2] = t;

	} else {

		_toSourceFaceIndices(indices);
	}
}

void CoordSysConv_Swizzle::toTargetUnits(float& units) const {

	if (convUnit) {

		scale(units, unitSourceToTarget);
	}

}

void CoordSysConv_Swizzle::fromTargetUnits(float& units) const {

	if (convUnit) {

		scale(units, unitTargetToSource);
	}
}

}