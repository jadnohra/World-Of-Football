#include "WEMatrix.h"

#include "memory.h"
#include "../WEMacros.h"
#include "../WEAssert.h"

#include "WEPlane.h"
#include "WEMathUtil.h"

namespace WE {

const Matrix4x3Base Matrix4x3Base::kIdentity(true);


Matrix3x3Base::Matrix3x3Base() {
}

Matrix3x3Base::Matrix3x3Base(bool setIdentity) {

	if (setIdentity) {

		identity();
	}
}

void diagonal(Matrix3x3Base& mat, float value) {

	mat.m11 = value; mat.m12 = 0.0f; mat.m13 = 0.0f;
	mat.m21 = 0.0f; mat.m22 = value; mat.m23 = 0.0f;
	mat.m31 = 0.0f; mat.m32 = 0.0f; mat.m33 = value;
}

void Matrix3x3Base::identity() {

	WE::diagonal(*this, 1.0f);
}

void Matrix3x3Base::diagonal(float value) {

	WE::diagonal(*this, value);
}


void Matrix3x3Base::transpose() {

	float temp;

	swapt<float>(m12, m21, temp); 
	swapt<float>(m13, m31, temp); 
	swapt<float>(m23, m32, temp); 
}

void Matrix3x3Base::transpose(Matrix3x3Base& result) {


	result.m11 = m11;
	result.m12 = m21;
	result.m13 = m31;

	result.m21 = m12;
	result.m22 = m22;
	result.m23 = m32;

	result.m31 = m13;
	result.m32 = m23;
	result.m33 = m33;
}

void Matrix3x3Base::scale(float multiplier) {

	row[0].mul(multiplier);
	row[1].mul(multiplier);
	row[2].mul(multiplier);
}


void Matrix3x3Base::inverse(Matrix3x3Base& result) {

	//shamelessly stolen from OpenTissue
	//From messer p.283 we know
    //
    //  -1     1
    // A   = -----  adj A
    //       det A
    //
    //                      i+j
    // ij-cofactor of A = -1    det A
    //                               ij
    //
    // i,j entry of the adjoint.
    //                                   i+j
    // adjoint A   = ji-cofactor = A = -1    det A
    //          ij                                ji
    //
    // As it can be seen the only numerical error
    // in these calculations is from the resolution
    // of the scalars. So it is a very accurate method.

	Matrix3x3Base& A = *this;
	Matrix3x3Base adj;
    float det = 0;

    adj(0,0) = A(1,1)*A(2,2) - A(2,1)*A(1,2);
    adj(1,1) = A(0,0)*A(2,2) - A(2,0)*A(0,2);
    adj(2,2) = A(0,0)*A(1,1) - A(1,0)*A(0,1);
    adj(0,1) = A(1,0)*A(2,2) - A(2,0)*A(1,2);
    adj(0,2) = A(1,0)*A(2,1) - A(2,0)*A(1,1);
    adj(1,0) = A(0,1)*A(2,2) - A(2,1)*A(0,2);
    adj(1,2) = A(0,0)*A(2,1) - A(2,0)*A(0,1);
    adj(2,0) = A(0,1)*A(1,2) - A(1,1)*A(0,2);
    adj(2,1) = A(0,0)*A(1,2) - A(1,0)*A(0,2);
    det = A(0,0)*adj(0,0) -  A(0,1)*adj(0,1) +   A(0,2)*adj(0,2);

	
	if(det)
    {
      adj(0,1) *= -1;
      adj(1,0) *= -1;
      adj(1,2) *= -1;
      adj(2,1) *= -1;

	  adj.transpose(result);
	  result.scale(1.0f / det);
	
	  return;
    }

	result.identity();
}

void mul(const Matrix3x3Base &a, const Matrix3x3Base &b, Matrix3x3Base &r) {

	// Compute the upper 3x3 (linear transformation) portion

	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;

	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;

	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;

}

void mul(const Matrix3x3Base& a, const Matrix3x3Base& b, Matrix3x3Base& r, bool transFirst, bool transSecond) {

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

			r.m11 = a.m11*b.m11 + a.m12*b.m12 + a.m13*b.m13;
			r.m12 = a.m11*b.m21 + a.m12*b.m22 + a.m13*b.m23;
			r.m13 = a.m11*b.m31 + a.m12*b.m32 + a.m13*b.m33;

			r.m21 = a.m21*b.m11 + a.m22*b.m12 + a.m23*b.m13;
			r.m22 = a.m21*b.m21 + a.m22*b.m22 + a.m23*b.m23;
			r.m23 = a.m21*b.m31 + a.m22*b.m32 + a.m23*b.m33;

			r.m31 = a.m31*b.m11 + a.m32*b.m12 + a.m33*b.m13;
			r.m32 = a.m31*b.m21 + a.m32*b.m22 + a.m33*b.m23;
			r.m33 = a.m31*b.m31 + a.m32*b.m32 + a.m33*b.m33;

		} else {

			r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
			r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
			r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;

			r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
			r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
			r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;

			r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
			r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
			r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;

		}

	}
	
}

