#ifndef _WEQuaternion_h
#define _WEQuaternion_h

#include "WEVector.h"
#include "WEMatrix.h"

namespace WE {

	class Quaternion {
	public:

		union {
			struct {
				float w, x, y, z;
			};
			struct {
				float el[4];
			};
		};

	public:

		Quaternion() {}
		Quaternion(bool dummy) { identity(); }

		void identity();
		void normalize();

		void derivativeFromAngularVelocity(const Vector3& w);
		void derivativeFromAngularVelocity(const Vector3& normalizedAxis, float velocity);
		void derivativeToAngularVelocity(Vector3& normalizedAxis, float& velocity);

		void fromAxisAngle(const Vector3& normalizedAxis, float angle, bool isNormalized);
		void toAxisAngle(Vector3& normalizedAxis, float& angle) const;

		void fromMatrix(const Matrix3x3Base& mat);
		void toMatrix(Matrix3x3Base& mat, bool isNormalized);
		void toMatrix(Matrix3x3Base& mat) const; //quat has to be already normalized

		void mul(float scale);
		void mul(float scale, Quaternion& result) const;
		void mulAndAdd(float scale, Quaternion& result) const;
		void add(const Quaternion& q);

	public:

		static const Quaternion kIdentity;
	};

	void mul(const Quaternion& a, const Quaternion& b, Quaternion& axb);
	void mul(const Vector& a, const Quaternion& b, Quaternion& axb);
	float dot(const Quaternion& a, const Quaternion& b);

	void slerp(const Quaternion& a, const Quaternion& b, float t, Quaternion& result);
	void nlerp(const Quaternion& a, const Quaternion& b, float t, Quaternion& result);

}

#endif