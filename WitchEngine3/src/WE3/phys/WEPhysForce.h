#ifndef _WEPhysForce_h
#define _WEPhysForce_h

#include "WEPhysTypes.h"
#include "../WETL/WETLArray.h"

namespace WE {

	struct PhysRigidBody;

	class PhysForce {
	public:

		virtual void apply(PhysRigidBody& body) = 0;
	};

	typedef WETL::Array<PhysForce*, false, short> PhysForcePtrArray;

	class PhysForceField : public PhysForce {
	public:

		Vector3 value;
		bool isAcceleration; //otherwize force
		bool isExternal;

	public:

		PhysForceField();

		void init(const Vector3& value, bool isAccel);

		virtual void apply(PhysRigidBody& body);
	};
}

#endif