void Matrix3x3Base::setupRotation(const Vector3 &_axis, float theta, bool isNormalized, bool leftHanded, bool inverse) {

	
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	//assrt(fabs(axis*axis - 1.0f) < .01f);

	// Get sin and cosine of rotation angle

	Vector3 axis = _axis;

	if (isNormalized == false) {
		axis.normalize();
	}

	float	s, c;
	sinCos(&s, &c, theta, leftHanded);

	// Compute 1 - cos(theta) and some common subexpressions

	float	a = 1.0f - c;
	float	ax = a * axis.x;
	float	ay = a * axis.y;
	float	az = a * axis.z;

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...

	if (inverse == false) {

		m11 = ax*axis.x + c;
		m12 = ax*axis.y + axis.z*s;
		m13 = ax*axis.z - axis.y*s;

		m21 = ay*axis.x - axis.z*s;
		m22 = ay*axis.y + c;
		m23 = ay*axis.z + axis.x*s;

		m31 = az*axis.x + axis.y*s;
		m32 = az*axis.y - axis.x*s;
		m33 = az*axis.z + c;

	} else {

		m11 = ax*axis.x + c;
		m21 = ax*axis.y + axis.z*s;
		m31 = ax*axis.z - axis.y*s;

		m12 = ay*axis.x - axis.z*s;
		m22 = ay*axis.y + c;
		m32 = ay*axis.z + axis.x*s;

		m13 = az*axis.x + axis.y*s;
		m23 = az*axis.y - axis.x*s;
		m33 = az*axis.z + c;
	}
}

void Matrix3x3Base::moveOrientation(Matrix3x3Base& rot) {

	Matrix3x3Base temp = *this;
	
	mul(temp, rot, *this);
}

void mul(const Matrix3x3Base& mat, const Vector3 &v, Vector3& result, bool inverse) {

	if (inverse == false) {
		
		result.x  = mat.m11*v.x + mat.m21*v.y + mat.m31*v.z;
		result.y = mat.m12*v.x + mat.m22*v.y + mat.m32*v.z;
		result.z = mat.m13*v.x + mat.m23*v.y + mat.m33*v.z;

	} else {

		result.x = mat.m11*v.x + mat.m12*v.y + mat.m13*v.z;
		result.y = mat.m21*v.x + mat.m22*v.y + mat.m23*v.z;
		result.z = mat.m31*v.x + mat.m32*v.y + mat.m33*v.z;

	}
}

void Matrix3x3Base::transform(const Vector3& vector, Vector3& result, bool inverse) const {

	mul(*this, vector, result, inverse);
}

void Matrix3x3Base::transform(Vector3& vector, bool inverse) const {

	WE::Vector3 temp = vector;

	mul(*this, temp, vector, inverse);
}


void identity(Matrix4x3Base& mat, bool rot, bool trans) {

	if (rot) {
		mat.m11 = 1.0f; mat.m12 = 0.0f; mat.m13 = 0.0f;
		mat.m21 = 0.0f; mat.m22 = 1.0f; mat.m23 = 0.0f;
		mat.m31 = 0.0f; mat.m32 = 0.0f; mat.m33 = 1.0f;
	}

	if (trans) {
		mat.tx = mat.ty = mat.tz = 0.0f;
	}
}

