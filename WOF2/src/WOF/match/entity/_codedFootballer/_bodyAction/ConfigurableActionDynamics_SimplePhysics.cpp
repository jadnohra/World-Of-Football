#include "ConfigurableActionDynamics_SimplePhysics.h"

#include "WE3/helper/load/WEMathDataLoadHelper.h"
using namespace WE;

namespace WOF { namespace match {

/*
ConfigurableActionDynamics_SimplePhysics::InitValue::InitValue() 
	: base(0.0f), inputMultiplier(0.0f) {
}

bool ConfigurableActionDynamics_SimplePhysics::InitValue::load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	if (!MathDataLoadHelper::extract(tempStr, chunk, base, true, true, true, L"base", pConv, false, true)) {

		assrt(false);
		return false;
	}

	if (!MathDataLoadHelper::extract(tempStr, chunk, inputMultiplier, true, true, true, L"inputMultiplier", NULL)) {

		assrt(false);
		return false;

	} else {

		if (pConv) {

			pConv->toTargetSwizzle(inputMultiplier.el);
		}
	}

	return true;
}

void ConfigurableActionDynamics_SimplePhysics::InitValue::setup(const Vector3& input, Vector3& result) {

	Vector3 temp;

	input.mul(inputMultiplier, temp);
	base.add(temp, result);
}

bool ConfigurableActionDynamics_SimplePhysics::AccelerationController_Acceleration::load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {


}

bool ConfigurableActionDynamics_SimplePhysics::AccelerationController_Acceleration::frameMove(const Time& t, const Time& dt, Vector3& inOutVel) {
}

bool ConfigurableActionDynamics_SimplePhysics::load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	SoftRef<DataChunk> child = chunk.getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"velocity")) {

			if (!mInitVel.load(tempStr, child, pConv)) {

				return false;
			}
		}

		if (child->equals(L"acceleration")) {

			if (!mInitAcc.load(tempStr, child, pConv)) {

				return false;
			}
		}

		if (child->equals(L"timeDamping")) {

			if (!mInitAcc.load(tempStr, child, pConv)) {

				return false;
			}
		}

		toNextSibling(child);
	}

	return true;
}

void ConfigurableActionDynamics_SimplePhysics::start(const Vector3& startAcc, const Vector3& startVel, const Time& t) {

	we need a beter way of representing vel based acceleration

	mInitAcc.setup(startAcc, mAcc);
	mInitVel.setup(startVel, mVel);
}

void ConfigurableActionDynamics_SimplePhysics::stop(const Time& t) {
}

bool ConfigurableActionDynamics_SimplePhysics::frameMove(const Time& t, const Time& dt) {

	Vector3 vDiff;

	mAcc.mul(dt, vDiff);
	mVel.add(vDiff);

	return true;
}
*/

} }