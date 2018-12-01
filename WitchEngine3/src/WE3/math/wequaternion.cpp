#include "WEQuaternion.h"

#include "../WEAssert.h"
#include "WEMathUtil.h"

namespace WE {

const Quaternion Quaternion::kIdentity(true);


void Quaternion::identity() {

	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

void Quaternion::derivativeFromAngularVelocity(const Vector3& w) {

	float mag;
	Vector3 axis = w;

	axis.normalize(mag);

	derivativeFromAngularVelocity(axis, mag);
}

void Quaternion::derivativeFromAngularVelocity(const Vector3& normalizedAxis, float velocity) {

	w = 0.0f;
	x = normalizedAxis.x * velocity;
	y = normalizedAxis.y * velocity;
	z = normalizedAxis.z * velocity;
}

void Quaternion::derivativeToAngularVelocity(Vector3& normalizedAxis, float& velocity) {

	normalizedAxis.x = x;
	normalizedAxis.y = y;
	normalizedAxis.z = z;

	normalizedAxis.normalize(velocity);
}


void Quaternion::fromAxisAngle(const Vector3& axis, float angle, bool isNormalized) {

	const Vector3* pAxis;
	Vector3 normalizedAxis;

	if (isNormalized) {

		pAxis = &axis;

	} else {

		axis.normalize(normalizedAxis);
		pAxis = &normalizedAxis;
	}

	// The axis of rotation must be normalized
	//assrt(fabs(normalizedAxis.mag() - 1.0f) < .01f);

	// Compute the half angle and its sin

	float   thetaOver2 = angle * .5f;
	float   sinThetaOver2 = sin(thetaOver2);

	// Set the values

	w = cos(thetaOver2);
	x = pAxis->x * sinThetaOver2;
	y = pAxis->y * sinThetaOver2;
	z = pAxis->z * sinThetaOver2;
}

void Quaternion::toAxisAngle(Vector3& normalizedAxis, float& angle) const {

	float thetaOver2 = safeAcos(w);

	angle = thetaOver2 * 2.0f;

	// Compute sin^2(theta/2).  Remember that w = cos(theta/2),
	// and sin^2(x) + cos^2(x) = 1

	float sinThetaOver2Sq = 1.0f - w*w;

	// Protect against numerical imprecision

	if (sinThetaOver2Sq <= 0.0f) {

		// Identity quaternion, or numerical imprecision.  Just
		// return any valid vector, since it doesn't matter

		normalizedAxis.zero();
	}

	// Compute 1 / sin(theta/2)

	float   oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

	// Return axis of rotation

	normalizedAxis.x = x * oneOverSinThetaOver2;
	normalizedAxis.y = y * oneOverSinThetaOver2;
	normalizedAxis.z = z * oneOverSinThetaOver2;
}

void Quaternion::mul(float scale) {

	x *= scale;
    y *= scale;
    z *= scale;
    w *= scale;
}

void Quaternion::mul(float scale, Quaternion& result) const {

	result.x = x * scale;
    result.y = y * scale;
    result.z = z * scale;
    result.w = w * scale;
}

void Quaternion::mulAndAdd(float scale, Quaternion& result) const {

	result.x += x * scale;
    result.y += y * scale;
    result.z += z * scale;
    result.w += w * scale;
}

void Quaternion::normalize() {

	// Compute magnitude of the quaternion

	float   mag = (float) sqrt(w*w + x*x + y*y + z*z);

	// Check for bogus length, to protect against divide by zero

	if (mag > 0.0f) {

		// Normalize it

		float   oneOverMag = 1.0f / mag;
		w *= oneOverMag;
		x *= oneOverMag;
		y *= oneOverMag;
		z *= oneOverMag;

	} else {

		assrt(false);
		identity();
	}
}

/*
void Quaternion::toMatrix(Matrix3x3Base& mat, bool isNormalized) {

	if (isNormalized == false) {
		normalize();
	}

	float sqw = w*w;
    float sqx = x*x;
    float sqy = y*y;
    float sqz = z*z;

    mat.m11 =  sqx - sqy - sqz + sqw; // since sqw + sqx + sqy + sqz =1
    mat.m22 = -sqx + sqy - sqz + sqw;
    mat.m33 = -sqx - sqy + sqz + sqw;
    
    float tmp1 = x*y;
    float tmp2 = z*w;
    mat.m21 = 2.0f * (tmp1 + tmp2);
    mat.m12 = 2.0f * (tmp1 - tmp2);
    
    tmp1 = x*z;
    tmp2 = y*w;
    mat.m31 = 2.0f * (tmp1 - tmp2);
    mat.m13 = 2.0f * (tmp1 + tmp2);
    tmp1 = y*z;
    tmp2 = x*w;
	mat. m32 = 2.0f * (tmp1 + tmp2);
    mat.m23 = 2.0f * (tmp1 - tmp2);   

}

void Quaternion::fromMatrix(const Matrix3x3Base& mat) {

	float trace = mat.m11 + mat.m22 + mat.m33 + 1.0f;

	if( trace > 0.0f ) {
		float s = 0.5f / sqrtf(trace);
		w = 0.25f / s;
		x = ( mat.m32 - mat.m23 ) * s;
		y = ( mat.m13 - mat.m31 ) * s;
		z = ( mat.m21 - mat.m12 ) * s;
	} else {
		if ( mat.m11 > mat.m22 && mat.m11 > mat.m33 ) {
			float s = 2.0f * sqrtf( 1.0f + mat.m11 - mat.m22 - mat.m33);
			x = 0.25f * s;
			y = (mat.m12 + mat.m21 ) / s;
			z = (mat.m13 + mat.m31 ) / s;
			w = (mat.m23 - mat.m32 ) / s;

		} else if (mat.m22 > mat.m33) {
			float s = 2.0f * sqrtf( 1.0f + mat.m22 - mat.m11 - mat.m33);
			x = (mat.m12 + mat.m21 ) / s;
			y = 0.25f * s;
			z = (mat.m23 + mat.m32 ) / s;
			w = (mat.m13 - mat.m31 ) / s;
		} else {
			float s = 2.0f * sqrtf( 1.0f + mat.m33 - mat.m11 - mat.m22 );
			x = (mat.m13 + mat.m31 ) / s;
			y = (mat.m23 + mat.m32 ) / s;
			z = 0.25f * s;
			w = (mat.m12 - mat.m21 ) / s;
		}
	}
}
*/

void Quaternion::toMatrix(Matrix3x3Base& mat) const {

	float sqw = w*w;
    float sqx = x*x;
    float sqy = y*y;
    float sqz = z*z;

    mat.m11 =  sqx - sqy - sqz + sqw; // since sqw + sqx + sqy + sqz =1
    mat.m22 = -sqx + sqy - sqz + sqw;
    mat.m33 = -sqx - sqy + sqz + sqw;
    
    float tmp1 = x*y;
    float tmp2 = z*w;
    mat.m12 = 2.0f * (tmp1 + tmp2);
    mat.m21 = 2.0f * (tmp1 - tmp2);
    
    tmp1 = x*z;
    tmp2 = y*w;
    mat.m13 = 2.0f * (tmp1 - tmp2);
    mat.m31 = 2.0f * (tmp1 + tmp2);
    tmp1 = y*z;
    tmp2 = x*w;
	mat. m23 = 2.0f * (tmp1 + tmp2);
    mat.m32 = 2.0f * (tmp1 - tmp2);   

}

void Quaternion::toMatrix(Matrix3x3Base& mat, bool isNormalized) {

	if (isNormalized == false) {
		normalize();
	}

	float sqw = w*w;
    float sqx = x*x;
    float sqy = y*y;
    float sqz = z*z;

    mat.m11 =  sqx - sqy - sqz + sqw; // since sqw + sqx + sqy + sqz =1
    mat.m22 = -sqx + sqy - sqz + sqw;
    mat.m33 = -sqx - sqy + sqz + sqw;
    
    float tmp1 = x*y;
    float tmp2 = z*w;
    mat.m12 = 2.0f * (tmp1 + tmp2);
    mat.m21 = 2.0f * (tmp1 - tmp2);
    
    tmp1 = x*z;
    tmp2 = y*w;
    mat.m13 = 2.0f * (tmp1 - tmp2);
    mat.m31 = 2.0f * (tmp1 + tmp2);
    tmp1 = y*z;
    tmp2 = x*w;
	mat. m23 = 2.0f * (tmp1 + tmp2);
    mat.m32 = 2.0f * (tmp1 - tmp2);   

}

void Quaternion::fromMatrix(const Matrix3x3Base& mat) {

	float trace = mat.m11 + mat.m22 + mat.m33 + 1.0f;

	if( trace > 0.0f ) {
		float s = 0.5f / sqrtf(trace);
		w = 0.25f / s;
		x = ( mat.m23 - mat.m32 ) * s;
		y = ( mat.m31 - mat.m13 ) * s;
		z = ( mat.m12 - mat.m21 ) * s;
	} else {
		if ( mat.m11 > mat.m22 && mat.m11 > mat.m33 ) {
			float s = 2.0f * sqrtf( 1.0f + mat.m11 - mat.m22 - mat.m33);
			x = 0.25f * s;
			y = (mat.m21 + mat.m12 ) / s;
			z = (mat.m31 + mat.m13 ) / s;
			w = (mat.m32 - mat.m23 ) / s;

		} else if (mat.m22 > mat.m33) {
			float s = 2.0f * sqrtf( 1.0f + mat.m22 - mat.m11 - mat.m33);
			x = (mat.m21 + mat.m12 ) / s;
			y = 0.25f * s;
			z = (mat.m32 + mat.m23 ) / s;
			w = (mat.m31 - mat.m13 ) / s;
		} else {
			float s = 2.0f * sqrtf( 1.0f + mat.m33 - mat.m11 - mat.m22 );
			x = (mat.m31 + mat.m13 ) / s;
			y = (mat.m32 + mat.m23 ) / s;
			z = 0.25f * s;
			w = (mat.m21 - mat.m12 ) / s;
		}
	}
}


void Quaternion::add(const Quaternion& q) {

	w = w + q.w;
	x = x + q.x;
	y = y + q.y;
	z = z + q.z;
}

void mul(const Quaternion& a, const Quaternion& b, Quaternion& axb) {

	/*
	(Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
	(Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
	(Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
	(Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2
	*/

	axb.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	axb.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	axb.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	axb.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
}

void mul(const Vector& a, const Quaternion& b, Quaternion& axb) {
	

	axb.w = 0.0f - a.x * b.x - a.y * b.y - a.z * b.z;
	axb.x = 0.0f + a.x * b.w + a.y * b.z - a.z * b.y;
	axb.y = 0.0f - a.x * b.z + a.y * b.w + a.z * b.x;
	axb.z = 0.0f + a.x * b.y - a.y * b.x + a.z * b.w;
}	

float dot(const Quaternion &a, const Quaternion &b) {
	return a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
}

void slerp(const Quaternion& a, const Quaternion& b, float t, Quaternion& result) {

  float w1, w2;

  float cosTheta = dot(a, b);
  float theta    = (float) acos(cosTheta);
  float sinTheta = (float) sin(theta);

  if( sinTheta > 0.001f ) {

	w1 = float( sin( (1.0f-t)*theta ) / sinTheta);
	w2 = float( sin( t*theta) / sinTheta);

  } else {

	// CQuat a ~= CQuat b
	w1 = 1.0f - t;
	w2 = t;
  }

  a.mul(w1, result);
  b.mulAndAdd(w2, result);
}

void nlerp(const Quaternion& a, const Quaternion& b, float t, Quaternion& result) {

	a.mul(1.0f - t, result);
	b.mulAndAdd(t, result);

	result.normalize();
}

}