inline float _mulDimNoTrans(const float &p, const Matrix4x3Base &m, short dim) {

	return p * m.row[dim].x + p * m.row[dim].y + p * m.row[dim].z;
}

inline float _mulDim(const float &p, const Matrix4x3Base &m, short dim) {

	return p * m.row[dim].x + p * m.row[dim].y + p * m.row[dim].z + m.row[3].el[dim];
}

void mul(const Matrix4x3Base &a, const Matrix4x3Base &b, Matrix4x3Base &r) {

	// Compute the upper 3x3 (linear transformation) portion

	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;

	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;

	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;

	// Compute the translation portion

	r.tx = a.tx*b.m11 + a.ty*b.m21 + a.tz*b.m31 + b.tx;
	r.ty = a.tx*b.m12 + a.ty*b.m22 + a.tz*b.m32 + b.ty;
	r.tz = a.tx*b.m13 + a.ty*b.m23 + a.tz*b.m33 + b.tz;
}


void mul(const Vector3 &p, const Matrix4x3Base &m, Vector3& result) {

	// Grind through the linear algebra.
	result.x = p.x*m.m11 + p.y*m.m21 + p.z*m.m31 + m.tx;
	result.y = p.x*m.m12 + p.y*m.m22 + p.z*m.m32 + m.ty;
	result.z = p.x*m.m13 + p.y*m.m23 + p.z*m.m33 + m.tz;
}



void mulNoTrans(const Vector3 &p, const Matrix4x3Base &m, Vector3& result) {

	// Grind through the linear algebra.
	result.x = p.x*m.m11 + p.y*m.m21 + p.z*m.m31;
	result.y = p.x*m.m12 + p.y*m.m22 + p.z*m.m32;
	result.z = p.x*m.m13 + p.y*m.m23 + p.z*m.m33;
}


Matrix4x3Base::Matrix4x3Base() {
}

Matrix4x3Base::Matrix4x3Base(bool identity) {

	WE::identity(*this, true, true);
}

Matrix3x3Base& Matrix4x3Base::castToMatrix3x3Base() {
	return *((Matrix3x3Base*) ((Matrix4x3Base*) this));
}

const Matrix3x3Base& Matrix4x3Base::castToCtMatrix3x3Base() const {
	return *((Matrix3x3Base*) ((Matrix4x3Base*) this));
}

void Matrix4x3Base::copy3x3BaseTo(Matrix3x3Base& mat) const {

	memcpy(mat.row, row, sizeof(mat));
}

void Matrix4x3Base::simpleInvTransformVector(const Vector3& vector, Vector3& result) const {

	castToCtMatrix3x3Base().transform(vector, result, true);
}

/*
void Matrix4x3Base::simpleInvTransformPoint(const Vector3& p, Vector3& result) const {

	plain wrong

	result.x = p.x*m11 + p.y*m12 + p.z*m13 - tz;
	result.y = p.x*m21 + p.y*m22 + p.z*m23 - ty;
	result.z = p.x*m31 + p.y*m32 + p.z*m33 - tx;

	Vector3 temp1 = result;
	Vector3 temp2;

	transformPoint(temp1, temp2);
}
*/

void Matrix4x3Base::invTransformVector(const Vector3& vector, Vector3& result) const {

	Matrix4x3Base inv;

	inverse(*this, inv);

	inv.transformVector(vector, result);
}

void Matrix4x3Base::invTransformPoint(const Vector3& point, Vector3& result) const {

	Matrix4x3Base inv;

	inverse(*this, inv);

	inv.transformPoint(point, result);

	Vector3 temp1 = result;
	Vector3 temp2;

	transformPoint(temp1, temp2);
}

void Matrix4x3Base::transformPoint(const Vector3& point, Vector3& result) const {

	mul(point, *this, result);
}

void Matrix4x3Base::transformPointDim(const float& point, short dim, float& result) const {

	result = _mulDim(point, *this, dim);
}

float Matrix4x3Base::transformPointDim(const float& point, short dim) const {

	return _mulDim(point, *this, dim);
}

void Matrix4x3Base::transformVector(const Vector3& vector, Vector3& result) const {

	mulNoTrans(vector, *this, result);
}

