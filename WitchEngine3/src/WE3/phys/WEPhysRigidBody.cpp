#include "WEPhysRigidBody.h"

namespace WE {

void PhysRigidBody::doPositionUpdate(const PhysScalar& dt, const PhysScalar& halfDt, Vector3& dPos, Quaternion& tempQ) {

	if (isFixed == false) {

		v.mul(dt, dPos);
		pos += dPos;

		mul(w, orient, tempQ);
		tempQ.mul(halfDt);
		orient.add(tempQ);
		orient.normalize();

		/*
		{
			orient.toMatrix(orientMat, false);
			Matrix33 test;

			test.m11 = 0.0f;
			test.m12 = w.z * dt;
			test.m13 = -w.y * dt;

			test.m21 = -w.z * dt;
			test.m22 = 0.0f;
			test.m23 = w.x * dt;

			test.m31 = w.y * dt;
			test.m32 = -w.x * dt;
			test.m33 = 0.0f;

			mul(orientMat, test, qDot);

			orientMat.m11 += qDot.m11;
			orientMat.m12 += qDot.m12;
			orientMat.m13 += qDot.m13;
			orientMat.m21 += qDot.m21;
			orientMat.m22 += qDot.m22;
			orientMat.m23 += qDot.m23;
			orientMat.m31 += qDot.m31;
			orientMat.m32 += qDot.m32;
			orientMat.m33 += qDot.m33;

			orient.fromMatrix(orientMat);
			orient.normalize();
		}
		*/
	}
}

}