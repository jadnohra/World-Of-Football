#include "WECoordSysConv.h"

#include "../WEMacros.h"

namespace WE {


CoordSysConv::~CoordSysConv() {
}

bool CoordSysConv::isConverting() const {

	return false;
}

void CoordSysConv::toTargetVolume(AAB& aab) {

	if (aab.isEmpty()) {

		return;
	}

	toTargetPoint(aab.min.el);
	toTargetPoint(aab.max.el);

	for (short d = 0; d < 3; d++) {

		if (aab.min[d] > aab.max[d]) {

			swapt<float>(aab.min[d], aab.max[d]);
		}
	}
}

void CoordSysConv::toTargetSwizzle(const float* from, float* to) const {

	_toTargetSwizzle(from, to);
}

void CoordSysConv::toTargetPoint(const float* from, float* to) const {

	_toTargetPoint(from, to);
}

void CoordSysConv::toTargetVector(const float* from, float* to) const {

	_toTargetVector(from, to);
}

void CoordSysConv::toTargetRotation(const float* fromAxis, float fromAngle, float* toAxis, float& toAngle) const {

	_toTargetRotation(fromAxis, fromAngle, toAxis, toAngle);
}

void CoordSysConv::toTargetFaceIndices(const RenderUINT* from, RenderUINT* to) const {

	_toTargetFaceIndices(from, to);
}


void CoordSysConv::toTargetSwizzle(float* vect) const {

	_toTargetSwizzle(vect);
}

void CoordSysConv::toTargetPoint(float* vect) const {

	_toTargetPoint(vect);
}

void CoordSysConv::toTargetVector(float* vect) const {

	_toTargetVector(vect);
}

void CoordSysConv::toTargetRotation(float* axis, float& angle) const {

	_toTargetRotation(axis, angle);
}

void CoordSysConv::toTargetFaceIndices(RenderUINT* indices) const {

	_toTargetFaceIndices(indices);
}

void CoordSysConv::toSourceSwizzle(const float* from, float* to) const {

	_toSourceSwizzle(from, to);
}

void CoordSysConv::toSourcePoint(const float* from, float* to) const {

	_toSourcePoint(from, to);
}

void CoordSysConv::toSourceVector(const float* from, float* to) const {

	_toSourceVector(from, to);
}

void CoordSysConv::toSourceRotation(const float* fromAxis, float fromAngle, float* toAxis, float& toAngle) const {

	_toSourceRotation(fromAxis, fromAngle, toAxis, toAngle);
}

void CoordSysConv::toSourceFaceIndices(const RenderUINT* from, RenderUINT* to) const {

	_toSourceFaceIndices(from, to);
}

void CoordSysConv::toSourceSwizzle(float* vect) const {

	_toSourceSwizzle(vect);
}

void CoordSysConv::toSourcePoint(float* vect) const {

	_toSourcePoint(vect);
}

void CoordSysConv::toSourceVector(float* vect) const {

	_toSourceVector(vect);
}

void CoordSysConv::toSourceRotation(float* axis, float& angle) const {

	_toSourceRotation(axis, angle);
}

void CoordSysConv::toSourceFaceIndices(RenderUINT* indices) const {

	_toSourceFaceIndices(indices);
}

void CoordSysConv::toTargetUnits(float& units) const {

	_toTargetUnits(units);
}


void CoordSysConv::fromTargetUnits(float& units) const {

	_fromTargetUnits(units);
}

void CoordSysConv::_toTargetSwizzle(const float* from, float* to)  {

	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void CoordSysConv::_toTargetPoint(const float* from, float* to)  {

	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void CoordSysConv::_toTargetVector(const float* from, float* to)  {

	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void CoordSysConv::_toTargetRotation(const float* fromAxis, float fromAngle, float* toAxis, float& toAngle)  {

	toAxis[0] = fromAxis[0];
	toAxis[1] = fromAxis[1];
	toAxis[2] = fromAxis[2];

	toAngle = fromAngle;
}

void CoordSysConv::_toTargetFaceIndices(const RenderUINT* from, RenderUINT* to)  {

	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void CoordSysConv::_toSourceSwizzle(const float* from, float* to)  {

	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void CoordSysConv::_toSourcePoint(const float* from, float* to)  {

	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void CoordSysConv::_toSourceVector(const float* from, float* to)  {

	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void CoordSysConv::_toSourceRotation(const float* fromAxis, float fromAngle, float* toAxis, float& toAngle)  {

	toAxis[0] = fromAxis[0];
	toAxis[1] = fromAxis[1];
	toAxis[2] = fromAxis[2];

	toAngle = fromAngle;
}

void CoordSysConv::_toSourceFaceIndices(const RenderUINT* from, RenderUINT* to)  {

	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}



}