#ifndef _WEMatrixBase_h
#define _WEMatrixBase_h

#include "WEVector.h"

namespace WE {

	
	struct Matrix3x3Base {

		union {
			struct {
				float	m11, m12, m13;
				float	m21, m22, m23;
				float	m31, m32, m33;
			};
			struct {
				Vector3 row[3];
			};
		};

		inline float& el(const short r, const short c) {

			return row[r].el[c];
		}

		inline float operator () ( const short i, const short j ) const{
			return row[i].el[j];
		}

		inline float& operator () ( const short i, const short j ) {
			return row[i].el[j];
		}

		Matrix3x3Base();
		Matrix3x3Base(bool identity);	

		void diagonal(float value);
		void identity();
		
		
		void transpose();
		void transpose(Matrix3x3Base& result);

		void inverse(Matrix3x3Base& result);

		void scale(float multiplier);

		void setupRotation(const Vector3 &axis, float theta, bool isNormalized = false, bool leftHanded = true, bool inverse = false);
		void moveOrientation(Matrix3x3Base& rot);

		void transform(const Vector3& vector, Vector3& result, bool inverse = false) const;
		void transform(Vector3& vector, bool inverse = false) const;
	};

	void mul(const Matrix3x3Base& first, const Vector3& second, Vector3& result, bool inverseMatrix = false);
	void mul(const Matrix3x3Base& first, const Matrix3x3Base& second, Matrix3x3Base& result); 
	void mul(const Matrix3x3Base& first, const Matrix3x3Base& second, Matrix3x3Base& result, bool transFirst, bool transSecond); 

	struct Matrix4x3Base {

		union {
			struct {
				float	m11, m12, m13;
				float	m21, m22, m23;
				float	m31, m32, m33;
				float	tx,  ty,  tz;
			};
			struct {
				Vector3 row[4];
			};
		};

		static const Matrix4x3Base kIdentity;

		Matrix4x3Base();
		Matrix4x3Base(bool identity);

		void identity();

		void transformVector(const Vector3& vector, Vector3& result) const;
		void transformPoint(const Vector3& point, Vector3& result) const;

		void simpleInvTransformVector(const Vector3& vector, Vector3& result) const;
		//void simpleInvTransformPoint(const Vector3& point, Vector3& result) const;

		void invTransformVector(const Vector3& vector, Vector3& result) const;
		void invTransformPoint(const Vector3& point, Vector3& result) const;

		void transformVector(Vector3& vector) const;
		void transformPoint(Vector3& point) const;

		void transformPointDim(const float& point, short dim, float& result) const;
		void transformVectorDim(const float& point, short dim, float& result) const;

		float transformPointDim(const float& point, short dim) const;
		float transformVectorDim(const float& point, short dim) const;

		void copy3x3BaseTo(Matrix3x3Base& mat) const;
		Matrix3x3Base& castToMatrix3x3Base();
		const Matrix3x3Base& castToCtMatrix3x3Base() const;
	};
	
	struct RigidMatrix4x3 : Matrix4x3Base {

		enum CardinalAxis {
			Axis_X = 0, Axis_Y = 1, Axis_Z = 2
		};

		void zeroTranslation();
		void setTranslation(const Vector3 &d);
		void setupTranslation(const Vector3 &d);
		
		void zeroRotation();
		void setRotate(CardinalAxis axis, float theta, bool leftHanded = true, bool setToTranspose = false);
		void setRotate(const Vector3 &axis, float theta, bool isNormalized = false, bool leftHanded = true, bool setToTranspose = false);
		void setRotate(const Vector3 &eulerAngles, bool leftHanded = true, bool setToTranspose = false);
		void setupRotation(CardinalAxis axis, float theta, bool leftHanded = true, bool setToTranspose = false);
		void setupRotation(const Vector3 &axis, float theta, bool isNormalized = false, bool leftHanded = true, bool setToTranspose = false);
		void setupRotation(const Vector3 &eulerAngles, bool leftHanded = true, bool setToTranspose = false);

		void setupLookAt(const Vector& eye, const Vector& at, const Vector& up, bool leftHanded = true);

		void __setupScale(const Vector3 &s);
		void __setScale(const Vector3 &s);
	};

	//void mul(const RigidMatrix4x3& first, const RigidMatrix4x3& second, RigidMatrix4x3& result); 

	struct Plane;

	struct Matrix4x3 : RigidMatrix4x3 {

		void setupScale(const Vector3 &s);
		void setScale(const Vector3 &s);
	};

	
	void diagonal(Matrix3x3Base& mat, float value);
	void identity(Matrix4x3Base& mat, bool rot, bool trans);

	void mul(const Matrix4x3Base& first, const Matrix4x3Base& second, Matrix4x3Base& result); 
	//void mul(const Matrix4x3& first, const Matrix4x3& second, Matrix4x3& result); 
	//void mul(const Matrix4x3& first, const RigidMatrix4x3& second, Matrix4x3& result); 
	//void mul(const RigidMatrix4x3& first, const Matrix4x3& second, Matrix4x3& result); 
	//void inverse(const RigidMatrix4x3& matrix, RigidMatrix4x3& result); 
	void inverse(const Matrix4x3Base& matrix, Matrix4x3Base& result); 
	

	void mul(const Vector3 &p, const Matrix4x3Base &m, Vector3& result);
	void mulNoTrans(const Vector3 &p, const Matrix4x3Base &m, Vector3& result);

	struct Matrix4x4 {

		union {
			struct {
				float	m11, m12, m13, m14;
				float	m21, m22, m23, m24;
				float	m31, m32, m33, m34;
				float	m41,  m42,  m43, m44;
			};
			struct {
				Vector3 row_1; float el_14;
				Vector3 row_2; float el_24;
				Vector3 row_3; float el_34;
				Vector3 row_4; float el_44;
			};
		};

		void setup(const Matrix4x3Base& ref);

		void setupPerspectiveFov(float fovY, float aspect, float zn, float zf, bool leftHanded = true);
		void setupPlanarProjection(Plane& plane, Vector3& direction);
		void setupOrtho(float w, float h, float zn, float zf, bool leftHanded = true);

		void transformPoint(const Vector3& point, Vector3& result, float& inOutW) const;
	};

	void mul(const Matrix4x4& first, const Matrix4x4& second, Matrix4x4& result); 
	void mul(const Matrix4x4& first, const Matrix4x3Base& second, Matrix4x3Base& result); 
	void mul(const Matrix4x3Base& first, const Matrix4x4& second, Matrix4x4& result); 

	typedef RigidMatrix4x3 RigidMatrix;
	typedef Matrix4x3 Matrix43;
	typedef Matrix4x3 Matrix;
	typedef Matrix3x3Base Matrix33;
	typedef Matrix3x3Base RotationMatrix;
	typedef Matrix3x3Base OrientationMatrix;
}


#endif