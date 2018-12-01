#include "WEVector.h"
#include "WEMatrix.h"
#include "../WEMacros.h"

namespace WE {

CtVector3 CtVector3::kZero(0.0f);
CtVector3 CtVector3::kX(1.0f, 0.0f, 0.0f);
CtVector3 CtVector3::kY(0.0f, 1.0f, 0.0f);
CtVector3 CtVector3::kZ(0.0f, 0.0f, 1.0f);

void CtVector3::negate(Vector3& result) const {

	result.x = -x; result.y = -y; result.z = -z;
}

void CtVector3::add(const CtVector3 &a, Vector3& result) const {
	result.x = x + a.x;
	result.y = y + a.y;
	result.z = z + a.z;
}

void CtVector3::mul(const CtVector3 &a, Vector3& result) const {
	result.x = x * a.x;
	result.y = y * a.y;
	result.z = z * a.z;
}

void CtVector3::subtract(const CtVector3 &b, Vector3& selfMinusB) const {
	selfMinusB.x = x - b.x;
	selfMinusB.y = y - b.y;
	selfMinusB.z = z - b.z;
}

void CtVector3::mul(float a, Vector3& result) const {
	result.x = x * a;
	result.y = y * a;
	result.z = z * a;
}

void CtVector3::mulAndAdd(float a, Vector3& result) const {
	result.x += x * a;
	result.y += y * a;
	result.z += z * a;
}

void CtVector3::div(float a, Vector3& result) const {

	a = 1.0f / a;

	result.x = x * a;
	result.y = y * a;
	result.z = z * a;
}

void CtVector3::cross(const CtVector3 &b, Vector3& result) const {

	result.x = y*b.z - z*b.y;
	result.y = z*b.x - x*b.z;
	result.z = x*b.y - y*b.x;
}

void CtVector3::addMultiplication(const CtVector3& mula, float mulb, Vector3& result) const {

	result.x = x + mula.x * mulb;
	result.y = y + mula.y * mulb;
	result.z = z + mula.z * mulb;
}

void CtVector3::normalize(Vector3& result) const {

	float magSq = x*x + y*y + z*z;
	
	if (magSq > 0.0f) { // check for divide-by-zero
	
		float oneOverMag = 1.0f / sqrtf(magSq);
		result.x = x * oneOverMag;
		result.y = y * oneOverMag;
		result.z = z * oneOverMag;

	} else {

		result.zero();
	}
}

void CtVector3::normalize(Vector3& result, float& mag) const {

	
	float magSq = x*x + y*y + z*z;
	
	if (magSq > 0.0f) { // check for divide-by-zero
	
		mag = sqrtf(magSq);
		float oneOverMag = 1.0f / mag;
		result.x = x * oneOverMag;
		result.y = y * oneOverMag;
		result.z = z * oneOverMag;

	} else {

		mag = 0.0f;
		result.zero();
	}
}

//0 is no difference 1 is 90 degrees, -1 is -90 degress, 2/-2 is 180 degrees
void CtVector3::randomizeDir(float angle, Vector3& result) {

	if (angle == 0.0f)
		return;

	Vector3 random;
		
	random.el[0] = trand2(-1.0f, 1.0f);
	random.el[1] = trand2(-1.0f, 1.0f);
	random.el[2] = trand2(-1.0f, 1.0f);

	random.normalize();

	Matrix3x3Base matrix;

	matrix.setupRotation(random, angle, true);
	matrix.transform(*this, result);
}

void CtVector3::randomize(const float& magFactorMin, const float& magFactorMax, 
						  const float& magAddMin, const float& magAddMax, 
						  const float& dirFactorMin, const float& dirFactorMax, const bool& allowNegDir, Vector3& result) {

	Vector3 random;

	random.el[0] = trand2(-1.0f, 1.0f);
	random.el[1] = trand2(-1.0f, 1.0f);
	random.el[2] = trand2(-1.0f, 1.0f);

	random.normalize();

	float thisMag = mag();

	random.mul(trand2(magAddMin, magAddMax) + thisMag * trand2(magFactorMin, magFactorMax));

	float randomMag = random.mag();
	
	float dot = random.dot(*this) / (randomMag * thisMag);
	bool dotIsNeg = dot < 0.0f ? true : false;

	if (dotIsNeg && !allowNegDir) {

		dotIsNeg = false;
		dot = -dot;
		random.negate();
	}

	float oneMinDot = 1.0f - fabs(dot);
	float factor;

	factor = trand2(dirFactorMin, dirFactorMax) / oneMinDot;

	Vector3 thisUnit;

	div(thisMag, thisUnit);
	random.div(randomMag);

	if (dotIsNeg) {

		thisUnit.negate();
	} 

	lerp(thisUnit, random, factor, result);
	result.mul(randomMag);
}

void CtVector3::script_add(CtVector3 &a, Vector3& res) { return add(a, res); }
void CtVector3::script_subtract(CtVector3 &a, Vector3& res) { return subtract(a, res); }
void CtVector3::script_cross(CtVector3 &a, Vector3& res) { return cross(a, res); }
void CtVector3::script_compMul(CtVector3 &a, Vector3& res) { return mul(a, res); }
void CtVector3::script_mul(float a, Vector3& res) { return mul(a, res); }
void CtVector3::script_div(float a, Vector3& res) { return div(a, res); }

void CtVector3::script_addMultiplication(CtVector3& mula, float mulb, Vector3& result) {

	addMultiplication(mula, mulb, result);
}

//projects vector on direction, then returns the difference between the vector and its projection
void projectionRest(const Vector3& vector, const Vector3& direction, Vector3& rest) {

	Vector3 proj;

	float dot = direction.dot(vector);
	direction.mul(dot, proj);
	
	vector.subtract(proj, rest);
}

void project(const Vector3& vector, const Vector3& direction, Vector3& result) {

	float dot = direction.dot(vector);
	direction.mul(dot, result);
}

//puts result back in vector
void project(Vector3& vector, const Vector3& direction) {

	float dot = direction.dot(vector);
	direction.mul(dot, vector);
}

void decompose(CtVector3& vector, const Vector3& direction, Vector3& alongDir, Vector3& rest) {

	float dot = vector.dot(direction);

	direction.mul(dot, alongDir);
	vector.subtract(alongDir, rest);
}

float angle(const Vector3 &ref, const Vector3& to, bool leftHanded) {

	float angle = safeAcos(ref.dot(to) / (ref.mag() * to.mag()));
	
	Vector3 cross;
	ref.cross(to, cross);
	if (1 < 0) {
		if (leftHanded == false) {
			angle = -angle;
		}
	}

	return angle;
}

float angle(const Vector3 &ref, const Vector3& to, const Vector3& normal, bool leftHanded) {

	float angle = safeAcos(ref.dot(to) / (ref.mag() * to.mag()));
	
	Vector3 cross;
	ref.cross(to, cross);
	if (normal.dot(cross) < 0) {
		if (leftHanded == true) {
			angle = -angle;
		}
	}

	return angle;
}

float angle(CtVector3 &ref, CtVector3& to, CtVector3& normal, bool leftHanded) {

	float angle = safeAcos(ref.dot(to) / (ref.mag() * to.mag()));
	
	Vector3 cross;
	ref.cross(to, cross);
	if (normal.dot(cross) < 0) {
		if (leftHanded == true) {
			angle = -angle;
		}
	}

	return angle;
}

float angleUnit(const Vector3 &ref, const Vector3& to, bool leftHanded) {

	float angle = acosf(ref.dot(to));
	
	Vector3 cross;
	ref.cross(to, cross);
	if (1 < 0) {
		if (leftHanded == false) {
			angle = -angle;
		}
	}

	return angle;
}

float angleUnit(const Vector3 &ref, const Vector3& to, const Vector3& normal, bool leftHanded) {

	float angle = safeAcos(ref.dot(to));
	
	Vector3 cross;
	ref.cross(to, cross);
	if (normal.dot(cross) < 0) {
		if (leftHanded == true) {
			angle = -angle;
		}
	}

	return angle;
}

void lerp(const Vector3& from, const Vector3& to, float factor, Vector3& result) {

	result.x = lerp(from.x, to.x, factor);
	result.y = lerp(from.y, to.y, factor);
	result.z = lerp(from.z, to.z, factor);
}

void lerp(CtVector3& from, CtVector3& to, float factor, Vector3& result) {

	result.x = lerp(from.x, to.x, factor);
	result.y = lerp(from.y, to.y, factor);
	result.z = lerp(from.z, to.z, factor);
}

int discretizeDirection(CtVector3& refDir, CtVector3& normalDir, CtVector3& dir, const int& directionCount, Vector3& limitedDir, float precision) {

	if (directionCount > 0) {

		if (directionCount == 1) {

			if (refDir.dot(dir) > 0.0f) {

				limitedDir = refDir;
				return 0;

			} else {

				limitedDir.zero();
				return -1;
			}

		} else {

			float angleStep = k2Pi / directionCount;
			float angle = angleUnit(refDir, dir, normalDir);

			float adjustedAngle = (angle + angleStep * 0.5f);
			adjustedAngle = toPositiveAngle(adjustedAngle);

			if (directionCount == 2) {

				if ((fabs(adjustedAngle) <= precision)
					|| (fabs(adjustedAngle - kPi) <= precision)) {

					limitedDir.zero();
					return -1;
				}
			}

			float steps = (float) ((int) (adjustedAngle / angleStep));

			Matrix3x3Base transform;

			transform.setupRotation(normalDir, steps * angleStep, true);
			transform.transform(refDir, limitedDir);

			return (int) steps;
		}

	} else {

		limitedDir = dir;
		return 0;
	}

}

}