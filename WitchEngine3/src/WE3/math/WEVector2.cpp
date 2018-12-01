#include "WEVector2.h"
#include "../WEMacros.h"

namespace WE {

CtVector2 CtVector2::kZero(0.0f);
CtVector2 CtVector2::kX(1.0f, 0.0f);
CtVector2 CtVector2::kY(0.0f, 1.0f);


void CtVector2::negate(Vector2& result) const {

	result.x = -x; result.y = -y;
}

void CtVector2::add(const CtVector2 &a, Vector2& result) const {
	result.x = x + a.x;
	result.y = y + a.y;
}

void CtVector2::mul(const CtVector2 &a, Vector2& result) const {
	result.x = x * a.x;
	result.y = y * a.y;
}

void CtVector2::subtract(const CtVector2 &b, Vector2& selfMinusB) const {
	selfMinusB.x = x - b.x;
	selfMinusB.y = y - b.y;
}

void CtVector2::mul(float a, Vector2& result) const {
	result.x = x * a;
	result.y = y * a;
}

void CtVector2::mulAndAdd(float a, Vector2& result) const {
	result.x += x * a;
	result.y += y * a;
}

void CtVector2::div(float a, Vector2& result) const {

	a = 1.0f / a;

	result.x = x * a;
	result.y = y * a;
}

void CtVector2::normalize(Vector2& result) const {

	float magSq = x*x + y*y;
	
	if (magSq > 0.0f) { // check for divide-by-zero
	
		float oneOverMag = 1.0f / sqrtf(magSq);
		result.x = x * oneOverMag;
		result.y = y * oneOverMag;
	
	} else {

		result.zero();
	}
}

void CtVector2::normalize(Vector2& result, float& mag) const {

	
	float magSq = x*x + y*y;
	
	if (magSq > 0.0f) { // check for divide-by-zero
	
		mag = sqrtf(magSq);
		float oneOverMag = 1.0f / mag;
		result.x = x * oneOverMag;
		result.y = y * oneOverMag;

	} else {

		mag = 0.0f;
		result.zero();
	}
}

void CtVector2::addMultiplication(const CtVector2& mula, float mulb, Vector2& result) const {

	result.x = x + mula.x * mulb;
	result.y = y + mula.y * mulb;
}


void CtVector2::randomize(const float& magFactorMin, const float& magFactorMax, 
						  const float& magAddMin, const float& magAddMax, 
						  const float& dirFactorMin, const float& dirFactorMax, const bool& allowNegDir, Vector2& result) {

	Vector2 random;

	random.el[0] = trand2(-1.0f, 1.0f);
	random.el[1] = trand2(-1.0f, 1.0f);
	
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

	Vector2 thisUnit;

	div(thisMag, thisUnit);
	random.div(randomMag);

	if (dotIsNeg) {

		thisUnit.negate();
	} 

	lerp(thisUnit, random, factor, result);
	result.mul(randomMag);
}


void CtVector2::script_add(CtVector2 &a, Vector2& res) { return add(a, res); }
void CtVector2::script_subtract(CtVector2 &a, Vector2& res) { return subtract(a, res); }
void CtVector2::script_compMul(CtVector2 &a, Vector2& res) { return mul(a, res); }
void CtVector2::script_mul(float a, Vector2& res) { return mul(a, res); }
void CtVector2::script_div(float a, Vector2& res) { return div(a, res); }

void CtVector2::script_addMultiplication(CtVector2& mula, float mulb, Vector2& result) {

	addMultiplication(mula, mulb, result);
}

//projects vector on direction, then returns the difference between the vector and its projection
void projectionRest(const CtVector2& vector, const CtVector2& direction, Vector2& rest) {

	Vector2 proj;

	float dot = direction.dot(vector);
	direction.mul(dot, proj);
	
	vector.subtract(proj, rest);
}

void project(const Vector2& vector, const Vector2& direction, Vector2& result) {

	float dot = direction.dot(vector);
	direction.mul(dot, result);
}

//puts result back in vector
void project(Vector2& vector, const Vector2& direction) {

	float dot = direction.dot(vector);
	direction.mul(dot, vector);
}

void decompose(CtVector2& vector, const Vector2& direction, Vector2& alongDir, Vector2& rest) {

	float dot = vector.dot(direction);

	direction.mul(dot, alongDir);
	vector.subtract(alongDir, rest);
}

float angle(const Vector2 &ref, const Vector2& to) {

	float angle = atan2f(to.y - ref.y, to.x - ref.x);

	return angle;
}


void lerp(const Vector2& from, const Vector2& to, float factor, Vector2& result) {

	result.x = lerp(from.x, to.x, factor);
	result.y = lerp(from.y, to.y, factor);
}

void lerp(CtVector2& from, CtVector2& to, float factor, Vector2& result) {

	result.x = lerp(from.x, to.x, factor);
	result.y = lerp(from.y, to.y, factor);
}

}