void Matrix4x3Base::transformVectorDim(const float& vector, short dim, float& result) const {

	result = _mulDimNoTrans(vector, *this, dim);
}

float Matrix4x3Base::transformVectorDim(const float& vector, short dim) const {

	return _mulDimNoTrans(vector, *this, dim);
}

void Matrix4x3Base::transformPoint(Vector3& point) const {
	Vector3 temp = point;

	mul(temp, *this, point);
}

void Matrix4x3Base::transformVector(Vector3& vector) const {
	Vector3 temp = vector;

	mulNoTrans(temp, *this, vector);
}

void Matrix4x3Base::identity() {

	WE::identity(*this, true, true);
}


void RigidMatrix4x3::zeroTranslation() {
	
	WE::identity(*this, false, true);
}


void RigidMatrix4x3::setTranslation(const Vector3 &d) {

	tx = d.x; ty = d.y; tz = d.z;
}



void RigidMatrix4x3::setupTranslation(const Vector3 &d) {

	WE::identity(*this, true, false);

	// Set the translation portion
	tx = d.x; ty = d.y; tz = d.z;
}


void _setRotate(RigidMatrix4x3& mat, const Vector3 &eulerAngles, bool leftHanded, bool setToTranspose) {

	// Fetch sine and cosine of angles
	float	sh,ch, sp,cp, sb,cb;
	sinCos(&sh, &ch, eulerAngles.el[0], leftHanded);
	sinCos(&sp, &cp, eulerAngles.el[1], leftHanded);
	sinCos(&sb, &cb, eulerAngles.el[2], leftHanded);

	// Fill in the matrix elements
	if (setToTranspose == false) {

		mat.m11 = ch * cb + sh * sp * sb;
		mat.m12 = -ch * sb + sh * sp * cb;
		mat.m13 = sh * cp;

		mat.m21 = sb * cp;
		mat.m22 = cb * cp;
		mat.m23 = -sp;

		mat.m31 = -sh * cb + ch * sp * sb;
		mat.m32 = sb * sh + ch * sp * cb;
		mat.m33 = ch * cp;

	} else {

		mat.m11 = ch * cb + sh * sp * sb;
		mat.m21 = -ch * sb + sh * sp * cb;
		mat.m31 = sh * cp;

		mat.m12 = sb * cp;
		mat.m22 = cb * cp;
		mat.m32 = -sp;

		mat.m13 = -sh * cb + ch * sp * sb;
		mat.m23 = sb * sh + ch * sp * cb;
		mat.m33 = ch * cp;

	}

}

inline void _setRotate(RigidMatrix4x3& mat, RigidMatrix4x3::CardinalAxis axis, float theta, bool leftHanded, bool setToTranspose) {

	// Get sin and cosine of rotation angle

	float	s, c;
	sinCos(&s, &c, theta, leftHanded);

	// Check which axis they are rotating about

	switch (axis) {

		case RigidMatrix4x3::Axis_X: // Rotate about the x-axis
			
			if (setToTranspose == false) {
				mat.m11 = 1.0f; mat.m12 = 0.0f; mat.m13 = 0.0f;
				mat.m21 = 0.0f; mat.m22 = c;    mat.m23 = s;
				mat.m31 = 0.0f; mat.m32 = -s;   mat.m33 = c;
			} else {
				mat.m11 = 1.0f; mat.m21 = 0.0f; mat.m31 = 0.0f;
				mat.m12 = 0.0f; mat.m22 = c;    mat.m32 = s;
				mat.m13 = 0.0f; mat.m23 = -s;   mat.m33 = c;
			}
			break;

		case RigidMatrix4x3::Axis_Z: // Rotate about the z-axis

			if (setToTranspose == false) {
				mat.m11 = c;    mat.m12 = s;    mat.m13 = 0.0f;
				mat.m21 = -s;   mat.m22 = c;    mat.m23 = 0.0f;
				mat.m31 = 0.0f; mat.m32 = 0.0f; mat.m33 = 1.0f;
			} else {
				mat.m11 = c;    mat.m21 = s;    mat.m31 = 0.0f;
				mat.m12 = -s;   mat.m22 = c;    mat.m32 = 0.0f;
				mat.m13 = 0.0f; mat.m23 = 0.0f; mat.m33 = 1.0f;
			}
			break;

		default: //case RigidMatrix4x3::Axis_Y:  Rotate about the y-axis

			if (setToTranspose == false) {
				mat.m11 = c;    mat.m12 = 0.0f; mat.m13 = -s;
				mat.m21 = 0.0f; mat.m22 = 1.0f; mat.m23 = 0.0f;
				mat.m31 = s;    mat.m32 = 0.0f; mat.m33 = c;
			} else {
				mat.m11 = c;    mat.m21 = 0.0f; mat.m31 = -s;
				mat.m12 = 0.0f; mat.m22 = 1.0f; mat.m32 = 0.0f;
				mat.m13 = s;    mat.m23 = 0.0f; mat.m33 = c;
			}
			break;
	}
}


