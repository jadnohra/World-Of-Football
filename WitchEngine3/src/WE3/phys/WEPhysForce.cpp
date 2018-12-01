#include "WEPhysForce.h"
#include "WEPhysRigidBody.h"

namespace WE {


PhysForceField::PhysForceField() {

	value.zero();
}

void PhysForceField::init(const Vector3& value, bool isAccel) {

	this->value = value;
	isAcceleration = isAccel;
}

void PhysForceField::apply(PhysRigidBody& body) {

	if (isAcceleration) {

		body.force += value;

	} else {

		Vector3 temp;

		value.mul(body.mass, temp);
		body.force += temp;
	}

}
	
}