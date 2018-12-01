#ifndef _WEMatrix2_h
#define _WEMatrix2_h

#include "WEVector2.h"

namespace WE {

	struct Matrix2x2 {

		union {
			struct {
				float m11, m12;
				float m21, m22;
			};
			struct {
				Vector2 row[2];
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

		Matrix2x2();
		Matrix2x2(bool identity);	

		void diagonal(float value);
		void identity();
		
		
		void transpose();
		void transpose(Matrix2x2& result);

		void scale(float multiplier);

		void setupRotation(float theta, bool leftHanded = true, bool inverse = false);
		void moveOrientation(Matrix2x2& rot);

		void transform(const Vector2& vector, Vector2& result, bool inverse = false) const;
		void transform(Vector2& vector, bool inverse = false) const;
	};

	void mul(const Matrix2x2& first, const Vector2& second, Vector2& result, bool inverseMatrix = false);
	void mul(const Matrix2x2& first, const Matrix2x2& second, Matrix2x2& result); 
	void mul(const Matrix2x2& first, const Matrix2x2& second, Matrix2x2& result, bool transFirst, bool transSecond); 


	struct Matrix3x2 {

		union {
			struct {
				float	m11, m12;
				float	m21, m22;
				float	tx,  ty;
			};
			struct {
				Vector2 row[3];
			};
		};

		static const Matrix3x2 kIdentity;

		Matrix3x2();
		Matrix3x2(bool identity);

		void identity();

		void transformVector(const Vector2& vector, Vector2& result) const;
		void transformPoint(const Vector2& point, Vector2& result) const;

		void simpleInvTransformVector(const Vector2& vector, Vector2& result) const;
		//void simpleInvTransformPoint(const Vector2& point, Vector2& result) const;

		void invTransformVector(const Vector2& vector, Vector2& result) const;
		void invTransformPoint(const Vector2& point, Vector2& result) const;

		void transformVector(Vector2& vector) const;
		void transformPoint(Vector2& point) const;

		void transformPointDim(const float& point, short dim, float& result) const;
		void transformVectorDim(const float& point, short dim, float& result) const;

		float transformPointDim(const float& point, short dim) const;
		float transformVectorDim(const float& point, short dim) const;

		void copy3x3BaseTo(Matrix2x2& mat) const;
		Matrix2x2& castToMatrix2x2();
		const Matrix2x2& castToCtMatrix2x2() const;

		void __setupScale(const Vector2 &s);
		void __setScale(const Vector2 &s);
	};
	
	struct RigidMatrix3x2 : Matrix3x2 {

		enum CardinalAxis {
			Axis_X = 0, Axis_Y = 1
		};

		void zeroTranslation();
		void setTranslation(const Vector2 &d);
		void setupTranslation(const Vector2 &d);
		
		void zeroRotation();
		void setRotate(CardinalAxis axis, float theta, bool leftHanded = true, bool setToTranspose = false);
		void setRotate(const Vector2 &axis, float theta, bool isNormalized = false, bool leftHanded = true, bool setToTranspose = false);
		void setRotate(const Vector2 &eulerAngles, bool leftHanded = true, bool setToTranspose = false);
		void setupRotation(CardinalAxis axis, float theta, bool leftHanded = true, bool setToTranspose = false);
		void setupRotation(const Vector2 &axis, float theta, bool isNormalized = false, bool leftHanded = true, bool setToTranspose = false);
		void setupRotation(const Vector2 &eulerAngles, bool leftHanded = true, bool setToTranspose = false);
	};
	
	void diagonal(Matrix2x2& mat, float value);
	void identity(Matrix3x2& mat, bool rot, bool trans);

	void mul(const Matrix3x2& first, const Matrix3x2& second, Matrix3x2& result); 
	void inverse(const Matrix3x2& matrix, Matrix3x2& result); 
	

	void mul(const Vector2 &p, const Matrix3x2 &m, Vector2& result);
	void mulNoTrans(const Vector2 &p, const Matrix3x2 &m, Vector2& result);

}

#endif