void _setRotate(RigidMatrix4x3& mat, const Vector3& _axis, float theta, bool isNormalized, bool leftHanded, bool setToTranspose) {

	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	//assrt(fabs(axis*axis - 1.0f) < .01f);

	// Get sin and cosine of rotation angle

	Vector3 axis = _axis;

	if (isNormalized == false) {
		axis.normalize();
	}

	float	s, c;
	sinCos(&s, &c, theta, leftHanded);

	// Compute 1 - cos(theta) and some common subexpressions

	float	a = 1.0f - c;
	float	ax = a * axis.x;
	float	ay = a * axis.y;
	float	az = a * axis.z;

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...

	if (setToTranspose == false) {

		mat.m11 = ax*axis.x + c;
		mat.m12 = ax*axis.y + axis.z*s;
		mat.m13 = ax*axis.z - axis.y*s;

		mat.m21 = ay*axis.x - axis.z*s;
		mat.m22 = ay*axis.y + c;
		mat.m23 = ay*axis.z + axis.x*s;

		mat.m31 = az*axis.x + axis.y*s;
		mat.m32 = az*axis.y - axis.x*s;
		mat.m33 = az*axis.z + c;

	} else {

		mat.m11 = ax*axis.x + c;
		mat.m21 = ax*axis.y + axis.z*s;
		mat.m31 = ax*axis.z - axis.y*s;

		mat.m12 = ay*axis.x - axis.z*s;
		mat.m22 = ay*axis.y + c;
		mat.m32 = ay*axis.z + axis.x*s;

		mat.m13 = az*axis.x + axis.y*s;
		mat.m23 = az*axis.y - axis.x*s;
		mat.m33 = az*axis.z + c;
	}
}



void RigidMatrix4x3::zeroRotation() {

	WE::identity(*this, true, false);
}


void RigidMatrix4x3::setRotate(CardinalAxis axis, float theta, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, axis, theta, leftHanded, setToTranspose);
}

void RigidMatrix4x3::setRotate(const Vector3 &axis, float theta, bool isNormalized, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, axis, theta, isNormalized, leftHanded, setToTranspose);
}

void RigidMatrix4x3::setRotate(const Vector3 &eulerAngles, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, eulerAngles, leftHanded, setToTranspose);
}

void RigidMatrix4x3::setupRotation(CardinalAxis axis, float theta, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, axis, theta, leftHanded, setToTranspose);
	WE::identity(*this, false, true);
}

void RigidMatrix4x3::setupRotation(const Vector3 &axis, float theta, bool isNormalized, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, axis, theta, isNormalized, leftHanded, setToTranspose);
	WE::identity(*this, false, true);
}

void RigidMatrix4x3::setupRotation(const Vector3 &eulerAngles, bool leftHanded, bool setToTranspose) {

	_setRotate(*this, eulerAngles, leftHanded, setToTranspose);
	WE::identity(*this, false, true);
}

