#include "WEMatrix2.h"

#include "../WEMacros.h"
#include "../WEAssert.h"
#include "WEMathUtil.h"

namespace WE {

Matrix2x2::Matrix2x2() {
}

Matrix2x2::Matrix2x2(bool setIdentity) {

	if (setIdentity) {

		identity();
	}
}

void diagonal(Matrix2x2& mat, float value) {

	mat.m11 = value; mat.m12 = 0.0f;
	mat.m21 = 0.0f; mat.m22 = value;
}

void Matrix2x2::identity() {

	WE::diagonal(*this, 1.0f);
}

void Matrix2x2::diagonal(float value) {

	WE::diagonal(*this, value);
}


void Matrix2x2::transpose() {

	float temp;

	swapt<float>(m12, m21, temp); 
}

void Matrix2x2::transpose(Matrix2x2& result) {


	result.m11 = m11;
	result.m12 = m21;
	
	result.m21 = m12;
	result.m22 = m22;
}

void Matrix2x2::scale(float multiplier) {

	row[0].mul(multiplier);
	row[1].mul(multiplier);
}


void mul(const Matrix2x2 &a, const Matrix2x2 &b, Matrix2x2 &r) {

	// Compute the upper 3x3 (linear transformation) portion

	r.m11 = a.m11*b.m11 + a.m12*b.m21;
	r.m12 = a.m11*b.m12 + a.m12*b.m22;

	r.m21 = a.m21*b.m11 + a.m22*b.m21;
	r.m22 = a.m21*b.m12 + a.m22*b.m22;
}

void mul(const Matrix2x2& a, const Matrix2x2& b, Matrix2x2& r, bool transFirst, bool transSecond) {

	if (transFirst) {

		if (transSecond) {

			//TODO
			assrt(false);

		} else {

			//TODO
			assrt(false);
		}

	} else {

		if (transSecond) {

			r.m11 = a.m11*b.m11 + a.m12*b.m12;
			r.m12 = a.m11*b.m21 + a.m12*b.m22;
		
			r.m21 = a.m21*b.m11 + a.m22*b.m12;
			r.m22 = a.m21*b.m21 + a.m22*b.m22;
		
		} else {

			r.m11 = a.m11*b.m11 + a.m12*b.m21;
			r.m12 = a.m11*b.m12 + a.m12*b.m22;
	
			r.m21 = a.m21*b.m11 + a.m22*b.m21;
			r.m22 = a.m21*b.m12 + a.m22*b.m22;
		}

	}
	
}

void Matrix2x2::setupRotation(float theta, bool leftHanded, bool inverse) {

	float	s, c;
	sinCos(&s, &c, theta, leftHanded);

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...

	if (inverse == false) {

		m11 = c;
		m12 = -s;
	
		m21 = s;
		m22 = c;
		
	} else {

		m11 = -c;
		m21 = s;
	
		m12 = -s;
		m22 = -c;
	}
}

void Matrix2x2::moveOrientation(Matrix2x2& rot) {

	Matrix2x2 temp = *this;
	
	mul(temp, rot, *this);
}

void mul(const Matrix2x2& mat, const Vector2 &v, Vector2& result, bool inverse) {

	if (inverse == false) {
		
		result.x  = mat.m11*v.x + mat.m21*v.y;
		result.y = mat.m12*v.x + mat.m22*v.y;
	
	} else {

		result.x = mat.m11*v.x + mat.m12*v.y;
		result.y = mat.m21*v.x + mat.m22*v.y;
	}
}

void Matrix2x2::transform(const Vector2& vector, Vector2& result, bool inverse) const {

	mul(*this, vector, result, inverse);
}

void Matrix2x2::transform(Vector2& vector, bool inverse) const {

	WE::Vector2 temp = vector;

	mul(*this, temp, vector, inverse);
}



void identity(Matrix3x2& mat, bool rot, bool trans) {

	if (rot) {
		mat.m11 = 1.0f; mat.m12 = 0.0f; 
		mat.m21 = 0.0f; mat.m22 = 1.0f; 
	}

	if (trans) {
		mat.tx = mat.ty = 0.0f;
	}
}

inline float _mulDimNoTrans(const float &p, const Matrix3x2 &m, short dim) {

	return p * m.row[dim].x + p * m.row[dim].y;
}

inline float _mulDim(const float &p, const Matrix3x2 &m, short dim) {

	return p * m.row[dim].x + p * m.row[dim].y + m.row[2].el[dim];
}

void mul(const Matrix3x2 &a, const Matrix3x2 &b, Matrix3x2 &r) {

	// Compute the upper 3x3 (linear transformation) portion

	r.m11 = a.m11*b.m11 + a.m12*b.m21;
	r.m12 = a.m11*b.m12 + a.m12*b.m22;

	r.m21 = a.m21*b.m11 + a.m22*b.m21;
	r.m22 = a.m21*b.m12 + a.m22*b.m22;

	// Compute the translation portion

	r.tx = a.tx*b.m11 + a.ty*b.m21 + b.tx;
	r.ty = a.tx*b.m12 + a.ty*b.m22 + b.ty;
}


void mul(const Vector2 &p, const Matrix3x2 &m, Vector2& result) {

	// Grind through the linear algebra.
	result.x = p.x*m.m11 + p.y*m.m21 + m.tx;
	result.y = p.x*m.m12 + p.y*m.m22 + m.ty;
}



void mulNoTrans(const Vector2 &p, const Matrix3x2 &m, Vector2& result) {

	// Grind through the linear algebra.
	result.x = p.x*m.m11 + p.y*m.m21;
	result.y = p.x*m.m12 + p.y*m.m22;
}


Matrix3x2::Matrix3x2() {
}

Matrix3x2::Matrix3x2(bool identity) {

	WE::identity(*this, true, true);
}

Matrix2x2& Matrix3x2::castToMatrix2x2() {
	return *((Matrix2x2*) ((Matrix3x2*) this));
}

const Matrix2x2& Matrix3x2::castToCtMatrix2x2() const {
	return *((Matrix2x2*) ((Matrix3x2*) this));
}

void Matrix3x2::copy3x3BaseTo(Matrix2x2& mat) const {

	memcpy(mat.row, row, sizeof(mat));
}

void Matrix3x2::simpleInvTransformVector(const Vector2& vector, Vector2& result) const {

	castToCtMatrix2x2().transform(vector, result, true);
}


void Matrix3x2::invTransformVector(const Vector2& vector, Vector2& result) const {

	Matrix3x2 inv;

	inverse(*this, inv);

	inv.transformVector(vector, result);
}

void Matrix3x2::invTransformPoint(const Vector2& point, Vector2& result) const {

	Matrix3x2 inv;

	inverse(*this, inv);

	inv.transformPoint(point, result);

	Vector2 temp1 = result;
	Vector2 temp2;

	transformPoint(temp1, temp2);
}

void Matrix3x2::transformPoint(const Vector2& point, Vector2& result) const {

	mul(point, *this, result);
}

void Matrix3x2::transformPointDim(const float& point, short dim, float& result) const {

	result = _mulDim(point, *this, dim);
}

float Matrix3x2::transformPointDim(const float& point, short dim) const {

	return _mulDim(point, *this, dim);
}

void Matrix3x2::transformVector(const Vector2& vector, Vector2& result) const {

	mulNoTrans(vector, *this, result);
}

void Matrix3x2::transformVectorDim(const float& vector, short dim, float& result) const {

	result = _mulDimNoTrans(vector, *this, dim);
}

float Matrix3x2::transformVectorDim(const float& vector, short dim) const {

	return _mulDimNoTrans(vector, *this, dim);
}

void Matrix3x2::transformPoint(Vector2& point) const {
	Vector2 temp = point;

	mul(temp, *this, point);
}

void Matrix3x2::transformVector(Vector2& vector) const {
	Vector2 temp = vector;

	mulNoTrans(temp, *this, vector);
}

void Matrix3x2::identity() {

	WE::identity(*this, true, true);
}


void RigidMatrix3x2::zeroTranslation() {
	
	WE::identity(*this, false, true);
}


void RigidMatrix3x2::setTranslation(const Vector2 &d) {

	tx = d.x; ty = d.y; 
}



void RigidMatrix3x2::setupTranslation(const Vector2 &d) {

	WE::identity(*this, true, false);

	// Set the translation portion
	tx = d.x; ty = d.y;
}


void _setRotate(RigidMatrix3x2& mat, const Vector2 &eulerAngles, bool leftHanded, bool setToTranspose) {

	// Fetch sine and cosine of angles
	float	sh,ch, sp,cp, sb,cb;
	sinCos(&sh, &ch, eulerAngles.el[0], leftHanded);
	sinCos(&sp, &cp, eulerAngles.el[1], leftHanded);
	sinCos(&sb, &cb, eulerAngles.el[2], leftHanded);

	// Fill in the matrix elements
	if (setToTranspose == false) {

		mat.m11 = ch * cb + sh * sp * sb;
		mat.m12 = -ch * sb + sh * sp * cb;
	
		mat.m21 = sb * cp;
		mat.m22 = cb * cp;
	

	} else {

		mat.m11 = ch * cb + sh * sp * sb;
		mat.m21 = -ch * sb + sh * sp * cb;
	
		mat.m12 = sb * cp;
		mat.m22 = cb * cp;
	}

}

inline void _setRotate(RigidMatrix3x2& mat, RigidMatrix3x2::CardinalAxis axis, float theta, bool leftHanded, bool setToTranspose) {

	// Get sin and cosine of rotation angle

	float	s, c;
	sinCos(&s, &c, theta, leftHanded);

	// Check which axis they are rotating about

	switch (axis) {

		case RigidMatrix3x2::Axis_X: // Rotate about the x-axis
			
			if (setToTranspose == false) {
				mat.m11 = c; mat.m12 = -s; 
				mat.m21 = s; mat.m22 = c; 
			} else {
				mat.m11 = c; mat.m21 = s; 
				mat.m12 = -s; mat.m22 = c;
			}
			break;

		default: //case RigidMatrix3x2::Axis_Y:  Rotate about the y-axis

			if (setToTranspose == false) {
				mat.m11 = s;    mat.m12 = -c;
				mat.m21 = c; mat.m22 = s;
			} else {
				mat.m11 = s;    mat.m21 = c;
				mat.m12 = -c; mat.m22 = s; 
			}
			break;
	}
}


void _setRotate(RigidMatrix3x2& mat, const Vector2& _axis, float theta, bool isNormalized, bool leftHanded, bool setToTranspose) {

	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	//assrt(fabs(axis*axis - 1.0f) < .01f);

	// Get sin and cosine of rotation angle

	Vector2 axis = _axis;

	if (isNormalized == false) {
		axis.normalize();
	}

	float	s, c;
	sinCos(&s, &c, theta, leftHanded);

	// Compute 1 - cos(theta) and some common subexpressions

	float	a = 1.0f - c;
	float	ax = a * axis.x;
	float	ay = a * axis.y;
	
	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...

	if (setToTranspose == false) {

		mat.m11 = ax*axis.x + c;
		mat.m12 = ax*axis.y + s;
	
		mat.m21 = ay*axis.x - s;
		mat.m22 = ay*axis.y + c;
	

	} else {

		mat.m11 = ax*axis.x + c;
		mat.m21 = ax*axis.y + s;
		
		mat.m12 = ay*axis.x - s;
		mat.m22 = ay*axis.y + c;
	}
}



void RigidMatrix3x2::zeroRotation() {

	WE::identity(*this, true, false);
}


void RigidMatrix3x2::setRotate(CardinalAxis axis, float theta, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, axis, theta, leftHanded, setToTranspose);
}

