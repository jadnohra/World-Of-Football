#include "WECoordSys.h"

#include "../WEAssert.h"
#include "../WEMem.h"
#include "../WEMacros.h"

namespace WE {

const float CoordSys::kCSU_1 = 1.0f;
const float CoordSys::kCSU_10 = 10.0f;
const float CoordSys::kCSU_100 = 100.0f;
const float CoordSys::kCSU_01 = 0.1f;
const float CoordSys::kCSU_001 = 0.01f;
const float CoordSys::kCSU_e = 0.00001f;
const float CoordSys::kCSU_None = 0.0f;


CoordSys::CoordSys() : _size(0) {
}

CoordSys::CoordSys(bool valid) : bitValid(valid) {
}

CoordSys::CoordSys(const CoordSys& ref) {

	set(ref);
}

CoordSys::CoordSys(const CoordSys* pRef) {

	if (pRef) 
		set(*pRef);
}

bool CoordSys::operator==(const CoordSys& comp) const {

	assrt(false);
	return true;
}

CoordSys& CoordSys::operator=(const CoordSys& ref) {

	assrt(false);
	return *this;
}


bool CoordSys::isValid() const {

	return bitValid;
}

void CoordSys::setValid(bool valid) {

	bitValid = valid;
}

bool CoordSys::hasUnitsPerMeter() const {

	return (unitsPerMeter != CSU_None);
}

bool CoordSys::setFloatUnitsPerMeter(const float& value) {

	static const float e = 0.00001f;

	if (equalf(value, kCSU_None, kCSU_e)) {
		unitsPerMeter = CSU_None;
	} else if (equalf(value, kCSU_1, kCSU_e)) {
		unitsPerMeter = CSU_1;
	} else if (equalf(value, kCSU_10, kCSU_e)) {
		unitsPerMeter = CSU_10;
	} else if (equalf(value, kCSU_100, kCSU_e)) {
		unitsPerMeter = CSU_100;
	} else if (equalf(value, kCSU_01, kCSU_e)) {
		unitsPerMeter = CSU_01;
	} else if (equalf(value, kCSU_001, kCSU_e)) {
		unitsPerMeter = CSU_001;
	} else {

		assrt(false);
		return false;
	}

	return true;
}

const float& CoordSys::getFloatUnitsPerMeter() const {

	switch (unitsPerMeter) {
		case CSU_1:
			return kCSU_1;
		case CSU_10:
			return kCSU_10;
		case CSU_100:
			return kCSU_100;
		case CSU_01:
			return kCSU_01;
		case CSU_001:
			return kCSU_001;
	}

	return kCSU_None;
}


void CoordSys::set(const CoordSys& ref, bool setUnitsPerMeterIfSpecified) {

	if (bitValid && ref.bitValid) {

		_bit_dir_10 = ref._bit_dir_10;

		if (setUnitsPerMeterIfSpecified && (ref.unitsPerMeter != CSU_None)) {

			unitsPerMeter = ref.unitsPerMeter;
		} 

	} else {

		_bit_14 = ref._bit_14;
	}
}

bool CoordSys::equals(const CoordSys& comp) const {

	return (_bit_14 == comp._bit_14);
}

bool CoordSys::isCompatibleWith(const CoordSys& comp) const {

	if (_bit_dir_10 != comp._bit_dir_10) {

		return false;
	}

	if (unitsPerMeter != comp.unitsPerMeter) {

		return ((unitsPerMeter == kCSU_None) || (comp.unitsPerMeter == kCSU_None));
	} 

	return true;
}

short CoordSys::findTarget(const CoordSysDirection& dir, const CoordSysDirection& dirNeg, short& target, bool& neg) const {

	if (el0 == dir) {

		target = 0;
		neg = false;

	} else if (el0 == dirNeg) {

		target = 0;
		neg = true;

	} else if (el1 == dir) {

		target = 1;
		neg = false;

	} else if (el1 == dirNeg) {

		target = 1;
		neg = true;

	} else if (el2 == dir) {

		target = 2;
		neg = false;

	} else if (el2 == dirNeg) {

		target = 2;
		neg = true;
	} 
	
	return target;
}

short CoordSys::findTarget(const CoordSysDirection& dir, short& target, bool& neg) const {

	CoordSysDirection dirNeg;

	switch (dir) {
		case CSD_Left:
			dirNeg = CSD_Right;
			break;
		case CSD_Right:
			dirNeg = CSD_Left;
			break;
		case CSD_Up:
			dirNeg = CSD_Down;
			break;
		case CSD_Down:
			dirNeg = CSD_Up;
			break;
		case CSD_Forward:
			dirNeg = CSD_Back;
			break;
		case CSD_Back:
			dirNeg = CSD_Forward;
			break;
		default:
			return -1;
	}

	return findTarget(dir, dirNeg, target, neg);
}

void CoordSys::setValue(const CoordSysDirection& dir, const float& val, float* vector) const {

	bool neg;
	short target; 
	
	findTarget(dir, target, neg);

	if (target == -1) {

		assrt(false);
		return;

	} 
	
	vector[target] = neg ? -val : val;
}

void CoordSys::getValue(const CoordSysDirection& dir, const float* source, float& val) const {

	bool neg;
	short target; 
	
	findTarget(dir, target, neg);

	if (target == -1) {

		assrt(false);
		return;

	}

	val = neg ? -source[target] : source[target];
}

void CoordSys::setRUF(const float& right, const float& up, const float& forward, float* result) const {

	setValue(CSD_Right, right, result);
	setValue(CSD_Up, up, result);
	setValue(CSD_Forward, forward, result);
}

void CoordSys::getUnitVector(const CoordSysDirection& dir, float* result) const {

	bool neg;
	short target; 
	
	findTarget(dir, target, neg);

	if (target == -1) {

		assrt(false);
		return;

	}

	result[target % 3] = neg ? -1.0f : 1.0f;
	result[(++target) % 3] = 0.0f;
	result[(++target) % 3] = 0.0f;
}


CoordSysExpanded::CoordSysExpanded() {
}

CoordSysExpanded::CoordSysExpanded(const CoordSys& ref) {

	if (ref.isValid() == false) {

		assrt(false);
	}

	el[0] = (CoordSysDirection) ref.el0;
	el[1] = (CoordSysDirection) ref.el1;
	el[2] = (CoordSysDirection) ref.el2;

	rotationLeftHanded = ref.rotationLeftHanded;
	unitsPerMeter = ref.getFloatUnitsPerMeter();
}

void CoordSysExpanded::convertTo(CoordSys& dest) const {

	dest.el0 = el[0];
	dest.el1 = el[1];
	dest.el2 = el[2];

	dest.rotationLeftHanded = rotationLeftHanded;

	dest.setFloatUnitsPerMeter(unitsPerMeter);
}

bool CoordSysExpanded::equals(const CoordSysExpanded& comp) const {

	return (memcmp(this, &comp, sizeof(CoordSysExpanded)) == 0);
}

bool FastCoordSys::init(const CoordSys& ref) {

	switch(ref.el0) {
		case CSD_Left:
			neg[CSD_Left] = true;
			neg[CSD_Right] = true;
			target[CSD_Left] = 0;
			target[CSD_Right] = 0;
			break;
		case CSD_Right:
			neg[CSD_Left] = false;
			neg[CSD_Right] = false;
			target[CSD_Left] = 0;
			target[CSD_Right] = 0;
			break;
		case CSD_Up: 
			neg[CSD_Down] = false;
			neg[CSD_Up] = false;
			target[CSD_Down] = 0;
			target[CSD_Up] = 0;
			break;
		case CSD_Down:
			neg[CSD_Down] = true;
			neg[CSD_Up] = true;
			target[CSD_Down] = 0;
			target[CSD_Up] = 0;
			break;
		case CSD_Back:
			neg[CSD_Back] = true;
			neg[CSD_Forward] = true;
			target[CSD_Back] = 0;
			target[CSD_Forward] = 0;
			break;
		case CSD_Forward:
			neg[CSD_Back] = false;
			neg[CSD_Forward] = false;
			target[CSD_Back] = 0;
			target[CSD_Forward] = 0;
			break;
		default:
			assrt(false);
			return false;
			break;
	}

	switch(ref.el1) {
		case CSD_Left:
			neg[CSD_Left] = true;
			neg[CSD_Right] = true;
			target[CSD_Left] = 1;
			target[CSD_Right] = 1;
			break;
		case CSD_Right:
			neg[CSD_Left] = false;
			neg[CSD_Right] = false;
			target[CSD_Left] = 1;
			target[CSD_Right] = 1;
			break;
		case CSD_Up: 
			neg[CSD_Down] = false;
			neg[CSD_Up] = false;
			target[CSD_Down] = 1;
			target[CSD_Up] = 1;
			break;
		case CSD_Down:
			neg[CSD_Down] = true;
			neg[CSD_Up] = true;
			target[CSD_Down] = 1;
			target[CSD_Up] = 1;
			break;
		case CSD_Back:
			neg[CSD_Back] = true;
			neg[CSD_Forward] = true;
			target[CSD_Back] = 1;
			target[CSD_Forward] = 1;
			break;
		case CSD_Forward:
			neg[CSD_Back] = false;
			neg[CSD_Forward] = false;
			target[CSD_Back] = 1;
			target[CSD_Forward] = 1;
			break;
		default:
			assrt(false);
			return false;
			break;
	}
	
	switch(ref.el0) {
		case CSD_Left:
			neg[CSD_Left] = true;
			neg[CSD_Right] = true;
			target[CSD_Left] = 0;
			target[CSD_Right] = 0;
			break;
		case CSD_Right:
			neg[CSD_Left] = false;
			neg[CSD_Right] = false;
			target[CSD_Left] = 0;
			target[CSD_Right] = 0;
			break;
		case CSD_Up: 
			neg[CSD_Down] = false;
			neg[CSD_Up] = false;
			target[CSD_Down] = 0;
			target[CSD_Up] = 0;
			break;
		case CSD_Down:
			neg[CSD_Down] = true;
			neg[CSD_Up] = true;
			target[CSD_Down] = 0;
			target[CSD_Up] = 0;
			break;
		case CSD_Back:
			neg[CSD_Back] = true;
			neg[CSD_Forward] = true;
			target[CSD_Back] = 0;
			target[CSD_Forward] = 0;
			break;
		case CSD_Forward:
			neg[CSD_Back] = false;
			neg[CSD_Forward] = false;
			target[CSD_Back] = 0;
			target[CSD_Forward] = 0;
			break;
		default:
			assrt(false);
			return false;
			break;
	}

	return true;
}

void FastCoordSys::setRUF(const float& right, const float& up, const float& forward, float* _target) const {

	_target[target[CSD_Right]] = neg[CSD_Right] ? -right : right;
	_target[target[CSD_Up]] = neg[CSD_Up] ? -up : up;
	_target[target[CSD_Forward]] = neg[CSD_Forward] ? -forward : forward;
}

void FastCoordSys::setValue(const CoordSysDirection& dir, const float& val, float* _target) const {

	_target[target[dir]] = neg[dir] ? -val : val;
}

void FastCoordSys::getValue(const CoordSysDirection& dir, const float* source, float& val) const {

	val = neg[dir] ? -source[target[dir]] : source[target[dir]];
}

void FastCoordSys::getUnitVector(const CoordSysDirection& dir, float* _target) const {

	int t = target[dir];

	_target[t] = neg[dir] ? -1.0f : 1.0f;
	_target[++t] = 0.0f;
	_target[++t] = 0.0f;
}


bool FastUnitCoordSys::init(const CoordSys& ref, const float& userUnit) {

	switch(ref.el0) {
		case CSD_Left:
			neg[CSD_Left] = true;
			neg[CSD_Right] = true;
			target[CSD_Left] = 0;
			target[CSD_Right] = 0;
			break;
		case CSD_Right:
			neg[CSD_Left] = false;
			neg[CSD_Right] = false;
			target[CSD_Left] = 0;
			target[CSD_Right] = 0;
			break;
		case CSD_Up: 
			neg[CSD_Down] = false;
			neg[CSD_Up] = false;
			target[CSD_Down] = 0;
			target[CSD_Up] = 0;
			break;
		case CSD_Down:
			neg[CSD_Down] = true;
			neg[CSD_Up] = true;
			target[CSD_Down] = 0;
			target[CSD_Up] = 0;
			break;
		case CSD_Back:
			neg[CSD_Back] = true;
			neg[CSD_Forward] = true;
			target[CSD_Back] = 0;
			target[CSD_Forward] = 0;
			break;
		case CSD_Forward:
			neg[CSD_Back] = false;
			neg[CSD_Forward] = false;
			target[CSD_Back] = 0;
			target[CSD_Forward] = 0;
			break;
		default:
			assrt(false);
			return false;
			break;
	}

	switch(ref.el1) {
		case CSD_Left:
			neg[CSD_Left] = true;
			neg[CSD_Right] = true;
			target[CSD_Left] = 1;
			target[CSD_Right] = 1;
			break;
		case CSD_Right:
			neg[CSD_Left] = false;
			neg[CSD_Right] = false;
			target[CSD_Left] = 1;
			target[CSD_Right] = 1;
			break;
		case CSD_Up: 
			neg[CSD_Down] = false;
			neg[CSD_Up] = false;
			target[CSD_Down] = 1;
			target[CSD_Up] = 1;
			break;
		case CSD_Down:
			neg[CSD_Down] = true;
			neg[CSD_Up] = true;
			target[CSD_Down] = 1;
			target[CSD_Up] = 1;
			break;
		case CSD_Back:
			neg[CSD_Back] = true;
			neg[CSD_Forward] = true;
			target[CSD_Back] = 1;
			target[CSD_Forward] = 1;
			break;
		case CSD_Forward:
			neg[CSD_Back] = false;
			neg[CSD_Forward] = false;
			target[CSD_Back] = 1;
			target[CSD_Forward] = 1;
			break;
		default:
			assrt(false);
			return false;
			break;
	}
	
	switch(ref.el2) {
		case CSD_Left:
			neg[CSD_Left] = true;
			neg[CSD_Right] = true;
			target[CSD_Left] = 2;
			target[CSD_Right] = 2;
			break;
		case CSD_Right:
			neg[CSD_Left] = false;
			neg[CSD_Right] = false;
			target[CSD_Left] = 2;
			target[CSD_Right] = 2;
			break;
		case CSD_Up: 
			neg[CSD_Down] = false;
			neg[CSD_Up] = false;
			target[CSD_Down] = 2;
			target[CSD_Up] = 2;
			break;
		case CSD_Down:
			neg[CSD_Down] = true;
			neg[CSD_Up] = true;
			target[CSD_Down] = 2;
			target[CSD_Up] = 2;
			break;
		case CSD_Back:
			neg[CSD_Back] = true;
			neg[CSD_Forward] = true;
			target[CSD_Back] = 2;
			target[CSD_Forward] = 2;
			break;
		case CSD_Forward:
			neg[CSD_Back] = false;
			neg[CSD_Forward] = false;
			target[CSD_Back] = 2;
			target[CSD_Forward] = 2;
			break;
		default:
			assrt(false);
			return false;
			break;
	}

	unitConv = ref.getFloatUnitsPerMeter() / userUnit;
	invUnitConv = 1.0f / unitConv;

	return true;
}

void FastUnitCoordSys::setRUF(const float& right, const float& up, const float& forward, float* _target) const {

	_target[target[CSD_Right]] = unitConv * (neg[CSD_Right] ? -right : right);
	_target[target[CSD_Up]] = unitConv * (neg[CSD_Up] ? -up : up);
	_target[target[CSD_Forward]] = unitConv * (neg[CSD_Forward] ? -forward : forward);
}

void FastUnitCoordSys::setValue(const CoordSysDirection& dir, const float& val, float* _target) const {

	_target[target[dir]] = unitConv * (neg[dir] ? -val : val);
}


void FastUnitCoordSys::getValue(const CoordSysDirection& dir, const float* source, float& val) const {

	val = invUnitConv * (neg[dir] ? -source[target[dir]] : source[target[dir]]);
}

void FastUnitCoordSys::getUnitVector(const CoordSysDirection& dir, float* _target) const {

	int t = target[dir];

	_target[t] = neg[dir] ? -1.0f : 1.0f;
	_target[++t % 3] = 0.0f;
	_target[++t % 3] = 0.0f;
}


void FastUnitCoordSys::convVectorUnits(const float* val3, float* targetVal3) const {

	targetVal3[0] = unitConv * val3[0];
	targetVal3[1] = unitConv * val3[1];
	targetVal3[2] = unitConv * val3[2];
}

void FastUnitCoordSys::invConvVectorUnits(const float* val3, float* targetVal3) const {

	targetVal3[0] = invUnitConv * val3[0];
	targetVal3[1] = invUnitConv * val3[1];
	targetVal3[2] = invUnitConv * val3[2];
}

void setupCoordSysConvTransorm(const CoordSys& _source, const CoordSys& _target, Matrix33& transf) {

	CoordSysExpanded source(_source);
	CoordSysExpanded target(_target);

	transf.diagonal(0.0f);

	float multip = 1.0f;

	if (_source.hasUnitsPerMeter() && _target.hasUnitsPerMeter()) {

		multip = _target.getFloatUnitsPerMeter() / _source.getFloatUnitsPerMeter();
	}

	for (int i = 0; i < 3; ++i) {

		switch (source.el[i]) {
			
			case CSD_Left: {	

				for (int j = 0; j < 3; ++j) {

					if (target.el[j] ==  CSD_Left) {

						transf.row[j].el[i] = multip;

					} else if (target.el[j] ==  CSD_Right) {

						transf.row[j].el[i] = -multip;

					} else {

						transf.row[j].el[i] = 0.0f;
					}
				}

			} break;

			case CSD_Right: {	

				for (int j = 0; j < 3; ++j) {

					if (target.el[j] ==  CSD_Right) {

						transf.row[j].el[i] = multip;

					} else if (target.el[j] ==  CSD_Left) {

						transf.row[j].el[i] = -multip;

					} else {

						transf.row[j].el[i] = 0.0f;
					}
				}

			} break;

			case CSD_Up: {	

				for (int j = 0; j < 3; ++j) {

					if (target.el[j] ==  CSD_Up) {

						transf.row[j].el[i] = multip;

					} else if (target.el[j] ==  CSD_Down) {

						transf.row[j].el[i] = -multip;

					} else {

						transf.row[j].el[i] = 0.0f;
					}
				}

			} break;

			case CSD_Down: {	

				for (int j = 0; j < 3; ++j) {

					if (target.el[j] ==  CSD_Down) {

						transf.row[j].el[i] = multip;

					} else if (target.el[j] ==  CSD_Up) {

						transf.row[j].el[i] = -multip;

					} else {

						transf.row[j].el[i] = 0.0f;
					}
				}

			} break;

			case CSD_Forward: {	

				for (int j = 0; j < 3; ++j) {

					if (target.el[j] ==  CSD_Forward) {

						transf.row[j].el[i] = multip;

					} else if (target.el[j] ==  CSD_Back) {

						transf.row[j].el[i] = -multip;

					} else {

						transf.row[j].el[i] = 0.0f;
					}
				}

			} break;

			case CSD_Back: {	

				for (int j = 0; j < 3; ++j) {

					if (target.el[j] ==  CSD_Back) {

						transf.row[j].el[i] = multip;

					} else if (target.el[j] ==  CSD_Forward) {

						transf.row[j].el[i] = -multip;

					} else {

						transf.row[j].el[i] = 0.0f;
					}
				}

			} break;
		} 
	}
}

}