void RigidMatrix4x3::setupLookAt(const Vector& eye, const Vector& at, const Vector& up, bool leftHanded) {

	/*
	zaxis = normal(At - Eye)
xaxis = normal(cross(Up, zaxis))
yaxis = cross(zaxis, xaxis)

 xaxis.x           yaxis.x           zaxis.x          0
 xaxis.y           yaxis.y           zaxis.y          0
 xaxis.z           yaxis.z           zaxis.z          0
-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1


RH
zaxis = normal(Eye - At)

	*/

	Vector axis[3];

	if (leftHanded) {
		at.subtract(eye, axis[2]);
	} else {
		eye.subtract(at, axis[2]);
	}
	axis[2].normalize();

	up.cross(axis[2], axis[0]);
	axis[0].normalize();

	axis[2].cross(axis[0], axis[1]);
	
	m11 = axis[0].x; m12 = axis[1].x; m13 = axis[2].x; 
	m21 = axis[0].y; m22 = axis[1].y; m23 = axis[2].y; 
	m31 = axis[0].z; m32 = axis[1].z; m33 = axis[2].z;

	tx = -axis[0].dot(eye); ty = -axis[1].dot(eye); tz = -axis[2].dot(eye);
}


void RigidMatrix4x3::__setupScale(const Vector3 &s) {

	// Set the matrix elements.  Pretty straightforward
	m11 = s.x;  m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = s.y;  m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = s.z;

	// Reset the translation portion
	WE::identity(*this, false, true);
}

void RigidMatrix4x3::__setScale(const Vector3 &s) {

	// Set the matrix elements.  Pretty straightforward
	m11 *= s.x;  
	m22 *= s.y;
	m33 *= s.z;
}

void Matrix4x3::setupScale(const Vector3 &s) {

	// Set the matrix elements.  Pretty straightforward
	m11 = s.x;  m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = s.y;  m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = s.z;

	// Reset the translation portion
	WE::identity(*this, false, true);
}

void Matrix4x3::setScale(const Vector3 &s) {

	// Set the matrix elements.  Pretty straightforward
	m11 *= s.x;
	m22 *= s.y;
	m33 *= s.z;
}


void Matrix4x4::setup(const Matrix4x3Base& ref) {

	row_1.set(ref.row[0]); el_14 = 0.0f;
	row_2.set(ref.row[1]); el_24 = 0.0f;
	row_3.set(ref.row[2]); el_34 = 0.0f;
	row_4.zero(); el_44 = 0.0f;
}

void Matrix4x4::setupPerspectiveFov(float fovY, float aspect, float zn, float zf, bool leftHanded) {

	float yScale = 1.0f / tan(fovY * 0.5f);
	float xScale = yScale / aspect;

	row_1.set(xScale, 0.0f, 0.0f); el_14 = 0.0f;
	row_2.set(0.0f, yScale, 0.0f); el_24 = 0.0f;
	row_3.set(0.0f, 0.0f, leftHanded ? zf/(zf-zn) : zf/(zn-zf)); el_34 = leftHanded ? 1.0f : -1.0f;
	row_4.set(0.0f, 0.0f, leftHanded ? -zn*zf/(zf-zn) : zn*zf/(zn-zf)); el_44 = 0.0f;

}

void Matrix4x4::setupOrtho(float w, float h, float zn, float zf, bool leftHanded) {

	row_1.set(2.0f / w, 0.0f, 0.0f); el_14 = 0.0f;
	row_2.set(0.0f, 2.0f / h, 0.0f); el_24 = 0.0f;
	row_3.set(0.0f, 0.0f, leftHanded ? 1.0f/(zf-zn) : 1.0f/(zn-zf)); el_34 = 0.0f;
	row_4.set(0.0f, 0.0f, zn/(zn-zf)); el_44 = 1.0f;
}