void RigidMatrix3x2::setRotate(const Vector2 &axis, float theta, bool isNormalized, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, axis, theta, isNormalized, leftHanded, setToTranspose);
}

void RigidMatrix3x2::setRotate(const Vector2 &eulerAngles, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, eulerAngles, leftHanded, setToTranspose);
}

void RigidMatrix3x2::setupRotation(CardinalAxis axis, float theta, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, axis, theta, leftHanded, setToTranspose);
	WE::identity(*this, false, true);
}

void RigidMatrix3x2::setupRotation(const Vector2 &axis, float theta, bool isNormalized, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, axis, theta, isNormalized, leftHanded, setToTranspose);
	WE::identity(*this, false, true);
}

void RigidMatrix3x2::setupRotation(const Vector2 &eulerAngles, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, eulerAngles, leftHanded, setToTranspose);
	WE::identity(*this, false, true);
}

void Matrix3x2::__setScale(const Vector2 &s) {

	// Set the matrix elements.  Pretty straightforward
	m11 *= s.x;  
	m22 *= s.y;
}

void Matrix3x2::__setupScale(const Vector2 &s) {

	// Set the matrix elements.  Pretty straightforward
	m11 = s.x;  m12 = 0.0f; 
	m21 = 0.0f; m22 = s.y; 
	
	// Reset the translation portion
	WE::identity(*this, false, true);
}

float determinant(const Matrix3x2 &m) {

	return
		  m.m11 * (m.m22)
		+ m.m12 * (- m.m21);
}

void inverse(const Matrix3x2 &m, Matrix3x2 &r) {

	// Compute the determinant

	float	det = determinant(m);

	// If we're singular, then the determinant is zero and there's
	// no inverse
	assrt(fabs(det) > 0.000001f);


	// Compute one over the determinant, so we divide once and
	// can *multiply* per element

	float	oneOverDet = 1.0f / det;

	// Compute the 2x2 portion of the inverse, by
	// dividing the adjoint by the determinant

	r.m11 = (m.m22 ) * oneOverDet;
	r.m12 = (- m.m12) * oneOverDet;
	
	r.m21 = (- m.m21) * oneOverDet;
	r.m22 = (m.m11) * oneOverDet;

	// Compute the translation portion of the inverse

	r.tx = -(m.tx*r.m11 + m.ty*r.m21);
	r.ty = -(m.tx*r.m12 + m.ty*r.m22);
}


}