void Matrix4x4::setupPlanarProjection(Plane& plane, Vector3& dir) {

	dir.negate();

	float d = plane.normal.dot(dir);
	float pleq[4];
	
	plane.toPlaneEquation(pleq[0], pleq[1], pleq[2], pleq[3]);


	/*
	P.a * L.x + d  P.a * L.y      P.a * L.z      P.a * L.w  
	P.b * L.x      P.b * L.y + d  P.b * L.z      P.b * L.w  
	P.c * L.x      P.c * L.y      P.c * L.z + d  P.c * L.w  
	P.d * L.x      P.d * L.y      P.d * L.z      P.d * L.w + d
	*/

	
	m11 = pleq[0] + dir.x + d; m12 = pleq[0] + dir.y; m13 = pleq[0] + dir.z; m14 = 0.0f;
	m21 = pleq[1] + dir.x; m22 = pleq[1] + dir.y + d; m23 = pleq[1] + dir.z; m24 = 0.0f;
	m31 = pleq[2] + dir.x; m32 = pleq[2] + dir.y; m33 = pleq[2] + dir.z + d; m34 = 0.0f;
	m41 = pleq[3] + dir.x; m42 = pleq[3] + dir.y; m43 = pleq[3] + dir.z; m44 = d;
	
	
	m11 = pleq[0] + dir.x + d; m12 = pleq[0] + dir.y; m13 = pleq[0] + dir.z; m14 = 0.0f;
	m21 = pleq[1] + dir.x; m22 = pleq[1] + dir.y + d; m23 = pleq[1] + dir.z; m24 = 0.0f;
	m31 = pleq[2] + dir.x; m32 = pleq[2] + dir.y; m33 = pleq[2] + dir.z + d; m34 = 0.0f;
	m41 = pleq[3] + dir.x; m42 = pleq[3] + dir.y; m43 = pleq[3] + dir.z; m44 = d;

	/*
	m11 = pleq[0] + dir.x + d; m12 = pleq[1] + dir.y; m13 = pleq[2] + dir.z; m14 = 0.0f;
	m21 = pleq[0] + dir.x; m22 = pleq[1] + dir.y + d; m23 = pleq[2] + dir.z; m24 = 0.0f;
	m31 = pleq[0] + dir.x; m32 = pleq[1] + dir.y; m33 = pleq[2] + dir.z + d; m34 = 0.0f;
	m41 = pleq[0] + dir.x; m42 = pleq[1] + dir.y; m43 = pleq[2] + dir.z; m44 = -d;
	*/

	/*
	m11 = pleq[0] - dir.x + d; m12 = pleq[0] - dir.y; m13 = pleq[0] - dir.z; m14 = 0.0f;
	m21 = pleq[1] - dir.x; m22 = pleq[1] - dir.y + d; m23 = pleq[1] - dir.z; m24 = 0.0f;
	m31 = pleq[2] - dir.x; m32 = pleq[2] - dir.y; m33 = pleq[2] - dir.z + d; m34 = 0.0f;
	m41 = pleq[3] - dir.x; m42 = pleq[3] - dir.y; m43 = pleq[3] - dir.z; m44 = d;
	*/
	
}

void mul(const Vector3 &p, const Matrix4x4& m, Vector3& result, float& wInOut) {

	// Grind through the linear algebra.
	result.x = p.x*m.m11 + p.y*m.m21 + p.z*m.m31 + wInOut * m.m41;
	result.y = p.x*m.m12 + p.y*m.m22 + p.z*m.m32 + wInOut * m.m42;
	result.z = p.x*m.m13 + p.y*m.m23 + p.z*m.m33 + wInOut * m.m43;
	wInOut = p.x*m.m14 + p.y*m.m24 + p.z*m.m34 + wInOut * m.m44;
}

void Matrix4x4::transformPoint(const Vector3& point, Vector3& result, float& inOutW) const {

	mul(point, *this, result, inOutW);
}


void mul(const Matrix4x4 &a, const Matrix4x4 &b, Matrix4x4 &r) {

	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31 + a.m14*b.m41;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32 + a.m14*b.m42;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33 + a.m14*b.m43;
	r.m14 = a.m11*b.m14 + a.m12*b.m24 + a.m13*b.m34 + a.m14*b.m44;

	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31 + a.m24*b.m41;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32 + a.m24*b.m42;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33 + a.m24*b.m43;
	r.m24 = a.m21*b.m14 + a.m22*b.m24 + a.m23*b.m34 + a.m24*b.m44;

	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31 + a.m34*b.m41;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32 + a.m34*b.m42;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33 + a.m34*b.m43;
	r.m34 = a.m31*b.m14 + a.m32*b.m24 + a.m33*b.m34 + a.m34*b.m44;

	r.m41 = a.m41*b.m11 + a.m42*b.m21 + a.m43*b.m31 + a.m44*b.m41;
	r.m42 = a.m41*b.m12 + a.m42*b.m22 + a.m43*b.m32 + a.m44*b.m42;
	r.m43 = a.m41*b.m13 + a.m42*b.m23 + a.m43*b.m33 + a.m44*b.m43;
	r.m44 = a.m41*b.m14 + a.m42*b.m24 + a.m43*b.m34 + a.m44*b.m44;
}

void mul(const Matrix4x4 &a, const Matrix4x3Base &b, Matrix4x3Base &r) {

	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31 + a.m14*b.tx;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32 + a.m14*b.ty;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33 + a.m14*b.tz;
	
	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31 + a.m24*b.tx;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32 + a.m24*b.ty;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33 + a.m24*b.tz;
	
	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31 + a.m34*b.tx;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32 + a.m34*b.ty;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33 + a.m34*b.tz;
	
	r.tx = a.m41*b.m11 + a.m42*b.m21 + a.m43*b.m31 + a.m44*b.tx;
	r.ty = a.m41*b.m12 + a.m42*b.m22 + a.m43*b.m32 + a.m44*b.ty;
	r.tz = a.m41*b.m13 + a.m42*b.m23 + a.m43*b.m33 + a.m44*b.tz;
}

void mul(const Matrix4x3Base &a, const Matrix4x4 &b, Matrix4x4 &r) {

	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;
	r.m14 = a.m11*b.m14 + a.m12*b.m24 + a.m13*b.m34;

	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;
	r.m24 = a.m21*b.m14 + a.m22*b.m24 + a.m23*b.m34;

	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;
	r.m34 = a.m31*b.m14 + a.m32*b.m24 + a.m33*b.m34;

	r.m41 = a.tx*b.m11 + a.ty*b.m21 + a.tz*b.m31 + b.m41;
	r.m42 = a.tx*b.m12 + a.ty*b.m22 + a.tz*b.m32 + b.m42;
	r.m43 = a.tx*b.m13 + a.ty*b.m23 + a.tz*b.m33 + b.m43;
	r.m44 = a.tx*b.m14 + a.ty*b.m24 + a.tz*b.m34 + b.m44;
}

float determinant(const Matrix4x3Base &m) {

	return
		  m.m11 * (m.m22*m.m33 - m.m23*m.m32)
		+ m.m12 * (m.m23*m.m31 - m.m21*m.m33)
		+ m.m13 * (m.m21*m.m32 - m.m22*m.m31);
}

void inverse(const Matrix4x3Base &m, Matrix4x3Base &r) {

	// Compute the determinant

	float	det = determinant(m);

	// If we're singular, then the determinant is zero and there's
	// no inverse
	assrt(fabs(det) > 0.000001f);


	// Compute one over the determinant, so we divide once and
	// can *multiply* per element

	float	oneOverDet = 1.0f / det;

	// Compute the 3x3 portion of the inverse, by
	// dividing the adjoint by the determinant

	r.m11 = (m.m22*m.m33 - m.m23*m.m32) * oneOverDet;
	r.m12 = (m.m13*m.m32 - m.m12*m.m33) * oneOverDet;
	r.m13 = (m.m12*m.m23 - m.m13*m.m22) * oneOverDet;

	r.m21 = (m.m23*m.m31 - m.m21*m.m33) * oneOverDet;
	r.m22 = (m.m11*m.m33 - m.m13*m.m31) * oneOverDet;
	r.m23 = (m.m13*m.m21 - m.m11*m.m23) * oneOverDet;

	r.m31 = (m.m21*m.m32 - m.m22*m.m31) * oneOverDet;
	r.m32 = (m.m12*m.m31 - m.m11*m.m32) * oneOverDet;
	r.m33 = (m.m11*m.m22 - m.m12*m.m21) * oneOverDet;

	// Compute the translation portion of the inverse

	r.tx = -(m.tx*r.m11 + m.ty*r.m21 + m.tz*r.m31);
	r.ty = -(m.tx*r.m12 + m.ty*r.m22 + m.tz*r.m32);
	r.tz = -(m.tx*r.m13 + m.ty*r.m23 + m.tz*r.m33);

}

}